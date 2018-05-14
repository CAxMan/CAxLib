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


/*
 * TETGEN available flags:
 * (from http://wias-berlin.de/software/tetgen/1.5/doc/manual/manual005.html#sec35)
 *
 * -p	Tetrahedralizes a piecewise linear complex (PLC).
 * -Y	Preserves the input surface mesh (does not modify it).
 * -r	Reconstructs a previously generated mesh.
 * -q	Refines mesh (to improve mesh quality).
 * -R	Mesh coarsening (to reduce the mesh elements).
 * -A	Assigns attributes to tetrahedra in different regions.
 * -a	Applies a maximum tetrahedron volume constraint.
 * -m	Applies a mesh sizing function.
 * -i	Inserts a list of additional TangentPoints.
 * -O	Specifies the level of mesh optimization.
 * -S	Specifies maximum number of added TangentPoints.
 * -T	Sets a tolerance for coplanar test (default 10Ã¢Ë†â€™8).
 * -X	Suppresses use of exact arithmetic.
 * -M	No merge of coplanar facets or very close vertices.
 * -w	Generates weighted Delaunay (regular) triangulation.
 * -c	Retains the convex hull of the PLC.
 * -d	Detects self-intersections of facets of the PLC.
 * -z	Numbers all output items starting from zero.
 * -f	Outputs all faces to .face file.
 * -e	Outputs all edges to .edge file.
 * -n	Outputs tetrahedra neighbors to .neigh file.
 * -v	Outputs Voronoi diagram to files.
 * -g	Outputs mesh to .mesh file for viewing by Medit.
 * -k	Outputs mesh to .vtk file for viewing by Paraview.
 * -J	No jettison of unused vertices from output .node file.
 * -B	Suppresses output of boundary information.
 * -N	Suppresses output of .node file.
 * -E	Suppresses output of .ele file.
 * -F	Suppresses output of .face and .edge file.
 * -I	Suppresses mesh iteration numbers.
 * -C	Checks the consistency of the final mesh.
 * -Q	Quiet: No terminal output except errors.
 * -V	Verbose: Detailed information, more terminal output.
 * -h	Help: A brief instruction for using TetGen.
 *
 * */


#include "include/eps3Dshape.h"
#include "include/sphere.h"
#include "include/wrapper_tetgen.h"

#ifdef GEOMETRICTOOLS

    #include "include/wrapper_gte.h"

    #include "Mathematics/GteHyperplane.h"
    #include "Mathematics/GteLine.h"
    #include "Mathematics/GteIntrLine3Triangle3.h"
    #include "Mathematics/GteIntrPlane3Plane3.h"
    #include "Mathematics/GteIntrPlane3Triangle3.h"

#endif

#ifdef CINOLIB
    #include "meshes/trimesh/drawable_trimesh.h"
#else
#ifdef CAXLIB
    #include "caxlib/plane.h"
#endif

#endif

namespace danilib
{

void RGBtoHSV (const float * color, float &h, float &s, float &v)
{
    float r,g,b;

    r = color[0];
    g = color[1];
    b = color[2];

    h = FLT_MAX;
    s = FLT_MAX;
    v = FLT_MAX;

    float min_color = std::min( r, std::min(g, b) );
    float max_color = std::max( r, std::max(g, b) );

    v = max_color;
    float delta = max_color - min_color;

    if( max_color != 0.0 )
        s = delta / max_color;        // s
    else
    {        // r = g = b = 0        // s = 0, v is undefined
        s = 0;
        h = -1;

        return;
    }

    if( r == max_color )
        h = ( g - b ) / delta;      // between yellow & magenta
    else
    if( g == max_color )
        h = 2 + ( b - r ) / delta;  // between cyan & yellow
    else
        h = 4 + ( r - g ) / delta;  // between magenta & cyan
    h *= 60;                // degrees

    if( h < 0 )
        h += 360;
    if ( std::isnan(h) )
        h = 0;
}

void HSVtoRGB  (const float * color, float &r, float &g, float &b)
{
        int i;
        float h,s,v;

        r = FLT_MAX;
        g = FLT_MAX;
        b = FLT_MAX;

        h= color[0];
        s= color[1];
        v= color[2];

        if(s == 0.0 ) {
            // achromatic (grey)
            r = g = b = v;

            return;
        }

        h /= 60;            // sector 0 to 5
        i = std::floor( h );

        float f = h - i;          // factorial part of h
        float p = v * ( 1 - s );
        float q = v * ( 1 - s * f );
        float t = v * ( 1 - s * ( 1 - f ) );

        switch( i ) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;

            default:        // case 5:
                r = v;
                g = p;
                b = q;
                break;
        }
        return ;
    }

DANI_INLINE
danilib::Sphere min_sphere (const danilib::Sphere &a, const danilib::Sphere &b)
{
    if (a < b)
        return a;

    return b;
}


DANI_INLINE
Epsilon3DShape::Epsilon3DShape (const char *filename)
{
    tin = EpsTrimesh(filename);

    epsilon = FLT_MAX;

    t_spheres = std::vector<std::vector<Sphere>> (tin.num_triangles());

    epsilons_v_map = std::vector<double> (tin.num_vertices(), FLT_MAX);
    epsilons_antipoeads = std::vector<vec3d> (tin.num_vertices(), vec3d (FLT_MAX));
    epsilons_antipoeads_type = std::vector<TangentPosition> (tin.num_vertices(), TANGENT_UNKNOWN);
    epsilons_antipoeads_id = std::vector<uint> (tin.num_vertices(), UINT_MAX);
}

DANI_INLINE
void Epsilon3DShape::compute_epsilon()
{

    time_t begin, end;

    if (tetmesh.num_vertices() == 0)
    {
        time(&begin);
        create_tetmesh();
        time(&end);

        time_tetgen = difftime(end, begin);
    }


    //are_dihedral_angles_greater_than(M_PI);
    //are_solid_vertex_angle_greater_than(M_PI);

    time(&begin);

    if (true)
        compute_all_epsilons();
    else
    {
        std::cout << "Creating global priority queue ... " << std::endl;

        SpherePriorityQueue queue;
        create_global_sphere_priority_queue(queue);

        std::vector<bool> handled_tids (tin.num_triangles(), false);

        double curr_eps = 0.0;

        std::cout << "Searching for triangles having epsilon <= " << max_epsilon << "..." << std::endl;

        bool found_min = false;

        while (queue.size() > 0)
        {
            Sphere smallest_sphere = *queue.begin();
            queue.erase(queue.begin());

            if (handled_tids.at(tin_triangle_from_tet(smallest_sphere.center_tri)) == true)
                continue;

            if (is_epsilon_sphere(smallest_sphere))
            {
                curr_eps = smallest_sphere.radius;

                Sphere tmp = convert_tet_tin_sphere(smallest_sphere);

                if (!found_min)
                {
                    std::cout << "[Minimum Epsilon] Triangle " << tmp.center_tri.first << " : " << curr_eps << std::endl;
                    found_min = true;
                }

                handled_tids.at(tmp.center_tri.first) = true;

                if (curr_eps <= max_epsilon)
                {
                    //std::cout << "Epsilon at triangle " << tmp.center_tri.first << " : " << curr_eps << " <= " << max_epsilon << std::endl;

                    tin.t_epsilons.at(tmp.center_tri.first) = tmp;
                    tin.sorted_t_epsilons.insert(tmp);

                }
                //else
                //    std::cout << "Epsilon at triangle " << tmp.center_tri.first << " : " << curr_eps << " > " << max_epsilon << std::endl;
                else
                if (!compute_all)
                {
                    std::cout << "[Maximum Epsilon] Triangle " << tmp.center_tri.first << " : " << curr_eps << std::endl;
                    break;
                }

#ifdef CAXLIB
                if(search_dir == THINWALLS && curr_eps <= max_epsilon)
                    tin.triangle_annotation(tin_triangle_from_tet(smallest_sphere.center_tri)).thin_walls = true;
                else
                if(search_dir == CAVITIES && curr_eps <= max_epsilon)
                     tin.triangle_annotation(tin_triangle_from_tet(smallest_sphere.center_tri)).thin_channels = true;
#endif
            }
            else
                update_global_priority_queue(queue, smallest_sphere);
        }
    }

    post_process_vertices();

    //split_epsilon_triangles();

    time(&end);

    time_analysis = difftime(end, begin);

    //assert(false);
}

DANI_INLINE
void Epsilon3DShape::post_process_vertices ()
{
    std::cout << "Processing unknown vertices ... ";

    std::vector<uint> unknown_vertices;

    uint count = 0;

    epsilons_v_map = std::vector<double> (tin.num_vertices(), FLT_MAX);

    for (Sphere sphere : tin.t_epsilons)
    {
        if (sphere.center_pos == CENTERED_ON_VERTEX && epsilons_v_map.at(sphere.center_id) == FLT_MAX)
        {
            epsilons_v_map.at(sphere.center_id) = sphere.radius;
            count++;
        }
    }

    uint missing = 0;
    uint index = 0;

    for (double radius : epsilons_v_map)
    {
        //std::cout << " Computing epsilon at vertex " << vid << " : "; //<< std::endl;
       bool is_less = false;

//       for (uint eid : tin.adj_vtx2edg(index))
//       {
//           if (get_dihedral_angle(tin, eid) < M_PI/2)
//           {
//               epsilons_v_map.at(index) = 0.0;
//               is_less = true;
//           }
//        }

        if (is_less == false && radius == FLT_MAX)
            unknown_vertices.push_back(index);

        index++;
    }

    missing = unknown_vertices.size();

    link_tets = std::vector<std::set<uint>> (tin.num_vertices(), std::set<uint> ());

    global_v_labels = std::vector<std::vector<bool>> (tin.num_vertices(), std::vector<bool> (tetmesh.num_vertices(), false));
    global_e_labels = std::vector<std::vector<bool>> (tin.num_vertices(), std::vector<bool> (tetmesh.num_edges(), false));
    global_tet_labels = std::vector<std::vector<bool>> (tin.num_vertices(), std::vector<bool> (tetmesh.num_tetrahedra(), false));

    std::cout << unknown_vertices.size() << " ... ";

    #pragma omp parallel \
    for if(parallelism_enabled) \
    schedule(dynamic)
    for (uint i = 0; i < unknown_vertices.size(); i++)
    {
        uint vid = unknown_vertices.at(i);
//        for (uint i = 0; i < tin.adj_vtx2tri(vid).size() -1; i++)
//        {
//            for (uint j = i+1; j < tin.adj_vtx2tri(vid).size(); j++)
//            {
//                uint adj_tid1 = tin.adj_vtx2tri(vid).at(i);
//                uint adj_tid2 = tin.adj_vtx2tri(vid).at(j);

//                double angle;

//                vec3d v0 = tin.triangle_vertex(adj_tid1,0);
//                vec3d v1 = tin.triangle_vertex(adj_tid1,1);
//                vec3d v2 = tin.triangle_vertex(adj_tid1,2);

//                vec3d u = v1 - v0;    u.normalize();
//                vec3d v = v2 - v0;    v.normalize();
//                vec3d n1 = u.cross(v); n1.normalize();

//                v0 = tin.triangle_vertex(adj_tid2,0);
//                v1 = tin.triangle_vertex(adj_tid2,1);
//                v2 = tin.triangle_vertex(adj_tid2,2);

//                u = v1 - v0;    u.normalize();
//                v = v2 - v0;    v.normalize();
//                vec3d n2 = u.cross(v); n2.normalize();


//                angle = (2*M_PI - (2 * std::acos (n1.dot(n2)))) / 2; //= get_angle_VVV(vec3d (0.0, 0.0, 0.0), tin.triangle_normal(adj_tid1), tin.triangle_normal(adj_tid2)) ;

//                if (angle < M_PI / 2 || angle > 3*M_PI/2)
//                {
//                    std::cout << " [ACUTE DIHEDRAL ANGLE] Vertex " << vid << " : " << angle << std::endl;

//                    epsilons_v_map.at(vid) = 0.0;

//                    break;
//                }
//            }
//        }

//        if (epsilons_v_map.at(vid) == FLT_MAX)
//        {
//            uint adj_tid1 = tin.adj_vtx2tri(vid).at(tin.adj_vtx2tri(vid).size() -1);
//            uint adj_tid2 = tin.adj_vtx2tri(vid).at(0);

//            double angle;

//            vec3d v0 = tin.triangle_vertex(adj_tid1,0);
//            vec3d v1 = tin.triangle_vertex(adj_tid1,1);
//            vec3d v2 = tin.triangle_vertex(adj_tid1,2);

//            vec3d u = v1 - v0;    u.normalize();
//            vec3d v = v2 - v0;    v.normalize();
//            vec3d n1 = u.cross(v); n1.normalize();

//            v0 = tin.triangle_vertex(adj_tid2,0);
//            v1 = tin.triangle_vertex(adj_tid2,1);
//            v2 = tin.triangle_vertex(adj_tid2,2);

//            u = v1 - v0;    u.normalize();
//            v = v2 - v0;    v.normalize();
//            vec3d n2 = u.cross(v); n2.normalize();


//            angle = (2*M_PI - (2 * std::acos (n1.dot(n2)))) / 2; //= get_angle_VVV(vec3d (0.0, 0.0, 0.0), tin.triangle_normal(adj_tid1), tin.triangle_normal(adj_tid2)) ;

//            if (angle < M_PI / 2 || angle > 3*M_PI/2)
//            {
//                std::cout << " [ACUTE DIHEDRAL ANGLE] Vertex " << vid << " : " << angle << std::endl;

//                epsilons_v_map.at(vid) = 0.0;
//            }
//        }


        if (epsilons_v_map.at(vid) == FLT_MAX)
            epsilons_v_map.at(vid) = compute_epsilon_at(CENTERED_ON_VERTEX, vid);

        if (i % 1000 == 0 || i == unknown_vertices.size() -1)
            std::cout << "Computed " << i << " / " << unknown_vertices.size() -1 << std::endl;

        //std::cout << epsilons_v_map.at(vid) << std::endl; //<< std::endl;
    }



    std::cout << " .. " << unknown_vertices.size() << " .. Done" << std::endl;
}

