#ifndef WRITE_ANN_H
#define WRITE_ANN_H

#include "../caxlib.h"

#include "../trimesh/annotations.h"

#include <sys/types.h>
#include <vector>

namespace caxlib
{

CAX_INLINE
void write_ANN(const char                                   * filename,
               const GlobalAnnotations                      & glob_ann,
               const std::vector<VertexAnnotations>    & vertex_ann,
               const std::vector<TriangleAnnotations>          & triangle_ann);
}

#ifndef  CAX_STATIC_LIB
#include "write_ANN.cpp"
#endif

#endif // WRITE_ANN
