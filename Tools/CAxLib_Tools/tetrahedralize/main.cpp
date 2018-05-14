#include <cinolib/meshes/tetmesh.h>
#include <cinolib/meshes/trimesh.h>
#include <cinolib/tetgen_wrap.h>

using namespace std;
using namespace cinolib;

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "\nUsage: tetrahedralize <srf_mesh> <target_size>\n" << std::endl;
        return -1;
    }

    logger.disable();

    Trimesh<> srf_m(argv[1]);
    uint target_size  = atoi(argv[2]);
    float mesh_volume = srf_m.mesh_volume();
    float vol_thresh  = 1.25 * (mesh_volume / static_cast<float>(target_size));

    std::cout << "\n" << std::endl;
    std::cout << "read " << argv[1] << std::endl;
    std::cout << "target size: " << target_size << " elements" << std::endl;
    std::cout << "overall mesh volume: " << mesh_volume << std::endl;
    std::cout << "max volume per tet : " << vol_thresh << std::endl;
    std::cout << "\n" << std::endl;

    std::vector<double> coords = srf_m.vector_coords();
    std::vector<uint>   tris   = serialized_vids_from_faces(srf_m.vector_polys());
    std::vector<uint>   edges, tets;
    std::vector<double> coords_out;

    do
    {
        char flags[1024];
        sprintf(flags, "a%f", vol_thresh);

        tetgen_wrap(coords, tris, edges, flags, coords_out, tets);

        vol_thresh *= 1.5;
    }
    while (tets.size()/4 > target_size * 0.9);

    Tetmesh<> m(coords_out, tets);
    std::string s(argv[1]);
    s.append(".vtu");
    m.save(s.c_str());

    return 0;
}
