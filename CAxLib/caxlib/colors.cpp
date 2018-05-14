/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2014.
*/

#include "colors.h"
#include "textures/quality_ramp_texture.h"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iostream>

namespace caxlib
{

CAX_INLINE void scattered_color(int n_colors, int pos, float * rgb)
{
    float fpos  = float(pos)/float(n_colors - 1) * 255.0;
    float alpha = fpos - floor(fpos);
    int   prev  = (int)floor(fpos);
    int   next  = std::min(prev + 1, 255); // just to avoid "index out of bounds" errors...

    int r = (1.0 - alpha) * quality_ramp_texture1D[prev*3 + 0] + alpha * quality_ramp_texture1D[next*3 + 0];
    int g = (1.0 - alpha) * quality_ramp_texture1D[prev*3 + 1] + alpha * quality_ramp_texture1D[next*3 + 1];
    int b = (1.0 - alpha) * quality_ramp_texture1D[prev*3 + 2] + alpha * quality_ramp_texture1D[next*3 + 2];

    rgb[0] = float(r)/255.0;
    rgb[1] = float(g)/255.0;
    rgb[2] = float(b)/255.0;
}

}
