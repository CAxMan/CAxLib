#ifndef WRITE_STL_H
#define WRITE_STL_H

#include "../bbox.h"
#include "../caxlib.h"
#include "../trimesh/annotations.h"

#include <sys/types.h>
#include <vector>

namespace caxlib
{

CAX_INLINE
void write_STL(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<u_int>  & tri);

CAX_INLINE
void export_STL(const char                  * filename,
                const std::vector<double>   & xyz,
                const std::vector<u_int>    & tri,
                const Bbox                  & bb,
                const GlobalAnnotations     & glob_ann);

}

#ifndef  CAX_STATIC_LIB
#include "write_STL.cpp"
#endif

#endif // WRITE_STL
