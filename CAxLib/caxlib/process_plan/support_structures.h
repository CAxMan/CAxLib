/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2017.
*/

#ifndef SUPPORTS_H
#define SUPPORTS_H

#include "../caxlib.h"
#include "../trimesh/trimesh.h"
#include "../intersection.h"
#include "../vec2.h"
#include "../intersection.h"
#include "../triangle.h"


namespace caxlib
{

typedef std::pair<vec2d,vec2d> seg2d;
typedef struct
{
    int tid;                      // triangle containing the segment
    std::vector<double> bary_beg; // barycentric coords of first  endpoint (wrt tid)
    std::vector<double> bary_end; // barycentric coords of second endpoint (wrt tid)
}
tri_seg_inters;

typedef struct
{
    double ground_area = 0.0; // cumulative area of supports segments incident on a triangle (measured on the floor)
    double lifted_area = 0.0; // cumulative area of supports segments incident on a triangle (measured on the shape)
}
TriangleCumulativeSupportData;

typedef struct
{
    double ground_area_ratio = 0.0; // ratio between cumulative support area and total overhang area (P in Jaiko's email)
    double lifted_area_ratio = 0.0; // ratio between cumulative support area and total overhang area (p in Jaiko's email)
    double vertex_area       = 0.0; // (V in Jaiko's email)
    double support_height    = 0.0; // (D in Jaiko's email)
}
VertexCumulativeSupportData;

CAX_INLINE void project_overhangs(const std::vector<int> &, const float, Trimesh &, Trimesh &);
CAX_INLINE void draw_2d_support_lines(const Trimesh &, const int, std::vector<seg2d> &);
CAX_INLINE void split_2d_support_lines(const Trimesh &, const std::vector<seg2d> &, const double, std::vector<tri_seg_inters> &);
CAX_INLINE void extrude_supports(const std::vector<tri_seg_inters> &, const Trimesh &, const Trimesh &, const std::vector<int> &, const double, std::vector<VertexCumulativeSupportData> &, Trimesh &);


CAX_INLINE
void create_support_structures(Trimesh & m,
                               const double thickness,
                               std::vector<VertexCumulativeSupportData> & per_vertex_supports_data)
{
    // ROTATE MESH ACCORDING TO BEST ORIENTATION (ANNOTATIONS)
    double R[3][3];
    for(int i=0; i<3; ++i)
    for(int j=0; j<3; ++j)
    {
        R[i][j] = m.global_annotations().orientation[3*i+j];
    }
    m.rotate(R);

    double z_floor = m.bbox().min.z() - (m.bbox().delta_z() * 0.1);

    std::vector<int> overhang_tids;
    m.get_overhangs(vec3d(0,0,1), 45.0, overhang_tids);

    Trimesh m_overhangs; // trimesh created projecting overhang triangles to the floor
    project_overhangs(overhang_tids, z_floor, m, m_overhangs);

    std::vector<seg2d> supp_lines;
    draw_2d_support_lines(m, 20, supp_lines);

    std::vector<tri_seg_inters> supp_segments;
    split_2d_support_lines(m_overhangs, supp_lines, z_floor, supp_segments);

    Trimesh m_supports;
    extrude_supports(supp_segments, m, m_overhangs, overhang_tids, thickness, per_vertex_supports_data, m_supports);

    m += m_supports;

    // invert matrix and put everything in the original global frame
    Eigen::Matrix3d eigenR;
    from_std_3x3_to_Eigen_3x3(R, eigenR);
    std::cout << eigenR << std::endl;
    eigenR = eigenR.inverse().eval();
    from_eigen_3x3_to_std_3x3(eigenR, R);
    m.rotate(R);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CAX_INLINE
void project_overhangs(const std::vector<int> & overhang_tids,
                       const float              z_floor,
                             Trimesh          & m,
                             Trimesh          & m_overhangs)
{
    std::vector<double> coords;
    std::vector<uint>   tris;

    for(int tid : overhang_tids)
    {
        int base = coords.size()/3;
        tris.push_back(base+0);
        tris.push_back(base+1);
        tris.push_back(base+2);

        for(int i=0; i<3; ++i)
        {
            vec3d p = m.triangle_vertex(tid,i);
            coords.push_back(p.x());
            coords.push_back(p.y());
            coords.push_back(z_floor);
        }
    }

    m_overhangs = Trimesh(coords, tris);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CAX_INLINE
void draw_2d_support_lines(const Trimesh            & m,
                           const int                  n_lines,
                                 std::vector<seg2d> & supp_lines)
{
    double min_y = m.bbox().min.y();
    double max_y = m.bbox().max.y();

    for(int i=0; i<n_lines; ++i)
    {
        double x = m.bbox().min.x() + double(i)*m.bbox().delta_x()/double(n_lines);
        supp_lines.push_back(std::make_pair(vec2d(x,min_y),vec2d(x,max_y)));
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CAX_INLINE
void split_2d_support_lines(const Trimesh                     & m_overhangs,
                            const std::vector<seg2d>          & supp_lines,
                            const double                        z_floor,
                                  std::vector<tri_seg_inters> & supp_segments)
{
    for(seg2d s : supp_lines)
    {
        for(int tid=0; tid<m_overhangs.num_triangles(); ++tid)
        {
            std::set< std::vector<double> > unique_inters; // useful if the line passes through a segment

            for(int i=0; i<3; ++i)
            {
                vec3d beg = m_overhangs.triangle_vertex(tid, TRI_EDGES[i][0]);
                vec3d end = m_overhangs.triangle_vertex(tid, TRI_EDGES[i][1]);

                std::vector<vec2d> res;
                if (segment2D_intersection(s.first, s.second, vec2d(beg.x(), beg.y()), vec2d(end.x(), end.y()), res))
                {
                    for(vec2d p : res)
                    {
                        std::vector<double> bary;
                        bool check = triangle_barycentric_coords(m_overhangs.triangle_vertex(tid,0),
                                                                 m_overhangs.triangle_vertex(tid,1),
                                                                 m_overhangs.triangle_vertex(tid,2),
                                                                 vec3d(p.x(), p.y(), z_floor),
                                                                 bary);
                        assert(check);
                        unique_inters.insert(bary);
                    }
                }
            }

            if (unique_inters.size() == 2)
            {
                tri_seg_inters tmp;
                tmp.tid = tid;
                tmp.bary_beg = *(unique_inters.begin());
                tmp.bary_end = *(--(unique_inters.end()));
                supp_segments.push_back(tmp);
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CAX_INLINE
void extrude_supports(const std::vector<tri_seg_inters> & supp_segments,
                      const Trimesh             & m,
                      const Trimesh             & m_overhangs,
                      const std::vector<int>    & overhang_tids,
                      const double                thickness,
                       std::vector<VertexCumulativeSupportData> & per_vertex_supports_data,
                            Trimesh             & m_supports)
{
    std::vector<double> coords;
    std::vector<uint>    tris;

    std::vector<TriangleCumulativeSupportData> per_face_supports_data(m.num_triangles());
    double projected_overhang_area = 0.0; // S in Jaiko's email

    for(tri_seg_inters obj : supp_segments)
    {
        int og_tid = overhang_tids.at(obj.tid);

        vec3d A = m_overhangs.triangle_point_from_bary(obj.tid, obj.bary_beg);
        vec3d B = m_overhangs.triangle_point_from_bary(obj.tid, obj.bary_end);
        vec3d C = m.triangle_point_from_bary(og_tid, obj.bary_beg);
        vec3d D = m.triangle_point_from_bary(og_tid, obj.bary_end);

        int base = coords.size()/3;
        tris.push_back(base);
        tris.push_back(base + 1);
        tris.push_back(base + 2);
        tris.push_back(base + 1);
        tris.push_back(base + 3);
        tris.push_back(base + 2);

        coords.push_back(A.x()); coords.push_back(A.y()); coords.push_back(A.z());
        coords.push_back(B.x()); coords.push_back(B.y()); coords.push_back(B.z());
        coords.push_back(C.x()); coords.push_back(C.y()); coords.push_back(C.z());
        coords.push_back(D.x()); coords.push_back(D.y()); coords.push_back(D.z());

        projected_overhang_area += m_overhangs.element_mass(obj.tid);

        per_face_supports_data.at(og_tid).ground_area += thickness * A.dist(B);
        per_face_supports_data.at(og_tid).lifted_area += thickness * C.dist(D) * 0.3;
        // I am assuming to shape the lifted profile with tiny dents, which
        // cover 1/3 of the lifted segment. This is why I am doing *0.3...
    }

    m_supports = Trimesh(coords, tris);

    per_vertex_supports_data.resize(m.num_vertices());

    for(uint vid=0; vid<m.num_vertices(); ++vid)
    {
        per_vertex_supports_data.at(vid).vertex_area    = m.vertex_mass(vid);
        per_vertex_supports_data.at(vid).support_height = m.vertex(vid).z();

        double per_vert_ground_area = 0.0;
        double per_vert_lifted_area = 0.0;
        for(uint tid : m.adj_vtx2tri(vid))
        {
            per_vert_ground_area += per_face_supports_data.at(tid).ground_area;
            per_vert_lifted_area += per_face_supports_data.at(tid).lifted_area;
        }

        per_vertex_supports_data.at(vid).ground_area_ratio = per_vert_ground_area / projected_overhang_area;
        per_vertex_supports_data.at(vid).lifted_area_ratio = per_vert_lifted_area / projected_overhang_area;
    }

}

}

#endif // SUPPORTS_H