DANI_INLINE
void Epsilon3DShape::create_global_sphere_priority_queue(SpherePriorityQueue &queue)
{
    std::vector<SpherePriorityQueue> tmp_queues (tin.num_triangles());

    #pragma omp parallel \
    for if(parallelism_enabled) \
    schedule(dynamic)
    for (uint tid = 0; tid < tin.num_triangles(); tid++)
    {
        std::set<uint> tets;

        compute_link_tetmesh(CENTERED_ON_TRIANGLE, tid, tets);

        compute_link(CENTERED_ON_TRIANGLE, tid, tets, tmp_queues.at(tid));
    }

    for (uint tid = 0; tid < tin.num_triangles(); tid++)
    {
        auto it = tmp_queues.at(tid).begin();

        while (it != tmp_queues.at(tid).end())
        {
            queue.insert(*it);
            it++;
        }

        tmp_queues.at(tid).clear();
    }
}

DANI_INLINE
void Epsilon3DShape::update_global_priority_queue(SpherePriorityQueue &queue, const Sphere &sphere)
{
    uint tid = tin_triangle_from_tet(sphere.center_tri);

    std::set<uint> new_tets;
    update_link_tets(CENTERED_ON_TRIANGLE, tid, new_tets, sphere);

    if (new_tets.size() > 0)
        compute_link(CENTERED_ON_TRIANGLE, tid, new_tets, queue);
}

DANI_INLINE
void Epsilon3DShape::compute_all_epsilons ()
{
    //return compute_epsilon_at(CENTERED_ON_TRIANGLE, 2381);

    if (tetmesh.num_vertices() == 0)
        create_tetmesh();

    min_epsilon_sphere.radius = FLT_MAX;
    max_epsilon_sphere.radius = 0.0;

    #pragma omp parallel \
    for if(parallelism_enabled) \
    schedule(dynamic)
    for (uint tid_tin = 0; tid_tin < tin.num_triangles(); tid_tin++)
    {
        compute_epsilon_at(CENTERED_ON_TRIANGLE, tid_tin);

        if (tid_tin % 1000 == 0 || tid_tin == tin.num_triangles()-1)
            std::cout << "Computed " << tid_tin << " / " << tin.num_triangles() << std::endl;
    }

    for (uint tid_tin = 0; tid_tin < tin.num_triangles(); tid_tin++)
    {
        tin.sorted_t_epsilons.insert(tin.t_epsilons.at(tid_tin));
    }
}

DANI_INLINE
void Epsilon3DShape::compute_triangles_with_epsilon_less_than(const double thresh)
{
    std::set<uint> triangles;

    SpherePriorityQueue queue;
    create_global_sphere_priority_queue(queue);

    double curr_eps = 0.0;

    while (queue.size() > 0 && curr_eps <= thresh)
    {
        Sphere smallest_sphere = *queue.begin();
        queue.erase(queue.begin());

        if (triangles.find(tin_triangle_from_tet(smallest_sphere.center_tri)) != triangles.end())
            continue;

        if (is_epsilon_sphere(smallest_sphere))
        {
            uint tid = tin_triangle_from_tet(smallest_sphere.center_tri);

            curr_eps = smallest_sphere.radius;
            triangles.insert(tid);

            tin.t_epsilons.at(tid) = smallest_sphere;
            //tin.sorted_t_epsilons.insert(smallest_sphere);
        }
        else
            update_global_priority_queue(queue, smallest_sphere);
    }

    assert(false);
}


DANI_INLINE
bool Epsilon3DShape::is_epsilon_sphere(Sphere &sph)
{
    if (sph.tangent_pos == TANGENT_ON_VERTEX)
    {
        //std::cout << " -- tangent on vertex : ";

        //if(v_map_tet_tin.at(sph.tangent_id).first == EXTRA_POINT)
        if (!is_tet_vertex_on_tin_srf(sph.tangent_id))
        {
            //std::cout << " inner vertex " << std::endl;
            return false;
        }


        // check radius direction compatible with the search direction

        for (uint tt : tin.adj_vtx2tri(tin_vertex_from_tet(sph.tangent_id)))
        {
            caxlib::Plane plane (sph.tangent, tin.triangle_normal(tt));

            double dist = 0.0;// = plane.point_plane_dist(sph.center);

            vec3d u = sph.center - sph.tangent;

            if (u.length() != 0)
                dist = u.dot(plane.n);

            if (search_dir == THINWALLS && dist > -thresh)
                return false;
            else
            if (search_dir == CAVITIES && dist < thresh)
                return false;
        }

        // check if radius is ortogonal to an adjacent face of the tangent point
        for (uint tt : tin.adj_vtx2tri(tin_vertex_from_tet(sph.tangent_id)))
        {
            vec3d t_normal = tin.triangle_normal(tt);
            vec3d r_normal = sph.tangent - sph.center;

            vec3d cross = t_normal.cross(r_normal);

//            if (cross[0] < 1e-5 && cross[0] > -1e-5 &&
//                cross[1] < 1e-5 && cross[1] > -1e-5 &&
//                cross[2] < 1e-5 && cross[2] > -1e-5 )

            if (cross[0] < thresh && cross[0] > -thresh &&
                cross[1] < thresh && cross[1] > -thresh &&
                cross[2] < thresh && cross[2] > -thresh )

                return true;
        }

        if (is_tangent_concave_wrt_sphere(sph))
            return true;

        if (is_tangent_saddle_wrt_sphere(sph))
            return true;

        return false;

    }
    else
    if(sph.tangent_pos == TANGENT_ON_EDGE)
    {
        if(!is_tet_edge_on_tin_srf(sph.tangent_id))
            return false;

        // check radius direction compatible with the search direction
        for (uint tt : tin.adj_edg2tri(tin_edge_from_tet(sph.tangent_id)))
        {
            Plane plane (sph.tangent, tin.triangle_normal(tt));

            double dist = 0.0;// = plane.point_plane_dist(sph.center);

            vec3d u = sph.center - sph.tangent;

            if (u.length() != 0)
                dist = u.dot(plane.n);

            if (search_dir == THINWALLS && dist > -thresh)
                return false;
            else
            if (search_dir == CAVITIES && dist < thresh)
                return false;
        }

        // check if radius is ortogonal to an adjacent face of the tangent point
        for (uint tt : tin.adj_edg2tri(tin_edge_from_tet(sph.tangent_id)))
        {
            vec3d t_normal = tin.triangle_normal(tt);
            vec3d r_normal = sph.tangent - sph.center;

            vec3d cross = t_normal.cross(r_normal);

            if (cross[0] < thresh && cross[0] > -thresh &&
                cross[1] < thresh && cross[1] > -thresh &&
                cross[2] < thresh && cross[2] > -thresh )

                return true;
        }

        if (is_tangent_concave_wrt_sphere(sph))
            return true;

        if (is_tangent_saddle_wrt_sphere(sph))
            return true;

        return false;

    }
    else
    if (sph.tangent_pos == TANGENT_ON_TRIANGLE)
    {
        // check if the triangle is on the surface

        uint tet     = sph.tangent_tri.first;
        uint tid_off = sph.tangent_tri.second;

        if (!is_tet_triangle_on_tin_srf(tet, tid_off))
            return false;

        for (uint tt : tin.adj_tri2tri(tin_triangle_from_tet(sph.tangent_tri)))
        {
            Plane plane (sph.tangent, tin.triangle_normal(tt));

            double dist = 0.0;// = plane.point_plane_dist(sph.center);

            vec3d u = sph.center - sph.tangent;

            if (u.length() != 0)
                dist = u.dot(plane.n);

            if (search_dir == THINWALLS && dist > 0)
                return false;
            else
            if (search_dir == CAVITIES && dist < 0)
                return false;
        }

        return true;
    }
    else
        assert (false);
}

DANI_INLINE
void Epsilon3DShape::set_v_map_tin_tet_tin()
{
    v_map_tin_tet = std::vector<uint> (tin.num_vertices(), UINT_MAX);
    v_map_tet_tin = std::vector<std::pair<VertexType, uint>> (tetmesh.num_vertices(), std::pair<VertexType, uint> (EXTRA_POINT, UINT_MAX));

    uint count_original = 0;

    SortedVertexList tin_list;
    SortedVertexList tet_list;

    for (uint vid_tin = 0; vid_tin < tin.num_vertices(); vid_tin++)
    {
        Point p;
        p.x = tin.vertex(vid_tin).x();
        p.y = tin.vertex(vid_tin).y();
        p.z = tin.vertex(vid_tin).z();
        p.id = vid_tin;

        tin_list.insert(p);
    }

    for (uint vid_tet = 0; vid_tet < tetmesh.num_vertices(); vid_tet++)
    {
        Point p;
        p.x = tetmesh.vertex(vid_tet).x();
        p.y = tetmesh.vertex(vid_tet).y();
        p.z = tetmesh.vertex(vid_tet).z();
        p.id = vid_tet;

        tet_list.insert(p);
    }

    SortedVertexList::iterator tin_iterator = tin_list.begin();
    SortedVertexList::iterator tet_iterator = tet_list.begin();

    while (tin_iterator != tin_list.end() && tet_iterator != tet_list.end())
    {
        Point p_tin = *tin_iterator;
        Point p_tet = *tet_iterator;

        if ( std::fabs( p_tin.x - p_tet.x ) < thresh &&
             std::fabs( p_tin.y - p_tet.y ) < thresh &&
             std::fabs( p_tin.z - p_tet.z ) < thresh )
        {
            v_map_tin_tet.at(p_tin.id) = p_tet.id;
            v_map_tet_tin.at(p_tet.id) = std::pair<VertexType, uint> (ORIGINAL_BOUNDARY_POINT, p_tin.id);

            count_original++;

            tin_iterator++;
        }

        tet_iterator++;
    }

    std::cout << "Mapping tin<-->tet vertices : " << count_original  << " / " << tin.num_vertices() << std::endl;

    if (count_original != tin.num_vertices())
    {
        std::cerr << "[ERROR] Impossible mapping between surface vertices and tetmesh vertices." << std::endl;
        exit(1);
    }

}

