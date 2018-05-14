/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2014.
*/

#ifndef TRIMESH_H
#define TRIMESH_H

#include <assert.h>
#include <float.h>
#include <vector>
#include <map>
#include <set>
#include <sys/types.h>

#include "../caxlib.h"
#include "../bbox.h"
#include "../vec3.h"
#include "../common.h"

#include "annotations.h"

namespace caxlib
{

static int TRI_EDGES[3][2] =
{
    { 0, 1 }, // 0
    { 1, 2 }, // 1
    { 2, 0 }, // 2
};

class Trimesh
{
    public:

        Trimesh(){}

        Trimesh(const char * filename);

        Trimesh(const std::vector<double> & coords,
                const std::vector<u_int>  & tris);

        Trimesh(const std::vector<double>              & coords,
                const std::vector<u_int>               & tris,
                const GlobalAnnotations                & glob_ann,
                const std::vector<VertexAnnotations>   & vertex_ann,
                const std::vector<TriangleAnnotations> & triangle_ann);

    protected:

        std::string filename;

        // bounding box
        //
        Bbox bb;

        // serialized xyz coordinates, triangles and edges
        //
        std::vector<double> coords;
        std::vector<u_int>  tris;
        std::vector<u_int>  edges;

        // per vertex/triangle normals
        //
        std::vector<double> v_norm;
        std::vector<double> t_norm;

        // general purpose float and int scalars
        //
        std::vector<float> u_text;  // per vertex: 1D texturing
        std::vector<int>   t_label; // per triangle

        // adjacencies
        //
        std::vector< std::vector<int> > vtx2vtx;
        std::vector< std::vector<int> > vtx2edg;
        std::vector< std::vector<int> > vtx2tri;
        std::vector< std::vector<int> > edg2tri;
        std::vector< std::vector<int> > tri2edg;
        std::vector< std::vector<int> > tri2tri;

        // Local and global annotations for CAxMan
        // see annotations.h
        //
        GlobalAnnotations                glob_ann;
        std::vector<VertexAnnotations>   vertex_ann;
        std::vector<TriangleAnnotations> triangle_ann;

        void load(const char * filename);

    public:

        //
        // BEGIN OF CAXMAN  UTILITIES
        //

        const GlobalAnnotations   & global_annotations() const { return glob_ann; }
              GlobalAnnotations   & global_annotations()       { return glob_ann; }

        const VertexAnnotations   & vertex_annotation(const int vid) const { return vertex_ann.at(vid); }
              VertexAnnotations   & vertex_annotation(const int vid)       { return vertex_ann.at(vid); }

        const TriangleAnnotations & triangle_annotation(const int tid) const { return triangle_ann.at(tid); }
              TriangleAnnotations & triangle_annotation(const int tid)       { return triangle_ann.at(tid); }

        //
        // Global Annotations handling
        //

        void set_orientation(const double m[9])
        {
            for(int i=0; i<9; ++i) glob_ann.orientation[i] = m[i];
        }

        void add_no_print_zone (const Bbox & pz)
        {
            glob_ann.printer.no_print_zones.push_back(pz);
        }

        bool  is_too_big  () const { return glob_ann.too_big;   }
        bool  is_too_heavy() const { return glob_ann.too_heavy; }

        //
        // Local VERTEX Annotations handling
        //

        // Get all the extra annotations of vertex VID.
        //
        std::set<std::string> get_vertex_extra_annotations (const u_int vid) const {return vertex_ann.at(vid).extra_annotations; }

        // Return true if vertex TID is annotated as ANN; false otherwise.
        //
        bool is_vertex_annotated_as (const u_int vid, const std::string ann) const {return (vertex_ann.at(vid).extra_annotations.find(ann) != vertex_ann.at(vid).extra_annotations.end()); }

        // Set ANN as an extra annotation of vertex TID.
        //
        void set_vertex_extra_annotation (const u_int vid, const std::string ann) {vertex_ann.at(vid).extra_annotations.insert(ann); }

        //
        // Local TRIANGLE Annotations handling
        //

