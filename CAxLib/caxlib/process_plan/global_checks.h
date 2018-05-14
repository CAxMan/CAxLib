/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2016.
*/

#ifndef GLOBAL_CHECKS_H
#define GLOBAL_CHECKS_H

#include "../caxlib.h"
#include "../trimesh/trimesh.h"
#include "../tetmesh/tetmesh.h"
#include "../tetgen_wrap.h"
#include "../bfs.h"


namespace caxlib
{

CAX_INLINE
bool check_material(Trimesh & m)
{
    const Printer  & printer  = m.global_annotations().printer;
    const Material & material = m.global_annotations().material;

    bool illegal_material = true;
    for(auto mat : printer.supported_materials)
    {
        if (material.name.compare(mat.name) == 0) illegal_material = false;
    }

    if (illegal_material)
    {
        for(int tid=0; tid<m.num_triangles(); ++tid) m.triangle_annotation(tid).bad_material = true;
        return false;
    }
    return true;
}


CAX_INLINE
bool check_size(Trimesh & m)
{
    const Printer & printer = m.global_annotations().printer;
    bool  too_big = false;

    if (m.bbox().delta_x() < printer.chamber_dimension[0]) too_big = true;
    if (m.bbox().delta_y() < printer.chamber_dimension[1]) too_big = true;
    if (m.bbox().delta_z() < printer.chamber_dimension[2]) too_big = true;

    if (too_big)
    {
        m.global_annotations().too_big = true;
        return false;
    }
    return true;
}



CAX_INLINE
bool check_weight(Trimesh & m)
{
    double weight = m.volume() * m.global_annotations().material.relative_weight;

    if (weight < m.global_annotations().printer.weight_max)
    {
        m.global_annotations().too_heavy = true;
        return false;
    }
    return true;
}




CAX_INLINE
void extract_connected_components_from_tetmesh(const Tetmesh & m, std::vector< std::set<int> > & ccs_tet)
{
    assert(ccs_tet.empty());

    std::map<int,int> tet2tri, tri2tet;
    Trimesh srf = m.export_surface(tet2tri, tri2tet);

    srf.connected_components(ccs_tet);

    for(std::set<int> & cc_tet : ccs_tet)
    {
        std::set<int> cc;
        for(int vid : cc_tet)
        {
            cc.insert(tri2tet.at(vid));
        }
        cc_tet = cc;
    }
}



CAX_INLINE
bool detect_voids(Trimesh & m)
{
    // 1) Extract connected components from the Brep
    //
    std::vector< std::set<int> > ccs_tri;
    m.connected_components(ccs_tri);

    // 2) Extract the connected components from a tetrahedralization of the Brep
    //
    std::vector<double> coords;
    std::vector<uint>   dummy, tets;
    caxlib::tetgen_wrap(m.vector_coords(), m.vector_triangles(), dummy, "", coords, tets);
    Tetmesh tet_m(coords, tets);
    std::vector< std::set<int> > ccs_tet;
    extract_connected_components_from_tetmesh(tet_m, ccs_tet);

    // 3) If a void exists, it will be a connected component in (1) but not in (2).
    //    If a connected component exists both in (1) and (2), it means that the
    //    brep contains more connected components, none of which has voids
    //
    int void_id = 0;
    for(std::set<int> cc_tri : ccs_tri)
    {
        bool is_void = true;

        for(std::set<int> cc_tet : ccs_tet)
        {
            if (cc_tri.size() == cc_tet.size() && cc_tri == cc_tet) is_void = false;
        }

        if (is_void)
        {
            caxlib::logger << "Void found! " << cc_tri.size() << " vertices on it" << caxlib::endl;

            ++void_id;
            for(int vid : cc_tri)
            {
                for(int tid : m.adj_vtx2tri(vid))
                {
                    m.set_closed_voids(tid, void_id);
                }
            }
        }
    }

    // return true if the mesh has voids
    return (void_id > 0);
}

}

#endif // GLOBAL_CHECKS_H