DANI_INLINE
void Epsilon3DShape::set_e_map_tin_tet_tin ()
{
    e_map_tin_tet = std::vector<uint> (tin.num_edges(), UINT_MAX);
    e_map_tet_tin = std::vector<std::pair<EdgeType, uint>> (tetmesh.num_edges(), std::pair<EdgeType, uint> (EXTRA_EDGE, UINT_MAX));

    uint count_original = 0;

    for (uint eid_tin = 0; eid_tin < tin.num_edges(); eid_tin++)
    {
        uint e0 = tin.edge_vertex_id(eid_tin, 0);
        uint e0_tet = tet_vertex_from_tin(e0);

        uint e1 = tin.edge_vertex_id(eid_tin, 1);
        uint e1_tet = tet_vertex_from_tin(e1);

        for (uint eid_tet : tetmesh.adj_vtx2edg(e0_tet))
        {
            if (tetmesh.edge_contains_vertex(eid_tet, e1_tet))
            {
                e_map_tin_tet.at(eid_tin) = eid_tet;
                e_map_tet_tin.at(eid_tet) = std::pair<EdgeType, uint> (ORIGINAL_BOUNDARY_EDGE, eid_tin);

                count_original++;

                break;
            }
        }
    }

    std::cout << "Mapping tin<-->tet edges : " << count_original  << " / " << tin.num_edges() << std::endl;

    if (count_original != tin.num_edges())
    {
        std::cerr << "[ERROR] Impossible mapping between surface edges and tetmesh edges." << std::endl;
        exit(1);
    }

}


DANI_INLINE
void Epsilon3DShape::set_t_map_tin_tet_tin ()
{
    t_map_tin_tet = std::vector<VectorOfTetTriangles> (tin.num_triangles(), VectorOfTetTriangles());
    t_map_tet_tin = std::vector<std::vector<std::pair<TriType, uint>>> (tetmesh.num_tetrahedra(), std::vector<std::pair<TriType, uint>> ());

    for (uint tet = 0; tet < tetmesh.num_tetrahedra(); tet++)
    {
        t_map_tet_tin.at(tet) = std::vector<std::pair<TriType,uint>> (4, std::pair<TriType, uint> (EXTRA_TRIANGLE, UINT_MAX));
    }

    uint count_original = 0;

    for (uint tid_tin = 0; tid_tin < tin.num_triangles(); tid_tin++)
    {
        uint v0 = tet_vertex_from_tin(tin.triangle_vertex_id(tid_tin, 0));
        uint v1 = tet_vertex_from_tin(tin.triangle_vertex_id(tid_tin, 1));
        uint v2 = tet_vertex_from_tin(tin.triangle_vertex_id(tid_tin, 2));

        for (uint adj_tet : tetmesh.adj_vtx2tet(v0))
        {
            if (tetmesh.tet_contains_vertex(adj_tet, v1) && tetmesh.tet_contains_vertex(adj_tet, v2) )
            {
                for (uint tid_off = 0; tid_off < 4; tid_off++)
                {
                    uint tet_vid0 = tetmesh.tet_vertex_id(adj_tet, TET_FACES[tid_off][0]);
                    uint tet_vid1 = tetmesh.tet_vertex_id(adj_tet, TET_FACES[tid_off][1]);
                    uint tet_vid2 = tetmesh.tet_vertex_id(adj_tet, TET_FACES[tid_off][2]);

                    if ((v0 == tet_vid0 || v0 == tet_vid1 || v0 == tet_vid2) &&
                            (v1 == tet_vid0 || v1 == tet_vid1 || v1 == tet_vid2) &&
                            (v2 == tet_vid0 || v2 == tet_vid1 || v2 == tet_vid2) )
                    {
                        count_original++;

                        t_map_tin_tet.at(tid_tin).push_back(std::pair<uint, uint> (adj_tet, tid_off));
                        t_map_tet_tin.at(adj_tet).at(tid_off) =  std::pair<TriType, uint> (ORIGINAL_BOUNDARY_TRIANGLE, tid_tin);

                        adj_tet = tetmesh.adjacent_tet_through_facet(adj_tet, tid_off);

                        if (adj_tet != -1)
                        {
                            for (uint tid_off = 0; tid_off < 4; tid_off++)
                            {
                                uint tet_vid0 = tetmesh.tet_vertex_id(adj_tet, TET_FACES[tid_off][0]);
                                uint tet_vid1 = tetmesh.tet_vertex_id(adj_tet, TET_FACES[tid_off][1]);
                                uint tet_vid2 = tetmesh.tet_vertex_id(adj_tet, TET_FACES[tid_off][2]);

                                if ((v0 == tet_vid0 || v0 == tet_vid1 || v0 == tet_vid2) &&
                                        (v1 == tet_vid0 || v1 == tet_vid1 || v1 == tet_vid2) &&
                                        (v2 == tet_vid0 || v2 == tet_vid1 || v2 == tet_vid2) )
                                {
                                    t_map_tin_tet.at(tid_tin).push_back(std::pair<uint, uint> (adj_tet, tid_off));
                                    t_map_tet_tin.at(adj_tet).at(tid_off) =  std::pair<TriType, uint> (ORIGINAL_BOUNDARY_TRIANGLE, tid_tin);
                                }
                            }
                        }

                        break;
                    }
                }

                break;
            }
        }

    }

    std::cout << "Mapping tin<-->tet triangles : " << count_original  << " / " << tin.num_triangles() << std::endl;

    if (count_original != tin.num_triangles())
    {
        std::cerr << "[ERROR] Impossible mapping between surface triangles and tetmesh triangles." << std::endl;
        exit(1);
    }

}


DANI_INLINE
bool Epsilon3DShape::is_tet_vertex_on_tin_srf(const uint vid) const
{
    assert (vid < tetmesh.num_vertices());

    //if (vid < tin.num_vertices())
    if (tin_vertex_from_tet(vid) != UINT_MAX)
        return true;

    return false;
}

DANI_INLINE
bool Epsilon3DShape::is_tet_edge_on_tin_srf(const uint eid) const
{
    assert (eid < tetmesh.num_edges());

    //if (vid < tin.num_vertices())
    if (e_map_tet_tin.at(eid).first == ORIGINAL_BOUNDARY_EDGE)
        return true;

    return false;
}

DANI_INLINE
bool Epsilon3DShape::is_tet_triangle_on_tin_srf (const uint tet, const uint tid_off) const
{
    if (t_map_tet_tin.at(tet).at(tid_off).first == ORIGINAL_BOUNDARY_TRIANGLE)
        return true;

    return false;
}


DANI_INLINE
bool Epsilon3DShape::is_tangent_concave_wrt_sphere (const Sphere &sphere) const
{
    if (sphere.tangent_pos == TANGENT_ON_TRIANGLE)
        assert(false);

    std::vector<int> triangles;
    std::vector<int> edges;

    uint tangent_tin;

    if (sphere.tangent_pos == TANGENT_ON_VERTEX)
    {
        tangent_tin = tin_vertex_from_tet(sphere.tangent_id);
        triangles = tin.adj_vtx2tri(tangent_tin);
        edges = tin.adj_vtx2edg(tangent_tin);
    }
    else
    if (sphere.tangent_pos == TANGENT_ON_EDGE)
    {
        tangent_tin = tin_edge_from_tet(sphere.tangent_id);
        triangles = tin.adj_edg2tri(tangent_tin);
        edges.push_back(tin_edge_from_tet(sphere.tangent_id));
    }
    else
        assert(false);

    for (uint tid : triangles)
    {
        vec3d t0;
        t0[0] = tin.triangle_vertex(tid, 0).x();
        t0[1] = tin.triangle_vertex(tid, 0).y();
        t0[2] = tin.triangle_vertex(tid, 0).z();

        vec3d t1;
        t1[0] = tin.triangle_vertex(tid, 1).x();
        t1[1] = tin.triangle_vertex(tid, 1).y();
        t1[2] = tin.triangle_vertex(tid, 1).z();

        vec3d  t2;
        t2[0] = tin.triangle_vertex(tid, 2).x();
        t2[1] = tin.triangle_vertex(tid, 2).y();
        t2[2] = tin.triangle_vertex(tid, 2).z();

        std::vector<vec3d> triangle;

        triangle.push_back(t0);
        triangle.push_back(t1);
        triangle.push_back(t2);

        //if (t_labels.at(tid) == true)
            //continue;


#ifdef GEOMETRICTOOLS

        gte::DCPQuery<double, gte::Vector3<double>, gte::Triangle3<double>>::Result result = VT_min_distance(sphere.center, triangle);

        vec3d closest;
        closest[0] = result.closest[0];
        closest[1] = result.closest[1];
        closest[2] = result.closest[2];

        if (result.distance <= thresh)
            return false;

        if (sphere.radius - result.distance > thresh)
            return false;
#endif
    }

    is_point_on_edge(sphere.tangent, tetmesh.edge_vertex(sphere.tangent_id, 0), tetmesh.edge_vertex(sphere.tangent_id, 1));

    return true;
}

