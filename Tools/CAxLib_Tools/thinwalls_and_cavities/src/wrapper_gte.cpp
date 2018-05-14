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

#include "include/wrapper_gte.h"

namespace danilib {

#ifdef GEOMETRICTOOLS

DANI_INLINE
gte::Vector3<double> get_gte_vertex (const vec3d vertex)
{
    gte::Vector3<double> v;

    v[0] = vertex.x();
    v[1] = vertex.y();
    v[2] = vertex.z();

    return v;
}


DANI_INLINE
gte::Triangle3<double> get_gte_triangle (const vec3d vertex0, const vec3d vertex1, const vec3d vertex2)
{

    gte::Vector3<double> v0, v1, v2;

    v0 = get_gte_vertex(vertex0);
    v1 = get_gte_vertex(vertex1);
    v2 = get_gte_vertex(vertex2);

    gte::Triangle3<double> triangle (v0, v1, v2);

    return triangle;
}

DANI_INLINE
gte::DCPQuery<double, gte::Vector3<double>, gte::Segment3<double>>::Result VE_min_distance(vec3d vertex, std::vector<vec3d> edg)
{
    gte::Vector3<double> point = get_gte_vertex(vertex);

    gte::Vector3<double> e0 = get_gte_vertex(edg.at(0));
    gte::Vector3<double> e1 = get_gte_vertex(edg.at(1));

    gte::Segment3<double> segment (e0, e1);

    gte::DCPQuery<double, gte::Vector3<double>, gte::Segment3<double>> query;

    return query (point, segment);
}

DANI_INLINE
gte::DCPQuery<double, gte::Vector3<double>, gte::Triangle3<double>>::Result VT_min_distance (const vec3d vertex, const std::vector<vec3d> tri)
{
    gte::Vector3<double> point = get_gte_vertex(vertex);
    gte::Triangle3<double> triangle = get_gte_triangle(tri.at(0), tri.at(1), tri.at(2));

    gte::DCPQuery<double, gte::Vector3<double>, gte::Triangle3<double>> query;

    return query (point, triangle);
}

DANI_INLINE
gte::DCPQuery<double, gte::Vector3<double>, gte::Triangle3<double>>::Result VT_min_distance (const vec3d vertex, vec3d tri0, vec3d tri1, vec3d tri2)
{
    gte::Vector3<double> point = get_gte_vertex(vertex);
    gte::Triangle3<double> triangle = get_gte_triangle(tri0, tri1, tri2);

    gte::DCPQuery<double, gte::Vector3<double>, gte::Triangle3<double>> query;

    return query (point, triangle);
}

DANI_INLINE
gte::DCPQuery<double, gte::Vector3<double>, gte::Triangle3<double>>::Result VT_min_distance (const Trimesh &m, const vec3d vertex, const uint tid)
{
    gte::Vector3<double> point = get_gte_vertex(vertex);
    gte::Triangle3<double> triangle = get_gte_triangle(m.triangle_vertex(tid, 0), m.triangle_vertex(tid, 1), m.triangle_vertex(tid, 2));

    gte::DCPQuery<double, gte::Vector3<double>, gte::Triangle3<double>> query;

    return query (point, triangle);
}

#endif


DANI_INLINE
double VT_distance (const vec3d vertex, const std::vector<vec3d> tri)
{
#ifdef GEOMETRICTOOLS

    return VT_min_distance(vertex, tri).distance;
#else
    std::cerr << "[ERROR] " GEOMETRICTOOLS is needed to compute minimum distances. << std::endl;
    exit(1);
#endif

}


DANI_INLINE
double VT_distance (const vec3d vertex, vec3d tri0, vec3d tri1, vec3d tri2)
{
#ifdef GEOMETRICTOOLS

    return VT_min_distance(vertex, tri0, tri1, tri2).distance;
#else
    std::cerr << "[ERROR] " GEOMETRICTOOLS is needed to compute minimum distances. << std::endl;
    exit(1);
#endif

}

DANI_INLINE
double VE_distance (vec3d vertex, std::vector<vec3d> edg)
{
#ifdef GEOMETRICTOOLS
    return VE_min_distance(vertex, edg).distance;
#else
    std::cerr << "[ERROR] " GEOMETRICTOOLS is needed to compute minimum distances. << std::endl;
    exit(1);
#endif
}

DANI_INLINE
double VT_distance (const Trimesh &m, const vec3d vertex, const uint tid)
{
#ifdef GEOMETRICTOOLS
    return VT_min_distance(m, vertex, tid).distance;
#else
    std::cerr << "[ERROR] " GEOMETRICTOOLS is needed to compute minimum distances. << std::endl;
    exit(1);
#endif
}


}
