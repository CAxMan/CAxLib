#ifndef SPHERE_COVERAGE_H
#define SPHERE_COVERAGE_H

#include "caxlib.h"
#include "vec3.h"

#include <vector>

namespace caxlib
{

// Compute an approximate (though good) approximation of the even
// coverage of a sphere. Reference:
//
// http://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere
//
CAX_INLINE void sphere_coverage(const int n_samples, std::vector<vec3d> & points)
{
    assert(points.empty());

    srand(time(NULL));
    double rnd      = rand() * n_samples;
    double offset   = 2.0/double(n_samples);
    double increment = M_PI * (3.0 - sqrt(5.0));

    for(int i=0; i<n_samples; ++i)
    {
        double y   = ((i * offset) - 1) + (offset / 2);
        double r   = sqrt(1 - pow(y,2));
        double phi = ((i + int(rnd)) % n_samples) * increment;
        double x   = cos(phi) * r;
        double z   = sin(phi) * r;

        points.push_back(vec3d(x,y,z));
    }
}

}
#endif // SPHERE_COVERAGE_H