DANI_INLINE
bool Epsilon3DShape::is_tangent_saddle_wrt_sphere(const Sphere &sphere) const
{
    std::vector<int> link_v;// = tin.adj_vtx2vtx_ordered(vid_tin);
    std::vector<int> link_t;// = tin.adj_vtx2vtx_ordered(vid_tin);
    std::vector<int> link_e;

    if (sphere.tangent_pos == TANGENT_ON_VERTEX)
    {
        uint vid_tin = tin_vertex_from_tet(tin_vertex_from_tet( sphere.tangent_id));

        //if (!is_saddle_vertex(vid_tin))
            //return false;

        //double angle = 0.0;

//        for (uint tid : tin.adj_vtx2tri(vid_tin))
//        {
//            angle += tin.triangle_angle_at_vertex(tid, vid_tin);
//        }

//        if (angle < 2 * M_PI)
//            return false;

        //bool is_in = false;

        uint curr_t = tin.adj_vtx2tri(vid_tin).at(0);
        uint curr_e = tin.edge_opposite_to(curr_t, vid_tin);

        uint first_v = tin.edge_vertex_id(curr_e, 0);

        uint curr_v = tin.edge_vertex_id(curr_e, 1);

        link_v.push_back(tin.edge_vertex_id(curr_e, 0));

        link_e.push_back(tin.edge_opposite_to(curr_t, curr_v));

        while (curr_v != first_v)
        {
            link_v.push_back(curr_v);
            link_t.push_back(curr_t);

            curr_t = tin.triangle_adjacent_along(curr_t, vid_tin, curr_v);

            tin.edge_opposite_to(curr_t, curr_v);

            curr_e = tin.edge_opposite_to(curr_t, vid_tin);
            curr_v = tin.edge_vertex_id(curr_e, 0) == curr_v ? tin.edge_vertex_id(curr_e, 1) : tin.edge_vertex_id(curr_e, 0);

            link_e.push_back(tin.edge_opposite_to(curr_t, curr_v));

        }

        assert (link_e.size() == tin.adj_vtx2tri(vid_tin).size());

        bool is_in_global = false;// = sphere.has_inner_point(tin.vertex(link_v.at(0)));
        double dist = 0.0;

#ifdef GEOMETRICTOOLS

        std::vector<vec3d> edge;

        edge.push_back(tin.edge_vertex(link_e.at(0), 0));
        edge.push_back(tin.edge_vertex(link_e.at(0), 1));

        dist = VE_distance(sphere.center, edge);
#else

#endif
        if (sphere.radius - dist > thresh)
            is_in_global = true;

        uint changes = 0;

        for (int eid : link_e)
        {
            bool is_in = false;// = sphere.has_inner_point(tin.vertex(vid));

            vec3d p;

#ifdef GEOMETRICTOOLS

            std::vector<vec3d> edge;

            edge.push_back(tin.edge_vertex(eid, 0));
            edge.push_back(tin.edge_vertex(eid, 1));

            dist = VE_distance(sphere.center, edge);

            p[0] = VE_min_distance(sphere.center, edge).segmentClosest[0];
            p[1] = VE_min_distance(sphere.center, edge).segmentClosest[1];
            p[2] = VE_min_distance(sphere.center, edge).segmentClosest[2];

#else

#endif
            if (sphere.radius - dist > thresh)
                is_in = true;

            if (is_in_global != is_in)
            {
                changes++;
                is_in_global = is_in;
            }
        }

        return (changes > 2);
    }
    else
    if (sphere.tangent_pos == TANGENT_ON_EDGE)
    {
        std::vector<int> link_t;
        std::vector<int> link_e;

        uint e0 = tin.edge_vertex_id(tin_edge_from_tet(sphere.tangent_id), 0);
        uint e1 = tin.edge_vertex_id(tin_edge_from_tet(sphere.tangent_id), 1);

        uint curr_t = tin.adj_edg2tri(tin_edge_from_tet(sphere.tangent_id)).at(0);
        uint first_t = curr_t;

        link_t.push_back(curr_t);

        uint opp_vid = tin.vertex_opposite_to(curr_t, e0, e1);

        curr_t = tin.triangle_adjacent_along(curr_t, e0, opp_vid);
        link_e.push_back(tin_edge_having_endpoints (e0, opp_vid));

        while (curr_t != first_t)
        {
            link_t.push_back(curr_t);

            if (tin.triangle_contains_vertex(curr_t, e0) && tin.triangle_contains_vertex(curr_t, e1))
            {
                opp_vid = tin.vertex_opposite_to(curr_t, e0, e1);

                curr_t = tin.triangle_adjacent_along(curr_t, e1, opp_vid);
                link_e.push_back(tin_edge_having_endpoints (e1, opp_vid));

            }
            else
            if (tin.triangle_contains_vertex(curr_t, e0))
            {
                opp_vid = tin.vertex_opposite_to(curr_t, e0 , opp_vid);

                curr_t = tin.triangle_adjacent_along(curr_t, e0, opp_vid);
                link_e.push_back(tin_edge_having_endpoints (e0, opp_vid));
            }
            else
            if (tin.triangle_contains_vertex(curr_t, e1))
            {
                opp_vid = tin.vertex_opposite_to(curr_t, e1, opp_vid);

                curr_t = tin.triangle_adjacent_along(curr_t, e1, opp_vid);
                link_e.push_back(tin_edge_having_endpoints (e1, opp_vid));
            }
        }

        assert (link_t.size() == tin.adj_vtx2tri(e0).size() + tin.adj_vtx2tri(e1).size() -2);

        assert (link_e.size() == tin.adj_vtx2tri(e0).size() + tin.adj_vtx2tri(e1).size() -2);

        bool is_in_global = false;// = sphere.has_inner_point(tin.vertex(link_v.at(0)));
        double dist = 0.0;

#ifdef GEOMETRICTOOLS

        std::vector<vec3d> edge;

        edge.push_back(tin.edge_vertex(link_e.at(0), 0));
        edge.push_back(tin.edge_vertex(link_e.at(0), 1));

        dist = VE_distance(sphere.center, edge);
#else

#endif
        if (sphere.radius - dist > thresh)
            is_in_global = true;

        uint changes = 0;

        //for (int tid : link_t)
        for (uint eid : link_e)
        {
            bool is_in = false;// = sphere.has_inner_point(tin.vertex(vid));

#ifdef GEOMETRICTOOLS

            std::vector<vec3d> edge;

            edge.push_back(tin.edge_vertex(eid, 0));
            edge.push_back(tin.edge_vertex(eid, 1));

            dist = VE_distance(sphere.center, edge);
#else

#endif
            if (sphere.radius - dist > thresh)
                is_in = true;

            if (is_in_global != is_in)
            {
                changes++;
                is_in_global = is_in;
            }
        }

        //if (changes > 2)
        //    std::cout << " Saddle Edge .. ";

        return (changes > 2);
    }
    else
        assert(false);

}

/**
 * @brief is_tangent_concave_or_saddle_wrt_sphere
 * @param sphere
 * @return
 *
 * NOT WORKING. TO BE TESTED AND DEBUG
 */
DANI_INLINE
bool Epsilon3DShape::is_tangent_concave_or_saddle_wrt_sphere (const Sphere &sphere) const
{
    std::vector<int> link_v;// = tin.adj_vtx2vtx_ordered(vid_tin);
    std::vector<int> link_t;// = tin.adj_vtx2vtx_ordered(vid_tin);
    std::vector<int> link_e;

    bool is_concave = true;
    bool is_saddle = false;

    if (sphere.tangent_pos == TANGENT_ON_VERTEX)
    {
        uint vid_tin = tin_vertex_from_tet(tin_vertex_from_tet( sphere.tangent_id));

        uint curr_t = tin.adj_vtx2tri(vid_tin).at(0);
        uint curr_e = tin.edge_opposite_to(curr_t, vid_tin);

        uint first_v = tin.edge_vertex_id(curr_e, 0);

        uint curr_v = tin.edge_vertex_id(curr_e, 1);

        link_v.push_back(tin.edge_vertex_id(curr_e, 0));

        link_e.push_back(curr_e);

        while (curr_v != first_v)
        {
            link_v.push_back(curr_v);
            link_t.push_back(curr_t);

            curr_t = tin.triangle_adjacent_along(curr_t, vid_tin, curr_v);
            curr_e = tin.edge_opposite_to(curr_t, vid_tin);
            curr_v = tin.edge_vertex_id(curr_e, 0) == curr_v ? tin.edge_vertex_id(curr_e, 1) : tin.edge_vertex_id(curr_e, 0);

            //link_e.push_back(tin.edge_opposite_to(curr_t, curr_v));

        }

        assert (link_e.size() == tin.adj_vtx2tri(vid_tin).size());

        Plane plane (sphere.tangent, sphere.tangent - sphere.center);

        bool is_last_out = false;

        uint changes = 0;
        uint counter = 0;

        for (int vid : link_v)
        {
            bool is_curr_out = false;// = sphere.has_inner_point(tin.vertex(vid));
            double dist = 0.0;
            double dist_radius = tin.vertex(vid).dist(sphere.center);

            vec3d u = tin.vertex(vid) - sphere.tangent;

            if (u.length() != 0)
                dist = u.dot(plane.n);

            if (dist >= 0.0 || dist_radius >= sphere.radius )
            {
                is_curr_out = true;
            }
            else
            if (dist < 0.0)
            {
                is_concave = false;
            }

            if (is_last_out != is_curr_out)
            {
                is_last_out = is_curr_out;

                if (counter > 0)
                    changes++;
            }

            counter++;
        }

        if (changes > 2)
            is_saddle = true;
    }
    else
    if (sphere.tangent_pos == TANGENT_ON_EDGE)
    {
        std::vector<int> link_t;
        std::vector<int> link_e;

        uint eid = tin_edge_from_tet(sphere.tangent_id);

        uint e0 = tin.edge_vertex_id(eid, 0);
        uint e1 = tin.edge_vertex_id(eid, 1);

        std::vector<uint> vertices;

        uint t0 = tin.adj_edg2tri(eid).at(0);
        uint t1 = tin.adj_edg2tri(eid).at(1);

        vertices.push_back(e0);
        vertices.push_back(tin.vertex_opposite_to(t0, e0, e1));

        vertices.push_back(e1);
        vertices.push_back(tin.vertex_opposite_to(t1, e0, e1));

        Plane plane (sphere.tangent, sphere.tangent - sphere.center);

        bool is_last_out = false;

        uint changes = 0;
        uint counter = 0;

        for (int vid : vertices)
        {
            bool is_curr_out = false;// = sphere.has_inner_point(tin.vertex(vid));
            double dist = 0.0;

            vec3d u = tin.vertex(vid) - sphere.tangent;

            if (u.length() != 0)
                dist = u.dot(plane.n);

            if (dist >= 0.0 || tin.vertex(vid).dist(sphere.center) > sphere.radius + thresh)
            {
                is_curr_out = true;
            }
            else
            if (dist < 0.0)
            {
                is_concave = false;
            }

            if (is_last_out != is_curr_out)
            {
                is_last_out = is_curr_out;

                if (counter > 0)
                    changes++;
            }

            counter++;
        }

        if (changes > 2)
            is_saddle = true;
    }


    if (is_concave)
        return true;

    if (is_saddle)
        return true;

    return false;
}

/**
 * @brief Epsilon3DShape::load_epsilons_from_file
 * @param filename
 */
DANI_INLINE
void Epsilon3DShape::load_epsilons_from_file(const char *filename)
{
    v_epsilon  = std::vector<double> (tin.num_vertices(), FLT_MAX);

    min_epsilon_sphere.radius = FLT_MAX;
    max_epsilon_sphere.radius = 0.0;

    std::ifstream in;

    in.open(filename);

    if (!in.is_open())
    {
        exit(1);
    }

    for (uint tid = 0; tid < tin.num_triangles(); tid++)
    {
        tin.t_epsilons.at(tid).read_sphere(in);

        uint n_spheres;
        in >> n_spheres;

        for (uint i=0; i < n_spheres; i++)
        {
            Sphere sph;
            sph.read_sphere(in);
            t_spheres.at(tid).push_back(sph);

            t_spheres.at(tid).at(t_spheres.at(tid).size()-1).center_pos = CENTERED_ON_TRIANGLE;
            t_spheres.at(tid).at(t_spheres.at(tid).size()-1).center_id = tid;
        }

        tin.t_epsilons.at(tid).center_pos = CENTERED_ON_TRIANGLE;
        tin.t_epsilons.at(tid).center_id = tid;


        if (tin.t_epsilons.at(tid).radius < min_epsilon_sphere.radius)
        {
            min_epsilon_sphere = tin.t_epsilons.at(tid);
        }

        if (tin.t_epsilons.at(tid).radius > max_epsilon_sphere.radius)
        {
            max_epsilon_sphere = tin.t_epsilons.at(tid);
        }


        //std::cout << " ... tid : " << tid << " : " << t_epsilons.at(tid).radius << std::endl;
    }

    in.close();

    is_epsilon_known = true;
}

