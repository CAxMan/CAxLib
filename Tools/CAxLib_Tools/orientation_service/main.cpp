#include <caxlib/process_plan/orient.h>
#include <caxlib/trimesh/trimesh.h>

#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        caxlib::logger << "Choose the best orientation according to three metrics (or any combination of them)." << caxlib::endl;
        caxlib::logger << "Metrics are:" << caxlib::endl;
        caxlib::logger << "" << caxlib::endl;
        caxlib::logger << "  - Maximal Surface Quality (i.e. minimize cusp height error)" << caxlib::endl;
        caxlib::logger << "  - Minimal print time (i.e., minimize number of slices)" << caxlib::endl;
        caxlib::logger << "  - Minimal Support Structures (i.e., minimize overhangs)" << caxlib::endl;
        caxlib::logger << "" << caxlib::endl;
        caxlib::logger << "The influence of these metrics is defined by three scalars (wgt_srf_quality, wgt_print_time, wgt_supports)." << caxlib::endl;
        caxlib::logger << "This function minimizes a functional defined as the weighted sum of these three components. Weigths must" << caxlib::endl;
        caxlib::logger << "sum up to 1." << caxlib::endl;
        caxlib::logger << "Usage: ./orientation_service input.zip wgt_srf_quality wgt_print_time wgt_supports output.zip" << caxlib::endl;
        return 0;
    }

    double wgt_srf_quality = atof(argv[2]);
    double wgt_print_time  = atof(argv[3]);
    double wgt_supports    = atof(argv[4]);

    double angle_thresh   = 30.0;
    int    dirs_pool_size = 100;

    caxlib::Trimesh m(argv[1]);
    bool ok = caxlib::orient(m, wgt_srf_quality, wgt_print_time, wgt_supports, angle_thresh, dirs_pool_size);

    m.save(argv[5]);

    if (ok) 
        return 0;

    return -1;
}
