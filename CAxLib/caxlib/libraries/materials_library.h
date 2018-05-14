#ifndef MATERIALS_LIBRARY_H
#define MATERIALS_LIBRARY_H

#include <vector>

// CAXMAN Printing parameters. Reference:
//
// https://project.sintef.no/eRoom/math/CAxMan/0_431c6
//
typedef struct
{
    std::string name;
    std::string isotropic_hardening_law;
    double      relative_weight;
    double      thermal_expansion_coefficient;
    double      thermal_shrinkage;
    double      instantaneous_elastic_modulus;
    double      elastic_modulus_at_infinite_time;
    double      poisson_ratio;
    double      elastic_viscosity;
    double      yield_stress;
    double      saturation_stress;
    double      isotropic_hardening_coefficient;
    double      plastic_viscosity;
    double      critical_temperature;
}
Material;

// examples... (to be valorized)
static Material Aluminum;
static Material Steel;
static Material Titanium;

#endif // PRINTERS_LIBRARY_H
