#ifndef READ_ZIP_H
#define READ_ZIP_H

#include "../caxlib.h"
#include "../trimesh/annotations.h"

#include <sys/types.h>
#include <vector>

namespace caxlib
{

typedef struct
{
    std::string OFF_filename;
    std::string ANN_filename;
}
OFF_ANN_Filenames;

CAX_INLINE
OFF_ANN_Filenames unzip (const char *filename);

CAX_INLINE
void read_ZIP(const char                * filename,
              std::vector<double>       & xyz,
              std::vector<u_int>        & tri,
              GlobalAnnotations         & glob_ann,
              std::vector<VertexAnnotations>    & vertex_ann,
              std::vector<TriangleAnnotations>  & triangle_ann);
}

#ifndef  CAX_STATIC_LIB
#include "read_ZIP.cpp"
#endif

#endif // READ_ZIP
