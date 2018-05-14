#include "tetgen_wrap.h"
#include <tetgen.h>

namespace caxlib
{


CAX_INLINE
void tetgen_wrap(const std::vector<double> & coords_in,
                 const std::vector<uint>   & tris_in,
                 const std::vector<uint>   & edges_in,
                 const std::string         & flags,       // options
                       std::vector<double> & coords_out,
                       std::vector<uint>   & tets_out)
{
    assert(!coords_in.empty());
    assert(!tris_in.empty());
    assert(coords_out.empty());
    assert(tets_out.empty());

    tetgenio           in, out;
    tetgenio::facet   *f;
    tetgenio::polygon *p;

    // vertices
    //
    in.firstnumber    = 0;
    in.numberofpoints = coords_in.size() / 3;
    in.pointlist      = new REAL[coords_in.size()];

    for(size_t i=0; i<coords_in.size(); ++i)
    {
        in.pointlist[i] = static_cast<REAL>(coords_in[i]);
    }

    // faces
    //
    in.numberoffacets = tris_in.size() / 3;
    in.facetlist      = new tetgenio::facet[in.numberoffacets];

    for(int tid=0; tid<in.numberoffacets; ++tid)
    {
        f = &in.facetlist[tid];
        f->numberofpolygons = 1;
        f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
        f->numberofholes = 0;
        f->holelist = NULL;
        p = &f->polygonlist[0];
        p->numberofvertices = 3;
        p->vertexlist = new int[p->numberofvertices];
        p->vertexlist[0] = tris_in[tid * 3];
        p->vertexlist[1] = tris_in[tid * 3 + 1];
        p->vertexlist[2] = tris_in[tid * 3 + 2];
    }

    // edges
    //
    in.numberofedges = edges_in.size() / 2;
    in.edgelist       = new int[edges_in.size()];
    for(size_t i=0; i<edges_in.size(); ++i)
    {
        in.edgelist[i] = edges_in[i];
    }

    // tetgen options
    //
    std::string s = flags + "p";

    tetrahedralize(const_cast<char*>(s.c_str()), &in, &out);

    // generate tetmesh
    //
    for(int vid=0; vid<out.numberofpoints; ++vid)
    {
        int vid_ptr = 3 * vid;
        coords_out.push_back(static_cast<double>(out.pointlist[vid_ptr    ]));
        coords_out.push_back(static_cast<double>(out.pointlist[vid_ptr + 1]));
        coords_out.push_back(static_cast<double>(out.pointlist[vid_ptr + 2]));
    }

    for(int tid=0; tid<out.numberoftetrahedra; ++tid)
    {
        int tid_ptr = 4 * tid;
        tets_out.push_back(out.tetrahedronlist[tid_ptr    ]);
        tets_out.push_back(out.tetrahedronlist[tid_ptr + 1]);
        tets_out.push_back(out.tetrahedronlist[tid_ptr + 2]);
        tets_out.push_back(out.tetrahedronlist[tid_ptr + 3]);
    }
}

}
