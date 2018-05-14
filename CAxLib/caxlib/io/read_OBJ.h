#ifndef READ_OBJ_H
#define READ_OBJ_H

#include "../caxlib.h"

#include <sys/types.h>
#include <vector>

namespace caxlib
{

CAX_INLINE
void read_OBJ(const char          * filename,
              std::vector<double> & xyz,
              std::vector<u_int>  & tri);

}

#ifndef  CAX_STATIC_LIB
#include "read_OBJ.cpp"
#endif

#endif // READ_OBJ