DANI_INLINE
void Epsilon3DShape::save_epsilons_on_file(const char *teps_filename)
{
    if (!compute_all)
    {
        std::cerr << "ERROR: impossible to store epsilon value for each triangle." << std::endl;
        return;
    }

    std::cout << "Saving on file for visualization ... " ;

    std::ofstream out;

    out.precision(std::numeric_limits<long double>::digits10 + 1);

    out.open(teps_filename);

    if (!out.is_open())
    {
        exit(1);
    }

    for (uint tid = 0; tid < tin.num_triangles(); tid++)
    {
        Sphere sph = (tin.t_epsilons.at(tid));

        sph.print_sphere(out);

        out << t_spheres.at(tid).size() << std::endl;

        for (uint i = 0; i < t_spheres.at(tid).size(); i++)
        {
            Sphere sph = (t_spheres.at(tid).at(i));

            sph.print_sphere(out);
        }

        out << std::endl;
    }

    out.close();

    std::cout << "DONE" << std::endl;

    std::cout << std::endl << "//////////////////////////////////////////////////////////////////" << std::endl << std::endl;
}


DANI_INLINE
void Epsilon3DShape::save_annotations_ZIP (const char *filename)
{
#ifdef CAXLIB

    std::cout << "Saving output into 'ZIP = OFF+ANN' file format" << std::endl;
    tin.save(filename);

#else

    std::cerr << "ERROR: CAxLib is required to save annotation into OFF+ANN format. Recompile by defining CAXLIB variable and linking CAxLib. " << std::endl;
    exit(1);

#endif
}

DANI_INLINE
void Epsilon3DShape::save_colored_obj(const char *filename)
{
    tin.save(filename);
}

DANI_INLINE
void Epsilon3DShape::save_colored_off(const char *filename, bool color)
{

#ifdef CINOLIB

    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    std::vector<float> field;
    std::vector<float[4]> colors = std::vector<float[4]> (tin.num_vertices());

    //std::map<float, std::pair<int, float *>> colormap;

    double min = FLT_MAX;
    double max = 0.0;
    uint id_max = UINT_MAX;

    if (color == true)
    {
        double real_max = 0.0;

        std::cout << "Converting into vertex colors ... " << std::endl << std::endl;

        for (uint vid = 0; vid < tin.num_vertices(); vid++)
        {
            double value = epsilons_v_map.at(vid);

            if (value > 0.0 && value < min)
                min = value;

            if (value != FLT_MAX && value > max)
            {
                max = value;
                id_max = vid;
            }

            if (value > max)
            {
                real_max = value;
            }
        }

        for (uint vid = 0; vid < tin.num_vertices(); vid++)
        {
            double value = epsilons_v_map.at(vid);

            if (value == FLT_MAX )
                value = max + thresh;

            field.push_back(value);
        }
    }

    if (min == FLT_MAX)
    {
        std::cout << "[WARNING] No thin vertex. Impossible to store colored OFF. " << std::endl;

        std::cout << "Output filename : " << filename << std::endl;
    }

    std::cout << " [Minimum != 0] " << min << std::endl;
    std::cout << " [Maximum != FLT_MAX] " << max << std::endl << std::endl;

    ScalarField sf(field);

    sf.normalize_in_01();
    //sf.copy_to_mesh(trimesh);

    std::cout << "Corlors ...... " << std::endl;

    for (uint vid = 0; vid < tin.num_vertices(); vid++)
    {
        //double value = FLT_MAX;
        double value;

        if (min == FLT_MAX)
            value = FLT_MAX;
        else
            value = sf[vid];

        int r, g, b;

        if (value == FLT_MAX || value >= sf[id_max] )
        {
            r=200; g=200; b=200;
        }
        else
        {
            /*plot short rainbow RGB*/
            double a=(1-value)/0.25;	//invert and group
            int X = std::floor(a);	//this is the integer part
            int Y = std::floor(255*(a-X)); //fractional part from 0 to 255

            switch(X)
            {
                case 0: r=255;g=Y;b=0;break;
                case 1: r=255-Y;g=255;b=0;break;
                case 2: r=0;g=255;b=Y;break;
                case 3: r=0;g=255-Y;b=255;break;
                case 4: r=0;g=0;b=255;break;
            }
        }

        float color[4];
        color[0] = r/255.0;
        color[1] = g/255.0;
        color[2] = b/255.0;

//        if (epsilons_v_map.at(vid) > max_epsilon)
//        {
//            float hsv[3];

//            RGBtoHSV( color, hsv[0], hsv[1], hsv[2]);

//            hsv[1] /= 3.5;

//            HSVtoRGB(hsv, color[0], color[1], color[2]);
//        }


        colors.at(vid)[0] = color[0];
        colors.at(vid)[1] = color[1];
        colors.at(vid)[2] = color[2];
        colors.at(vid)[3] = 0.1;

        //colormap[value] = std::pair<int, float *> (tid,color);

    }

    std::cout << "Done " << std::endl;

    std::cout << "Saving COFF ... ";

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OFF() : couldn't save file " << filename << endl;
        exit(-1);
    }

    fprintf (fp, "COFF\n%zu %d 0\n", tin.num_vertices(), tin.num_triangles());

    for(size_t vid=0; vid<tin.num_vertices(); vid++)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(fp, "%.17g %.17g %.17g ", tin.vertex(vid).x(), tin.vertex(vid).y(), tin.vertex(vid).z());
        fprintf(fp, "%lf %lf %lf %lf\n", colors.at(vid)[0], colors.at(vid)[1], colors.at(vid)[2], colors.at(vid)[3]);

    }

    for(size_t tid=0; tid<tin.num_triangles(); tid++)
    {
        fprintf(fp, "3 %d %d %d\n", tin.triangle_vertex_id(tid, 0), tin.triangle_vertex_id(tid, 1), tin.triangle_vertex_id(tid, 2));
    }

    fclose(fp);

    std::cout << "Done " << std::endl;

#else

    std::cerr << "[ERROR] Cinolib required to run this operation. " << std::endl;
    exit(1);

#endif
}


DANI_INLINE
void Epsilon3DShape::create_tetmesh()
{
   std::cout << "Generating Tetmesh ... " << std::endl;

    std::vector<double> coords;
    std::vector<uint>   tets;

    std::vector<bool>   in_tets;

    std::string flags = "pQYA";

    if (search_dir != THINWALLS)
        flags += "c";

    std::cout << "Computing tetmesh using [ " << flags << " ]" << std::endl;

    tetgen_wrap (tin.vector_coords(), tin.vector_triangles(), tin.vector_edges(), flags, coords, tets, in_tets);

    tetmesh = Tetmesh(coords, tets);

    in_tet_labels = in_tets;

    coords.clear();
    tets.clear();

    set_v_map_tin_tet_tin();
    set_e_map_tin_tet_tin();
    set_t_map_tin_tet_tin();

    link_tets = std::vector<std::set<uint>> (tin.num_triangles(), std::set<uint> ());

    global_v_labels = std::vector<std::vector<bool>> (tin.num_triangles(), std::vector<bool> (tetmesh.num_vertices(), false));
    global_e_labels = std::vector<std::vector<bool>> (tin.num_triangles(), std::vector<bool> (tetmesh.num_edges(), false));
    global_tet_labels = std::vector<std::vector<bool>> (tin.num_triangles(), std::vector<bool> (tetmesh.num_tetrahedra(), false));


    std::cout << std::endl << "//////////////////////////////////////////////////////////////////" << std::endl << std::endl;

}


DANI_INLINE
double Epsilon3DShape::compute_epsilon_at(const CenterPosition cpos, const uint cid_tin)
{
    if (tetmesh.num_vertices() == 0)
        create_tetmesh();

    SpherePriorityQueue ext_link;

    Sphere smallest_sphere;

    smallest_sphere.id_ref = REFERENCE_TO_TET;
    smallest_sphere.center_pos = cpos;

    if (cpos == CENTERED_ON_VERTEX)
    {
        smallest_sphere.tangent_pos = TANGENT_ON_VERTEX;
        smallest_sphere.center_id = tet_vertex_from_tin(cid_tin);
        smallest_sphere.tangent_id = tet_vertex_from_tin(cid_tin);
    }
    else
    if (cpos == CENTERED_ON_TRIANGLE)
    {
        smallest_sphere.tangent_pos = TANGENT_ON_TRIANGLE;
        smallest_sphere.center_tri = tet_triangle_from_tin(cid_tin);
        smallest_sphere.tangent_tri = tet_triangle_from_tin(cid_tin);
    }


    smallest_sphere.radius = 0.0;

    compute_link_tetmesh(cpos, cid_tin, link_tets.at(cid_tin)); //, tet_labels);
    compute_link(cpos, cid_tin, link_tets.at(cid_tin), ext_link);

    bool is_thick = false;
    bool has_antipodean = false;

    while (ext_link.size() > 0 && is_thick == false)
    {
        smallest_sphere = *ext_link.begin();
        ext_link.erase(ext_link.begin());

        if (smallest_sphere.radius < thresh)
            continue;

        if (!compute_all && smallest_sphere.radius > max_epsilon)
        {
            is_thick = true;
            break;
        }

        has_antipodean = is_epsilon_sphere(smallest_sphere);

        if (has_antipodean == true)
            break;

        if (store_all_spheres)
        {
            if (smallest_sphere.tangent_pos == TANGENT_ON_VERTEX &&
                is_tet_vertex_on_tin_srf(smallest_sphere.tangent_id))
            {
                 t_spheres.at(cid_tin).push_back(convert_tet_tin_sphere(smallest_sphere));
            }
            else
            if (smallest_sphere.tangent_pos == TANGENT_ON_EDGE &&
                is_tet_edge_on_tin_srf(smallest_sphere.tangent_id))
            {
                t_spheres.at(cid_tin).push_back(convert_tet_tin_sphere(smallest_sphere));
            }
            else
            if (smallest_sphere.tangent_pos == TANGENT_ON_TRIANGLE &&
                is_tet_triangle_on_tin_srf(smallest_sphere.tangent_tri.first, smallest_sphere.tangent_tri.second))
            {
                t_spheres.at(cid_tin).push_back(convert_tet_tin_sphere(smallest_sphere));
            }
        }

        std::set<uint> new_tets;
        update_link_tets(cpos, cid_tin, new_tets, smallest_sphere);

        if (new_tets.size() > 0)
            compute_link(cpos, cid_tin, new_tets, ext_link);
    }

    if (is_thick || (search_dir == CAVITIES && has_antipodean == false) )
        smallest_sphere.radius = FLT_MAX;

    if (cpos == CENTERED_ON_TRIANGLE)
    {
        tin.t_epsilons.at(cid_tin) = convert_tet_tin_sphere(smallest_sphere);

        if (smallest_sphere.center_pos == CENTERED_ON_VERTEX)
        {
            epsilons_v_map.at(tin_vertex_from_tet(smallest_sphere.center_id)) = smallest_sphere.radius;
            epsilons_antipoeads.at(tin_vertex_from_tet(smallest_sphere.center_id)) = smallest_sphere.tangent;
        }
    }
    else
    if (cpos == CENTERED_ON_VERTEX)
    {
       epsilons_v_map.at(cid_tin) = smallest_sphere.radius;
       epsilons_antipoeads.at(cid_tin) = smallest_sphere.tangent;
    }

#ifdef CAXLIB

    if (max_epsilon > 0.0)
    {
        if (search_dir == THINWALLS && smallest_sphere.radius < max_epsilon)
        {
            tin.triangle_annotation(cid_tin).thin_walls = true;
        }
        else
        if (search_dir == CAVITIES && smallest_sphere.radius < max_epsilon)
        {
            tin.triangle_annotation(cid_tin).thin_channels = true;
        }
    }

#endif

    return smallest_sphere.radius;
}

/**
 * @brief Epsilon3DShape::compute_link
 * @param cpos
 * @param cid_tin
 * @param new_link_tets
 * @param link
 * @param v_labels
 * @param e_labels
 */