        bool is_bad_geometry   (const int tid) const { return triangle_ann.at(tid).bad_geometry;   }
        bool is_bad_material   (const int tid) const { return triangle_ann.at(tid).bad_material;   }
        bool is_thin_walls     (const int tid) const { return triangle_ann.at(tid).thin_walls;     }
        bool is_thin_channels  (const int tid) const { return triangle_ann.at(tid).thin_channels;  }
        bool is_overhangs      (const int tid) const { return triangle_ann.at(tid).overhangs;      }
        bool is_weak_feature   (const int tid) const { return triangle_ann.at(tid).weak_feature;   }
        bool is_mach_allowance (const int tid) const { return triangle_ann.at(tid).mach_allowance; }

        // Return true if triangle TID belongs to a closed void; false otherwise.
        //
        bool is_closed_voids (const int tid) const { return (triangle_ann.at(tid).closed_voids != 0);   }

        // Get the closed void ID to which triangle TID belongs to.
        // If zero is returned, triangle TID does not belong to any cosed void.
        //
        u_int get_closed_void_label (const int tid) const { return triangle_ann.at(tid).closed_voids;   }

        // Get all the triangles that belong to closed void CID.
        //
        std::vector<u_int> get_closed_void_triangles (const u_int cid) const
        {
            std::vector<u_int> triangles;

            for (u_int i=0; i < triangle_ann.size(); i++)
            {
                if (triangle_ann.at(i).closed_voids == cid)
                    triangles.push_back(i);
            }

            return triangles;
        }

        // Get all the extra annotations of triangle TID.
        //
        std::set<std::string> get_triangle_extra_annotations (const u_int tid) const {return triangle_ann.at(tid).extra_annotations; }

        // Return true if triangle TID is annotated as ANN; false otherwise.
        //
        bool is_triangle_annotated_as (const int tid, const std::string ann) const {return (triangle_ann.at(tid).extra_annotations.find(ann) != triangle_ann.at(tid).extra_annotations.end()); }

        //
        void set_bad_geometry   (const int tid, const bool b) { triangle_ann.at(tid).bad_geometry = b;   }
        void set_bad_material   (const int tid, const bool b) { triangle_ann.at(tid).bad_material = b;   }
        void set_thin_walls     (const int tid, const bool b) { triangle_ann.at(tid).thin_walls     = b; }
        void set_thin_channels  (const int tid, const bool b) { triangle_ann.at(tid).thin_channels = b;  }
        void set_overhangs      (const int tid, const bool b) { triangle_ann.at(tid).overhangs = b;      }
        void set_weak_feature   (const int tid, const bool b) { triangle_ann.at(tid).weak_feature = b;   }
        void set_mach_allowance (const int tid, const bool b) { triangle_ann.at(tid).mach_allowance = b; }

        void set_closed_voids   (const int tid, const u_int b) { triangle_ann.at(tid).closed_voids = b;   }

        // Set ANN as an extra annotation of triangle TID.
        //
        void set_triangle_extra_annotation (const int tid, const std::string ann) {triangle_ann.at(tid).extra_annotations.insert(ann); }

        // returns the total overhang area as the sum of the area of the triangles
        // that need be supported (overhang_tris will contain the ids of each and every
        // triangle).
        //
        double get_overhangs(const vec3d      & build_dir,
                             const double       angle_thresh_deg,
                             std::vector<int> & overhang_tris) const;

        //
        // END OF CAXMAN  UTILITIES
        //

        void init();
        void clear();
        void update_adjacency();
        void update_t_normals();
        void update_v_normals();

        std::string loaded_file() const { return filename; }

        int num_vertices()  const { return coords.size()/3; }
        int num_triangles() const { return tris.size()  /3; }
        int num_elements()  const { return tris.size()  /3; }
        int num_edges()     const { return edges.size() /2; }

        const std::vector<int> & adj_vtx2vtx(const int vid) const { return vtx2vtx.at(vid); }
        const std::vector<int> & adj_vtx2edg(const int vid) const { return vtx2edg.at(vid); }
        const std::vector<int> & adj_vtx2tri(const int vid) const { return vtx2tri.at(vid); }
        const std::vector<int> & adj_edg2tri(const int eid) const { return edg2tri.at(eid); }
        const std::vector<int> & adj_tri2edg(const int tid) const { return tri2edg.at(tid); }
        const std::vector<int> & adj_tri2tri(const int tid) const { return tri2tri.at(tid); }

