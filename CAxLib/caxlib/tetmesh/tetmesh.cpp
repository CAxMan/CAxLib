#include "tetmesh.h"
#include "../timer.h"

#include <float.h>
#include <map>
#include <set>

#include "../io/read_MESH.h"
#include "../io/read_TET.h"
#include "../io/write_MESH.h"
#include "../io/write_TET.h"

namespace caxlib
{


CAX_INLINE
Tetmesh::Tetmesh(const char * filename)
{
    timer_start("load tetmesh");

    load(filename);
    init();

    print_quality_statistics();

    timer_stop("load tetmesh");
}

CAX_INLINE
void Tetmesh::load(const char * filename)
{
    timer_start("Load Tetmesh");

    clear();

    std::string str(filename);
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare("mesh") == 0 ||
        filetype.compare("MESH") == 0)
    {
        read_MESH(filename, coords, tets);
    }
    else if (filetype.compare(".tet") == 0 ||
             filetype.compare(".TET") == 0)
    {
        read_TET(filename, coords, tets);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    logger << tets.size()   / 4 << " tetrahedra read" << endl;
    logger << coords.size() / 3 << " vertices   read" << endl;

    this->filename = std::string(filename);

    timer_stop("Load Tetmesh");
}

CAX_INLINE
void Tetmesh::save(const char * filename) const
{
    timer_start("Save Tetmesh");

    std::string str(filename);
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare("mesh") == 0 ||
        filetype.compare("MESH") == 0)
    {
        write_MESH(filename, coords, tets);
    }
    else if (filetype.compare(".tet") == 0 ||
             filetype.compare(".TET") == 0)
    {
        write_TET(filename, coords, tets);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    timer_stop("Save Tetmesh");
}

CAX_INLINE
Tetmesh::Tetmesh(const std::vector<double> & coords,
                 const std::vector<u_int>  & tets)
{
    clear();
    std::copy(coords.begin(), coords.end(), std::back_inserter(this->coords));
    std::copy(tets.begin(), tets.end(), std::back_inserter(this->tets));
    init();
}

CAX_INLINE
void Tetmesh::clear()
{
    bb.reset();
    coords.clear();
    tets.clear();
    edges.clear();
    tris.clear();
    v_on_srf.clear();
    e_on_srf.clear();
    t_norm.clear();
    u_text.clear();
    t_label.clear();
    vtx2vtx.clear();
    vtx2edg.clear();
    vtx2tet.clear();
    vtx2tri.clear();
    edg2tet.clear();
    tet2edg.clear();
    tet2tet.clear();
    tet2tri.clear();
    tri2tet.clear();
}

CAX_INLINE
void Tetmesh::init()
{
    u_text.resize(num_vertices());
    t_label.resize(num_tetrahedra());

    for(int vid=0; vid<num_vertices(); ++vid)
    {
        u_text[vid] = vertex(vid).y();
    }

    update_bbox();
    update_interior_adjacency();
    update_surface_adjacency();
    update_t_normals();

    logger << "BB min: " << bb.min << endl;
    logger << "BB max: " << bb.max << endl;
}

CAX_INLINE
void Tetmesh::update_bbox()
{
    bb.reset();
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        vec3d v = vertex(vid);
        bb.min = bb.min.min(v);
        bb.max = bb.max.max(v);
    }
}

