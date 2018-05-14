#ifndef WRITE_MESH_H
#define WRITE_MESH_H

#include "../caxlib.h"

#include <sys/types.h>
#include <vector>

namespace caxlib
{

CAX_INLINE
void write_MESH(const char               * filename,
               const std::vector<double> & xyz,
               const std::vector<u_int>  & tets);

}

#ifndef  CAX_STATIC_LIB
#include "write_MESH.cpp"
#endif

#endif // WRITE_MESH
