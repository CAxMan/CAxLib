#ifndef WRITE_ZIP_H
#define WRITE_ZIP_H

#include "../caxlib.h"
#include "../trimesh/annotations.h"

#include <sys/types.h>
#include <vector>

namespace caxlib
{

CAX_INLINE
void create_zip (const std::string zip_filename, const std::string off_filename, const std::string ann_filename);


CAX_INLINE
void write_ZIP( const char                * filename,
                const std::vector<double>       & xyz,
                const std::vector<u_int>        & tri,
                const GlobalAnnotations         & glob_ann,
                const std::vector<VertexAnnotations>    & vertex_ann,
                const std::vector<TriangleAnnotations>  & triangle_ann);
}

#ifndef  CAX_STATIC_LIB
#include "write_ZIP.cpp"
#endif

#endif // WRITE_ZIP
