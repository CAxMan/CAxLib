#include <caxlib/process_plan/global_checks.h>
#include <caxlib/trimesh/trimesh.h>
#include <caxlib/tetmesh/tetmesh.h>
#include <caxlib/tetgen_wrap.h>

#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        caxlib::logger << "Usage: ./detect_voids input.zip output.zip" << caxlib::endl;
        return 0;
    }

    caxlib::Trimesh m(argv[1]);

    bool has_voids = caxlib::detect_voids(m); //, angle_thresh, dirs_pool_size);

    m.save(argv[2]);

    if (has_voids)
    	return 1;

    return 0;
}