DANI_INLINE
void Epsilon3DShape::compute_link(const CenterPosition cpos, const uint cid_tin, const std::set<uint> &new_link_tets, SpherePriorityQueue &link)
{
    std::set<uint> new_tets = new_link_tets;

    if (cpos == CENTERED_ON_VERTEX)
    {
        uint tet_vid = tet_vertex_from_tin(cid_tin);

        for (uint tet : new_tets)
        {
            for (uint tid_off = 0; tid_off < 4; tid_off++)
            {
                uint v0 = tetmesh.tet_vertex_id(tet, TET_FACES[tid_off][0]);
                uint v1 = tetmesh.tet_vertex_id(tet, TET_FACES[tid_off][1]);
                uint v2 = tetmesh.tet_vertex_id(tet, TET_FACES[tid_off][2]);

                if (v0 == tet_vid || v1 == tet_vid || v2 == tet_vid)
                {
                    continue;
                }

                int adj_tet = tetmesh.adjacent_tet_through_facet(tet, tid_off);

                if (adj_tet == -1 || global_tet_labels.at(cid_tin).at(adj_tet) == false)
                {
                    Sphere t_sphere;
                    t_sphere.create_Vcentered_Ttangent(tetmesh, tet_vid, std::pair<uint, uint> (tet, tid_off));

                    link.insert(t_sphere);

                    // Edges

                    std::vector<int> edges;
                    edges.push_back(tetmesh.tet_edge_id(tet, v0, v1));
                    edges.push_back(tetmesh.tet_edge_id(tet, v1, v2));
                    edges.push_back(tetmesh.tet_edge_id(tet, v2, v0));

                    for (int eid : edges)
                    {
                        if (global_e_labels.at(cid_tin).at(eid) == true)
                            continue;

                        Sphere sphere_e;
                        sphere_e.create_Vcentered_Etangent(tetmesh, tet_vid, eid);
                        link.insert(sphere_e);

//                        if (is_tet_edge_on_tin_srf(eid))
//                        {
//                            // update with new spheres having radius perpendicular to the edge
//                            std::vector<Sphere> spheres = create_additional_ortogonal_spheres(sphere_e);

//                            for (Sphere sph : spheres)
//                                link.insert(sph);
//                        }

                        global_e_labels.at(cid_tin).at(eid) = true;
                    }

                    // Vertices

                    std::vector<int> vertices;

                    vertices.push_back(v0);
                    vertices.push_back(v1);
                    vertices.push_back(v2);

                    for (int vid : vertices)
                    {
                        if (global_v_labels.at(cid_tin).at(vid) == true)
                            continue;

                        Sphere sphere_v;
                        sphere_v.create_Vcentered_Vtangent(tetmesh, tet_vid, vid);
                        link.insert(sphere_v);

//                        if (is_tet_vertex_on_tin_srf(vid))
//                        {
//                            // update with new spheres having radius perpendicular to the edge
//                            std::vector<Sphere> spheres = create_additional_ortogonal_spheres(sphere_v);

//                            for (Sphere sph : spheres)
//                                link.insert(sph);
//                        }

                        global_v_labels.at(cid_tin).at(vid) = true;

                    }
                }
            }
        }
    }
    else
    if (cpos == CENTERED_ON_TRIANGLE)
    {
        std::pair<uint,uint> tid_tet = tet_triangle_from_tin(cid_tin);

        uint tv0 = tetmesh.tet_vertex_id(tid_tet.first, TET_FACES[tid_tet.second][0]);
        uint tv1 = tetmesh.tet_vertex_id(tid_tet.first, TET_FACES[tid_tet.second][1]);
        uint tv2 = tetmesh.tet_vertex_id(tid_tet.first, TET_FACES[tid_tet.second][2]);

        for (uint tet : new_tets)
        {
            for (uint tid_off = 0; tid_off < 4; tid_off++)
            {
                uint v0 = tetmesh.tet_vertex_id(tet, TET_FACES[tid_off][0]);
                uint v1 = tetmesh.tet_vertex_id(tet, TET_FACES[tid_off][1]);
                uint v2 = tetmesh.tet_vertex_id(tet, TET_FACES[tid_off][2]);

                if ((v0 == tv0 || v0 == tv1 || v0 == tv2) ||
                    (v1 == tv0 || v1 == tv1 || v1 == tv2) ||
                    (v2 == tv0 || v2 == tv1 || v2 == tv2) )
                {
                    continue;
                }

                int adj_tet = tetmesh.adjacent_tet_through_facet(tet, tid_off);

                if (adj_tet == -1 || global_tet_labels.at(cid_tin).at(adj_tet) == false)
                {
                    Sphere t_sphere;
                    t_sphere.create_Tcentered_Ttangent(tetmesh, tid_tet, std::pair<uint, uint> (tet, tid_off));

                    link.insert(t_sphere);

                    // Edges

                    std::vector<int> edges;
                    edges.push_back(tetmesh.tet_edge_id(tet, v0, v1));
                    edges.push_back(tetmesh.tet_edge_id(tet, v1, v2));
                    edges.push_back(tetmesh.tet_edge_id(tet, v2, v0));

                    for (int eid : edges)
                    {
                        if (global_e_labels.at(cid_tin).at(eid) == true)
                            continue;

                        Sphere sphere_e;
                        sphere_e.create_Tcentered_Etangent(tetmesh, tid_tet, eid);
                        link.insert(sphere_e);

                        if (is_tet_edge_on_tin_srf(eid))
                        {
                            // update with new spheres having radius perpendicular to the edge
                            std::vector<Sphere> spheres = create_additional_ortogonal_spheres(sphere_e);

                            for (Sphere sph : spheres)
                                link.insert(sph);
                        }

                        global_e_labels.at(cid_tin).at(eid) = true;
                    }

                    // Vertices

                    std::vector<int> vertices;

                    vertices.push_back(v0);
                    vertices.push_back(v1);
                    vertices.push_back(v2);

                    for (int vid : vertices)
                    {
                        if (global_v_labels.at(cid_tin).at(vid) == true)
                            continue;

                        Sphere sphere_v;
                        sphere_v.create_Tcentered_Vtangent(tetmesh, tid_tet, vid);
                        link.insert(sphere_v);

                        if (is_tet_vertex_on_tin_srf(vid))
                        {
                            // update with new spheres having radius perpendicular to the edge
                            std::vector<Sphere> spheres = create_additional_ortogonal_spheres(sphere_v);

                            for (Sphere sph : spheres)
                                link.insert(sph);
                        }

                        global_v_labels.at(cid_tin).at(vid) = true;

                    }
                }
            }
        }
    }
    else
    {
        std::cerr << "[ERROR] Epsilon cannot be computed for a specific edge." << std::endl;
        exit(1);
    }
}

DANI_INLINE
void Epsilon3DShape::compute_link_tetmesh(const CenterPosition cpos, const uint cid, std::set<uint> &link_tets)
{
    if (cpos == CENTERED_ON_VERTEX)
    {
        for (uint tet : tetmesh.adj_vtx2tet(cid))
        {
            if (global_tet_labels.at(cid).at(tet) == true)
                continue;

            if ((search_dir == BOTH) ||
                (search_dir == THINWALLS && in_tet_labels.at(tet) == true) ||
                (search_dir == CAVITIES  && in_tet_labels.at(tet) == false) )
            {
                link_tets.insert(tet);
                global_tet_labels.at(cid).at(tet) = true;
            }
        }
    }
    else
    if (cpos == CENTERED_ON_EDGE)
    {
        for (uint vid_off = 0; vid_off < 2; vid_off++)
        {
            for (uint tet : tetmesh.adj_vtx2tet(tetmesh.edge_vertex_id(e_map_tin_tet.at(cid), vid_off)))
            {
                if (global_tet_labels.at(cid).at(tet) == true)
                    continue;


                if ((search_dir == BOTH) ||
                    (search_dir == THINWALLS && in_tet_labels.at(tet) == true) ||
                    (search_dir == CAVITIES  && in_tet_labels.at(tet) == false) )
                {
                    link_tets.insert(tet);
                    global_tet_labels.at(cid).at(tet) = true;
                }

            }
        }
    }
    else
    if (cpos == CENTERED_ON_TRIANGLE)
    {
        uint tet     = t_map_tin_tet.at(cid)[0].first;
        uint tid_off = t_map_tin_tet.at(cid)[0].second;

        std::vector<uint> t_vertices;
        t_vertices.push_back(tetmesh.tet_vertex_id(tet, TET_FACES[tid_off][0]));
        t_vertices.push_back(tetmesh.tet_vertex_id(tet, TET_FACES[tid_off][1]));
        t_vertices.push_back(tetmesh.tet_vertex_id(tet, TET_FACES[tid_off][2]));

        for (uint vid : t_vertices)
        {
            for (uint tet : tetmesh.adj_vtx2tet(vid))
            {
                if (global_tet_labels.at(cid).at(tet) == true)
                    continue;

                if ((search_dir == BOTH) ||
                    (search_dir == THINWALLS && in_tet_labels.at(tet) == true) ||
                    (search_dir == CAVITIES  && in_tet_labels.at(tet) == false) )
                {

                    link_tets.insert(tet);
                    global_tet_labels.at(cid).at(tet) = true;
                }
            }
        }
    }
    else
        assert (false);
}


DANI_INLINE
void Epsilon3DShape::update_link_tets (const CenterPosition cpos,
                                       const uint cid,
                                       std::set<uint> &new_link_tets,
                                       const Sphere &sphere)
{

    if (sphere.tangent_pos == TANGENT_ON_VERTEX)
    {
        for (uint tet : tetmesh.adj_vtx2tet(sphere.tangent_id))
        {
            //if (tet_labels.at(tet) == true)
            //    continue;

            if ((search_dir == BOTH) ||
                (search_dir == THINWALLS && in_tet_labels.at(tet) == true) ||
                (search_dir == CAVITIES  && in_tet_labels.at(tet) == false) )
            {
                //link_tets.insert(tet);

                if (global_tet_labels.at(cid).at(tet) == false)
                {
                    global_tet_labels.at(cid).at(tet) = true;
                    new_link_tets.insert(tet);
                }
            }

            //tet_labels.at(tet) = true;
        }
    }
    else
    if (sphere.tangent_pos == TANGENT_ON_EDGE)
    {
        for (uint tet : tetmesh.adj_edg2tet(sphere.tangent_id))
        {
            //if (tet_labels.at(tet) == true)
            //    continue;

            if ((search_dir == BOTH) ||
                (search_dir == THINWALLS && in_tet_labels.at(tet) == true) ||
                (search_dir == CAVITIES  && in_tet_labels.at(tet) == false) )
            {
                //link_tets.insert(tet);

                if (global_tet_labels.at(cid).at(tet) == false)
                {
                    global_tet_labels.at(cid).at(tet) = true;
                    new_link_tets.insert(tet);
                }
            }

        }
    }
    else
    if (sphere.tangent_pos == TANGENT_ON_TRIANGLE)
    {
        int adj_tet = tetmesh.adjacent_tet_through_facet(sphere.tangent_tri.first, sphere.tangent_tri.second);

        if (adj_tet != -1 /*&& tet_labels.at(adj_tet) == false*/)
        {
            if ((search_dir == BOTH) ||
                (search_dir == THINWALLS && in_tet_labels.at(adj_tet) == true) ||
                (search_dir == CAVITIES  && in_tet_labels.at(adj_tet) == false) )
            {
                //link_tets.insert(adj_tet);

                if (global_tet_labels.at(cid).at(adj_tet) == false)
                {
                    global_tet_labels.at(cid).at(adj_tet) = true;
                    new_link_tets.insert(adj_tet);
                }
            }

            //tet_labels.at(adj_tet) = true;
        }
    }
    else
        assert(false);

}

