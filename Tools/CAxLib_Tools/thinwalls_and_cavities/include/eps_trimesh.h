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

#ifndef EPSTRIMESH_H
#define EPSTRIMESH_H

#ifdef CAXLIB
#include "caxlib/trimesh/trimesh.h"
using namespace caxlib;
#else
#ifdef CINOLIB
#include "meshes/trimesh/trimesh.h"
using namespace cinolib;
#endif
#endif

#include "include/sphere.h"

namespace danilib
{

typedef std::set<Sphere, Sphere::LessThan> SpherePriorityQueue;


class EpsTrimesh : public Trimesh
{

public:

    std::vector<double> v_offsets;
    std::vector<Sphere> t_epsilons;

    SpherePriorityQueue sorted_t_epsilons;

    std::vector<bool> scaled_triangles;


    //
    EpsTrimesh () : Trimesh () {}
    EpsTrimesh (const char * filename) : Trimesh (filename)
    {
        t_epsilons.resize(num_triangles(), Sphere ());

        scaled_triangles = std::vector<bool> (num_triangles(), false);
    }


    void set_t_label       (const int tid, const int i) { t_label.at(tid) = i; }
    void set_t_labels      (const std::vector<int> &labels);
    void set_t_labels      (const std::vector<double> labels);
    void set_epsilon_triangle_label ();

    int edge_local_to_global (const uint v0, const uint v1) const;
    uint triangle_local_to_global (const uint v0, const uint v1, const uint v2) const;

    virtual int add_triangle(const int vid0, const int vid1, const int vid2, const int scalar = 0);

    uint split_edge (const uint eid, const vec3d point);
    uint split_edge (const uint eid, const vec3d point, std::vector<double> &radius, std::vector<vec3d> &centers, std::vector<vec3d> &antipodeans);
    uint split_triangle (const uint tid, const vec3d point);
    uint split_triangle(const uint tid, const vec3d point, std::vector<double> &radius, std::vector<vec3d> &centers, std::vector<vec3d> &antipodeans);
    uint split_on_sphere_center (const uint tid);
    void split_on_sphere_tangent (const uint tid);

    void remove_edge (const uint eid);
    void remove_triangle (const uint tid);

    void update_center_of_sphere (Sphere &sphere);
    void update_t_epsilon_tangent_at (const uint tid, const std::vector<int> &added_tri);

};

}

#ifndef  DANI_STATIC_LIB
#include "src/eps_trimesh.cpp"
#endif

#endif
