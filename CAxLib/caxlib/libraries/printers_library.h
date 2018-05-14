#ifndef PRINTERS_LIBRARY_H
#define PRINTERS_LIBRARY_H

#include "materials_library.h"

#include <caxlib/bbox.h>
#include <string>
#include <vector>

namespace caxlib
{

    typedef struct
    {
        std::string           name;
        double                layer_thickness_min;
        double                layer_thickness_max;
        double                layer_thickness;
        double                laser_beam_diameter_min;
        double                laser_beam_diameter_max;
        double                laser_beam_diameter;
        double                chamber_temperature_min;
        double                chamber_temperature_max;
        double                chamber_temperature;
        double                chamber_dimension[3];
        double                weight_max;
        std::vector<Bbox>     no_print_zones;
        std::vector<Material> supported_materials;
    }
    Printer;

    // examples... (to be valorized)
    static Printer EOSM627;
    static Printer HPFusionJet;

}

#endif // PRINTERS_LIBRARY_H
