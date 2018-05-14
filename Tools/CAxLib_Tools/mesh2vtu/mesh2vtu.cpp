#include <cinolib/geometry/vec3.h>
#include <cinolib/io/read_write.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "\nUsage: mesh2vtu mesh_in.mesh mesh_out.vtu\n" << std::endl;
        return -1;
    }
    std::vector<cinolib::vec3d> verts;
    std::vector<std::vector<uint>> poly;
    cinolib::read_MESH(argv[1], verts, poly);
    cinolib::write_VTU(argv[2], verts, poly);

    return 0;
}
