#include "read_MESH.h"

#include <iostream>

namespace caxlib
{

CAX_INLINE
void read_MESH(const char          * filename,
               std::vector<double> & xyz,
               std::vector<u_int>  & tets)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_MESH() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    char line[1024];

    fgets(line,1024,fp);
    fgets(line,1024,fp);

    while(fgets(line, 1024, fp))
    {
        // read vertices
        //
        if (line[0] == 'V')
        {
            int    nverts;
            double x,y,z;
            fgets(line,1024,fp);
            sscanf(line, "%d", &nverts);
            for(int i=0; i<nverts; ++i)
            {
                //http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
                //
                fgets(line, 1024, fp);
                sscanf(line, "%lf %lf %lf", &x, &y, &z);
                xyz.push_back(x);
                xyz.push_back(y);
                xyz.push_back(z);
            }
        }

        // read tetrahedra
        //
        if (line[0] == 'T' && line[1] == 'e')
        {
            int ntets, v0, v1, v2, v3;
            fgets(line, 1024, fp);
            sscanf(line, "%d", &ntets);
            for( int i=0; i<ntets; i++ )
            {
                fgets(line,1024,fp);
                sscanf(line, "%d %d %d %d", &v0, &v1, &v2, &v3);
                tets.push_back(v0 - 1);
                tets.push_back(v1 - 1);
                tets.push_back(v2 - 1);
                tets.push_back(v3 - 1);
            }
        }
    }

    fclose(fp);
}

}
