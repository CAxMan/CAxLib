#ifndef WRITE_TET_H
#define WRITE_TET_H

#include "../caxlib.h"

#include <sys/types.h>
#include <vector>

namespace caxlib
{

CAX_INLINE
void write_TET(const char               * filename,
               const std::vector<double> & xyz,
               const std::vector<u_int>  & tets);

}

#ifndef  CAX_STATIC_LIB
#include "write_TET.cpp"
#endif

#endif // WRITE_TET
