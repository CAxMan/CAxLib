#ifndef READ_ANN_H
#define READ_ANN_H

#include "../caxlib.h"

#include "../trimesh/annotations.h"

#include <sys/types.h>
#include <vector>

namespace caxlib
{

CAX_INLINE
void read_ANN(const char          * filename,
              GlobalAnnotations & glob_ann,
              std::vector<VertexAnnotations> & vertex_ann,
              std::vector<TriangleAnnotations> & triangle_ann);
}


#ifndef  CAX_STATIC_LIB
#include "read_ANN.cpp"
#endif

#endif // READ_ANN