CAX_INLINE
void Tetmesh::update_interior_adjacency()
{
    timer_start("Build adjacency");

    edges.clear();
    vtx2vtx.clear();
    vtx2edg.clear();
    vtx2tet.clear();
    edg2tet.clear();
    tet2tet.clear();
    tet2edg.clear();

    vtx2tet.resize(num_vertices());

    typedef std::map<ipair, std::vector<int> > mymap;
    mymap edge_tet_map;

    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        int tid_ptr = tid * 4;

        int vid4 = tets[tid_ptr + 3];
        vtx2tet[vid4].push_back(tid);

        for(int i=0; i<3; ++i)
        {
            int  vid0 = tets[tid_ptr + i];
            int  vid1 = tets[tid_ptr + (i+1)%3];

            vtx2tet[vid0].push_back(tid);

            ipair e1 = unique_pair(vid0, vid1);
            edge_tet_map[e1].push_back(tid);

            ipair e2 = unique_pair(vid0, vid4);
            edge_tet_map[e2].push_back(tid);
        }
    }

    edg2tet.resize(edge_tet_map.size());
    tet2edg.resize(num_tetrahedra());
    tet2tet.resize(num_tetrahedra());
    vtx2vtx.resize(num_vertices());
    vtx2edg.resize(num_vertices());

    std::set<ipair> tet_pairs;

    int fresh_id = 0;
    for(mymap::iterator it=edge_tet_map.begin(); it!=edge_tet_map.end(); ++it)
    {
        ipair e    = it->first;
        int  eid  = fresh_id++;
        int  vid0 = e.first;
        int  vid1 = e.second;

        edges.push_back(vid0);
        edges.push_back(vid1);

        vtx2vtx[vid0].push_back(vid1);
        vtx2vtx[vid1].push_back(vid0);

        vtx2edg[vid0].push_back(eid);
        vtx2edg[vid1].push_back(eid);

        std::vector<int> tids = it->second;
        //assert(tids.size() <= 2);
        //sassert(tids.size() >= 1);
        for(int i=0; i<(int)tids.size(); ++i)
        {
            int tid = tids[i];

            tet2edg[tid].push_back(eid);
            edg2tet[eid].push_back(tid);

            for(int j=i+1; j<(int)tids.size(); ++j)
            {
                if (shared_facet(tids[i], tids[j]) != -1)
                {
                    ipair p = unique_pair(tids[j], tids[i]);
                    if (DOES_NOT_CONTAIN(tet_pairs, p))
                    {
                        tet_pairs.insert(p);
                        tet2tet[tids[i]].push_back(tids[j]);
                        tet2tet[tids[j]].push_back(tids[i]);
                        // sanity checks
                        assert(tet2tet[tids[j]].size() <= 4);
                        assert(tet2tet[tids[i]].size() <= 4);
                    }
                }
            }
        }

//        if (tids.size() == 2)
//        {
//            tet2tet[tids[0]].push_back(tids[1]);
//            tet2tet[tids[1]].push_back(tids[0]);
//        }
    }

    logger << num_vertices()   << "\tvertices"   << endl;
    logger << num_tetrahedra() << "\ttetrahedra" << endl;
    logger << num_edges()      << "\tedges"      << endl;

    timer_stop("Build adjacency");
}

CAX_INLINE
void Tetmesh::update_surface_adjacency()
{
    tris.clear();
    tri2tet.clear();
    v_on_srf.resize(num_vertices(), false);
    e_on_srf.resize(num_edges(), false);
    tet2tri.resize(num_tetrahedra());
    vtx2tri.resize(num_vertices());

    edg2tri.resize(num_edges());
    tri2edg.clear();
    tri2tri.clear();

    timer_start("Build Surface");

    typedef std::vector<int> face;
    std::map< face,std::pair<int,int> > tri2tet_map;

    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        int tid_ptr = tid * 4;

        for(int fid=0; fid<4; ++fid)
        {
            face f;
            f.push_back(tets[tid_ptr + TET_FACES[fid][0]]);
            f.push_back(tets[tid_ptr + TET_FACES[fid][1]]);
            f.push_back(tets[tid_ptr + TET_FACES[fid][2]]);

            sort(f.begin(), f.end());

            if (CONTAINS(tri2tet_map,f)) tri2tet_map.erase(f);
            else                         tri2tet_map[f] = std::make_pair(tid,fid);
        }
    }

    for(auto it=tri2tet_map.begin(); it!=tri2tet_map.end(); ++it)
    {
        auto obj     = *it;
        int  tid     = obj.second.first;
        int  fid     = obj.second.second;
        int  tid_ptr = tid * 4;

        int vid0 = tets[tid_ptr + TET_FACES[fid][0]];
        int vid1 = tets[tid_ptr + TET_FACES[fid][1]];
        int vid2 = tets[tid_ptr + TET_FACES[fid][2]];

        tris.push_back(vid0);
        tris.push_back(vid1);
        tris.push_back(vid2);

        v_on_srf[vid0] = true;
        v_on_srf[vid1] = true;
        v_on_srf[vid2] = true;

        int fresh_id = tri2tet.size();

        vtx2tri[vid0].push_back(fresh_id);
        tet2tri[tid].push_back(fresh_id);
        tri2tet.push_back(tid);

        std::vector<int> edges = adj_tet2edg(tid);
        std::vector<int> tmp;
        tri2edg.push_back(tmp);
        assert(edges.size() == 6);
        for(size_t i=0; i<edges.size(); ++i)
        {
            int  eid   = edges[i];
            int  eid0  = edge_vertex_id(eid, 0);
            int  eid1  = edge_vertex_id(eid, 1);
            bool has_0 = (eid0 == vid0 || eid0 == vid1 || eid0 == vid2);
            bool has_1 = (eid1 == vid0 || eid1 == vid1 || eid1 == vid2);

            if (has_0 && has_1)
            {
                edg2tri[eid].push_back(fresh_id);
                tri2edg[fresh_id].push_back(eid);
            }
        }
    }

    tri2tri.resize(num_srf_triangles());

    for(int eid=0; eid<num_edges(); ++eid)
    {
        std::vector<int> tris = edg2tri[eid];
        if (!(tris.empty() || tris.size() == 2))
        {
            logger << "\tedge " << eid << " is non manifold! " << tris.size() << endl;
            logger << edge_vertex(eid,0) << "\t" << edge_vertex(eid,1) << endl;
            u_text[edge_vertex_id(eid,0)] = 10.0;
            u_text[edge_vertex_id(eid,1)] = 10.0;
        }
        //assert(tris.empty() || tris.size() == 2);
        if (!tris.empty())
        {
            int t0 = tris[0];
            int t1 = tris[1];
            tri2tri[t0].push_back(t1);
            tri2tri[t1].push_back(t0);
        }

        e_on_srf[eid] = !(edg2tri[eid].empty());
        //if (e_on_srf[eid]) assert(adj_edg2tri.size() == 2);
    }

    timer_stop("Build Surface");

    logger << tris.size() / 3 << " triangles were generated" << endl;
}

