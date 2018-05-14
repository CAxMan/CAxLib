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

#ifndef EPS3DSHAPE_H
#define EPS3DSHAPE_H

#include <iostream>
#include <tuple>

#include "epsshape.h"
#include "eps_trimesh.h"

namespace danilib
{

enum SearchDir
{
    THINWALLS,
    CAVITIES,
    BOTH
};

enum SplitPos
{
    SPLIT_ON_CENTER,
    SPLIT_ON_TANGENT
};

typedef std::vector<std::pair<uint, uint>> VectorOfTetTriangles;

class Epsilon3DShape : public EpsilonShape {

public:

    EpsTrimesh tin;
    Tetmesh tetmesh;

    // Settings from input parameters
    bool        compute_all         = false;
    double      max_epsilon         = 0.0;
    double      desired_epsilon     = 0.0;
    bool        store_all_spheres   = false;
    SearchDir   search_dir          = BOTH;
    int         parallelism_enabled = 0;

    // timing
    double time_tetgen      = 0.0;
    double time_analysis    = 0.0;
    double time_thickening  = 0.0;

    std::vector<uint> v_map_tin_tet; // map each vertex of polygon to vertex in tin
    std::vector<std::pair<VertexType, uint>> v_map_tet_tin; // map each vertex of tin to vertex in polygon

    std::vector<uint> e_map_tin_tet; // map each edge of polygon to edge in tin
    std::vector<std::pair<EdgeType, uint>> e_map_tet_tin; // map each edge of tin to edge in polygon

    std::vector<VectorOfTetTriangles> t_map_tin_tet;
    std::vector<std::vector<std::pair<TriType, uint>>> t_map_tet_tin;

    std::vector<bool> in_tet_labels;    // for each tet, true if the tet is inside the input surface, false otherwise

    std::vector<std::vector<Sphere>> t_spheres;

    std::vector<std::set<uint>> link_tets;

    std::vector<std::vector<bool>> global_v_labels;
    std::vector<std::vector<bool>> global_e_labels;
    std::vector<std::vector<bool>> global_tet_labels;

    Sphere min_epsilon_sphere;
    Sphere max_epsilon_sphere;

    std::vector<double> epsilons_v_map;
    std::vector<vec3d>  epsilons_antipoeads;
    std::vector<TangentPosition>  epsilons_antipoeads_type;
    std::vector<uint>  epsilons_antipoeads_id;

    //Methods
    Epsilon3DShape() : EpsilonShape() {}
    Epsilon3DShape(const char *filename);

    void set_max_epsilon        (const double eps)  {max_epsilon = eps;     }
    void set_desired_epsilon    (const double eps)  {desired_epsilon = eps; }
    void set_store_all_spheres  (const bool b)      {store_all_spheres = b; }
    void set_compute_all        (const bool b)      {compute_all = b;       }

    void set_search_direction   (const bool tb, const bool cb)
    {
        if (tb && cb) search_dir = BOTH;
        else if (cb) search_dir = CAVITIES;
        else if (tb) search_dir = THINWALLS;
        else
            assert(false);
    }

    void set_parallelism        (const int do_it_parallel) {parallelism_enabled = do_it_parallel; }

    void    compute_epsilon ();
    void    compute_all_epsilons();
    void    compute_triangles_with_epsilon_less_than(const double thresh);
    double  compute_epsilon_at (const CenterPosition cpos, const uint cid_tin);

    void    post_process_vertices ();
    void    split_epsilon_triangles ();


    void repair ();

    void load_epsilons_from_file (const char *filename);
    void save_epsilons_on_file (const char *filename);
    void save_annotations_ZIP (const char *filename);

    void save_colored_obj   (const char *filename);
    void save_colored_off   (const char *filename, bool color = true);

private:

    // Tetmesh
    void create_tetmesh ();

    // Tin - tetmesh mapping
    void set_v_map_tin_tet_tin ();
    void set_e_map_tin_tet_tin ();
    void set_t_map_tin_tet_tin ();

    bool is_tet_vertex_on_tin_srf   (const uint vid) const;
    bool is_tet_edge_on_tin_srf     (const uint eid) const;
    bool is_tet_triangle_on_tin_srf (const uint tet, const uint tid_off) const;

    uint tin_vertex_from_tet (const uint vid) const { return v_map_tet_tin.at(vid).second; }
    uint tet_vertex_from_tin (const uint vid) const { return v_map_tin_tet.at(vid); }

    uint tin_edge_from_tet (const uint eid) const { return e_map_tet_tin.at(eid).second; }
    uint tet_edge_from_tin (const uint eid) const { return e_map_tin_tet.at(eid); }

    uint tin_triangle_from_tet (const std::pair<uint,uint> tet_tidoff) const   { return t_map_tet_tin.at(tet_tidoff.first).at(tet_tidoff.second).second; }
    std::pair<uint,uint> tet_triangle_from_tin (const uint tid)        const   { return t_map_tin_tet.at(tid)[0]; }


    // Computing epsilon
    bool is_epsilon_sphere (Sphere &sph);

    bool is_tangent_concave_wrt_sphere (const Sphere &sphere) const;
    bool is_tangent_saddle_wrt_sphere (const Sphere &sphere) const;
    bool is_tangent_concave_or_saddle_wrt_sphere (const Sphere &sphere) const;

    bool are_center_tangent_connected (const Sphere &sphere);

    void create_global_sphere_priority_queue (SpherePriorityQueue &);
    void update_global_priority_queue(SpherePriorityQueue &, const Sphere &);


    void compute_link(const CenterPosition cpos, const uint cid, const std::set<uint> &new_link_tets, SpherePriorityQueue &link);

    void compute_link_tetmesh(const CenterPosition cpos, const uint cid, std::set<uint> &link_tets);

    void extend_link (const CenterPosition cpos, const uint cid, std::set<uint> &link_tets,
                      SpherePriorityQueue &link,  const Sphere &sph);

    void update_link_tets (const CenterPosition cpos, const uint cid, std::set<uint> &new_link_tets, const Sphere &sph);

    uint tin_edge_having_endpoints (const uint vid0, const uint vid1) const;

    bool check_center_on_tin_srf (const Sphere &sphere);

    Sphere convert_tet_tin_sphere (const Sphere &sphere) const ;
    
    std::vector<Sphere> create_additional_ortogonal_spheres (const Sphere &sphere);


    // Mesh editing

    void epsilons_to_vertices    ();
    void min_epsilon_to_vertices ();

    void thickening ();

    // Geometry

    bool is_saddle_vertex (const uint vid) const;

    double get_angle_TV  (const uint tid, const uint vid) const;
    double get_angle_VVV (const uint vid, const uint vid0, const uint vid1) const;
    double get_angle_VV(const vec3d v0, const vec3d v1) const;
    double get_angle_VVV(const vec3d vid, const vec3d vid0, const vec3d vid1) const;


    double get_dihedral_angle (const uint tin_eid) ;
    double get_dihedral_angle (Trimesh &local_tin, const uint tin_eid);

    bool are_dihedral_angles_greater_than (const double thresh);
    bool are_dihedral_vertex_angles_greater_than (const double thresh);
    bool are_solid_vertex_angle_greater_than (const double thresh);


};

}

#ifndef  DANI_STATIC_LIB
#include "src/eps3Dshape.cpp"
#include "src/eps3Dshape_edit.cpp"
#endif

#endif
