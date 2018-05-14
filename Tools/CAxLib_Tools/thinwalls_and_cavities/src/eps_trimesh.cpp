///****************************************************************************
//* Italian National Research Council                                         *
//* Institute for Applied Mathematics and Information Technologies, Genoa     *
//* IMATI-GE / CNR                                                            *
//*                                                                           *
//* Author: Daniela Cabiddu (daniela.cabiddu@ge.imati.cnr.it)                 *
//*                                                                           *
//* Copyright(C) 2016                                                         *
//* All rights reserved.                                                      *
//*                                                                           *
//* This file is part of EpsilonShapes.                                       *
//*                                                                           *
//* This program is distributed in the hope that it will be useful,           *
//* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
//* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
//* for more details.                                                         *
//****************************************************************************/

#include "include/danilib.h"
#include "include/eps_trimesh.h"
#include "include/geometry.h"

#include <algorithm>    // std::sort

#ifdef CINOLIB

    #include "scalar_field.h"

#endif

namespace danilib
{

DANI_INLINE
void EpsTrimesh::set_t_labels(const std::vector<int> &labels)
{
    assert (labels.size() == num_triangles());

    for (uint tid = 0; tid < num_triangles(); tid++)
    {
        set_t_label(tid, labels.at(tid));
    }

}

DANI_INLINE
void EpsTrimesh::set_t_labels(const std::vector<double> labels_sdf)
{
#ifdef CINOLIB

    assert (labels_sdf.size() == num_triangles());

    double min_value = FLT_MAX;

    std::vector<float> labels ;//= labels_sdf;

    for (uint tid = 0; tid < num_triangles(); tid++)
    {
        labels.push_back(labels_sdf.at(tid));
    }

    ScalarField values (labels);
    values.normalize_in_01();

    for (uint tid = 0; tid < num_triangles(); tid++)
    {
        double value = values[tid]; //.at(tid);

        if (value < min_value)
            min_value = labels.at(tid);
    }

    std::cout << "Minimum value from double to integer..." << std::endl;

    int factor = 1;
    int int_value = (int) min_value;

    double delta = 0.0; // used to handle minimum values equal to 0

    if (min_value == 0.0)
    {
       delta = 1e-5;
       min_value += delta;
    }

    while (int_value <= 10)
    {
        factor *= 10;

        min_value = (min_value * factor);
        int_value = (int) min_value;

        std::cout << min_value << " -- " << int_value << std::endl;
    }

    //(std::cout << std::endl << " Min value " << min_value << " -- integer : " << int_value << std::endl;

    std::cout << "Associating triangle labels ..." << std::endl;

    for (uint tid = 0; tid < num_triangles(); tid++)
    {
        double value = values[tid] ;

        //int_labels.push_back( (int) (value *factor));
        set_t_label(tid, (int) (value *10000));
    }

    //set_t_labels (int_labels);
#else

    std::cerr << "[ERROR] Cinolib required to run this operation. " << std::endl;
    exit(1);

#endif

}

DANI_INLINE
void EpsTrimesh::set_epsilon_triangle_label()
{
    std::vector<double> field;

    //std::map<float, std::pair<int, float *>> colormap;

    for (uint tid = 0; tid < num_triangles(); tid++)
    {
        field.push_back(t_epsilons.at(tid).radius);
    }

    set_t_labels (field);
}

/**
 * @brief EpsTrimesh::edge_local_to_global
 * @param v0
 * @param v1
 * @return
 */
DANI_INLINE
int EpsTrimesh::edge_local_to_global(const uint v0, const uint v1) const
{
    for (uint edg : vtx2edg.at(v0))
        if (edge_contains_vertex(edg, v1))
            return edg;

    return -1;
}

/**
 * @brief EpsTrimesh::triangle_local_to_global
 * @param v0
 * @param v1
 * @return
 */
DANI_INLINE
uint EpsTrimesh::triangle_local_to_global(const uint v0, const uint v1, const uint v2) const
{
    for (uint tid : vtx2tri.at(v0))
        if (triangle_contains_vertex(tid, v1) && triangle_contains_vertex(tid, v2))
            return tid;

    assert(false);
}

DANI_INLINE
int EpsTrimesh::add_triangle(const int vid0, const int vid1, const int vid2, const int scalar)
{
    assert(vid0 >= 0 && vid0 < num_vertices());
    assert(vid1 >= 0 && vid1 < num_vertices());
    assert(vid2 >= 0 && vid2 < num_vertices());

    int tid = num_triangles();
    //
    tris.push_back(vid0);
    tris.push_back(vid1);
    tris.push_back(vid2);
    //
    t_label.push_back(scalar);
    //
    tri2edg.push_back(std::vector<int>());
    tri2tri.push_back(std::vector<int>());
    //
    vtx2tri.at(vid0).push_back(tid);
    vtx2tri.at(vid1).push_back(tid);
    vtx2tri.at(vid2).push_back(tid);
    //
    ipair new_e[3]   = { unique_pair(vid0, vid1), unique_pair(vid1, vid2), unique_pair(vid2, vid0) };
    int   new_eid[3] = { -1, -1, -1 };
    for(int eid=0; eid<num_edges(); ++eid)
    {
        ipair e = unique_pair(edge_vertex_id(eid, 0), edge_vertex_id(eid, 1));
        for(int i=0; i<3; ++i) if (e == new_e[i]) new_eid[i] = eid;
    }
    //
    for(int i=0; i<3; ++i)
    {
        if (new_eid[i] == -1)
        {
            new_eid[i] = num_edges();
            edges.push_back(new_e[i].first);
            edges.push_back(new_e[i].second);
            edg2tri.push_back(std::vector<int>());

            vtx2edg.at(new_e[i].first).push_back(num_edges()-1);
            vtx2edg.at(new_e[i].second).push_back(num_edges()-1);

            vtx2vtx.at(new_e[i].first).push_back(new_e[i].second);
            vtx2vtx.at(new_e[i].second).push_back(new_e[i].first);

        }
        //
        for(int nbr : edg2tri.at(new_eid[i]))
        {
            tri2tri.at(nbr).push_back(tid);
            tri2tri.at(tid).push_back(nbr);
        }
        edg2tri.at(new_eid[i]).push_back(tid);
        tri2edg.at(tid).push_back(new_eid[i]);
    }
    //
    t_norm.push_back(0); //tnx
    t_norm.push_back(0); //tny
    t_norm.push_back(0); //tnz

#ifdef CINOLIB
    //
    update_t_normal(tid);
    update_v_normal(vid0);
    update_v_normal(vid1);
    update_v_normal(vid2);
#else

    std::cerr << "[WARNING] Cinolib required to update triangle and vertex normals." << std::endl;

#endif

    return tid;
}

DANI_INLINE
uint EpsTrimesh::split_edge(const uint eid, const vec3d point, std::vector<double> &radius, std::vector<vec3d> &centers, std::vector<vec3d> &antipodeans)
{
#ifdef CINOLIB

    uint e0 = edge_vertex_id(eid, 0);
    uint e1 = edge_vertex_id(eid, 1);

    uint vid = add_vertex(point);

    std::vector<int> rem_tris = adj_edg2tri(eid);

    for (uint tt : rem_tris)
    {
        uint opp_vid = vertex_opposite_to(tt, e0, e1);
        uint off_opp_vid = triangle_vertex_global_to_local(tt, opp_vid);

        uint t0 = add_triangle(vid, opp_vid, triangle_vertex_id(tt, (off_opp_vid+1) %3));
        uint t1 = add_triangle(vid, triangle_vertex_id(tt, (off_opp_vid+2) %3), opp_vid);

        radius.push_back(radius.at(tt));
        radius.push_back(radius.at(tt));

        centers.push_back(centers.at(tt));
        centers.push_back(centers.at(tt));

        antipodeans.push_back(antipodeans.at(tt));
        antipodeans.push_back(antipodeans.at(tt));
    }

    std::sort (rem_tris.begin(), rem_tris.end());
    std::reverse (rem_tris.begin(), rem_tris.end());

    for (uint tt : rem_tris)
    {
        tris.at(tt*3+0) = INT_MAX;
        tris.at(tt*3+1) = INT_MAX;
        tris.at(tt*3+2) = INT_MAX;

        remove_unreferenced_triangle(tt);

        std::swap(radius.at(radius.size()-1), radius.at(tt));
        radius.pop_back();

        std::swap(antipodeans.at(antipodeans.size()-1), antipodeans.at(tt));
        antipodeans.pop_back();

        std::swap(centers.at(centers.size()-1), centers.at(tt));
        centers.pop_back();
    }

    edges.at(eid*2+0) = INT_MAX;
    edges.at(eid*2+1) = INT_MAX;

    remove_unreferenced_edge(eid);

    update_adjacency();

    return vid;
#else

    std::cerr << "[ERROR] Cinolib required to run this operation." << std::endl;
    exit(1);

#endif
}

/**
 * @brief EpsTrimesh::split_triangle
 * @param tid
 * @param point
 * @return
 */
DANI_INLINE
uint EpsTrimesh::split_triangle(const uint tid, const vec3d point, std::vector<double> &radius, std::vector<vec3d> &centers, std::vector<vec3d> &antipodeans)
{

#ifdef CINOLIB

    uint nt = num_triangles();

    uint vid = add_vertex(point);

    double r = radius.at(tid);
    vec3d a = antipodeans.at(tid);
    vec3d c = centers.at(tid);

    uint v0 = triangle_vertex_id(tid, 0);
    uint v1 = triangle_vertex_id(tid, 1);
    uint v2 = triangle_vertex_id(tid, 2);

    uint t0 = add_triangle(vid, v2, v0);
    uint t1 = add_triangle(vid, v0, v1);
    uint t2 = add_triangle(vid, v1, v2);

    radius.resize(num_triangles());
    centers.resize(num_triangles());
    antipodeans.resize(num_triangles());

    radius.at(t0) = FLT_MAX;
    radius.at(t1) = FLT_MAX;
    radius.at(t2) = FLT_MAX;

    centers.at(t0) = {FLT_MAX};
    centers.at(t1) = {FLT_MAX};
    centers.at(t2) = {FLT_MAX};

    antipodeans.at(t0) = {FLT_MAX};
    antipodeans.at(t1) = {FLT_MAX};
    antipodeans.at(t2) = {FLT_MAX};

    tris.at(tid*3+0) = INT_MAX;
    tris.at(tid*3+1) = INT_MAX;
    tris.at(tid*3+2) = INT_MAX;

    if (tid == num_triangles()-1)
    {
        tris.resize(tris.size()-3);
        t_norm.resize(t_norm.size()-3);
        t_label.resize(t_label.size()-1);
        tri2edg.pop_back();
        tri2tri.pop_back();

        radius.pop_back();
        antipodeans.pop_back();
        centers.pop_back();
    }
    else
    {
        remove_unreferenced_triangle(tid);

        std::swap(radius.at(radius.size()-1), radius.at(tid));
        radius.pop_back();

        std::swap(antipodeans.at(antipodeans.size()-1), antipodeans.at(tid));
        antipodeans.pop_back();

        std::swap(centers.at(centers.size()-1), centers.at(tid));
        centers.pop_back();
    }

    update_adjacency();


    assert (num_triangles() == nt+2);
    assert (radius.size() == nt+2);
    assert (centers.size() == nt+2);
    assert (antipodeans.size() == nt+2);

    return vid;

#else

    std::cerr << "[ERROR] Cinolib required to run this operation." << std::endl;
    exit(1);

#endif
}

/**
 * @brief EpsTrimesh::split_on_sphere_center_and_tangent
 * @param tid
 */
DANI_INLINE
uint EpsTrimesh::split_on_sphere_center(const uint tid)
{
    Sphere *sphere = &t_epsilons.at(tid);

    if (sphere->center_pos == CENTERED_UNKNOWN)
        return UINT_MAX;

    if (sphere->radius == 0.0 || sphere->radius == FLT_MAX)
        return UINT_MAX;

    uint vid = UINT_MAX;

    // center
    if (sphere->center_pos == CENTERED_ON_EDGE)
    {
        //std::cout << "Sphere associated to triangle " << tid << " is centered on edge .. splitting .. " << std::endl;

        //vid = split_edge(sphere->center_id, sphere->center);
        vid = UINT_MAX;
    }
    else
    if (sphere->center_pos == CENTERED_ON_TRIANGLE)
    {
        //std::cout << "Sphere associated to triangle " << tid << " is centered on triangle .. splitting .. " << std::endl;

        vid = split_triangle(tid, sphere->center);
    }
    else
    if (sphere->center_pos == CENTERED_ON_VERTEX)
    {
        //std::cout << "Sphere associated to triangle " << tid << " is centered on vertex. " << std::endl;
        vid = sphere->center_id;
    }

//    if (vid != UINT_MAX)
//    {
//        for (uint tt : adj_vtx2tri(vid))
//        {
//            t_epsilons.at(tt).center_tri.first = tt;

//            t_epsilons.at(tt).center_pos = CENTERED_ON_VERTEX;
//            t_epsilons.at(tt).center_id = vid;
//        }
//    }

//    // Recursive

//    if (vid != UINT_MAX)
//    {
//        std::vector<int> ppoo = adj_vtx2tri(vid);

//        for (int tt : ppoo)
//        {
//            split_on_sphere_center(tt);
//        }
//    }

    return vid;

}

/**
 * @brief EpsTrimesh::split_on_sphere_center_and_tangent
 * @param tid
 */
DANI_INLINE
void EpsTrimesh::split_on_sphere_tangent(const uint tid)
{
    Sphere *sphere = &t_epsilons.at(tid);

    if (sphere->tangent_pos == TANGENT_UNKNOWN)
        return;

    if (sphere->radius == 0.0)
        return;

    if (sphere->center_pos != CENTERED_ON_VERTEX)
        assert(false);

    uint vid = UINT_MAX;

    // center
    if (sphere->tangent_pos == TANGENT_ON_EDGE)
    {
        std::cout << ">>>>Sphere associated to triangle " << tid << " is tangent on edge .. splitting .. " << std::endl;

        vid = split_edge(sphere->tangent_id, sphere->tangent);

        for (uint tt : adj_vtx2tri(vid))
            t_epsilons.at(tt).center_tri.first = tt;
    }
    else
    if (sphere->tangent_pos == TANGENT_ON_TRIANGLE)
    {
        std::cout << ">>>>Sphere associated to triangle " << tid << " is tangent on triangle .. splitting .. " << std::endl;

        vid = split_triangle(sphere->tangent_tri.first, sphere->tangent);

        for (uint tt : adj_vtx2tri(vid))
            t_epsilons.at(tt).center_tri.first = tt;
    }
    else
    if (sphere->tangent_pos == TANGENT_ON_VERTEX)
    {
        std::cout << ">>>>Sphere associated to triangle " << tid << " is tangent on vertex. " << std::endl;
    }

    // Recursive

    if (vid != UINT_MAX)
    {
        for (uint tt=0; tt < num_triangles(); tt++)
        {
            if (t_epsilons.at(tt).tangent_pos == TANGENT_ON_EDGE &&
                    std::find(adj_vtx2edg(vid).begin(), adj_vtx2edg(vid).end(), t_epsilons.at(tt).tangent_id) != adj_vtx2edg(vid).end())
                split_on_sphere_tangent(tt);
            else
            if (t_epsilons.at(tt).tangent_pos == TANGENT_ON_TRIANGLE &&
                    std::find(adj_vtx2tri(vid).begin(), adj_vtx2tri(vid).end(), t_epsilons.at(tt).tangent_tri.first) != adj_vtx2tri(vid).end())
                split_on_sphere_tangent(tt);
        }
    }

    return;

}

/**
 * @brief EpsTrimesh::update_center_of_sphere
 * @param sphere
 */
DANI_INLINE
void EpsTrimesh::update_center_of_sphere(Sphere &sphere)
{
    vec3d center = sphere.center;

    uint tid = sphere.center_tri.first;

    PointPosition pp = is_point_on_triangle(center, triangle_vertex(tid, 0), triangle_vertex(tid, 1), triangle_vertex(tid,2));

    if (pp == EXTERNAL_POINT)
    {
        t_epsilons.at(tid) = Sphere();
        return;
    }

    if (pp == POINT_TRIANGLE_P0)
    {
        t_epsilons.at(tid).center_pos = CENTERED_ON_VERTEX;
        t_epsilons.at(tid).center_id = triangle_vertex_id(tid, 0);
        return;
    }

    if (pp == POINT_TRIANGLE_P1)
    {
        t_epsilons.at(tid).center_pos = CENTERED_ON_VERTEX;
        t_epsilons.at(tid).center_id = triangle_vertex_id(tid, 1);
        return;
    }

    if (pp == POINT_TRIANGLE_P2)
    {
        t_epsilons.at(tid).center_pos = CENTERED_ON_VERTEX;
        t_epsilons.at(tid).center_id = triangle_vertex_id(tid, 2);
        return;
    }

    if (pp == POINT_TRIANGLE_E0)
    {
        t_epsilons.at(tid).center_pos = CENTERED_ON_EDGE;
        t_epsilons.at(tid).center_id = edge_local_to_global(triangle_vertex_id(tid, 0), triangle_vertex_id(tid, 1));
        return;
    }

    if (pp == POINT_TRIANGLE_E1)
    {
        t_epsilons.at(tid).center_pos = CENTERED_ON_EDGE;
        t_epsilons.at(tid).center_id = edge_local_to_global(triangle_vertex_id(tid, 1), triangle_vertex_id(tid, 2));
        return;
    }

    if (pp == POINT_TRIANGLE_E2)
    {
        t_epsilons.at(tid).center_pos = CENTERED_ON_EDGE;
        t_epsilons.at(tid).center_id = edge_local_to_global(triangle_vertex_id(tid, 2), triangle_vertex_id(tid, 0));
        return;
    }

    t_epsilons.at(tid).center_pos = CENTERED_ON_TRIANGLE;
}

/**
 * @brief EpsTrimesh::update_t_epsilon_tangent_at
 * @param tid
 * @param added_tri
 */
DANI_INLINE
void EpsTrimesh::update_t_epsilon_tangent_at(const uint tid, const std::vector<int> &added_tris)
{
    vec3d tangent = t_epsilons.at(tid).tangent;

    for (uint tt : added_tris)
    {
        PointPosition pp = is_point_on_triangle(tangent, triangle_vertex(tt, 0), triangle_vertex(tt, 1), triangle_vertex(tt, 2));

        if (pp == EXTERNAL_POINT)
            continue;

        if (pp == POINT_TRIANGLE_P0)
        {
            t_epsilons.at(tid).tangent_pos = TANGENT_ON_VERTEX;
            t_epsilons.at(tid).tangent_id = triangle_vertex_id(tt, 0);
            return;
        }

        if (pp == POINT_TRIANGLE_P1)
        {
            t_epsilons.at(tid).tangent_pos = TANGENT_ON_VERTEX;
            t_epsilons.at(tid).tangent_id = triangle_vertex_id(tt, 1);
            return;
        }

        if (pp == POINT_TRIANGLE_P2)
        {
            t_epsilons.at(tid).tangent_pos = TANGENT_ON_VERTEX;
            t_epsilons.at(tid).tangent_id = triangle_vertex_id(tt, 2);
            return;
        }

        if (pp == POINT_TRIANGLE_E0)
        {
            uint eid = edge_local_to_global(triangle_vertex_id(tt, 0), triangle_vertex_id(tt, 1));

            t_epsilons.at(tid).tangent_pos = TANGENT_ON_EDGE;
            t_epsilons.at(tid).tangent_id = eid;

            //e_tangencies.at(eid).push_back(tid);

            return;
        }

        if (pp == POINT_TRIANGLE_E1)
        {
            uint eid = edge_local_to_global(triangle_vertex_id(tt, 1), triangle_vertex_id(tt, 2));

            t_epsilons.at(tid).tangent_pos = TANGENT_ON_EDGE;
            t_epsilons.at(tid).tangent_id = eid;

            //e_tangencies.at(eid).push_back(tid);

            return;
        }

        if (pp == POINT_TRIANGLE_E2)
        {
            uint eid = edge_local_to_global(triangle_vertex_id(tt, 2), triangle_vertex_id(tt, 0));

            t_epsilons.at(tid).tangent_pos = TANGENT_ON_EDGE;
            t_epsilons.at(tid).tangent_id = eid;

            //e_tangencies.at(eid).push_back(tid);

            return;
        }

        t_epsilons.at(tid).tangent_pos = TANGENT_ON_TRIANGLE;
        t_epsilons.at(tid).tangent_tri.first = tt;

        //t_tangencies.at(tt).push_back(tid);

        return;
    }

    t_epsilons.at(tid) = Sphere();
}


}