CAX_INLINE
void Tetmesh::update_t_normals()
{
    t_norm.clear();
    t_norm.resize(num_srf_triangles()*3);

    for(int tid=0; tid<num_srf_triangles(); ++tid)
    {
        int tid_ptr = tid * 3;

        vec3d v0 = vertex(tris[tid_ptr+0]);
        vec3d v1 = vertex(tris[tid_ptr+1]);
        vec3d v2 = vertex(tris[tid_ptr+2]);

        vec3d u = v1 - v0;    u.normalize();
        vec3d v = v2 - v0;    v.normalize();
        vec3d n = u.cross(v); n.normalize();

        t_norm[tid_ptr + 0] = n.x();
        t_norm[tid_ptr + 1] = n.y();
        t_norm[tid_ptr + 2] = n.z();
    }
}

CAX_INLINE
int Tetmesh::adjacent_tet_through_facet(const int tid, const int facet)
{
    std::vector<int> nbrs = adj_tet2tet(tid);
    for(size_t i=0; i<nbrs.size(); ++i)
    {
        if (shared_facet(tid, nbrs[i]) == facet) return nbrs[i];
    }
    return -1;
}

CAX_INLINE
int Tetmesh::shared_facet(const int tid0, const int tid1) const
{
    for(int f=0; f<4; ++f)
    {
        if (tet_contains_vertex(tid1, tet_vertex_id(tid0, TET_FACES[f][0])) &&
            tet_contains_vertex(tid1, tet_vertex_id(tid0, TET_FACES[f][1])) &&
            tet_contains_vertex(tid1, tet_vertex_id(tid0, TET_FACES[f][2])) )
        {
            return f;
        }
    }
    //assert("Tets do not share a facet!" && false);
    return -1;
}

CAX_INLINE
void Tetmesh::print_quality_statistics(bool list_folded_elements) const
{
    double asj = 0.0;
    double msj = FLT_MAX;
    int    inv = 0;

    if (list_folded_elements) logger << "Folded Tets: ";

    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        double q = tet_quality(tid);

        asj += q;
        msj = std::min(msj, q);

        if (q <= 0.0)
        {
            ++inv;

            if (list_folded_elements) logger << tid << " - ";
        }
    }
    asj /= double(num_tetrahedra());

    if (list_folded_elements) logger << endl << endl;

    logger << endl;
    logger << "MIN SJ : " << msj << endl;
    logger << "AVG SJ : " << asj << endl;
    logger << "INV EL : " << inv << " (out of " << num_tetrahedra() << ")" << endl;
    logger << endl;
}

