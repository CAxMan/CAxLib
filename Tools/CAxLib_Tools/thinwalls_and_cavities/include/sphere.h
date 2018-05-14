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

#ifndef SPHERE_H
#define SPHERE_H


#include "include/geometry.h"

#ifdef CINOLIB
#include "geometry/vec3.h"
using namespace cinolib;
#else
#ifdef CAXLIB
#include "caxlib/vec3.h"
using namespace caxlib;
#endif
#endif

namespace danilib
{

enum CenterPosition
{
    CENTERED_ON_VERTEX,
    CENTERED_ON_EDGE,
    CENTERED_ON_TRIANGLE,
    CENTERED_UNKNOWN
};

enum TangentPosition
{
    TANGENT_ON_VERTEX,
    TANGENT_ON_EDGE,
    TANGENT_ON_TRIANGLE,
    TANGENT_UNKNOWN

};

enum TangentReference
{
    TANGENT_REFERENCE_ON_VERTEX,
    TANGENT_REFERENCE_ON_EDGE,
    TANGENT_REFERENCE_ON_TRIANGLE,
    TANGENT_REFERENCE_UNKNOWN

};

enum IDReference
{
    REFERENCE_TO_TIN,
    REFERENCE_TO_TET
};

enum SphereStatus
{
    EPSILON_SPHERE,
    NOT_EPSILON_SPHERE,
    TO_BE_CHECKED,
};


class Sphere {

public:

    uint dimension;

    IDReference id_ref = REFERENCE_TO_TET;
    SphereStatus status = TO_BE_CHECKED;

    std::string description;

    vec3d center = {FLT_MAX};
    vec3d tangent = {FLT_MAX};
    double radius = FLT_MAX;

    uint cvid;  // Used only in 2D case // To be removed

    CenterPosition center_pos = CENTERED_UNKNOWN;
    TangentPosition tangent_pos = TANGENT_UNKNOWN;
    TangentReference tangent_ref = TANGENT_REFERENCE_UNKNOWN;

    uint center_id = UINT_MAX;
    uint tangent_id = UINT_MAX;
    uint tangent_ref_id = UINT_MAX;

    std::pair<uint, uint> center_tri;// = std::pair<uint, uint> (UINT_MAX, UINT_MAX); // Used to identify tangent triangles by < tet_id - tid_off >
    std::pair<uint, uint> tangent_tri;// = std::pair<uint, uint> (UINT_MAX, UINT_MAX); // Used to identify tangent triangles by < tet_id - tid_off >
    std::pair<uint, uint> tangent_ref_tri;// = std::pair<uint, uint> (UINT_MAX, UINT_MAX); // Used to identify tangent triangles by < tet_id - tid_off >

    uint ortogonal_to_tin_triangle = UINT_MAX;

    // Methods

    Sphere() { init(); }

    void init();

    void print_sphere (std::ofstream &out);
    void read_sphere (std::ifstream &in);

    void set_center_on_vertex ()                    {center_pos = CENTERED_ON_VERTEX;}
    void set_center_on_vertex (const uint vid)      {set_center_on_vertex(); center_id = vid;}

    void set_center_on_edge ()                      {center_pos = CENTERED_ON_EDGE;}
    void set_center_on_edge (const uint eid)        {set_center_on_edge(); center_id = eid;}

    void set_center_on_triangle ()                                  {center_pos = CENTERED_ON_TRIANGLE;}
    void set_center_on_triangle (const uint tet, const uint tid)    {set_center_on_triangle(); center_tri = std::pair<uint, uint> (tet, tid);}

    void set_tangent_on_vertex ()                   {tangent_pos = TANGENT_ON_VERTEX;}
    void set_tangent_on_vertex (const uint vid)     {set_tangent_on_vertex(); tangent_id = vid;}

    void set_tangent_on_edge ()                     {tangent_pos = TANGENT_ON_EDGE;}
    void set_tangent_on_edge (const uint eid)       {set_tangent_on_edge(); tangent_id = eid;}

    void set_tangent_on_triangle ()                                 {tangent_pos = TANGENT_ON_TRIANGLE;}
    void set_tangent_on_triangle (const uint tet, const uint tid)   {set_tangent_on_triangle(); tangent_tri = std::pair<uint, uint> (tet, tid);}

    void create_Vcentered_Vtangent (const Tetmesh &tetmesh, const uint cvid, const uint vid) ;
//    void create_Ecentered_Vtangent (const Tetmesh &tetmesh, const uint eid, const uint vid) ;
    void create_Tcentered_Vtangent (const Tetmesh &tetmesh, std::pair<uint, uint> tet_tri0, const uint vid) ;

