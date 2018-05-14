/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Daniela Cabiddu (daniela.cabiddu@ge.imati.cnr.it)                 *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of EpsilonShapes.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/

#include "include/geometry.h"
#include "include/sphere.h"

#ifdef GEOMETRICTOOLS
    #include "Mathematics/GteDistPointSegment.h"
    #include "Mathematics/GteDistPointTriangle.h"
    #include "Mathematics/GteDistSegmentSegment.h"
    #include "Mathematics/GteDistSegment3Triangle3.h"
    #include "Mathematics/GteDistTriangle3Triangle3.h"
    #include "Mathematics/GteSegment.h"
    #include "Mathematics/GteTriangle.h"
    #include "Mathematics/GteVector3.h"

    #include "include/wrapper_gte.h"
#else

#endif

#ifdef CAXLIB
using namespace caxlib;

#endif

namespace danilib {

DANI_INLINE
void Sphere::init()
{
    center = {FLT_MAX};
    tangent = {FLT_MAX};

    radius = FLT_MAX;

    center_pos = CENTERED_UNKNOWN;
    tangent_pos = TANGENT_UNKNOWN;

    center_id = UINT_MAX;
    tangent_id = UINT_MAX;

    tangent_tri = std::pair<uint, uint> (UINT_MAX, UINT_MAX);
}

DANI_INLINE
void Sphere::print_sphere (std::ofstream &out)
{
    if (!out.is_open())
        assert(false);

    out << radius << " ";

    out << center.x() << " " << center.y() << " " << center.z() << " ";

    if (tangent_pos == TANGENT_ON_VERTEX)
    {
        out << "0 " << tangent_id << " ";
    }
    else
    if (tangent_pos == TANGENT_ON_EDGE)
    {
        out << "1 " << tangent_id << " ";
    }
    else
    if (tangent_pos == TANGENT_ON_TRIANGLE)
    {
        out << "2 " << tangent_id << " ";
    }
    else
        assert(false);

    out << tangent.x() << " " << tangent.y() << " " << tangent.z() << " ";
}

DANI_INLINE
void Sphere::read_sphere (std::ifstream &in)
{
    if (!in.is_open())
        assert(false);

    in >> radius;

    in >> center[0];
    in >> center[1];
    in >> center[2];

    int tangent_pos_tmp, tangent_id_tmp;

    in >> tangent_pos_tmp;
    in >> tangent_id_tmp;

    if (tangent_pos_tmp == 0)
        tangent_pos = TANGENT_ON_VERTEX;
    else
    if (tangent_pos_tmp == 1)
       tangent_pos = TANGENT_ON_EDGE;
    else
    if (tangent_pos_tmp == 2)
        tangent_pos = TANGENT_ON_TRIANGLE;
    else assert(false);

    tangent_id = tangent_id_tmp;

    in >> tangent[0];
    in >> tangent[1];
    in >> tangent[2];
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///



/**
 * @brief Sphere::create_Tcentered_Vtangent
 * @param tetmesh
 * @param tet_tri
 * @param vid
 */
DANI_INLINE
void Sphere::create_Vcentered_Vtangent (const Tetmesh &tetmesh, uint tet_vid0, const uint tet_vid1)
{
    radius = tetmesh.vertex(tet_vid0).dist(tetmesh.vertex(tet_vid1));

    center = tetmesh.vertex(tet_vid0);
    tangent = tetmesh.vertex(tet_vid1);

    center_pos = CENTERED_ON_VERTEX;
    tangent_pos = TANGENT_ON_VERTEX;

    center_id = tet_vid0;
    tangent_id = tet_vid1;

}

/**
 * @brief Sphere::create_Vcentered_Etangent
 * @param tetmesh
 * @param tet_tri
 * @param eid
 */
DANI_INLINE
void Sphere::create_Vcentered_Etangent (const Tetmesh &tetmesh, uint tet_vid, const uint eid)
{
#ifdef GEOMETRICTOOLS

    gte::Vector3<double> e0 = get_gte_vertex(tetmesh.edge_vertex(eid, 0));
    gte::Vector3<double> e1 = get_gte_vertex(tetmesh.edge_vertex(eid, 1));

    gte::Segment3<double> segment (e0, e1);

    gte::Vector3<double> v = get_gte_vertex(tetmesh.vertex(tet_vid));

    gte::DCPQuery<double, gte::Vector3<double>, gte::Segment3<double>> query;

    gte::DCPQuery<double, gte::Vector3<double>, gte::Segment3<double>>::Result result = query(v, segment);

    center = tetmesh.vertex(tet_vid);

    set_center_on_vertex(tet_vid);

    tangent[0] = result.segmentClosest[0];
    tangent[1] = result.segmentClosest[1];
    tangent[2] = result.segmentClosest[2];

    radius = result.distance;

    PointPosition pp_t = is_point_on_edge(tangent, tetmesh.edge_vertex(eid, 0), tetmesh.edge_vertex(eid, 1));

    if (pp_t == EXTERNAL_POINT)
        assert(false);

    //////////

    if (pp_t == POINT_EDGE_P0)
        set_tangent_on_vertex(tetmesh.edge_vertex_id(eid, 0));
    else
    if (pp_t == POINT_EDGE_P1)
        set_tangent_on_vertex(tetmesh.edge_vertex_id(eid, 1));
    else
        set_tangent_on_edge(eid);

    description = __FUNCTION__;

    tangent_ref = TANGENT_REFERENCE_ON_EDGE;
    tangent_ref_id = eid;

#else

#endif
}

/**
 * @brief Sphere::create_Vcentered_Ttangent
 * @param tetmesh
 * @param tet_vid
 * @param tet_tri
 */
DANI_INLINE
void Sphere::create_Vcentered_Ttangent (const Tetmesh &tetmesh, uint tet_vid, std::pair<uint, uint> tet_tri)
{

#ifdef GEOMETRICTOOLS
    //T0

    uint v0_vid = tetmesh.tet_vertex_id(tet_tri.first, TET_FACES[tet_tri.second][0]);
    uint v1_vid = tetmesh.tet_vertex_id(tet_tri.first, TET_FACES[tet_tri.second][1]);
    uint v2_vid = tetmesh.tet_vertex_id(tet_tri.first, TET_FACES[tet_tri.second][2]);

    gte::Triangle3<double> t = get_gte_triangle(tetmesh.vertex(v0_vid), tetmesh.vertex(v1_vid), tetmesh.vertex(v2_vid));
    gte::Vector3<double> v = get_gte_vertex(tetmesh.vertex(tet_vid));

    // Run query
    gte::DCPQuery<double, gte::Vector3<double>, gte::Triangle3<double>> query;
    gte::DCPQuery<double, gte::Vector3<double>, gte::Triangle3<double>>::Result result = query(v, t);

    radius = result.distance;

    center = tetmesh.vertex(tet_vid);
    set_center_on_vertex(tet_vid);

    tangent[0] = result.closest[0];
    tangent[1] = result.closest[1];
    tangent[2] = result.closest[2];

    //check...................................

    PointPosition pp_t = is_point_on_triangle(tangent, tetmesh.vertex(v0_vid), tetmesh.vertex(v1_vid), tetmesh.vertex(v2_vid));

    if (pp_t == EXTERNAL_POINT)
        assert(false);

    //////////////////////////////////////////////

    center_pos = CENTERED_ON_VERTEX;

    if (pp_t == POINT_TRIANGLE_P0)
        set_tangent_on_vertex(v0_vid);
    else
    if (pp_t == POINT_TRIANGLE_P1)
        set_tangent_on_vertex(v1_vid);
    else
    if (pp_t == POINT_TRIANGLE_P2)
        set_tangent_on_vertex(v2_vid);
    else
    if (pp_t == POINT_TRIANGLE_E0)
        set_tangent_on_edge(tetmesh.tet_edge_id(tet_tri.first, v0_vid, v1_vid));
    else
    if (pp_t == POINT_TRIANGLE_E1)
        set_tangent_on_edge(tetmesh.tet_edge_id(tet_tri.first, v1_vid, v2_vid));
    else
    if (pp_t == POINT_TRIANGLE_E2)
        set_tangent_on_edge(tetmesh.tet_edge_id(tet_tri.first, v2_vid, v0_vid));
    else
        set_tangent_on_triangle(tet_tri.first, tet_tri.second);

    description = __FUNCTION__;

    tangent_ref = TANGENT_REFERENCE_ON_TRIANGLE;
    tangent_ref_tri = tet_tri;


#else

#endif

}


/**
 * @brief Sphere::create_Tcentered_Vtangent
 * @param tetmesh
 * @param tet_tri
 * @param vid
 */
DANI_INLINE
void Sphere::create_Tcentered_Vtangent (const Tetmesh &tetmesh, std::pair<uint, uint> tet_tri, const uint vid)
{
#ifdef GEOMETRICTOOLS

    uint v0_vid = tetmesh.tet_vertex_id(tet_tri.first, TET_FACES[tet_tri.second][0]);
    uint v1_vid = tetmesh.tet_vertex_id(tet_tri.first, TET_FACES[tet_tri.second][1]);
    uint v2_vid = tetmesh.tet_vertex_id(tet_tri.first, TET_FACES[tet_tri.second][2]);

    gte::Triangle3<double> t = get_gte_triangle(tetmesh.vertex(v0_vid), tetmesh.vertex(v1_vid), tetmesh.vertex(v2_vid));

    gte::Vector3<double> v;
    v[0] = tetmesh.vertex(vid).x();
    v[1] = tetmesh.vertex(vid).y();
    v[2] = tetmesh.vertex(vid).z();

    gte::DCPQuery<double, gte::Vector3<double>, gte::Triangle3<double>> query;
    gte::DCPQuery<double, gte::Vector3<double>, gte::Triangle3<double>>::Result result = query (v,t);

    radius = result.distance;

    set_tangent_on_vertex(vid);

    tangent = tetmesh.vertex(vid);

    center[0] = result.closest[0];
    center[1] = result.closest[1];
    center[2] = result.closest[2];

    PointPosition pp = is_point_on_triangle(center, tetmesh.vertex(v0_vid), tetmesh.vertex(v1_vid), tetmesh.vertex(v2_vid));

    if (pp == EXTERNAL_POINT)
        assert(false);

    if (pp == POINT_TRIANGLE_P0)
        set_center_on_vertex(v0_vid);
    else
    if (pp == POINT_TRIANGLE_P1)
        set_center_on_vertex(v1_vid);
    else
    if (pp == POINT_TRIANGLE_P2)
        set_center_on_vertex(v2_vid);
    else
    if (pp == POINT_TRIANGLE_E0)
        set_center_on_edge(tetmesh.tet_edge_id(tet_tri.first, v0_vid, v1_vid));
    else
    if (pp == POINT_TRIANGLE_E1)
        set_center_on_edge(tetmesh.tet_edge_id(tet_tri.first, v1_vid, v2_vid));
    else
    if (pp == POINT_TRIANGLE_E2)
        set_center_on_edge(tetmesh.tet_edge_id(tet_tri.first, v2_vid, v0_vid));
    else
        set_center_on_triangle(tet_tri.first, tet_tri.second);

    center_tri = tet_tri;

    description = __FUNCTION__;

    tangent_ref = TANGENT_REFERENCE_ON_VERTEX;
    tangent_ref_id = vid;

#else

#endif

}

/**
 * @brief Sphere::create_Tcentered_Etangent
 * @param tetmesh
 * @param tet_tri
 * @param eid
 */
DANI_INLINE
void Sphere::create_Tcentered_Etangent (const Tetmesh &tetmesh, std::pair<uint, uint> tet_tri, const uint eid)
{
#ifdef GEOMETRICTOOLS

    uint v0_vid = tetmesh.tet_vertex_id(tet_tri.first, TET_FACES[tet_tri.second][0]);
    uint v1_vid = tetmesh.tet_vertex_id(tet_tri.first, TET_FACES[tet_tri.second][1]);
    uint v2_vid = tetmesh.tet_vertex_id(tet_tri.first, TET_FACES[tet_tri.second][2]);

    gte::Triangle3<double> triangle = get_gte_triangle(tetmesh.vertex(v0_vid), tetmesh.vertex(v1_vid), tetmesh.vertex(v2_vid));

    gte::Vector3<double> e0 = get_gte_vertex(tetmesh.edge_vertex(eid, 0));
    gte::Vector3<double> e1 = get_gte_vertex(tetmesh.edge_vertex(eid, 1));

    gte::Segment3<double> segment (e0, e1);

    gte::DCPQuery<double, gte::Segment3<double>, gte::Triangle3<double>> query;

    gte::DCPQuery<double, gte::Segment3<double>, gte::Triangle3<double>>::Result result = query(segment, triangle);

    center[0] = result.closestPoint[0][0];
    center[1] = result.closestPoint[0][1];
    center[2] = result.closestPoint[0][2];

    tangent[0] = result.closestPoint[1][0];
    tangent[1] = result.closestPoint[1][1];
    tangent[2] = result.closestPoint[1][2];

    radius = result.distance;

    PointPosition pp_c = is_point_on_triangle(center, tetmesh.vertex(v0_vid), tetmesh.vertex(v1_vid), tetmesh.vertex(v2_vid));

    // check
    if (pp_c == EXTERNAL_POINT)
    {
        vec3d tmp = center;
        center = tangent;
        tangent = tmp;

        pp_c = is_point_on_triangle(center, tetmesh.vertex(v0_vid), tetmesh.vertex(v1_vid), tetmesh.vertex(v2_vid));
    }

    PointPosition pp_t = is_point_on_edge(tangent, tetmesh.edge_vertex(eid, 0), tetmesh.edge_vertex(eid, 1));

//    if (pp_c == EXTERNAL_POINT)
//        assert(false);

//    if (pp_t == EXTERNAL_POINT)
//    {
//        is_point_on_edge(tangent, tetmesh.edge_vertex(eid, 0), tetmesh.edge_vertex(eid, 1));
//        assert(false);
//    }

    //////////

    if (pp_c == POINT_TRIANGLE_P0)
        set_center_on_vertex(v0_vid);
    else
    if (pp_c == POINT_TRIANGLE_P1)
        set_center_on_vertex(v1_vid);
    else
    if (pp_c == POINT_TRIANGLE_P2)
        set_center_on_vertex(v2_vid);
    else
    if (pp_c == POINT_TRIANGLE_E0)
        set_center_on_edge(tetmesh.tet_edge_id(tet_tri.first, v0_vid, v1_vid));
    else
    if (pp_c == POINT_TRIANGLE_E1)
        set_center_on_edge(tetmesh.tet_edge_id(tet_tri.first, v1_vid, v2_vid));
    else
    if (pp_c == POINT_TRIANGLE_E2)
        set_center_on_edge(tetmesh.tet_edge_id(tet_tri.first, v2_vid, v0_vid));
    else
        set_center_on_triangle(tet_tri.first, tet_tri.second);

    center_tri = tet_tri;

    if (pp_t == POINT_EDGE_P0)
        set_tangent_on_vertex(tetmesh.edge_vertex_id(eid, 0));
    else
    if (pp_t == POINT_EDGE_P1)
        set_tangent_on_vertex(tetmesh.edge_vertex_id(eid, 1));
    else
        set_tangent_on_edge(eid);

    description = __FUNCTION__;

    tangent_ref = TANGENT_REFERENCE_ON_EDGE;
    tangent_ref_id = eid;

#else

#endif
}

/**
 * @brief Sphere::create_Tcentered_Ttangent
 * @param tetmesh
 * @param tet_tri0
 * @param tet_tri1
 */
DANI_INLINE
void Sphere::create_Tcentered_Ttangent (const Tetmesh &tetmesh, std::pair<uint, uint> tet_tri0, std::pair<uint, uint> tet_tri1)
{

#ifdef GEOMETRICTOOLS
    //T0

    uint t0_v0_vid = tetmesh.tet_vertex_id(tet_tri0.first, TET_FACES[tet_tri0.second][0]);
    uint t0_v1_vid = tetmesh.tet_vertex_id(tet_tri0.first, TET_FACES[tet_tri0.second][1]);
    uint t0_v2_vid = tetmesh.tet_vertex_id(tet_tri0.first, TET_FACES[tet_tri0.second][2]);

    gte::Triangle3<double> t0 = get_gte_triangle(tetmesh.vertex(t0_v0_vid), tetmesh.vertex(t0_v1_vid), tetmesh.vertex(t0_v2_vid));

    uint t1_v0_vid = tetmesh.tet_vertex_id(tet_tri1.first, TET_FACES[tet_tri1.second][0]);
    uint t1_v1_vid = tetmesh.tet_vertex_id(tet_tri1.first, TET_FACES[tet_tri1.second][1]);
    uint t1_v2_vid = tetmesh.tet_vertex_id(tet_tri1.first, TET_FACES[tet_tri1.second][2]);

    gte::Triangle3<double> t1 = get_gte_triangle(tetmesh.vertex(t1_v0_vid), tetmesh.vertex(t1_v1_vid), tetmesh.vertex(t1_v2_vid));

    // Run query
    gte::DCPQuery<double, gte::Triangle3<double>, gte::Triangle3<double>> query;

    gte::DCPQuery<double, gte::Triangle3<double>, gte::Triangle3<double>>::Result result = query(t0, t1);

    radius = result.distance;

    center[0] = result.closestPoint[0][0];
    center[1] = result.closestPoint[0][1];
    center[2] = result.closestPoint[0][2];

    tangent[0] = result.closestPoint[1][0];
    tangent[1] = result.closestPoint[1][1];
    tangent[2] = result.closestPoint[1][2];

    //check...................................

    PointPosition pp_c = is_point_on_triangle(center, tetmesh.vertex(t0_v0_vid), tetmesh.vertex(t0_v1_vid), tetmesh.vertex(t0_v2_vid));


    if (pp_c == EXTERNAL_POINT )
    {
       vec3d tmp = center;
       center = tangent;
       tangent = tmp;

       pp_c = is_point_on_triangle(center, tetmesh.vertex(t0_v0_vid), tetmesh.vertex(t0_v1_vid), tetmesh.vertex(t0_v2_vid));
    }

    PointPosition pp_t = is_point_on_triangle(tangent, tetmesh.vertex(t1_v0_vid), tetmesh.vertex(t1_v1_vid), tetmesh.vertex(t1_v2_vid));
//    if (pp_c == EXTERNAL_POINT)
//    {
//        assert(false);
//    }

//    if (pp_t == EXTERNAL_POINT)
//        assert(false);

    //////////////////////////////////////////////

    if (pp_c == POINT_TRIANGLE_P0)
        set_center_on_vertex(t0_v0_vid);
    else
    if (pp_c == POINT_TRIANGLE_P1)
        set_center_on_vertex(t0_v1_vid);
    else
    if (pp_c == POINT_TRIANGLE_P2)
        set_center_on_vertex(t0_v2_vid);
    else
    if (pp_c == POINT_TRIANGLE_E0)
        set_center_on_edge(tetmesh.tet_edge_id(tet_tri0.first, t0_v0_vid, t0_v1_vid));
    else
    if (pp_c == POINT_TRIANGLE_E1)
        set_center_on_edge(tetmesh.tet_edge_id(tet_tri0.first, t0_v1_vid, t0_v2_vid));
    else
    if (pp_c == POINT_TRIANGLE_E2)
        set_center_on_edge(tetmesh.tet_edge_id(tet_tri0.first, t0_v2_vid, t0_v0_vid));
    else
        set_center_on_triangle(tet_tri0.first, tet_tri0.second);

    center_tri = tet_tri0;

    if (pp_t == POINT_TRIANGLE_P0)
        set_tangent_on_vertex(t1_v0_vid);
    else
    if (pp_t == POINT_TRIANGLE_P1)
        set_tangent_on_vertex(t1_v1_vid);
    else
    if (pp_t == POINT_TRIANGLE_P2)
        set_tangent_on_vertex(t1_v2_vid);
    else
    if (pp_t == POINT_TRIANGLE_E0)
        set_tangent_on_edge(tetmesh.tet_edge_id(tet_tri1.first, t1_v0_vid, t1_v1_vid));
    else
    if (pp_t == POINT_TRIANGLE_E1)
        set_tangent_on_edge(tetmesh.tet_edge_id(tet_tri1.first, t1_v1_vid, t1_v2_vid));
    else
    if (pp_t == POINT_TRIANGLE_E2)
        set_tangent_on_edge(tetmesh.tet_edge_id(tet_tri1.first, t1_v2_vid, t1_v0_vid));
    else
        set_tangent_on_triangle(tet_tri1.first, tet_tri1.second);

    description = __FUNCTION__;

    tangent_ref = TANGENT_REFERENCE_ON_TRIANGLE;
    tangent_ref_tri = tet_tri1;


#else

#endif

}

DANI_INLINE
bool Sphere::has_inner_point(vec3d v_coords) const
{

    if (v_coords.dist(center) <= radius + thresh)
        return true;

    return false;
}


}
