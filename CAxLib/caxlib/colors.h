/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2014.
*/

#ifndef COLORS_H
#define COLORS_H

#include "caxlib.h"

namespace caxlib
{

static const float RED     [3] = { 1.0 , 0.0 , 0.0 };
static const float GREEN   [3] = { 0.0 , 1.0 , 0.0 };
static const float BLUE    [3] = { 0.0 , 0.0 , 1.0 };
static const float YELLOW  [3] = { 1.0 , 1.0 , 0.0 };
static const float MAGENTA [3] = { 1.0 , 0.0 , 1.0 };
static const float CYAN    [3] = { 0.0 , 1.0 , 1.0 };
static const float BLACK   [3] = { 0.0 , 0.0 , 0.0 };
static const float WHITE   [3] = { 1.0 , 1.0 , 1.0 };

CAX_INLINE void scattered_color(int n_colors, int pos, float * rgb);

}

#ifndef  CAX_STATIC_LIB
#include "colors.cpp"
#endif

#endif // COLORS_H
