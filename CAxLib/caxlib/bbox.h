/**
  @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2014.
*/

#ifndef BBOX_H
#define BBOX_H

#include <float.h>
#include "vec3.h"
#include "caxlib.h"

namespace caxlib
{

class Bbox
{
    public:

        Bbox() { reset(); }

        void reset()
        {
            min = vec3d( FLT_MAX,  FLT_MAX,  FLT_MAX);
            max = vec3d(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        }

        vec3d  min, max;
        inline vec3d  center()    const { return (min + max) * 0.5;    }
        inline double diag()      const { return (min - max).length(); }
        inline double delta_x()   const { return (max.x() - min.x());  }
        inline double delta_y()   const { return (max.y() - min.y());  }
        inline double delta_z()   const { return (max.z() - min.z());  }
        inline vec3d  delta()     const { return (max     - min);      }
        inline double min_entry() const { return  std::min(min.min_entry(), max.min_entry()); }
        inline double max_entry() const { return  std::max(max.max_entry(), max.max_entry()); }
};

CAX_INLINE std::ostream & operator<<(std::ostream & in, const Bbox & bb);

}

#ifndef  CAX_STATIC_LIB
#include "bbox.cpp"
#endif

#endif // BBOX_H