CAX_INLINE
double Tetmesh::vertex_mass(const int vid) const
{
    std::vector<int> tets = adj_vtx2tri(vid);
    double mass = 0.0;
    for(int i=0; i<(int)tets.size(); ++i)
    {
        int tid = tets[i];
        mass += tet_volume(tid);
    }
    mass /= 4.0;
    return mass;
}

CAX_INLINE
int Tetmesh::tet_vertex_opposite_to(const int tid, const int facet) const
{
    for(int offset=0; offset<4; ++offset)
    {
        int vid = tet_vertex_id(tid, offset);

        if (tet_vertex_id(tid, TET_FACES[facet][0]) != vid &&
            tet_vertex_id(tid, TET_FACES[facet][1]) != vid &&
            tet_vertex_id(tid, TET_FACES[facet][2]) != vid)
        {
            return vid;
        }
    }
    assert(false);
}

CAX_INLINE
int Tetmesh::tet_face_opposite_to(const int tid, const int vid) const
{
    assert(tet_contains_vertex(tid, vid));
    for(int f=0; f<4; ++f)
    {
        if (tet_vertex_id(tid, TET_FACES[f][0]) != vid &&
            tet_vertex_id(tid, TET_FACES[f][1]) != vid &&
            tet_vertex_id(tid, TET_FACES[f][2]) != vid)
        {
            return f;
        }
    }
    assert(false);
}

CAX_INLINE
int Tetmesh::tet_edge_id(const int tid, const int vid0, const int vid1) const
{
    assert(tet_contains_vertex(tid, vid0));
    assert(tet_contains_vertex(tid, vid1));

    ipair ref = unique_pair(vid0, vid1);

    for(int eid : adj_tet2edg(tid))
    {
        if (unique_pair(edge_vertex_id(eid,0), edge_vertex_id(eid,1))==ref) return eid;
    }
    assert(false);
}

CAX_INLINE
int Tetmesh::tet_edge_opposite_to(const int tid, const int vid0, const int vid1) const
{
    for(int e=0; e<6; ++e)
    {
        if ((tet_vertex_id(tid, TET_EDGES[e][0]) == vid0 && tet_vertex_id(tid, TET_EDGES[e][1]) == vid1)) return e;
        if ((tet_vertex_id(tid, TET_EDGES[e][0]) == vid1 && tet_vertex_id(tid, TET_EDGES[e][1]) == vid0)) return e;
    }
    assert(false);
}

CAX_INLINE
double Tetmesh::tet_edge_length(const int tid, const int eid) const
{
    vec3d A = tet_vertex(tid, TET_EDGES[eid][0]);
    vec3d B = tet_vertex(tid, TET_EDGES[eid][1]);
    return (A-B).length();
}


CAX_INLINE
double Tetmesh::tet_face_area(const int tid, const int fid) const
{
    vec3d A = tet_vertex(tid, TET_FACES[fid][0]);
    vec3d B = tet_vertex(tid, TET_FACES[fid][1]);
    vec3d C = tet_vertex(tid, TET_FACES[fid][2]);
    return (0.5 * (B-A).cross(C-A).length());
}

CAX_INLINE
void Tetmesh::scale(const double x_scale, const double y_scale, const double z_scale)
{
    vec3d scale_fact(x_scale, y_scale, z_scale);
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        set_vertex(vid, vertex(vid) * scale_fact);
    }
    update_bbox();
    update_t_normals();
}

// http://math.stackexchange.com/questions/1680607/eulers-formula-for-tetrahedral-mesh
//
CAX_INLINE
int Tetmesh::euler_number() const
{
    std::set< std::vector<int> > faces;
    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        for(int f=0; f<4; ++f)
        {
            std::vector<int> tri;
            tri.push_back(tet_vertex_id(tid, TET_FACES[f][0]));
            tri.push_back(tet_vertex_id(tid, TET_FACES[f][1]));
            tri.push_back(tet_vertex_id(tid, TET_FACES[f][2]));
            sort(tri.begin(), tri.end());
            faces.insert(tri);
        }
    }

    int nv = num_vertices();
    int ne = num_edges();
    int nf = faces.size();
    int nc = num_tetrahedra();

    return nv - ne + nf - nc;
}

