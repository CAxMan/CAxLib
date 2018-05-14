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

#include "../danilib/include/eps2Dshape.h"

//#include <geometry/vec3.h>
#include <triangle_wrap.h>

//#include <limits.h>

namespace danilib {

DANI_INLINE
Epsilon2DShape::Epsilon2DShape (const char *filename)
{
//    epsilon = FLT_MAX;

    polygon.load(filename);

    std::vector<double> coords;
    std::vector<uint> tris;

    std::cout << "Triangulating ... ";

    cinolib::triangle_wrap(polygon.in_coords, polygon.in_segms, std::vector<double>(), 0.0, "BcDpqYYVz", coords, tris);

    tin = cinolib::Trimesh(coords, tris);

    std::cout << "COMPLETED." << std::endl;

    std::string output = filename;
    output += ".off";

    tin.save (output.c_str());

//    v_labels = std::vector<bool> (tin.num_vertices(), false);
//    e_labels = std::vector<bool> (tin.num_edges(), false);

//    v_epsilon = std::vector<double> (tin.num_vertices(), FLT_MAX);

//    std::cout << "Vertex mapping ... ";

//    set_v_map_ptp();

//    std::cout << "COMPLETED. " << std::endl << "Edge mapping ... ";

//    set_e_map_ptp();

//    std::cout << "COMPLETED. " << std::endl;

//    is_epsilon_known = false;

//    //tin.save("/home/daniela/Scrivania/ch.off");

}

}

//DANI_INLINE
//double Epsilon2DShape::compute_epsilon ()
//{
//    for (uint vid_p = 0; vid_p < polygon.num_points(); vid_p++)
//    {
//        uint vid = v_map_pt.at(vid_p);

//        bool eps_vid = false;
//        Sphere sphere;

//        sphere.cvid   = vid;
//        sphere.center = tin.vertex(vid);
//        sphere.dimension = 2;

//        TangentPoint p;
//        p.coords = tin.vertex(vid);
//        p.type = VERTEX;
//        p.vid = vid;

//        v_labels.at(vid) = true;

//        std::multimap<double, TangentPoint> ext_link;

//        //std::cout << " Link of vertex " << vid << " ... " << std::endl;

//        update_ext_link(vid, ext_link, p);

//        while (eps_vid == false && ext_link.size() > 0)
//        {
//            // get the nearest TangentPoint in the link
//            TangentPoint pvid = (*ext_link.begin()).second;
//            double pdist = (*ext_link.begin()).first;
//            ext_link.erase(ext_link.begin());

//            // check if it is on the boundary
//            if ((pvid.type == VERTEX && v_map_tp.at(pvid.vid).first != EXTRA_POINT) ||
//                (pvid.type == E_PROJECTION && e_map_tp.at(pvid.eid).first != EXTRA_EDGE))
//            {
//                // check path inside the sphere from vid to p.coords

//                sphere.radius = pdist;

//                bool topology_check = pvid.type == E_PROJECTION ? true : !is_path_inside_sphere(pvid, sphere);

//                if (topology_check) // path is external
//                {
//                    eps_vid = true;

//                    v_epsilon.at(vid) = pdist;

//                    if (pdist < epsilon)
//                    {
//                        epsilon = pdist;
//                        epsilon_center_tin = v_map_pt.at(vid);
//                    }
//                }
//            }

//            if (eps_vid == false)
//            {
//                update_ext_link(vid, ext_link, pvid);

//                //std::cout << " ... COMPLETED" << std::endl;
//            }
//        }

//        std::cout << " Epsilon at vertex " << vid << " : " << v_epsilon.at(vid) << std::endl << std::endl;

//        // clear
//        v_labels.clear();
//        e_labels.clear();

//        v_labels = std::vector<bool> (tin.num_vertices(), false);
//        e_labels = std::vector<bool> (tin.num_edges(), false);
//    }

//    std::cout << " EPSILON : " << epsilon << std::endl;

//    is_epsilon_known = true;

//    return epsilon;
//}

//DANI_INLINE
//void Epsilon2DShape::update_ext_link(const uint vid, std::multimap<double, TangentPoint> &link, const TangentPoint &p)
//{
//    if (p.type == VERTEX)
//    {
//        //std::cout << " ... adjacent vertices ... " << std::endl;