DANI_INLINE
uint Epsilon3DShape::tin_edge_having_endpoints(const uint vid0, const uint vid1) const
{
    for (uint eid : tin.adj_vtx2edg(vid0))
    {
        if (tin.edge_contains_vertex(eid, vid1))
            return eid;
    }

    assert (false);
}

DANI_INLINE
bool Epsilon3DShape::check_center_on_tin_srf (const Sphere &sphere)
{
    if (sphere.center_pos == CENTERED_ON_VERTEX)
    {
        if (tin_vertex_from_tet(sphere.center_id) != UINT_MAX)
            return true;

        return false;
    }
    else
    if (sphere.center_pos == CENTERED_ON_EDGE)
    {
        if (tin_edge_from_tet(sphere.center_id) != UINT_MAX)
            return true;

        return false;
    }
    else
    if (sphere.center_pos == CENTERED_ON_TRIANGLE)
    {
        if (tin_triangle_from_tet(sphere.center_tri) != UINT_MAX)
            return true;

        return false;
    }
    else
        assert(false);
}

DANI_INLINE
Sphere Epsilon3DShape::convert_tet_tin_sphere (const Sphere &sphere) const
{
    Sphere sph = sphere;

    sph.id_ref = REFERENCE_TO_TIN;

    // center info

    if (sphere.center_pos == CENTERED_ON_VERTEX)
    {
        sph.center_id = tin_vertex_from_tet(sphere.center_id);
    }
    else
    if (sphere.center_pos == CENTERED_ON_EDGE)
    {
        sph.center_id = tin_edge_from_tet(sphere.center_id);
    }
    else
    if (sphere.center_pos == CENTERED_ON_TRIANGLE)
    {
        sph.center_id = tin_triangle_from_tet(sphere.center_tri);
    }
    else assert(false);

    sph.center_tri.first = tin_triangle_from_tet(sphere.center_tri);

    //tangent info

    if (sphere.tangent_pos == TANGENT_ON_VERTEX)
    {
        sph.tangent_id = tin_vertex_from_tet(sphere.tangent_id);
    }
    else
    if (sphere.tangent_pos == TANGENT_ON_EDGE)
    {
        sph.tangent_id = tin_edge_from_tet(sphere.tangent_id);
    }
    else
    if (sphere.tangent_pos == TANGENT_ON_TRIANGLE)
    {
        sph.tangent_id = tin_triangle_from_tet(sphere.tangent_tri);
        sph.tangent_tri.first = tin_triangle_from_tet(sphere.tangent_tri);
    }
    else assert(false);

    if (sphere.tangent_tri.first != UINT_MAX)
    {
        sph.tangent_tri.first = tin_triangle_from_tet(sphere.tangent_tri);
    }

    return sph;
}

/**
 * @brief Epsilon3DShape::create_updated_spheres
 * Creates a vector of epsilon spheres. Each sphere is the last sphere not changing the topology
 * wrt one of the triangles adjacent to the tangent point of the input sphere.
 * @param sphere
 * @return a vector of epsilon spheres.
 */
DANI_INLINE
std::vector<Sphere> Epsilon3DShape::create_additional_ortogonal_spheres(const Sphere &sphere)
{
#ifdef GEOMETRICTOOLS

    std::vector<Sphere> new_spheres;

    if (sphere.tangent_pos == TANGENT_ON_TRIANGLE)
        return new_spheres;

    int tin_id = -1;
    std::vector<int> triangles;

    if (sphere.tangent_ref == TANGENT_REFERENCE_ON_VERTEX)
    {
        tin_id = tin_vertex_from_tet(sphere.tangent_ref_id);
        triangles = tin.adj_vtx2tri(tin_id);
    }
    else
    if (sphere.tangent_ref == TANGENT_REFERENCE_ON_EDGE)
    {
        tin_id = tin_edge_from_tet(sphere.tangent_ref_id);
        triangles = tin.adj_edg2tri(tin_id);
    }

    gte::Vector3<double> origin = get_gte_vertex(sphere.tangent);

    vec3d v0, v1, v2;
    uint vid0, vid1, vid2;

    vid0 = tetmesh.tet_vertex_id(sphere.center_tri.first, TET_FACES[sphere.center_tri.second][0]);
    vid1 = tetmesh.tet_vertex_id(sphere.center_tri.first, TET_FACES[sphere.center_tri.second][1]);
    vid2 = tetmesh.tet_vertex_id(sphere.center_tri.first, TET_FACES[sphere.center_tri.second][2]);

    v0 = tetmesh.vertex(vid0);
    v1 = tetmesh.vertex(vid1);
    v2 = tetmesh.vertex(vid2);

    gte::Triangle3<double> triangle = get_gte_triangle(v0, v1, v2);

    for (uint tid : triangles)
    {
        vec3d t_norm = tin.triangle_normal(tid);

        gte::Vector3<double> dir;

        dir = get_gte_vertex(t_norm);

        gte::Line3<double> line (origin, dir);

        // calcola intersezione con il top simplex di riferimento (triangolo)
        gte::FIQuery<double, gte::Line3<double>, gte::Triangle3<double>> query;
        gte::FIQuery<double, gte::Line3<double>, gte::Triangle3<double>>::Result result = query(line, triangle);

        if(result.intersect == false)
            continue;

        // il punto di intersezione ÃƒÂ¨ il nuovo centro

        Sphere updated_sphere = sphere;

        updated_sphere.center[0] = result.point[0];
        updated_sphere.center[1] = result.point[1];
        updated_sphere.center[2] = result.point[2];

        PointPosition pp = is_point_on_triangle(updated_sphere.center, v0, v1, v2);

        if (pp == EXTERNAL_POINT)
            continue;

        if (pp == POINT_TRIANGLE_P0)
        {
            updated_sphere.center_pos = CENTERED_ON_VERTEX;
            updated_sphere.center_id = vid0;
        }
        else
        if (pp == POINT_TRIANGLE_P1)
        {
            updated_sphere.center_pos = CENTERED_ON_VERTEX;
            updated_sphere.center_id = vid1;
        }
        else
        if (pp == POINT_TRIANGLE_P2)
        {
            updated_sphere.center_pos = CENTERED_ON_VERTEX;
            updated_sphere.center_id = vid2;
        }
        else
        if (pp == POINT_TRIANGLE_E0)
        {
            updated_sphere.center_pos = CENTERED_ON_EDGE;

            uint tin_v0 = tin_vertex_from_tet(vid0);
            uint tin_v1 = tin_vertex_from_tet(vid1);

            uint tin_eid = tin.edge_local_to_global(tin_v0, tin_v1);

            updated_sphere.center_id = tet_edge_from_tin(tin_eid);
        }
        else
        if (pp == POINT_TRIANGLE_E1)
        {
            updated_sphere.center_pos = CENTERED_ON_EDGE;

            uint tin_v1 = tin_vertex_from_tet(vid1);
            uint tin_v2 = tin_vertex_from_tet(vid2);

            uint tin_eid = tin.edge_local_to_global(tin_v1, tin_v2);

            updated_sphere.center_id = tet_edge_from_tin(tin_eid);
        }
        else
        if (pp == POINT_TRIANGLE_E2)
        {
            updated_sphere.center_pos = CENTERED_ON_EDGE;

            uint tin_v2 = tin_vertex_from_tet(vid2);
            uint tin_v0 = tin_vertex_from_tet(vid0);

            uint tin_eid = tin.edge_local_to_global(tin_v2, tin_v0);

            updated_sphere.center_id = tet_edge_from_tin(tin_eid);
        }
        else
        if (pp == POINT_TRIANGLE)
        {
            updated_sphere.center_pos = CENTERED_ON_TRIANGLE;
        }
//        else
//        {
//            double dist = VT_distance(updated_sphere.center, v0, v1, v2);
//            std::cout << "[ERROR] Distance : " << dist << std::endl;

//            assert(false);
//        }

        // la distanza tra il centro e il vertice di tangenza ÃƒÂ¨ il valore di epsilon
        updated_sphere.radius = updated_sphere.center.dist(updated_sphere.tangent);

        updated_sphere.tangent_tri = tet_triangle_from_tin(tid);

        //updated_sphere.description = __FUNCTION__ ;
        updated_sphere.description = "ortogonal_vertex_or_edge";
        updated_sphere.status = EPSILON_SPHERE;
        updated_sphere.ortogonal_to_tin_triangle = tid;

        new_spheres.push_back(updated_sphere);
    }

    if (sphere.tangent_ref == TANGENT_REFERENCE_ON_EDGE)
    {
        gte::Vector3<double> origin1 = get_gte_vertex(tin.edge_vertex(tin_id, 0));
        gte::Vector3<double> origin2 = get_gte_vertex(tin.edge_vertex(tin_id, 1));

        for (uint tt : triangles)
        {
            Sphere updated_sphere = sphere;

            vec3d t_norm = tin.triangle_normal(tt);

            //gte::Vector3<double> dir = get_gte_vertex(t_norm);
            gte::Vector3<double> third_v = get_gte_vertex(tin.edge_vertex(tin_id, 1) + t_norm);

            std::array<gte::Vector<3, double>, 3> points;
            points.at(0) = origin1;
            points.at(1) = origin2;
            points.at(2) = third_v;

            gte::Plane3<double> plane (points);

            // calcola intersezione con il top simplex di riferimento (triangolo)
            gte::FIQuery<double, gte::Plane3<double>, gte::Triangle3<double>> query;
            gte::FIQuery<double, gte::Plane3<double>, gte::Triangle3<double>>::Result result = query(plane, triangle);

            if (result.intersect == false)
                continue;

            if (result.numIntersections == 1)
            {
                //std::cout << " single point intersection" << std::endl;

                vec3d point;
                point[0] = result.point[0][0];
                point[1] = result.point[0][1];
                point[2] = result.point[0][2];

                updated_sphere.center = point;

                vec3d v = tin.edge_vertex(tin_id, 1) - tin.edge_vertex(tin_id, 0);
                vec3d w = point - tin.edge_vertex(tin_id, 0);

                double cos_wv = w.dot(v);
                double cos_vv = v.dot(v);

                if (cos_wv <= 0.0 || cos_vv <= cos_wv)
                {
                    if (cos_wv <= 0.0) // before P0
                    {
                        updated_sphere.set_tangent_on_vertex(tet_vertex_from_tin(tin.edge_vertex_id(tin_id, 0)));

                        updated_sphere.tangent = tin.edge_vertex(tin_id, 0);
                    }
                    else
                    if (cos_vv <= cos_wv) // after P1
                    {
                        updated_sphere.set_tangent_on_vertex(tet_vertex_from_tin(tin.edge_vertex_id(tin_id, 1)));

                        updated_sphere.tangent = tin.edge_vertex(tin_id, 1);
                    }
                }
                else
                {
                    double b  = cos_wv / cos_vv;
                    vec3d Pb = tin.edge_vertex(tin_id, 0) + v*b;

                    updated_sphere.tangent = Pb;
                    //set_tangent_on_edge(eid);
                    //radius = (Pb - tetmesh.vertex(vid)).length();
                }
            }
            else
            if (result.numIntersections == 2)
            {
                //std::cout << "segment intersection : " << std::endl;

                gte::Vector3<double> p0 = result.point[0];
                gte::Vector3<double> p1 = result.point[1];

                gte::Segment3<double> c_segment (p0, p1);
                gte::Segment3<double> t_segment (get_gte_vertex(tin.edge_vertex(tin_id, 0)), get_gte_vertex(tin.edge_vertex(tin_id, 1)));

                gte::DCPQuery<double, gte::Segment3<double>, gte::Segment3<double>> query;
                gte::DCPQuery<double, gte::Segment3<double>, gte::Segment3<double>>::Result result = query (c_segment, t_segment);

                updated_sphere.center[0] = result.closest[0][0];
                updated_sphere.center[1] = result.closest[0][1];
                updated_sphere.center[2] = result.closest[0][2];

                updated_sphere.tangent[0] = result.closest[1][0];
                updated_sphere.tangent[1] = result.closest[1][1];
                updated_sphere.tangent[2] = result.closest[1][2];

                PointPosition pp_c = is_point_on_triangle(updated_sphere.center, v0, v1, v2);

                if (pp_c == EXTERNAL_POINT)
                {
                    vec3d tmp = updated_sphere.center;
                    updated_sphere.center = updated_sphere.tangent;
                    updated_sphere.tangent = tmp;
                }

                pp_c = is_point_on_triangle(updated_sphere.center, v0, v1, v2);

                //assert (pp_c != EXTERNAL_POINT);
            }

            PointPosition pp = is_point_on_triangle(updated_sphere.center, v0, v1, v2);

            if (pp == EXTERNAL_POINT)
            {
                vec3d tmp = updated_sphere.center;
                updated_sphere.center = updated_sphere.tangent;
                updated_sphere.tangent = tmp;
            }

            pp = is_point_on_triangle(updated_sphere.center, v0, v1, v2);

            if (pp == POINT_TRIANGLE_P0)
            {
                updated_sphere.center_pos = CENTERED_ON_VERTEX;
                updated_sphere.center_id = vid0;
            }
            else
            if (pp == POINT_TRIANGLE_P1)
            {
                updated_sphere.center_pos = CENTERED_ON_VERTEX;
                updated_sphere.center_id = vid1;
            }
            else
            if (pp == POINT_TRIANGLE_P2)
            {
                updated_sphere.center_pos = CENTERED_ON_VERTEX;
                updated_sphere.center_id = vid2;
            }
            else
            if (pp == POINT_TRIANGLE_E0)
            {
                updated_sphere.center_pos = CENTERED_ON_EDGE;

                uint tin_v0 = tin_vertex_from_tet(vid0);
                uint tin_v1 = tin_vertex_from_tet(vid1);

                uint tin_eid = tin.edge_local_to_global(tin_v0, tin_v1);

                updated_sphere.center_id = tet_edge_from_tin(tin_eid);
            }
            else
            if (pp == POINT_TRIANGLE_E1)
            {
                updated_sphere.center_pos = CENTERED_ON_EDGE;

                uint tin_v1 = tin_vertex_from_tet(vid1);
                uint tin_v2 = tin_vertex_from_tet(vid2);

                uint tin_eid = tin.edge_local_to_global(tin_v1, tin_v2);

                updated_sphere.center_id = tet_edge_from_tin(tin_eid);
            }
            else
            if (pp == POINT_TRIANGLE_E2)
            {
                updated_sphere.center_pos = CENTERED_ON_EDGE;

                uint tin_v2 = tin_vertex_from_tet(vid2);
                uint tin_v0 = tin_vertex_from_tet(vid0);

                uint tin_eid = tin.edge_local_to_global(tin_v2, tin_v0);

                updated_sphere.center_id = tet_edge_from_tin(tin_eid);
            }
            else
            if (pp == POINT_TRIANGLE)
            {
                updated_sphere.center_pos = CENTERED_ON_TRIANGLE;
            }
            else
                assert(false);

            // la distanza tra il centro e il vertice di tangenza ÃƒÂ¨ il valore di epsilon
            updated_sphere.radius = updated_sphere.center.dist(updated_sphere.tangent);

            updated_sphere.tangent_tri = tet_triangle_from_tin(tt);

            //updated_sphere.description = __FUNCTION__;
            updated_sphere.description = "ortogonal_edge";
            updated_sphere.status = EPSILON_SPHERE;
            updated_sphere.ortogonal_to_tin_triangle = tt;

            new_spheres.push_back(updated_sphere);
        }
    }

    return new_spheres;

#else

    std::cerr << "" << std::endl;
    exit(1);

#endif
}


