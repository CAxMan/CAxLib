#include "write_MESH.h"

#include <iostream>

namespace caxlib
{

CAX_INLINE
void write_MESH(const char                * filename,
                const std::vector<double> & xyz,
                const std::vector<u_int>  & tets)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_MESH() : couldn't write output file " << filename << endl;
        exit(-1);
    }

    fprintf(fp, "MeshVersionFormatted 1\n" );
    fprintf(fp, "Dimension 3\n" );

    int nv = (int)xyz.size()/3;
    int nt = (int)tets.size()/4;

    if (nv > 0)
    {
        fprintf(fp, "Vertices\n" );
        fprintf(fp, "%d\n", nv);

        for(size_t i=0; i<xyz.size(); i+=3)
        {
            //http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
            //
            fprintf( fp, "%.17g %.17g %.17g 0\n", xyz[i], xyz[i+1], xyz[i+2]);
        }
    }

    if (nt > 0)
    {
        fprintf(fp, "Tetrahedra\n" );
        fprintf(fp, "%d\n", nt );

        for(size_t i=0; i<tets.size(); i+=4)
        {
            fprintf(fp, "%d %d %d %d 0\n", tets[i+0]+1, tets[i+1]+1, tets[i+2]+1, tets[i+3]+1);
        }
    }

    fprintf(fp, "End\n\n");
    fclose(fp);
}

}
