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

#include "include/eps3Dshape.h"


namespace danilib
{

/**
 * @brief Epsilon3DShape::repair
 */
DANI_INLINE
void Epsilon3DShape::repair()
{
    std::cout << "Thickening ... " << std::endl;

    time_t begin, end;

    time(&begin);

    // creating vertex scalar field

    //min_epsilon_to_vertices();

    // smoothing scalar field

    tin.update_normals();

    std::vector<double> smoothing (tin.num_vertices());

    for (uint vid = 0; vid < tin.num_vertices(); vid++)
    {

        if (epsilons_v_map.at(vid) > max_epsilon || epsilons_v_map.at(vid) == 0.0)
           smoothing.at(vid) = 0.0;
        else
        {
            double value =  epsilons_v_map.at(vid);

            for  (uint tid : tin.adj_vtx2tri(vid))
            {
                if (tin.t_epsilons.at(tid).radius < value)
                    value = tin.t_epsilons.at(tid).radius;
            }

            smoothing.at(vid) = (max_epsilon - value) / 2 ;
        }
    }

    // displacement computation

    std::vector<vec3d> thickening (tin.num_vertices());

    for (uint vid = 0; vid < tin.num_vertices(); vid++)
    {
        thickening.at(vid) = tin.vertex(vid) + ((tin.vertex_normal(vid) / tin.vertex_normal(vid).length()) * smoothing.at(vid));
    }

    // actual displacement

    for (uint vid = 0; vid < tin.num_vertices(); vid++)
    {
        vec3d avg = thickening.at(vid);
        uint counter = 0;

        for (uint vv : tin.adj_vtx2vtx(vid))
        {
            if (smoothing.at(vv) > 0.0)
            {
                avg += thickening.at(vv);
                counter++;
            }
        }

        avg /= counter +1;

        vec3d displacement = (avg - tin.vertex(vid)) / (avg - tin.vertex(vid)).length();

        if (smoothing.at(vid) > 0.0)
            tin.set_vertex(vid, tin.vertex(vid) + displacement * smoothing.at(vid));
    }

    time(&end);

    time_thickening = difftime(end, begin);

    std::cout << "Completed." << std::endl;

}

/**
 * @brief Epsilon3DShape::epsilons_to_vertices
 */
DANI_INLINE
void Epsilon3DShape::epsilons_to_vertices()
{
    uint nt = tin.num_triangles();

    // center on vertex
    for (uint tid = 0; tid < nt; tid++)
    {
        tin.split_on_sphere_center(tid);
    }

//    // check
//    for (uint tid = 0; tid < tin.num_triangles(); tid++)
//    {
//        Sphere *sphere = &tin.t_epsilons.at(tid);

//        if (sphere->radius > 0.0)
//            if (sphere->center_pos != CENTERED_ON_VERTEX && sphere->center_pos != CENTERED_UNKNOWN)
//                assert(false);
//    }

    // tangent on vertex
    for (uint tid = 0; tid < tin.num_triangles(); tid++)
    {
        tin.split_on_sphere_tangent(tid);
    }

//    // check
//    for (uint tid = 0; tid < tin.num_triangles(); tid++)
//    {
//        Sphere *sphere = &tin.t_epsilons.at(tid);

//        if (sphere->radius > 0.0)
//            if (sphere->tangent_pos != TANGENT_ON_VERTEX && sphere->tangent_pos != TANGENT_UNKNOWN)
//                assert(false);
//    }

//    tin.v_epsilons.resize(tin.num_vertices());

//    for (uint tid = 0; tid < tin.num_triangles(); tid++)
//    {
//        Sphere *sphere = &tin.t_epsilons.at(tid);

//        if (sphere->center_pos == CENTERED_ON_VERTEX)
//        {
//            Sphere tmp = *sphere;
//            tin.v_epsilons.at(sphere->center_id) = tmp;
//        }
//    }

}

/**
 * @brief Epsilon3DShape::min_epsilon_to_vertices
 */
DANI_INLINE
void Epsilon3DShape::min_epsilon_to_vertices ()
{
    // compute vertex scalar field

//    tin.v_epsilons.resize(tin.num_vertices());

//    for (uint vid = 0; vid < tin.num_vertices(); vid++)
//    {
//        double min_epsilon = FLT_MAX;
//        Sphere *min_sphere;

//        for (uint tt : tin.adj_vtx2tri(vid))
//        {
//            Sphere *sphere = &tin.t_epsilons.at(tt);

//            if (sphere->radius > 0.0 && sphere->radius < min_epsilon)
//            {
//                min_epsilon = sphere->radius;
//                min_sphere = sphere;
//            }
//        }

//        if (min_epsilon == FLT_MAX)
//            tin.set_vertex_u_text(vid, 0.0);
//        else
//            tin.set_vertex_u_text(vid, (max_epsilon - min_sphere->radius) / 2);
//    }
}

/**
 * @brief Epsilon3DShape::thickening
 */
DANI_INLINE
void Epsilon3DShape::thickening ()
{
//    std::vector<bool> is_center (tin.num_vertices(), false);
//    std::vector<bool> moved_vertices (tin.num_vertices(), false);

//    SpherePriorityQueue queue;

//    for (uint vid = 0; vid < tin.num_vertices(); vid++)
//    {
//        Sphere sphere = tin.v_epsilons.at(vid);

//        if (sphere.center_pos == CENTERED_UNKNOWN)
//            continue;

//        if (sphere.tangent_pos == TANGENT_UNKNOWN)
//            continue;

//        if (sphere.radius == 0.0 || sphere.radius > max_epsilon)
//            continue;

//        if (sphere.center_pos != CENTERED_ON_VERTEX)
//            assert(false);

//        if (sphere.tangent_pos != TANGENT_ON_VERTEX)
//            continue;

//        queue.insert(sphere);
//    }

//    while (queue.size() > 0)
//    {
//        Sphere sphere = *queue.begin();
//        queue.erase(queue.begin());

//        std::cout << " Thickening vertex " << sphere.center_id << std::endl; //" with scale factor " << scale_factor << std::endl;

//        uint vid_c = sphere.center_id;
//        uint vid_t = sphere.tangent_id;

//        vec3d s = sphere.tangent - sphere.center;

//        //vec3d radius_center_old = (sphere.center + sphere.tangent) / 2;
//        //double scale_factor = max_epsilon / sphere.radius;

//        double delta = ((max_epsilon - s.length()));

//        Sphere tangent_sphere = tin.v_epsilons.at(vid_t);

//        if (tangent_sphere.center_pos == CENTERED_ON_VERTEX && tangent_sphere.center_id == vid_t)
//        {
//            if (std::abs(tangent_sphere.radius - sphere.radius) < thresh)
//            {
//                delta /= 2;
//            }
//        }

//        if (moved_vertices.at(vid_c) == false)
//        {
//            //tin.set_vertex(vid_c, ((tin.vertex(vid_c) - radius_center_old) * scale_factor) + radius_center_old);

//            tin.set_vertex(vid_c, tin.vertex(vid_c) - ((s / s.length()) * delta));

//            moved_vertices.at(vid_c) = true;
//            is_center.at(vid_c) = true;
//        }
//    }

//    // Smoothing

//    for (uint vid = 0; vid < tin.num_vertices(); vid++)
//    {
//        Sphere sphere = tin.v_epsilons.at(vid);

//        if (sphere.center_pos == CENTERED_UNKNOWN)
//            continue;

//        if (sphere.tangent_pos == TANGENT_UNKNOWN)
//            continue;

//        if (sphere.radius == 0.0 || sphere.radius > max_epsilon)
//            continue;

//        if (sphere.center_pos != CENTERED_ON_VERTEX)
//            assert(false);

//        if (sphere.tangent_pos != TANGENT_ON_VERTEX)
//            continue;

//        queue.insert(sphere);
//    }

//    std::cout << std::endl;
//    std::cout << "Smooting ... " << std::endl;

//    while (queue.size() > 0)
//    {
//        Sphere sphere = *queue.begin();
//        queue.erase(queue.begin());

//        uint vid_c = sphere.center_id;
//        uint vid_t = sphere.tangent_id;

//        vec3d original_center = sphere.center;
//        vec3d original_tangent = sphere.tangent;

//        vec3d s = original_tangent - original_center;

//        std::set<uint> center_n_rings;

//        double delta = (((max_epsilon - s.length())));

//        Sphere tangent_sphere = tin.v_epsilons.at(vid_t);

//        if (tangent_sphere.center_pos == CENTERED_ON_VERTEX && tangent_sphere.center_id == vid_t)
//        {
//            if (std::abs(tangent_sphere.radius - sphere.radius) < thresh)
//            {
//                delta /= 2;
//            }
//        }

//        //center neighborhood

//        assert (tin.adj_vtx2vtx(vid_c).size() > 0);

//        for (uint vv : tin.adj_vtx2vtx(vid_c))
//        {
//            if (!is_center.at(vv) && moved_vertices.at(vv) == false)  //&& v_visited.at(vv) == false
//                    //&& original_center.dist(tin.vertex(vv)) <= sphere.radius)
//            {
//                center_n_rings.insert(vv);
//            }
//        }

//        while (center_n_rings.size() > 0)
//        {
//            uint vid = *center_n_rings.begin();
//            center_n_rings.erase(center_n_rings.begin());

//            std::cout << " Thickening/smoothing vertex " << vid << std::endl; //" with scale factor " << scale_factor << std::endl;

//            tin.set_vertex(vid, tin.vertex(vid) - ((tin.vertex_normal(vid) / tin.vertex_normal(vid).length()) * delta));
//            moved_vertices.at(vid) = true;

////            for (uint vv : tin.adj_vtx2vtx(vid))
////            {
////                if (!is_center.at(vv) && v_visited.at(vv) == false &&
////                        original_center.dist(tin.vertex(vv)) <= sphere.radius )
////                {
////                    center_n_rings.insert(vv);
////                }
////            }
//        }
//    }
}


}
