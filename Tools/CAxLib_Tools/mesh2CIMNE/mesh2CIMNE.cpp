#include <cinolib/geometry/vec3.h>
#include <cinolib/io/read_write.h>
#include <iostream>
#include <assert.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "\nUsage: mesh2CIMNE mesh_in.mesh mesh_out.CIMNE\n" << std::endl;
        return -1;
    }
    std::vector<cinolib::vec3d> verts;
    std::vector<std::vector<uint>> polys;
    cinolib::read_MESH(argv[1], verts, polys);

    FILE *fp = fopen(argv[2], "w");
    if(!fp) exit(-1);

    for(uint i=0; i<polys.size(); ++i)
    {
        assert(polys.at(i).size()==4); // make sure it's a tet
        fprintf(fp, "%d 1 %d %d %d %d\n", i+1, polys.at(i).at(0)+1, polys.at(i).at(1)+1, polys.at(i).at(2)+1, polys.at(i).at(3)+1);
    }

    for(uint i=0; i<verts.size(); ++i)
    {
        fprintf(fp, "%d %.17g %.17g %.17g\n", i+1, verts.at(i).x(), verts.at(i).y(), verts.at(i).z());
    }
    fclose(fp);
    return 0;
}