/**
 * @brief Epsilon3DShape::is_saddle_vertex.
 * Checks if a vertex is saddle.
 * Implemented according to the definition in
 * http://doc.cgal.org/latest/Surface_mesh_shortest_path/index.html#Surface_mesh_shortest_pathSaddleVertex (Section 5.1.3)
 *
 * @param vid the id of the vertex referred to the trimesh
 * @return true if the vertex is saddle; false otherwise.
 */
DANI_INLINE
bool Epsilon3DShape::is_saddle_vertex(const uint vid) const
{
    double ta = 0.0;

    for (uint tid : tin.adj_vtx2tri(vid))
    {
        //uint leftTriangle = (tin.edge_vertex_id(eid, 0) == vid) ? tin.adj_edg2tri(eid).at(0) : tin.adj_edg2tri(eid).at(1);

        ta += get_angle_TV(tid, vid);
    }

    if (ta > 2*M_PI)
        return true;

    return false;
}

/**
 * @brief Epsilon3DShape::get_angle_TV
 * @param tid
 * @param vid
 * @return
 */
DANI_INLINE
double Epsilon3DShape::get_angle_TV (const uint tid, const uint vid) const
{
    uint va = tin.triangle_vertex_id(tid, 0);
    uint vb = tin.triangle_vertex_id(tid, 1);
    uint vc = tin.triangle_vertex_id(tid, 2);

    if (vid == va) return get_angle_VVV(vid, vb, vc);
    if (vid == vb) return get_angle_VVV(vid, va, vc);
}

/**
 * @brief Epsilon3DShape::get_angle_VVV
 * Computes the angle defined by <vid0, vid, vid1> in radians.
 * @param vid ID of central vertex referred to trimesh
 * @param vid0 ID of the first extreme vertex referred to trimesh
 * @param vid1 ID of the second extreme vertex referred to trimesh
 * @return the angle in radians
 */
DANI_INLINE
double Epsilon3DShape::get_angle_VVV(const uint vid, const uint vid0, const uint vid1) const
{
    return get_angle_VV(tin.vertex(vid0) - tin.vertex(vid), tin.vertex(vid1) - tin.vertex(vid));
}


/**
 * @brief Epsilon3DShape::get_angle_VVV
 * Computes the angle defined by <vid0, vid, vid1> in radians.
 * @param vid ID of central vertex referred to trimesh
 * @param vid0 ID of the first extreme vertex referred to trimesh
 * @param vid1 ID of the second extreme vertex referred to trimesh
 * @return the angle in radians
 */
DANI_INLINE
double Epsilon3DShape::get_angle_VVV(const vec3d vid, const vec3d vid0, const vec3d vid1) const
{
    return get_angle_VV(vid0 - vid, vid1 - vid);
}

/**
 * @brief Epsilon3DShape::get_angle_VV
 * @param v0
 * @param v1
 * @return
 */
DANI_INLINE
double Epsilon3DShape::get_angle_VV(const vec3d v0, const vec3d v1) const
{
    double ac, l = (v0.length()*(v1.length()));

     //if (l == 0.0) {JMesh::warning("getAngle : One or both vectors are null !\n"); return -1.0;}

     //ac = ((*this)*p)/l;

    ac = (v0.x()*v1.x() + v0.y()*v1.y() + v0.z()*v1.z())/l;

    if ((std::fabs((ac - 1.0))) < 0.0001)
        ac = 0.0;
    else
    if ((std::fabs((1.0 + ac))) < 0.0001)
        ac = M_PI;
    else
        ac = acos(ac);

    return ac;
}


/**
 * @brief Epsilon3DShape::get_dihedral_angle
 * @param tin_eid
 * @return
 */
DANI_INLINE
double Epsilon3DShape::get_dihedral_angle (const uint tin_eid)
{
    uint t0 = tin.adj_edg2tri(tin_eid).at(0);
    uint t1 = tin.adj_edg2tri(tin_eid).at(1);

    vec3d nor1 = tin.triangle_normal(t0);//t1->getNormal();
    vec3d nor2 = tin.triangle_normal(t1); //t2->getNormal();

    //if (nor1.isNull() || nor2.isNull()) return -1.0;

    double c = get_angle_VV(nor1, nor2); //nor1.getAngle(&nor2);

    vec3d ov = tin.vertex_opposite_to(t1, tin.edge_vertex_id(tin_eid, 0), tin.edge_vertex_id(tin_eid, 1));
    //Vertex *ov = t2->oppositeVertex(this);

    //if (((*ov)*nor1) - ((*v1)*nor1) < 0)

    if ((ov * nor1) - (tin.edge_vertex(tin_eid, 0) * nor1) < 0)
        return -(c - M_PI);

    return c+M_PI;
}

DANI_INLINE
double Epsilon3DShape::get_dihedral_angle (Trimesh &local_tin, const uint tin_eid)
{
    uint t0 = local_tin.adj_edg2tri(tin_eid).at(0);
    uint t1 = local_tin.adj_edg2tri(tin_eid).at(1);

    vec3d nor1 = local_tin.triangle_normal(t0);//t1->getNormal();
    vec3d nor2 = local_tin.triangle_normal(t1); //t2->getNormal();

    //if (nor1.isNull() || nor2.isNull()) return -1.0;

    double c = get_angle_VV(nor1, nor2); //nor1.getAngle(&nor2);

    vec3d ov = local_tin.vertex_opposite_to(t1, local_tin.edge_vertex_id(tin_eid, 0), local_tin.edge_vertex_id(tin_eid, 1));
    //Vertex *ov = t2->oppositeVertex(this);

    //if (((*ov)*nor1) - ((*v1)*nor1) < 0)

    if ((ov * nor1) - (local_tin.edge_vertex(tin_eid, 0) * nor1) < 0)
        return -(c - M_PI);

    return c+M_PI;
}

/**
 * @brief Epsilon3DShape::are_dihedral_vertex_angle_greater_than
 * @param thresh
 * @return
 */
DANI_INLINE
bool Epsilon3DShape::are_dihedral_angles_greater_than (const double thresh)
{
    std::cout << "Checking dihedral angles ... " ; //<< std::endl;

    for (uint eid = 0; eid < tin.num_edges(); eid++)
    {
        double angle = get_dihedral_angle(eid);

        if (angle < thresh)
            std:: cout << std::endl << " [INVALID ANGLE] Edge " << eid << " : " << angle ;
        else
            std:: cout << std::endl << " [OK ANGLE] Edge " << eid << " : " << angle ;
    }

    std::cout << std:: endl << std::endl;


    return true;
}

/**
 * @brief Epsilon3DShape::are_dihedral_vertex_angle_greater_than
 * @param thresh
 * @return
 */
DANI_INLINE
bool Epsilon3DShape::are_solid_vertex_angle_greater_than (const double thresh)
{
    std::cout << "Checking solid angles ... " ; //<< std::endl;

    for (uint vid = 0; vid < tin.num_vertices(); vid++ )
    {
        double sum = 0.0;

        uint internal_edges = 0;

        uint tet_vid = tet_vertex_from_tin(vid);

        for (uint tet_edg : tetmesh.adj_vtx2edg(tet_vid))
        {
            double local_sum = 0.0;

            if (is_tet_edge_on_tin_srf(tet_edg))
            {
                double dihedral = get_dihedral_angle(tin_edge_from_tet(tet_edg));

                local_sum += dihedral;
            }
            else
                internal_edges++;

            sum += local_sum;
        }

        sum = sum + (internal_edges * 2 * M_PI) - (tetmesh.adj_vtx2tet(tet_vid).size() * M_PI);

        if (sum < thresh)
            std:: cout << std::endl << " [INVALID ANGLE] Vertex " << vid << " : " << sum ;
        else
            std:: cout << std::endl << " [OK ANGLE] Vertex " << vid << " : " << sum ;
    }

    std::cout << std:: endl << std::endl;

    return true;
}


}
