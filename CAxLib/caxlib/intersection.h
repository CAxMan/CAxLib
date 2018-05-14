/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2017.
*/
#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "caxlib.h"
#include "vec2.h"

namespace caxlib
{

CAX_INLINE
bool segment2D_intersection(const vec2d        & s0_beg,
                            const vec2d        & s0_end,
                            const vec2d        & s1_beg,
                            const vec2d        & s1_end,
                            std::vector<vec2d> & inters);
}

#ifndef  CAX_STATIC_LIB
#include "intersection.cpp"
#endif

#endif // INTERSECTION_H
