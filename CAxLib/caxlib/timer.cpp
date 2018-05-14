#include "timer.h"

#include <time.h>
#include <iostream>
#include <map>

namespace caxlib
{

static std::map<int,clock_t>      t_start;
static std::map<int,clock_t>      t_stop;
static std::map<int,std::string>  msgs;
static unsigned int               first_call = 0;

////////////////////////////////////////////////////////////////////////////////////////

// See: http://stackoverflow.com/questions/98153/whats-the-best-hashing-algorithm-to-use-on-a-stl-string-when-using-hash-map
//
CAX_INLINE
unsigned int hash(const char * s, unsigned int seed = 0)
{
    unsigned hash = seed;
    while (*s)
    {
        hash = hash * 101  +  *s++;
    }
    return hash;
}

inline float secs(unsigned int key)
{
    return ((float)(t_stop[key] - t_start[key])) / ((float)CLOCKS_PER_SEC);
}

////////////////////////////////////////////////////////////////////////////////////////

CAX_INLINE
void timer_start(const char * msg)
{
    unsigned int key = hash(msg);
    t_start[key] = clock();
    msgs[key]        = msg;

    if (first_call == 0) first_call = key;
}

CAX_INLINE
void timer_stop(const char * msg)
{
    unsigned int key = hash(msg);
    t_stop[key]  = clock();

    if (key == first_call)
    {
        logger << "\n" << msgs[key] << " (" << secs(key) << " secs)" << endl;

        for(auto obj : t_start)
        {
            if (obj.first != first_call)
            {
                logger << "   - " << msgs[key] << " (" << secs(key) << " secs)" << endl;
            }
        }
        logger << endl;

        // reset timer
        //
        t_start.clear();
        t_stop.clear();
        msgs.clear();
        first_call = 0;
    }
}

}
