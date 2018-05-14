#include <caxlib/process_plan/global_checks.h>
#include <caxlib/trimesh/trimesh.h>

#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        caxlib::logger << "Usage: ./checks_service input.zip output.zip" << caxlib::endl;
        return 0;
    }

    caxlib::Trimesh m(argv[1]);
    caxlib::check_material(m);
    caxlib::check_size(m);
    caxlib::check_weight(m);

    m.save(argv[2]);

    return 0;
}
