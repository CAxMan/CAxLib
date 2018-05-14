/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2015.
*/

#ifndef BFS_H
#define BFS_H

#include "caxlib.h"

#include <set>

namespace caxlib
{

template<class Mesh>
CAX_INLINE
void bfs_exahustive(const Mesh & m, const int seed, std::set<int> & visited);

}


#ifndef  CAX_STATIC_LIB
#include "bfs.cpp"
#endif

#endif // BFS_H
