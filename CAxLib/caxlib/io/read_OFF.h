#ifndef READ_OFF_H
#define READ_OFF_H

#include "../caxlib.h"

#include <sys/types.h>
#include <vector>

namespace caxlib
{

CAX_INLINE
void read_OFF(const char          * filename,
              std::vector<double> & xyz,
              std::vector<u_int>  & tri);

}

#ifndef  CAX_STATIC_LIB
#include "read_OFF.cpp"
#endif

#endif // READ_OFF
