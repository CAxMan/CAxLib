/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2015.
*/

#ifndef TIMER_H
#define TIMER_H

#include "caxlib.h"

namespace caxlib
{


CAX_INLINE void timer_start(const char * msg);
CAX_INLINE void timer_stop (const char * msg);

}


#ifndef  CAX_STATIC_LIB
#include "timer.cpp"
#endif

#endif // TIMER_H
