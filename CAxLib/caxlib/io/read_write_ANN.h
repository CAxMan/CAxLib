#ifndef READ_WRITE_ANN_H
#define READ_WRITE_ANN_H

// List of element IDs used in the .ANN codification
//
const std::string ROOT_NAME         = "Annotations";
const std::string ANNOTATION_NAME   = "Annotation";
const std::string IDENTIFIER_NAME   = "Identifier";

const std::string PRINTING_PARAMETERS   = "Printing_parameters";

const std::string MATERIAL              = "Material";
const std::string LAYER_THICKNESS       = "Layer_Thickness";
const std::string LASER_BEAM_DIAMETER   = "Laser_Beam_Diameter";
const std::string CHAMBER_DIMENSIONS    = "Chamber_Size";
const std::string CHAMBER_TEMPERATURE   = "Chamber_Temperature";
const std::string THERMAL_SHRINKAGE     = "Thermal_Shrinkage";
const std::string POISSON_RATIO         = "Poisson_Ratio";
const std::string ELASTIC_VISCOSITY     = "Elastic_Viscosity";
const std::string YIELD_STRESS          = "Yield_Stress";
const std::string SATURATION_STRESS     = "Saturation_Stress";
const std::string PLASTIC_VISCOSITY     = "Plastic_Viscosity";
const std::string NO_PRINT_ZONE         = "No_Print_Zone";
const std::string CRITICAL_TEMPERATURE  = "Critical_Temperature";

const std::string INSTANTANEOUS_ELASTIC_MODULUS     = "Instantaneous_Elastic_Modulus";
const std::string ELASTIC_MODULUS_AT_INFINITE_TIME  = "Elastic_Modulus_At_Infinite_Time";
const std::string ISOTROPIC_HARDENING_LAW           = "Isotropic_Hardening_Law";
const std::string ISOTROPIC_HARDENING_COEFFICIENT   = "Isotropic_Hardening_Coefficient";
const std::string THERMAL_EXPANSION_COEFFICIENT     = "Thermal_Expansion_Coefficient";

// List of element IDs used in the .ANN codification
//
const std::string ORIENTATION = "Orientation_matrix";
const std::string TOO_BIG = "Size";
const std::string TOO_HEAVY = "Part_Weight";
const std::string NO_ORIENTATION = "Orientation_Failure";
const std::string MATERIAL_USAGE = "Material_Usage";
const std::string BUILD_TIME = "Build_Time";

const std::string BAD_GEOMETRY = "Bad_Geometry";       // Bad geometry
const std::string CLOSED_VOIDS = "Closed_Voids";       // Closed voidss
const std::string BAD_MATERIAL = "Bad_Material";       // Material
const std::string THIN_WALLS_ABS = "Absolute_Thin_Walls";     // Absolute thin walls
const std::string THIN_WALLS_REL = "Relative_Thin_Walls";     // Relative thin walls
const std::string THIN_CHANNELS = "Channel_Dimension";       // Channel dimensions
const std::string OVERHANGS = "Overhangs";          // Overhangs
const std::string WEAK_FEATURES = "Weak_Features";      // Weak feature
const std::string MACH_ALLOWANCE = "Mach_Allowance";     // Machining allowance

const std::string TRIANGLES = "Triangles";          // Machining allowance
const std::string VERTICES = "Vertices";           // Machining allowance
const std::string MATRIX = "Matrix";

#endif
