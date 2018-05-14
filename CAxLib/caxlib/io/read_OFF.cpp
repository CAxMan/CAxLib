/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2014.
*/

#include "read_OFF.h"

#include <iostream>

namespace caxlib
{

CAX_INLINE
void read_OFF(const char          * filename,
              std::vector<double> & xyz,
              std::vector<u_int>  & tri)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_OFF() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    int nv, nt, dummy;

    fscanf(fp, "OFF\n");
    fscanf(fp, "%d %d %d\n", &nv, &nt, &dummy);

    for(int i=0; i<nv; ++i)
    {
        //http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        double x, y, z;
        fscanf(fp, "%lf %lf %lf\n", &x, &y, &z);
        xyz.push_back(x);
        xyz.push_back(y);
        xyz.push_back(z);
    }

    for(int i=0; i<nt; ++i)
    {
        int v0, v1, v2;
        fscanf(fp, "%*d %d %d %d\n", &v0, &v1, &v2);
        tri.push_back(v0);
        tri.push_back(v1);
        tri.push_back(v2);
    }

    fclose(fp);
}

}
