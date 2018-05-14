#ifndef ANNOTATIONS_H
#define ANNOTATIONS_H

#include <iostream>
#include <set>
#include <vector>

#include "../libraries/materials_library.h"
#include "../libraries/printers_library.h"

namespace caxlib
{

    // CAXMAN Global Annotations. Reference:
    //
    // https://project.sintef.no/eRoom/math/CAxMan/0_431c6
    //
    typedef struct
    {
        Printer  printer;
        Material material;

        double   orientation[9] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 }; // 3x3 orientation matrix
        bool     too_big              = false;        // Absolute size
        bool     too_heavy            = false;        // Part weight
        bool     no_legal_orientation = false;        // Unorientable item

        std::vector<std::string> extra_annotations; // to store extra information not explicitly encoded in the data structure
    }
    GlobalAnnotations;



    // CAXMAN Local Vertex Annotations.
    //
    typedef struct
    {
        std::set<std::string> extra_annotations; // to store extra information not explicitly encoded in the data structure
    }
    VertexAnnotations;


    // CAXMAN Local Triangle Annotations. Reference:
    //
    // https://project.sintef.no/eRoom/math/CAxMan/0_431c6
    //
    typedef struct
    {
        bool bad_geometry   = false;    // Bad geometry
        bool bad_material   = false;    // Material
        bool thin_walls     = false;    // Thin walls
        bool thin_channels  = false;    // Channel dimensions
        bool overhangs      = false;    // Overhangs
        bool weak_feature   = false;    // Weak feature
        bool mach_allowance = false;    // Machining allowance
        u_int closed_voids  = 0;        // Closed void index

        std::set<std::string> extra_annotations; // to store extra information not explicitly encoded in the data structure
    }
    TriangleAnnotations;

}

#endif // ANNOTATIONS_H