        std::vector<int> adj_vtx2vtx_ordered(const int vid) const;

        const std::vector<double> & vector_coords()    const { return coords; }
        const std::vector<u_int>  & vector_triangles() const { return tris;   }
        const std::vector<u_int>  & vector_edges()     const { return edges;  }
        const Bbox                & bbox()             const { return bb;     }

        const std::vector<float> & vector_v_float_scalar() const { return u_text; }
        const std::vector<int>   & vector_t_int_scalar() const { return t_label; }

        void save(const char * filename) const;

        void export_mesh (const char * filename) const;

        virtual int add_vertex(const vec3d & v, const float scalar = 0.0);
        virtual int add_triangle(const int vid0, const int vid1, const int vid2, const int scalar);

        void set_triangle(const int tid, const int vid0, const int vid1, const int vid2);

        std::set<int> vertex_n_ring(const int vid, const int n) const;

        double element_mass(const int tid) const;
        double vertex_mass (const int vid) const;

        std::vector<int>  get_boundary_vertices() const;
        std::vector<ipair> get_boundary_edges()    const;

        int vertex_opposite_to(const int tid, const int vid0, const int vid1) const;
        int vertex_opposite_to(const int eid, const int vid) const;
        int edge_opposite_to(const int tid, const int vid) const;

        double angle_at_vertex(const int tid, const int vid) const;

        double edge_length(const int eid) const;
        double avg_edge_length() const;
        double max_edge_length() const;
        double min_edge_length() const;

        vec3d triangle_vertex(const int tid, const int offset) const
        {
            int tid_ptr = tid * 3;
            int vid     = tris.at(tid_ptr+offset);
            int vid_ptr = vid * 3;
            return vec3d(coords.at(vid_ptr+0), coords.at(vid_ptr+1), coords.at(vid_ptr+2));
        }

        vec3d triangle_point_from_bary(const int tid, const std::vector<double> & wgts) const
        {
            vec3d p(0,0,0);
            assert(wgts.size()==3);
            for(int offset=0; offset<3; ++offset)
            {
                p += wgts.at(offset) * triangle_vertex(tid,offset);
            }
            return p;
        }


        bool edge_contains_vertex(const int eid, const int vid) const
        {
            if (edge_vertex_id(eid,0) == vid) return true;
            if (edge_vertex_id(eid,1) == vid) return true;
            return false;
        }

        vec3d edge_vertex(const int eid, const int offset) const
        {
            int eid_ptr = eid * 2;
            int vid     = edges[eid_ptr + offset];
            int vid_ptr = vid * 3;
            return vec3d(coords[vid_ptr + 0], coords[vid_ptr + 1], coords[vid_ptr + 2]);
        }

        bool vertex_is_border(const int vid) const; // has adjacent triangles with different per triangle iscalars

        bool vertex_is_boundary(const int vid) const; // is along an open boundary

        bool edge_is_manifold(const int eid) const
        {
            return (adj_edg2tri(eid).size() == 2);
        }

        bool edge_is_boundary(const int eid) const
        {
            return (adj_edg2tri(eid).size() < 2);
        }

        bool edge_is_border(const int eid) const
        {
            assert(edge_is_manifold(eid));

            int tid_0 = adj_edg2tri(eid)[0];
            int tid_1 = adj_edg2tri(eid)[1];

            return (triangle_label(tid_0) != triangle_label(tid_1));
        }

        bool triangle_is_boundary(const int tid) const
        {
            return (adj_tri2tri(tid).size() < 3);
        }

        int edge_vertex_id(const int eid, const int offset) const
        {
            int eid_ptr = eid * 2;
            return edges[eid_ptr + offset];
        }

        int triangle_vertex_id(const int tid, const int offset) const
        {
            int tid_ptr = tid * 3;
            return tris[tid_ptr + offset];
        }

        vec3d triangle_normal(const int tid) const
        {
            int tid_ptr = tid * 3;
            return vec3d(t_norm[tid_ptr + 0], t_norm[tid_ptr + 1], t_norm[tid_ptr + 2]);
        }