    void create_Vcentered_Etangent (const Tetmesh &tetmesh, const uint vid, const uint eid) ;
//    void create_Ecentered_Etangent (const Tetmesh &tetmesh, const uint ceid, const uint teid) ;
//    void create_Ecentered_Etangent (const Tetmesh &tetmesh, const uint ceid, const uint tv0, const uint tv1) ;
    void create_Tcentered_Etangent (const Tetmesh &tetmesh, std::pair<uint, uint> tet_tri0, const uint eid) ;

    void create_Vcentered_Ttangent (const Tetmesh &tetmesh, const uint vid, std::pair<uint, uint> tet_tri) ;
//    void create_Ecentered_Ttangent (const Tetmesh &tetmesh, const uint eid, std::pair<uint, uint> tet_tri) ;
    void create_Tcentered_Ttangent (const Tetmesh &tetmesh, std::pair<uint, uint> tet_tri0, std::pair<uint, uint> tet_tri1) ;

    bool has_inner_point (const vec3d v_coords) const ;

    bool operator<(const Sphere& sph) const
    {
        if (are_points_equal(center, sph.center) && are_points_equal(tangent, sph.tangent))
            return false;

        if (radius - sph.radius < thresh||
            (radius - sph.radius == thresh && center_pos < sph.center_pos) ||
            (radius - sph.radius == thresh && center_pos == sph.center_pos && center_id < sph.center_id) ||
            (radius - sph.radius == thresh && center_pos == sph.center_pos && center_id == sph.center_id && tangent_pos < sph.tangent_pos) ||
            (radius - sph.radius == thresh && center_pos == sph.center_pos && center_id == sph.center_id && tangent_pos == sph.tangent_pos && tangent_id < sph.tangent_id) ||
            (radius - sph.radius == thresh && center_pos == sph.center_pos && center_id == sph.center_id && tangent_pos == sph.tangent_pos && tangent_id == sph.tangent_id && tangent_tri.first < sph.tangent_tri.first) ||
            (radius - sph.radius == thresh && center_pos == sph.center_pos && center_id == sph.center_id && tangent_pos == sph.tangent_pos && tangent_id == sph.tangent_id && tangent_tri.first == sph.tangent_tri.first && tangent_tri.second < sph.tangent_tri.second) ||
            (radius - sph.radius == thresh && center_pos == sph.center_pos && center_id == sph.center_id && tangent_pos == sph.tangent_pos && tangent_id == sph.tangent_id && tangent_tri.first == sph.tangent_tri.first && tangent_tri.second == sph.tangent_tri.second && status < sph.status))
            return true;

        return false;
    }

    bool operator>(const Sphere& sph) const
    {
        if (are_points_equal(center, sph.center) && are_points_equal(tangent, sph.tangent))
            return false;

        if (radius - sph.radius > thresh  ||
            (radius - sph.radius == thresh && center_pos > sph.center_pos) ||
            (radius - sph.radius == thresh && center_pos == sph.center_pos && center_id > sph.center_id) ||
            (radius - sph.radius == thresh && center_pos == sph.center_pos && center_id == sph.center_id && tangent_pos > sph.tangent_pos) ||
            (radius - sph.radius == thresh && center_pos == sph.center_pos && center_id == sph.center_id && tangent_pos == sph.tangent_pos && tangent_id > sph.tangent_id) ||
            (radius - sph.radius == thresh && center_pos == sph.center_pos && center_id == sph.center_id && tangent_pos == sph.tangent_pos && tangent_id == sph.tangent_id && tangent_tri.first > sph.tangent_tri.first) ||
            (radius - sph.radius == thresh && center_pos == sph.center_pos && center_id == sph.center_id && tangent_pos == sph.tangent_pos && tangent_id == sph.tangent_id && tangent_tri.first == sph.tangent_tri.first && tangent_tri.second > sph.tangent_tri.second) ||
            (radius - sph.radius == thresh && center_pos == sph.center_pos && center_id == sph.center_id && tangent_pos == sph.tangent_pos && tangent_id == sph.tangent_id && tangent_tri.first == sph.tangent_tri.first && tangent_tri.second == sph.tangent_tri.second && status > sph.status))
            return true;

        return false;
    }


    bool operator==(const Sphere& sph) const
    {
        if (are_points_equal(center, sph.center) && are_points_equal(tangent, sph.tangent))
            return true;

        if (!(*this < sph) && !(*this > sph) )
            return true;

        return false;
    }

    struct LessThan : public std::binary_function<Sphere, Sphere, bool>
    {
        bool operator()(const Sphere& lhs, const Sphere& rhs) const
        {
            return /*!(lhs == rhs) &&*/ (lhs < rhs);
        }
    };

};

}


#ifndef  DANI_STATIC_LIB
#include "src/sphere.cpp"
#endif

#endif // SPHERE_H
