#ifndef READ_MESH_H
#define READ_MESH_H

#include "../caxlib.h"

#include <sys/types.h>
#include <vector>

namespace caxlib
{

CAX_INLINE
void read_MESH(const char          * filename,
               std::vector<double> & xyz,
               std::vector<u_int>  & tet);

}

#ifndef  CAX_STATIC_LIB
#include "read_MESH.cpp"
#endif

#endif // READ_MESH