CAX_INLINE
vec3d Tetmesh::tet_face_normal(const int tid, const int fid) const
{
    vec3d A = tet_vertex(tid, TET_FACES[fid][0]);
    vec3d B = tet_vertex(tid, TET_FACES[fid][1]);
    vec3d C = tet_vertex(tid, TET_FACES[fid][2]);
    vec3d n = (B-A).cross(C-A);
    n.normalize();
    return n;
}

CAX_INLINE
double Tetmesh::tet_dihedral_angle(const int tid, const int fid0, const int fid1) const
{
    vec3d   n0 = tet_face_normal(tid, fid0);
    vec3d   n1 = tet_face_normal(tid, fid1);
    double  alpha = acos(n0.dot(-n1));
    assert(!std::isnan(alpha));
    return alpha;
}

CAX_INLINE
double Tetmesh::edge_length(const int eid) const
{
    return (edge_vertex(eid, 0) - edge_vertex(eid, 1)).length();
}

CAX_INLINE
bool Tetmesh::tet_is_adjacent_to(const int tid, const int nbr) const
{
    for(int t : adj_tet2tet(tid))
    {
        if (t==nbr) return true;
    }
    return false;
}

CAX_INLINE
double Tetmesh::avg_edge_length() const
{
    double sum = 0.0;
    for(int eid=0; eid<(int)num_edges(); ++eid)
    {
        sum += edge_length(eid);
    }
    return sum/=double(num_edges());
}

CAX_INLINE
vec3d Tetmesh::element_barycenter(const int tid) const
{
    vec3d b(0,0,0);
    for(int i=0; i<4; ++i)
    {
        b += tet_vertex(tid,i);
    }
    b /= 4.0;
    return b;
}

CAX_INLINE
double Tetmesh::vertex_quality(const int vid) const
{
    double q = 1.0;
    std::vector<int> nbrs = adj_vtx2tet(vid);
    for(size_t i=0; i<nbrs.size(); ++i)
    {
        q = std::min(q, tet_quality(nbrs[i]));
    }
    return q;
}

CAX_INLINE
int Tetmesh::vertex_inverted_elements(const int vid) const
{
    int count = 0;
    std::vector<int> nbrs = adj_vtx2tet(vid);
    for(size_t i=0; i<nbrs.size(); ++i)
    {
        if (tet_quality(nbrs[i]) < 0) ++count;
    }
    return count;
}

CAX_INLINE
std::vector<int> Tetmesh::tet_one_ring(const int tid) const
{
    int vid_a = tet_vertex_id(tid, 0);
    int vid_b = tet_vertex_id(tid, 1);
    int vid_c = tet_vertex_id(tid, 2);
    int vid_d = tet_vertex_id(tid, 3);

    std::vector<int> vtx_adj[4] =
    {
        adj_vtx2tet(vid_a),
        adj_vtx2tet(vid_b),
        adj_vtx2tet(vid_c),
        adj_vtx2tet(vid_d)
    };

    std::set<int> unique_one_ring;
    for(int i=0; i<4; ++i)
    {
        for(size_t j=0; j<vtx_adj[i].size(); ++j)
        {
            if (vtx_adj[i][j] != tid) unique_one_ring.insert(vtx_adj[i][j]);
        }
    }

    std::vector<int> one_ring;
    std::copy(unique_one_ring.begin(), unique_one_ring.end(), std::back_inserter(one_ring));

    return one_ring;
}

CAX_INLINE
std::vector<int> Tetmesh::get_flipped_tets() const
{
    std::vector<int> list;
    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        if (tet_quality(tid) < 0) list.push_back(tid);
    }
    return list;
}

