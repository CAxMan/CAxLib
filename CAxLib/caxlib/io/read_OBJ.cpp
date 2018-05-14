/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2014.
*/

#include "read_OBJ.h"

#include <iostream>

namespace caxlib
{

CAX_INLINE
uint read_point_id(char *s)
{
    int pos = 0;
    while(s[pos] != '\0')
    {
        if(s[pos] == '/')
        {
            s[pos] = ' ';
        }
        ++pos;
    }
    sscanf(s, "%d", &pos);
    return pos-1;
}


CAX_INLINE
void read_OBJ(const char           * filename,
              std::vector<double>  & xyz,
              std::vector<u_int>   & tri)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_OBJ() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    char line[1024],
          s0[1024],
          s1[1024],
          s2[1024];

    while(fgets(line, 1024, fp))
    {
        switch(line[0])
        {
            case 'v':

                if(!isspace(line[1])) continue;
                if(line[1] != 't' && line[1] != 'n' )
                {
                    //http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
                    //
                    double x, y, z;
                    sscanf( line, "%*c%lf %lf %lf", &x, &y, &z );
                    xyz.push_back(x);
                    xyz.push_back(y);
                    xyz.push_back(z);
                }
                break;

            case 'f':
                sscanf(line, "%*c%s%s%s", s0, s1, s2);
                tri.push_back(read_point_id(s0));
                tri.push_back(read_point_id(s1));
                tri.push_back(read_point_id(s2));
                break;
        }
    }
    fclose(fp);
}

}
