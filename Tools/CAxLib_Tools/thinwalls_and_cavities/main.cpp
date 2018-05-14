/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Daniela Cabiddu (daniela.cabiddu@ge.imati.cnr.it)                 *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of EpsilonShapes.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/

#include <iostream>

#include "include/eps3Dshape.h"
#include "include/wrapper_sdf.h"

#include "caxlib/io/read_ZIP.h"

#include <boost/program_options.hpp>

const char *all_param               = "all";
const char *cavities_param          = "cavities";
const char *help_param              = "help";
const char *input_param             = "input";
const char *max_epsilon_param       = "thresh";
const char *output_param            = "output";
const char *store_all_spheres       = "sas";
const char *shape_diameter_param    = "sdf";
const char *parallel_param          = "parallel";
const char *repair_param            = "repair";
const char *thinwalls_param         = "thinwalls";

const char *thresh_perc_param       = "perc";
const char *map_param               = "map";

const char *grey_param              = "grey";

using namespace std;

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    std::string filename;

    std::string output_filename = filename;

    double  max_epsilon                = FLT_MAX;

    bool    store_intermediate_spheres = false;
    bool    compute_all                = false;
    int     do_it_parallel             = 0;
    bool    search_thinwalls           = false;
    bool    search_cavities            = false;
    bool    repair                     = false;
    bool    compute_sdf                = false;


    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        (all_param,                                     "compute epsilon for each triangle")
        (cavities_param,                                "search for cavities")
        (help_param,                                    "produce help message")
        (input_param,       po::value<std::string>(),   "set the input file")
        (map_param,       po::value<std::string>(),   "epsilon map file")
        (max_epsilon_param, po::value<double>(),        "set epsilon threshold (>0)")
        (output_param,      po::value<std::string>(),   "set the output file basename")
        (shape_diameter_param,                          "compute shape diameter function.")
        (store_all_spheres,                             "store all the teted spheres")
        (parallel_param,                                "run the computation in parallel mode")
        (repair_param,                                  "transform the input so that min_eps >= max_eps")
        (thinwalls_param,                               "search for thin walls")
        (thresh_perc_param,   po::value<double>(), "")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    std::cout << std::endl << "//////////////////////////////////////////////////////////////////" << std::endl << std::endl;

    if (argc > 1)
    {

        // Help
        if (vm.count(help_param)) {
            std::cout << desc << "\n";
            return 1;
        }

        // Input Filename
        if (vm.count(input_param)) {
            filename = vm[input_param].as<std::string>();
        } else {
            std::cerr << "An input file must be specified by using --" << input_param << std::endl;
            exit(1);
        }

        // Output Filename
        if (vm.count(output_param)) {
            output_filename = vm[output_param].as<std::string>();
        } else {
            //std::cerr << "An output base name must be specified by using --" << output_param << std::endl;
            //exit(1);

            output_filename = filename + "_OUT" ;
        }

        // Epsilon threshold
        if (vm.count(max_epsilon_param))
        {
            if (vm[max_epsilon_param].as<double>() > 0.0)
                max_epsilon = vm[max_epsilon_param].as<double>();
            else
            {
                std::cerr << "Max Epsilon must be positive." << std::endl;
                exit(1);
            }
        }

        // Search Direction
        if (vm.count(thinwalls_param))
            search_thinwalls = true;

        if (vm.count(cavities_param))
            search_cavities = true;

        if (vm.count(shape_diameter_param))
            compute_sdf = true;

        if (!search_cavities && !search_thinwalls && !compute_sdf)
        {
            std::cerr << "At least one inbtween --" << cavities_param << " , --" << thinwalls_param << " and --"
                      << shape_diameter_param << " must be used." << std::endl;
            exit(1);
        }

        // Parallelism
        if (vm.count(parallel_param))
                do_it_parallel = 1;

        // Repair
        if (vm.count(repair_param))
        {
            repair = true;
        }
    }

    std::cout << "[Input file]: " << filename << std::endl;

    if (search_thinwalls == true)
        std::cout << "Searching thin walls." << std::endl;

    if (search_cavities == true)
        std::cout << "Searching cavities." << std::endl;

    if(do_it_parallel == 1)
        std::cout << "Parallelism enabled." << std::endl;
    else
        std::cout << "Iterative process." << std::endl;

    std::cout << std::endl << "//////////////////////////////////////////////////////////////////" << std::endl << std::endl;

    std::cout << "Loading ... " << std::endl;
    danilib::Epsilon3DShape eps3D (filename.c_str());

    eps3D.set_max_epsilon(eps3D.tin.global_annotations().printer.laser_beam_diameter);
    eps3D.set_store_all_spheres(store_intermediate_spheres);
    eps3D.set_parallelism(do_it_parallel);
    eps3D.set_compute_all(compute_all);

    eps3D.save_annotations_ZIP(output_filename.c_str());

    return 0;
}
