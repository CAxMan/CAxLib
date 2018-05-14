#ifndef READ_IV_H
#define READ_IV_H

#include "../caxlib.h"

#include <sys/types.h>
#include <vector>

namespace caxlib
{

CAX_INLINE
void read_IV(const char          * filename,
             std::vector<double> & xyz,
             std::vector<u_int>  & tri);

}

#ifndef  CAX_STATIC_LIB
#include "read_IV.cpp"
#endif

#endif // READ_IV