//        // adjacent vertices
//        for (uint adj_vid : tin.adj_vtx2vtx(p.vid))
//        {
//            if (v_labels.at(adj_vid) == true)
//                continue;

//            TangentPoint ptmp;
//            ptmp.coords = tin.vertex(adj_vid);
//            ptmp.vid = adj_vid;
//            ptmp.type = PType::VERTEX;

//            double dist = tin.vertex(vid).dist(ptmp.coords);

//            link.insert(std::pair<double, TangentPoint> (dist, ptmp));

//            v_labels.at(ptmp.vid) = true;
//        }

//        //std::cout << " ... COMPLETED" << std::endl;
//        //std::cout << " ... adjacent edegs ... " << std::endl;

//        for (uint tid : tin.adj_vtx2tri(p.vid))
//        {
//            uint eid = tin.edge_opposite_to(tid, p.vid);

//            if (e_labels.at(eid) == true)
//                continue;

//            TangentPoint ptmp (tin, vid, eid);

//            double dist = tin.vertex(vid).dist(ptmp.coords);

//            if(dist > 0)
//                link.insert(std::pair<double, TangentPoint> (dist, ptmp));

//            if (ptmp.type == VERTEX)
//                v_labels.at(ptmp.vid) = true;

//            e_labels.at(eid) = true;

//        }
//    }
//    else    // TangentPoint p is internal to some edge
//    {
//        for (uint tid : tin.adj_edg2tri(p.eid))
//        {
//            for (uint t_eid : tin.adj_tri2edg(tid))
//            {
//                if (e_labels.at(t_eid) == true)
//                    continue;

//                if (t_eid == p.eid)
//                    continue;

//                TangentPoint ptmp (tin, vid, t_eid);

//                double dist = tin.vertex(vid).dist(ptmp.coords);

//                if(dist > 0)
//                    link.insert(std::pair<double, TangentPoint> (dist, ptmp));

//                if (ptmp.type == VERTEX)
//                    v_labels.at(ptmp.vid) = true;

//                e_labels.at(t_eid) = true;

//            }
//        }
//    }
//}

//DANI_INLINE
//bool Epsilon2DShape::is_path_inside_sphere(const TangentPoint p, const Sphere sph)
//{
//    std::vector<bool> ee_labels (tin.num_edges(), false);
//    std::multimap<uint, std::pair<uint,uint>> edges_on_path;

//    uint path_counter = 0;

//    for (uint eid : tin.adj_vtx2edg(sph.cvid))
//    {
//        if (e_map_tp.at(eid).first != EXTRA_EDGE)
//        {
//            edges_on_path.insert(std::pair<uint, std::pair<uint, uint>> (eid, std::pair<uint,uint>(sph.cvid, tin.vertex_opposite_to(eid, sph.cvid))));

//            ee_labels.at(eid) = true;
//        }
//    }

//    while (edges_on_path.size() > 0)
//    {
//        uint eid = (*edges_on_path.begin()).first;
//        std::pair<uint, uint> endpoints = (*edges_on_path.begin()).second;
//        edges_on_path.erase(edges_on_path.begin());

//        if (p.type == VERTEX)
//        {
//            if (endpoints.second == p.vid)
//            {
//                path_counter++;
//                continue;
//            }

//            if (tin.vertex(endpoints.second).dist(sph.center) <= sph.radius)
//            {
//                for (uint ee : tin.adj_vtx2edg(endpoints.second))
//                {
//                    if (e_map_tp.at(ee).first != EXTRA_EDGE && ee_labels.at(ee) == false)
//                    {
//                        edges_on_path.insert(std::pair<uint, std::pair<uint, uint>> (ee, std::pair<uint,uint>(endpoints.second, tin.vertex_opposite_to(ee, endpoints.second))));
//                        ee_labels.at(ee) = true;
//                    }
//                }
//            }
//        }
//        else    // E_PROJECTION
//        {
//            //if (cinolib::distance_to_segment(p.coords, tin.edge_vertex(eid, 0), tin.edge_vertex(eid, 1)) == 0)
//            //if (std::abs(tin.edge_vertex(eid, 0).dist(tin.edge_vertex(eid, 1)) - p.coords.dist(tin.edge_vertex(eid, 0)) - p.coords.dist(tin.edge_vertex(eid, 1))) <= 0.0000000001)
//            if (p.eid == eid)
//            {
//                path_counter++;
//                continue;
//            }

