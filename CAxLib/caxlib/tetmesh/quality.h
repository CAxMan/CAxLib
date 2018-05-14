#ifndef QUALITY
#define QUALITY

#include "../caxlib.h"
#include "../vec3.h"
#include <float.h>


/* Scaled Jacobian and Volume computation are both based on:
 *
 * The Verdict Geometric Quality Library
 * SANDIA Report SAND2007-1751
 *
*/


namespace caxlib
{

static const double sqrt_2 = 1.414213562373095;


CAX_INLINE
double tet_scaled_jacobian(const vec3d & a, const vec3d & b, const vec3d & c, const vec3d & d);

CAX_INLINE
double tet_unsigned_volume(const vec3d & a, const vec3d & b, const vec3d & c, const vec3d & d);

}


#ifndef  CAX_STATIC_LIB
#include "quality.cpp"
#endif


#endif // QUALITY