        vec3d vertex_normal(const int vid) const
        {
            int vid_ptr = vid * 3;
            return vec3d(v_norm[vid_ptr+0], v_norm[vid_ptr+1], v_norm[vid_ptr+2]);
        }

        vec3d vertex(const int vid) const
        {
            int vid_ptr = vid * 3;
            return vec3d(coords[vid_ptr+0], coords[vid_ptr+1], coords[vid_ptr+2]);
        }

        void set_vertex(const int vid, const vec3d & pos)
        {
            int vid_ptr = vid * 3;
            coords[vid_ptr + 0] = pos.x();
            coords[vid_ptr + 1] = pos.y();
            coords[vid_ptr + 2] = pos.z();
        }

        int vertex_valence(const int vid) const
        {
            return adj_vtx2vtx(vid).size();
        }

        float triangle_min_u_text(const int tid) const
        {
            float vals[3] =
            {
                vertex_u_text(triangle_vertex_id(tid,0)),
                vertex_u_text(triangle_vertex_id(tid,1)),
                vertex_u_text(triangle_vertex_id(tid,2)),
            };
            return *std::min_element(vals, vals+3);
        }

        float vertex_u_text(const int vid) const
        {
            CHECK_BOUNDS(u_text, vid);
            return u_text[vid];
        }

        void set_vertex_u_text(const int vid, const float s)
        {
            CHECK_BOUNDS(u_text, vid);
            u_text[vid] = s;
        }

        int vertex_with_min_u_text() const
        {
            return (std::min_element(u_text.begin(), u_text.end()) - u_text.begin());
        }

        int triangle_label(const int tid) const
        {
            CHECK_BOUNDS(t_label, tid);
            return t_label[tid];
        }

        void set_triangle_label(const int tid, const int i)
        {
            CHECK_BOUNDS(t_label, tid);
            t_label[tid] = i;
        }

        void update_normals()
        {
            update_t_normals();
            update_v_normals();
        }

        bool triangle_contains_vertex(const int tid, const int vid) const
        {
            if (triangle_vertex_id(tid, 0) == vid) return true;
            if (triangle_vertex_id(tid, 1) == vid) return true;
            if (triangle_vertex_id(tid, 2) == vid) return true;
            return false;
        }

        ipair shared_edge(const int tid0, const int tid1) const;

        int shared_vertex(const int eid0, const int eid1) const;

        int shared_triangle(const int eid0, const int eid1) const;

        bool edges_share_same_triangle(const int eid1, const int eid2) const;

        int triangle_adjacent_along(const int tid, const int vid0, const int vid1) const;

        int triangle_edge_local_to_global(const int tid, const int off) const
	{
	    assert(off>=0 && off <=2);

	    int vid0 = triangle_vertex_id(tid, TRI_EDGES[off][0]);
	    int vid1 = triangle_vertex_id(tid, TRI_EDGES[off][1]);

	    for(int eid : adj_tri2edg(tid))
	    {
		if (edge_contains_vertex(eid, vid0) && edge_contains_vertex(eid, vid1))
		{
		    return eid;
		}
	    }
	    assert(false && "Something is off here...");
	}

        vec3d element_barycenter(const int tid) const;

        void normalize_area();
        void center_bbox();
        void update_bbox();        

        void translate(const vec3d & delta);
        void rotate(const vec3d & axis, const double angle);
        void rotate(const double R[3][3]);

        virtual void operator+=(const Trimesh & m);

        void remove_duplicated_vertices(const double eps = 1e-7);
        void remove_duplicated_triangles();

        void scale(const float scale_factor)
        {
            for(int vid=0; vid<num_vertices(); ++vid)
            {
                vec3d pos = vertex(vid) * scale_factor;
                set_vertex(vid, pos);
            }
        }

        int connected_components(std::vector< std::set<int> > & ccs) const;
        int connected_components() const;

        double volume() const { return 0.0; } // TODO
};

}

#ifndef  CAX_STATIC_LIB
#include "trimesh.cpp"
#endif

#endif // TRIMESH_H