//            if (tin.vertex(endpoints.second).dist(sph.center) <= sph.radius)
//            {
//                for (uint ee : tin.adj_vtx2edg(endpoints.second))
//                {
//                    if (e_map_tp.at(ee).first != EXTRA_EDGE && ee_labels.at(ee) == false)
//                    {
//                        edges_on_path.insert(std::pair<uint, std::pair<uint, uint>> (ee, std::pair<uint, uint>(endpoints.second, tin.vertex_opposite_to(ee, endpoints.second))));
//                        ee_labels.at(ee) = true;
//                    }
//                }
//            }
//        }
//    }

//    if (path_counter == 1)
//        return true;

//    return false;
//}

//DANI_INLINE
//void Epsilon2DShape::set_v_map_ptp()
//{
//    v_map_pt = std::vector<uint> (polygon.num_points());
//    v_map_tp = std::vector<std::pair<VertexType, uint>> (tin.num_vertices(), std::pair<VertexType, uint> (EXTRA_POINT, UINT_MAX));

//    uint count_original = 0;

//    for (uint vid_p = 0; vid_p < polygon.num_points(); vid_p++)
//    {
//        for (uint vid_t = 0; vid_t < tin.num_vertices(); vid_t++)
//        {
//            if (tin.vertex(vid_t).x() == polygon.in_coords.at(vid_p*2) &&
//                tin.vertex(vid_t).y() == polygon.in_coords.at(vid_p*2+1) )
//            {
//                v_map_pt.at(vid_p) = vid_t;
//                v_map_tp.at(vid_t) = std::pair<VertexType, uint> (ORIGINAL_BOUNDARY_POINT, vid_p);

//                count_original++;

//                break;
//            }
//        }
//    }

//    std::cout << "Original vertices : " << count_original << " / " << polygon.num_points() << std::endl;

//    if (count_original != polygon.num_points())
//    {
//        std::cerr << "[ERROR] Impossible mapping between polygon vertices and mesh vertices." << std::endl;
//        exit(1);
//    }

//}

//DANI_INLINE
//void Epsilon2DShape::set_e_map_ptp()
//{
//    e_map_pt = std::vector<uint> (polygon.num_edges());
//    e_map_tp = std::vector<std::pair<EdgeType, uint>> (tin.num_edges(), std::pair<EdgeType, uint> (EXTRA_EDGE, UINT_MAX));

//    uint count_original = 0;

//    for (uint eid_p = 0; eid_p < polygon.num_edges(); eid_p++)
//    {
//        for (uint eid_t = 0; eid_t < tin.num_edges(); eid_t++)
//        {
//            if ((tin.edge_vertex_id(eid_t, 0) == v_map_pt.at(polygon.in_segms.at(eid_p*2)) &&
//                 tin.edge_vertex_id(eid_t, 1) == v_map_pt.at(polygon.in_segms.at(eid_p*2+1))) ||
//                 (tin.edge_vertex_id(eid_t, 1) == v_map_pt.at(polygon.in_segms.at(eid_p*2)) &&
//                  tin.edge_vertex_id(eid_t, 0) == v_map_pt.at(polygon.in_segms.at(eid_p*2+1))))
//            {
//                e_map_pt.at(eid_p) = eid_t;
//                e_map_tp.at(eid_t) = std::pair<EdgeType, uint> (ORIGINAL_BOUNDARY_EDGE, eid_p);

//                count_original++;

//                break;
//            }
//        }
//    }

//    std::cout << "Original edges : " << count_original  << " / " << polygon.num_edges() << std::endl;

//    if (count_original != polygon.num_edges())
//    {
//        std::cerr << "[ERROR] Impossible mapping between polygon edges and mesh edges." << std::endl;
//        exit(1);
//    }
//}

//}
