#ifndef WRITE_OBJ_H
#define WRITE_OBJ_H

#include "../caxlib.h"

#include <sys/types.h>
#include <vector>

namespace caxlib
{

CAX_INLINE
void write_OBJ(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<u_int>  & tri);

}

#ifndef  CAX_STATIC_LIB
#include "write_OBJ.cpp"
#endif

#endif // WRITE_OBJ