CAX_INLINE
void Tetmesh::export_submesh_with_label(const int             label,
                                        std::vector<double> & sub_coords,
                                        std::vector<u_int>  & sub_tets,
                                        std::map<int, int>  & vid2sub_vid,
                                        std::map<int, int>  & sub_vid2vid) const
{
    assert(sub_coords.empty());
    assert(sub_tets.empty());
    assert(vid2sub_vid.empty());
    assert(sub_vid2vid.empty());

    int fresh_id = 0;

    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        if (tet_label(tid) != label) continue;

        for(int i=0; i<4; ++i)
        {
            int  vid = tet_vertex_id(tid,i);
            int  sub_vid;

            auto query = vid2sub_vid.find(vid);
            if (query == vid2sub_vid.end())
            {
                vec3d pos = vertex(vid);
                sub_coords.push_back(pos.x());
                sub_coords.push_back(pos.y());
                sub_coords.push_back(pos.z());

                vid2sub_vid[vid] = fresh_id;
                sub_vid2vid[fresh_id] = vid;

                sub_vid = fresh_id;
                ++fresh_id;
            }
            else
            {
                sub_vid = query->second;
            }

            sub_tets.push_back(sub_vid);
        }
    }
}

CAX_INLINE
std::vector<int> Tetmesh::edge_ordered_tet_ring(const int eid) const
{
    std::vector<int> ring = adj_edg2tet(eid);

    assert(!ring.empty());
    assert((!is_surface_edge(eid)) || (is_surface_edge(eid) && !adj_edg2tri(eid).empty()));

    // if it's a surface edge start from a surface tet, if it's not you can start anywhere
    //
    int curr  = (is_surface_edge(eid)) ? adj_tri2tet(adj_edg2tri(eid)[0]) : ring[0];

    std::set<int> visited;
    std::vector<int> ordered_ring;
    do
    {
        ordered_ring.push_back(curr);
        visited.insert(curr);
        int next = -1;
        for(int tid : ring)
        {
            assert(tet_contains_edge(tid, eid));
            if (tid != curr && tet_is_adjacent_to(tid, curr) && DOES_NOT_CONTAIN(visited, tid))
            {
                next = tid;
            }
        }
        curr = next;
    } while (curr != -1);

    assert(ordered_ring.size() == adj_edg2tet(eid).size());
    return ordered_ring;
}

