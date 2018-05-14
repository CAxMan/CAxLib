#ifndef READ_TET_H
#define READ_TET_H

#include "../caxlib.h"

#include <sys/types.h>
#include <vector>

namespace caxlib
{

CAX_INLINE
void read_TET(const char          * filename,
              std::vector<double> & xyz,
              std::vector<u_int>  & tet);

}

#ifndef  CAX_STATIC_LIB
#include "read_TET.cpp"
#endif

#endif // READ_TET
