#include "bbox.h"

namespace caxlib
{

CAX_INLINE std::ostream & operator<<(std::ostream & in, const Bbox & bb)
{
    in << "\n[Bbox Info] MIN: " << bb.min << "\tMAX: " << bb.max << "\n";
    return in;
}

}
