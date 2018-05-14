#include "read_TET.h"

#include <iostream>

namespace caxlib
{

CAX_INLINE
void read_TET(const char          * filename,
              std::vector<double> & xyz,
              std::vector<u_int>  & tets)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_TET() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    int  nv, nt;
    char line[1024];

    fgets(line,1024,fp);
    sscanf(line, "%d vertices", &nv);

    fgets(line,1024,fp);
    sscanf(line, "%d tets", &nt);

    for(int vid=0; vid<nv; ++vid)
    {
        fgets(line, 1024, fp);

        //http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        double x,y,z;
        sscanf(line, "%lf %lf %lf ", &x, &y, &z);

        xyz.push_back(x);
        xyz.push_back(y);
        xyz.push_back(z);
    }

    for(int tid=0; tid<nt; ++tid)
    {
        fgets(line, 1024, fp);

        int v0, v1, v2, v3;
        sscanf(line, "4 %d %d %d %d", &v0, &v1, &v2, &v3);

        tets.push_back(v0);
        tets.push_back(v3);
        tets.push_back(v2);
        tets.push_back(v1);
    }

    fclose(fp);
}

}
