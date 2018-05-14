#ifndef TETGEN_WRAP_H
#define TETGEN_WRAP_H

#include "caxlib.h"

#include <vector>
#include <sys/types.h>

namespace caxlib
{

CAX_INLINE
void tetgen_wrap(const std::vector<double> & coords_in,
                 const std::vector<uint>   & tris_in,
                 const std::vector<uint>   & edges_in,
                 const std::string         & flags,       // options
                       std::vector<double> & coords_out,
                       std::vector<uint>   & tets_out);
}

#ifndef  CAX_STATIC_LIB
#include "tetgen_wrap.cpp"
#endif


#endif // TETGEN_WRAP_H
