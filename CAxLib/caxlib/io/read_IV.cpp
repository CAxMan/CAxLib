#include "read_IV.h"

#include <iostream>

namespace caxlib
{

CAX_INLINE
bool seek_keyword(FILE *fp, const char *kw)
{
    static char s[256];
    s[0]='\0';
    do fscanf(fp,"%255s",s); while (strcmp(s,kw) && !feof(fp));
    if (feof(fp)) return 0;
    return 1;
}

CAX_INLINE
void read_IV(const char          * filename,
             std::vector<double> & xyz,
             std::vector<u_int>  & tri,
             std::vector<int>    & patch)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_IV() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    assert(seek_keyword(fp, "vertex"));
    assert(seek_keyword(fp, "["));

    //http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
    //
    double x,y,z;
    while (fscanf(fp, "%lf %lf %lf,", &x, &y, &z) == 3)
    {
        xyz.push_back(x);
        xyz.push_back(y);
        xyz.push_back(z);
    }

    assert(seek_keyword(fp, "coordIndex"));
    assert(seek_keyword(fp, "["));

    int v0, v1, v2;
    while (fscanf(fp,"%d, %d, %d, -1,", &v0, &v1, &v2) == 3)
    {
        tri.push_back(v0);
        tri.push_back(v1);
        tri.push_back(v2);
    }

    if (seek_keyword(fp, "materialIndex"))
    {
        assert(seek_keyword(fp, "["));

        int patch_id;
        while (fscanf(fp,"%d,", &patch_id) == 1)
        {
            patch.push_back(patch_id);
        }
    }

    fclose(fp);
}


}
