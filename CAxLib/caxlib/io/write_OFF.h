#ifndef WRITE_OFF_H
#define WRITE_OFF_H

#include "../caxlib.h"

#include <sys/types.h>
#include <vector>

namespace caxlib
{

CAX_INLINE
void write_OFF(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<u_int>  & tri);

}

#ifndef  CAX_STATIC_LIB
#include "write_OFF.cpp"
#endif

#endif // WRITE_OFF
