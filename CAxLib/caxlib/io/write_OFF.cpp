/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2014.
*/

#include "write_OFF.h"

#include <iostream>

namespace caxlib
{

CAX_INLINE
void write_OFF(const char                * filename,
              const std::vector<double> & xyz,
              const std::vector<u_int>  & tri)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OFF() : couldn't save file " << filename << endl;
        exit(-1);
    }

    fprintf (fp, "OFF\n%zu %zu 0\n", xyz.size()/3, tri.size()/3);

    for(size_t i=0; i<xyz.size(); i+=3)
    {
        //http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(fp, "%.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
    }

    for(size_t i=0; i<tri.size(); i+=3)
    {
        fprintf(fp, "3 %d %d %d\n", tri[i], tri[i+1], tri[i+2]);
    }

    fclose(fp);
}

}