CAX_INLINE
double Tetmesh::barycentric_coordinates(const int tid, const vec3d & P, double wgt[4]) const
{
    vec3d v0 = tet_vertex(tid, 0);
    vec3d v1 = tet_vertex(tid, 1);
    vec3d v2 = tet_vertex(tid, 2);
    vec3d v3 = tet_vertex(tid, 3);

    Eigen::Matrix<double,4,4> D;
    D(0,0) = v0[0]; D(0,1) = v0[1]; D(0,2) = v0[2]; D(0,3) = 1;
    D(1,0) = v1[0]; D(1,1) = v1[1]; D(1,2) = v1[2]; D(1,3) = 1;
    D(2,0) = v2[0]; D(2,1) = v2[1]; D(2,2) = v2[2]; D(2,3) = 1;
    D(3,0) = v3[0]; D(3,1) = v3[1]; D(3,2) = v3[2]; D(3,3) = 1;

    Eigen::Matrix<double,4,4> D0;
    D0(0,0) =  P[0]; D0(0,1) =  P[1]; D0(0,2) =  P[2]; D0(0,3) = 1;
    D0(1,0) = v1[0]; D0(1,1) = v1[1]; D0(1,2) = v1[2]; D0(1,3) = 1;
    D0(2,0) = v2[0]; D0(2,1) = v2[1]; D0(2,2) = v2[2]; D0(2,3) = 1;
    D0(3,0) = v3[0]; D0(3,1) = v3[1]; D0(3,2) = v3[2]; D0(3,3) = 1;

    Eigen::Matrix<double,4,4> D1;
    D1(0,0) = v0[0]; D1(0,1) = v0[1]; D1(0,2) = v0[2]; D1(0,3) = 1;
    D1(1,0) =  P[0]; D1(1,1) =  P[1]; D1(1,2) =  P[2]; D1(1,3) = 1;
    D1(2,0) = v2[0]; D1(2,1) = v2[1]; D1(2,2) = v2[2]; D1(2,3) = 1;
    D1(3,0) = v3[0]; D1(3,1) = v3[1]; D1(3,2) = v3[2]; D1(3,3) = 1;

    Eigen::Matrix<double,4,4> D2;
    D2(0,0) = v0[0]; D2(0,1) = v0[1]; D2(0,2) = v0[2]; D2(0,3) = 1;
    D2(1,0) = v1[0]; D2(1,1) = v1[1]; D2(1,2) = v1[2]; D2(1,3) = 1;
    D2(2,0) =  P[0]; D2(2,1) =  P[1]; D2(2,2) =  P[2]; D2(2,3) = 1;
    D2(3,0) = v3[0]; D2(3,1) = v3[1]; D2(3,2) = v3[2]; D2(3,3) = 1;

    Eigen::Matrix<double,4,4> D3;
    D3(0,0) = v0[0]; D3(0,1) = v0[1]; D3(0,2) = v0[2]; D3(0,3) = 1;
    D3(1,0) = v1[0]; D3(1,1) = v1[1]; D3(1,2) = v1[2]; D3(1,3) = 1;
    D3(2,0) = v2[0]; D3(2,1) = v2[1]; D3(2,2) = v2[2]; D3(2,3) = 1;
    D3(3,0) =  P[0]; D3(3,1) =  P[1]; D3(3,2) =  P[2]; D3(3,3) = 1;

    double det_D  = D.determinant();
    double det_D0 = D0.determinant();
    double det_D1 = D1.determinant();
    double det_D2 = D2.determinant();
    double det_D3 = D3.determinant();

    double eps = 1e-10;

    bool is_inside_pos = (det_D >= -eps && det_D0 >= -eps && det_D1 >= -eps && det_D2 >= -eps && det_D3 >= -eps);
    bool is_inside_neg = (det_D <=  eps && det_D0 <=  eps && det_D1 <=  eps && det_D2 <=  eps && det_D3 <=  eps);

    if (is_inside_pos || is_inside_neg)
    {
        double sum = det_D0 + det_D1 + det_D2 + det_D3;

        wgt[0] = det_D0 / sum;
        wgt[1] = det_D1 / sum;
        wgt[2] = det_D2 / sum;
        wgt[3] = det_D3 / sum;

        for(int i=0; i<4; ++i)
        {
            if (std::isnan(wgt[i]))
            {
                assert(fabs(det_D) < 1e-10);
                //cerr << "WARNING : nan barycentric coordinate! P: " <<  P << endl;
                wgt[0] = 0.25;
                wgt[1] = 0.25;
                wgt[2] = 0.25;
                wgt[3] = 0.25;
                return 0.0;
            }
        }

        // the idea is: among the tets that contain the query vertex rely on the best one
        // that is the one with the highest MSJ
        //
        return tet_quality(tid);
    }
    else // P is outside
    {
        wgt[0] = 0.25;
        wgt[1] = 0.25;
        wgt[2] = 0.25;
        wgt[3] = 0.25;
        return -FLT_MAX;
    }
}

CAX_INLINE
Trimesh Tetmesh::export_surface(std::map<int,int> & tet2tri_map,
                                std::map<int,int> & tri2tet_map) const
{
    assert(tet2tri_map.empty());
    assert(tri2tet_map.empty());

    std::vector<double> coords;
    std::vector<u_int>  srf;

    int fresh_id = 0;

    for(int vid=0; vid<num_vertices(); ++vid)
    {
        if (is_surface_vertex(vid))
        {
            vec3d pos = vertex(vid);
            coords.push_back(pos.x());
            coords.push_back(pos.y());
            coords.push_back(pos.z());
            tet2tri_map[vid] = fresh_id;
            tri2tet_map[fresh_id] = vid;
            ++fresh_id;
        }
    }
    for(size_t i=0; i<tris.size(); ++i)
    {
        srf.push_back(tet2tri_map[tris[i]]);
    }
    return Trimesh(coords, tris);
}

CAX_INLINE
Trimesh Tetmesh::export_surface() const
{
    std::map<int,int> tet2tri, tri2tet;
    return export_surface(tet2tri, tri2tet);
}

