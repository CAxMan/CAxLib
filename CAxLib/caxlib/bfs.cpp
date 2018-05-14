#include "bfs.h"

namespace caxlib
{

template<class Mesh>
CAX_INLINE
void bfs_exahustive(const Mesh & m, const int seed, std::set<int> & visited)
{
    assert(visited.empty());

    std::set<int> active_set;
    active_set.insert(seed);

    int vid;
    while (!active_set.empty())
    {
        vid = *active_set.begin();
        active_set.erase(active_set.begin());

        visited.insert(vid);

        for(int nbr : m.adj_vtx2vtx(vid))
        {
            if (DOES_NOT_CONTAIN(visited,nbr))
            {
                active_set.insert(nbr);
            }
        }
    }
}

}
