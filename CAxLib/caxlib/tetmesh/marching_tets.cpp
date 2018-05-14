#include "marching_tets.h"


namespace caxlib
{

enum // Look-up Table
{
    C_1111 = 0xF,
    C_0111 = 0x7,
    C_1011 = 0xB,
    C_0011 = 0x3,
    C_1101 = 0xD,
    C_0101 = 0x5,
    C_1001 = 0x9,
    C_0001 = 0x1,
    C_1110 = 0xE,
    C_0110 = 0x6,
    C_1010 = 0xA,
    C_0010 = 0x2,
    C_1100 = 0xC,
    C_0100 = 0x4,
    C_1000 = 0x8,
    C_0000 = 0x0
};


CAX_INLINE
void make_triangle(const Tetmesh       & m,
                   const double          isovalue,
                   const int             vids[],
                   const float           func[],
                   const int             e[],
                   std::map<ipair,int> & edg2vid_map,
                   std::vector<double> & coords,
                   std::vector<u_int>  & tris,
                   std::vector<double> & norm);


CAX_INLINE
void marching_tets(const Tetmesh       & m,
                   const double          isovalue,
                   std::vector<double> & coords,
                   std::vector<u_int>  & tris,
                   std::vector<double> & norm)
{
    std::map<ipair,int> edg2vid_map;

    for(int tid=0; tid<m.num_tetrahedra(); ++tid)
    {
        int vids[] =
        {
            m.tet_vertex_id(tid, 0),
            m.tet_vertex_id(tid, 1),
            m.tet_vertex_id(tid, 2),
            m.tet_vertex_id(tid, 3)
        };

        float func[] =
        {
            m.vertex_u_text(vids[0]),
            m.vertex_u_text(vids[1]),
            m.vertex_u_text(vids[2]),
            m.vertex_u_text(vids[3])
        };

        u_char c = 0x0;
        if (isovalue >= func[0]) c |= C_1000;
        if (isovalue >= func[1]) c |= C_0100;
        if (isovalue >= func[2]) c |= C_0010;
        if (isovalue >= func[3]) c |= C_0001;

        switch (c)
        {
            case C_1000 : { int e [] = { 2, 0, 4 }; make_triangle(m, isovalue, vids, func, e , edg2vid_map, coords, tris, norm); break; }
            case C_0111 : { int e [] = { 0, 2, 4 }; make_triangle(m, isovalue, vids, func, e , edg2vid_map, coords, tris, norm); break; }
            case C_1011 : { int e [] = { 2, 1, 3 }; make_triangle(m, isovalue, vids, func, e , edg2vid_map, coords, tris, norm); break; }
            case C_0100 : { int e [] = { 1, 2, 3 }; make_triangle(m, isovalue, vids, func, e , edg2vid_map, coords, tris, norm); break; }
            case C_1101 : { int e [] = { 1, 0, 5 }; make_triangle(m, isovalue, vids, func, e , edg2vid_map, coords, tris, norm); break; }
            case C_0010 : { int e [] = { 0, 1, 5 }; make_triangle(m, isovalue, vids, func, e , edg2vid_map, coords, tris, norm); break; }
            case C_0001 : { int e [] = { 5, 3, 4 }; make_triangle(m, isovalue, vids, func, e , edg2vid_map, coords, tris, norm); break; }
            case C_1110 : { int e [] = { 3, 5, 4 }; make_triangle(m, isovalue, vids, func, e , edg2vid_map, coords, tris, norm); break; }
            case C_0101 : { int e0[] = { 5, 2, 4 }; make_triangle(m, isovalue, vids, func, e0, edg2vid_map, coords, tris, norm);
                            int e1[] = { 2, 5, 1 }; make_triangle(m, isovalue, vids, func, e1, edg2vid_map, coords, tris, norm); break; }
            case C_1010 : { int e0[] = { 2, 5, 4 }; make_triangle(m, isovalue, vids, func, e0, edg2vid_map, coords, tris, norm);
                            int e1[] = { 5, 2, 1 }; make_triangle(m, isovalue, vids, func, e1, edg2vid_map, coords, tris, norm); break; }
            case C_0011 : { int e0[] = { 3, 4, 1 }; make_triangle(m, isovalue, vids, func, e0, edg2vid_map, coords, tris, norm);
                            int e1[] = { 1, 4, 0 }; make_triangle(m, isovalue, vids, func, e1, edg2vid_map, coords, tris, norm); break; }
            case C_1100 : { int e0[] = { 4, 3, 1 }; make_triangle(m, isovalue, vids, func, e0, edg2vid_map, coords, tris, norm);
                            int e1[] = { 4, 1, 0 }; make_triangle(m, isovalue, vids, func, e1, edg2vid_map, coords, tris, norm); break; }
            case C_1001 : { int e0[] = { 3, 2, 0 }; make_triangle(m, isovalue, vids, func, e0, edg2vid_map, coords, tris, norm);
                            int e1[] = { 5, 3, 0 }; make_triangle(m, isovalue, vids, func, e1, edg2vid_map, coords, tris, norm); break; }
            case C_0110 : { int e0[] = { 2, 3, 0 }; make_triangle(m, isovalue, vids, func, e0, edg2vid_map, coords, tris, norm);
                            int e1[] = { 3, 5, 0 }; make_triangle(m, isovalue, vids, func, e1, edg2vid_map, coords, tris, norm); break; }
            default : break;
        }
    }
}

CAX_INLINE
void make_triangle(const Tetmesh       & m,
                   const double          isovalue,
                   const int             vids[],
                   const float           func[],
                   const int             e[],
                   std::map<ipair,int> & edg2vid_map,
                   std::vector<double> & coords,
                   std::vector<u_int>  & tris,
                   std::vector<double> & norm)
{
    assert(isovalue >= *std::min_element(func, func+4));
    assert(isovalue <= *std::max_element(func, func+4));

    vec3d v[3];

    int fresh_vid = coords.size()/3;

    for(int i=0; i<3; ++i)
    {
        int v_a = vids[TET_EDGES[e[i]][0]];
        int v_b = vids[TET_EDGES[e[i]][1]];

        // avoid duplicated vertices. If an edge has already
        // been visited retrieve its id and coordinates
        //
        ipair pair  = unique_pair(v_a, v_b);
        auto  query = edg2vid_map.find(pair);

        if (query != edg2vid_map.end())
        {
            int vid     = query->second;
            int vid_ptr = vid * 3;
            tris.push_back(vid);
            v[i] = vec3d(coords[vid_ptr], coords[vid_ptr+1], coords[vid_ptr+2]);
        }
        else
        {
            float f_a = func[TET_EDGES[e[i]][0]];
            float f_b = func[TET_EDGES[e[i]][1]];

            if (f_a < f_b)
            {
                std::swap(v_a, v_b);
                std::swap(f_a, f_b);
            }

            double alpha = (isovalue - f_a) / (f_b - f_a);

            v[i] = (1.0 - alpha) * m.vertex(v_a) + alpha * m.vertex(v_b);

            coords.push_back(v[i].x());
            coords.push_back(v[i].y());
            coords.push_back(v[i].z());

            tris.push_back(fresh_vid);
            edg2vid_map[pair] = fresh_vid++;
        }
    }

    vec3d u  = v[1] - v[0]; u.normalize();
    vec3d w  = v[2] - v[0]; w.normalize();
    vec3d n = u.cross(w);
    n.normalize();

    norm.push_back(n.x());
    norm.push_back(n.y());
    norm.push_back(n.z());
}


}