CAX_INLINE
void Tetmesh::operator+=(const Tetmesh & m)
{
    int nv = num_vertices();
    int nt = num_tetrahedra();
    int ne = num_edges();
    int ns = num_srf_triangles();

    for(int tid=0; tid<m.num_tetrahedra(); ++tid)
    {
        tets.push_back(nv + m.tet_vertex_id(tid,0));
        tets.push_back(nv + m.tet_vertex_id(tid,1));
        tets.push_back(nv + m.tet_vertex_id(tid,2));
        tets.push_back(nv + m.tet_vertex_id(tid,3));

        t_label.push_back(m.tet_label(tid));

        std::vector<int> tmp;
        for(int eid : m.tet2edg[tid]) tmp.push_back(ne + eid);
        tet2edg.push_back(tmp);

        tmp.clear();
        for(int tid : m.tet2tet[tid]) tmp.push_back(nt + tid);
        tet2tet.push_back(tmp);

        tmp.clear();
        for(int tid : m.tet2tri[tid]) tmp.push_back(ns + tid);
        tet2tri.push_back(tmp);
    }
    for(int eid=0; eid<m.num_edges(); ++eid)
    {
        edges.push_back(nv + m.edge_vertex_id(eid,0));
        edges.push_back(nv + m.edge_vertex_id(eid,1));

        e_on_srf.push_back(m.e_on_srf[eid]);

        std::vector<int> tmp;
        for(int tid : m.edg2tet[eid]) tmp.push_back(nt + tid);
        edg2tet.push_back(tmp);

        tmp.clear();
        for(int tid : m.edg2tri[eid]) tmp.push_back(ns + tid);
        edg2tri.push_back(tmp);
    }
    for(int sid=0; sid<m.num_srf_triangles(); ++sid)
    {
        tris.push_back(nv + m.tri_vertex_id(sid,0));
        tris.push_back(nv + m.tri_vertex_id(sid,1));
        tris.push_back(nv + m.tri_vertex_id(sid,2));

        vec3d n = m.triangle_normal(sid);
        t_norm.push_back(n.x());
        t_norm.push_back(n.y());
        t_norm.push_back(n.z());

        tri2tet.push_back(nt + m.tri2tet[sid]);

        std::vector<int> tmp;
        for(int eid : m.tri2edg[sid]) tmp.push_back(ne + eid);
        tri2edg.push_back(tmp);

        tmp.clear();
        for(int nbr : m.tri2tri[sid]) tmp.push_back(ns + nbr);
        tri2tri.push_back(tmp);
    }
    for(int vid=0; vid<m.num_vertices(); ++vid)
    {
        vec3d pos = m.vertex(vid);
        coords.push_back(pos.x());
        coords.push_back(pos.y());
        coords.push_back(pos.z());

        v_on_srf.push_back(m.v_on_srf[vid]);
        u_text.push_back(m.vertex_u_text(vid));

        std::vector<int> tmp;
        for(int eid : m.vtx2edg[vid]) tmp.push_back(ne + eid);
        vtx2edg.push_back(tmp);

        tmp.clear();
        for(int tid : m.vtx2tet[vid]) tmp.push_back(nt + tid);
        vtx2tet.push_back(tmp);

        tmp.clear();
        for(int sid : m.vtx2tri[vid]) tmp.push_back(ns + sid);
        vtx2tri.push_back(tmp);

        tmp.clear();
        for(int nbr : m.vtx2vtx[vid]) tmp.push_back(nv + nbr);
        vtx2vtx.push_back(tmp);
    }

    update_bbox();
}

CAX_INLINE
void Tetmesh::normalize_volume()
{
    double vol = 0.0;
    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        vol += tet_volume(tid);
    }
    logger << "volume before: " << vol << endl;
    if (vol < 1e-4)
    {
        std::cerr << "\nWARNING!! Tetmesh Volume is close to zero: " << vol << endl << endl;
        vol = 1e-4;
    }
    double s = 1.0 / pow(vol, 1.0/3.0);
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        set_vertex(vid, vertex(vid) * s);
    }
    {
        double vol = 0.0;
        for(int tid=0; tid<num_tetrahedra(); ++tid)
        {
            vol += tet_volume(tid);
        }
        logger << "volume after: " << vol << endl;
    }
    update_bbox();
}

CAX_INLINE
void Tetmesh::center_bbox()
{
    update_bbox();
    vec3d center = bb.center();
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        vec3d pos = vertex(vid) - center;
        set_vertex(vid, pos);
    }
    bb.min -= center;
    bb.max -= center;
}

CAX_INLINE
void Tetmesh::translate(const vec3d & delta)
{
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        vec3d pos = vertex(vid);
        pos += delta;
        set_vertex(vid, pos);
    }
    update_bbox();
}

}
