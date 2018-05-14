#include "write_TET.h"

#include <iostream>

namespace caxlib
{

CAX_INLINE
void write_TET(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<u_int>  & tets)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_TET() : couldn't write output file " << filename << endl;
        exit(-1);
    }

    int nv = (int)xyz.size()/3;
    int nt = (int)tets.size()/4;

    fprintf(fp, "%d vertices\n", nv);
    fprintf(fp, "%d tets\n",     nt);

    if (nv > 0)
    {
        for(size_t i=0; i<xyz.size(); i+=3)
        {
            //http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
            //
            fprintf(fp, "%.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
        }
    }

    if (nt > 0)
    {
        for(size_t i=0; i<tets.size(); i+=4)
        {
            fprintf(fp, "4 %d %d %d %d\n", tets[i+0], tets[i+3], tets[i+2], tets[i+1]);
        }
    }

    fclose(fp);
}

}
