/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2017.
*/
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "caxlib.h"
#include "caxlib/vec3.h"

#include <set>

namespace caxlib
{

CAX_INLINE
bool triangle_barycentric_coords(const vec3d  & A,
                                 const vec3d  & B,
                                 const vec3d  & C,
                                 const vec3d  & P,
                                 std::vector<double> & wgts,
                                 const double   tol = 1e-5);

CAX_INLINE
bool triangle_point_is_inside(const vec3d  & A,
                              const vec3d  & B,
                              const vec3d  & C,
                              const vec3d  & P,
                              const double   tol = 0.0);

CAX_INLINE
bool triangle_bary_is_vertex(const std::vector<double> & bary,
                             int                       & vid, // 0,1,2
                             const double                tol = 1e-5);

CAX_INLINE
bool triangle_bary_is_edge(const std::vector<double> & bary,
                             int                     & eid, // 0,1,2 (see TRI_EDGES)
                             const double              tol = 1e-5);
}

#ifndef  CAX_STATIC_LIB
#include "triangle.cpp"
#endif

#endif // TRIANGLE_H
