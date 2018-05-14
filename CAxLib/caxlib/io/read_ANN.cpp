/**
 @author    Daniela Cabiddu (dani.cabiddu@gmail.com)
 @copyright Daniela Cabiddu 2016.
*/

#include "read_ANN.h"
#include "read_write_ANN.h"

#include "tinyxml2.h" // external library TINYXML2

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>


using namespace tinyxml2;

namespace caxlib
{

CAX_INLINE
void exitOnEmptyList (std::vector<std::string> & list)
{
    if(list.size() == 0)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : couldn't read the list of annotated elements " << std::endl;
        exit(-1);
    }
}

CAX_INLINE
void exitOnInvalidElementIndex (int index, u_int threshold )
{
    if (index < 0 || index >= threshold)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : invalid index of annotated element " << index << "[ 0 - " << threshold << " ]" << std::endl;
        exit(-1);
    }
}

CAX_INLINE
std::vector<std::string> getAnnotatedLocalGeometry (XMLElement * element, const std::string elementType)
{
    XMLElement *elements = element->FirstChildElement(elementType.c_str());

    std::istringstream iss(elements->GetText());
    std::string token;
    std::vector<std::string> lelements;

    while (iss >> token)
        lelements.push_back(token);

    return lelements;
}


CAX_INLINE
void read_ANN(const char                       * filename,
              GlobalAnnotations                & glob_ann,
              std::vector<VertexAnnotations>   & vertex_ann,
              std::vector<TriangleAnnotations> & triangle_ann)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8");

    {
        TIXMLASSERT( true );
    }

    XMLDocument* doc = new XMLDocument();
    doc->LoadFile( filename );

    if (doc->ErrorID() != 0)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : couldn't load input file " << filename << std::endl;
        exit(-1);
    }

    XMLElement* root = doc->FirstChildElement();

    if( root == NULL || root->Name() != ROOT_NAME)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : couldn't read root element " << filename << std::endl;
        exit(-1);
    }

    XMLElement* annotation = root->FirstChildElement(ANNOTATION_NAME.c_str());

    u_int closed_void_index = 1;

    while (annotation != NULL) {

        if(ANNOTATION_NAME.compare(annotation->Name()) != 0)
        {        
            std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : couldn't read annotation element " << annotation->Name() << std::endl;
            exit(-1);
        }

        XMLElement *identifier = annotation->FirstChildElement(IDENTIFIER_NAME.c_str());

        if(identifier == NULL)
        {
            std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : couldn't read annotation identifier " << annotation->Name() << std::endl;
            exit(-1);
        }

        const char *id_value = identifier->GetText();

        if (PRINTING_PARAMETERS.compare(id_value) == 0)
        {
            XMLElement *element = annotation->FirstChildElement();

            while (element != NULL)
            {
                if (MATERIAL.compare(element->Name()) == 0)
                {
                    glob_ann.material.name = element->GetText();
                }
                else
                if (LAYER_THICKNESS.compare(element->Name()) == 0)
                {
                    glob_ann.printer.layer_thickness = atof(element->GetText());
                }
                else
                if (LASER_BEAM_DIAMETER.compare(element->Name()) == 0)
                {
                    glob_ann.printer.laser_beam_diameter = atof(element->GetText());
                }
                else
                if (CHAMBER_DIMENSIONS.compare(element->Name()) == 0)
                {
                    std::istringstream is(element->GetText());
                    std::string dim;

                    for (uint i = 0; i < 3; i++)
                    {
                        if (is >> dim)
                            glob_ann.printer.chamber_dimension[i] = atof(dim.c_str());
                        else
                        {
                            std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : invalid chamber dimensions. " << std::endl;
                            exit(-1);
                        }
                    }
                }
                else
                if (CHAMBER_TEMPERATURE.compare(element->Name()) == 0)
                {
                    glob_ann.printer.chamber_temperature = atof(element->GetText());
                }
                else
                if (THERMAL_SHRINKAGE.compare(element->Name()) == 0)
                {
                    glob_ann.material.thermal_shrinkage = atof(element->GetText());
                }
                else
                if (INSTANTANEOUS_ELASTIC_MODULUS.compare(element->Name()) == 0)
                {
                    glob_ann.material.instantaneous_elastic_modulus = atof(element->GetText());
                }
                else
                if (ELASTIC_MODULUS_AT_INFINITE_TIME.compare(element->Name()) == 0)
                {
                    glob_ann.material.elastic_modulus_at_infinite_time = atof(element->GetText());
                }

                if (POISSON_RATIO.compare(element->Name()) == 0)
                {
                    glob_ann.material.poisson_ratio = atof(element->GetText());
                }
                else
                if (ELASTIC_VISCOSITY.compare(element->Name()) == 0)
                {
                    glob_ann.material.elastic_viscosity = atof(element->GetText());
                }
                else
                if (YIELD_STRESS.compare(element->Name()) == 0)
                {
                    glob_ann.material.yield_stress = atof(element->GetText());
                }
                else
                if (SATURATION_STRESS.compare(element->Name()) == 0)
                {
                    glob_ann.material.saturation_stress = atof(element->GetText());
                }
                else
                if (PLASTIC_VISCOSITY.compare(element->Name()) == 0)
                {
                    glob_ann.material.plastic_viscosity = atof(element->GetText());
                }
                else
                if (NO_PRINT_ZONE.compare(element->Name()) == 0)
                {
                    std::istringstream iss(element->GetText());
                    Bbox npz;

                    if (! (iss >> npz.min[0] >> npz.min[1] >> npz.min[2] >> npz.max[0] >> npz.max[1] >> npz.max[2]))
                    {
                        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : invalid no-print zone. " << std::endl;
                        exit(-1);
                    }

                    glob_ann.printer.no_print_zones.push_back(npz);
                }
                else
                if (ISOTROPIC_HARDENING_LAW.compare(element->Name()) == 0)
                {
                    glob_ann.material.isotropic_hardening_law = element->GetText();
                }
                else
                if (ISOTROPIC_HARDENING_COEFFICIENT.compare(element->Name()) == 0)
                {
                    glob_ann.material.isotropic_hardening_coefficient = atof(element->GetText());
                }
                else
                if (THERMAL_EXPANSION_COEFFICIENT.compare(element->Name()) == 0)
                {
                    glob_ann.material.thermal_expansion_coefficient = atof(element->GetText());
                }

                element = element->NextSiblingElement();
            }
        }
        else
        if (ORIENTATION.compare(id_value) == 0)
        {
            caxlib::logger << " Annotation [ " << ORIENTATION << " ] found. " << std::endl;

            std::vector<std::string> lmatrix = getAnnotatedLocalGeometry(annotation, MATRIX);

            if (lmatrix.size() != 9)
            {
                std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : couldn't read orientation matrix in" << annotation->Name() << std::endl;
                exit(-1);
            }

            for (u_int i=0; i < lmatrix.size(); i++)
                glob_ann.orientation[i] = atof(lmatrix.at(i).c_str());
        }
        else
        if (TOO_BIG.compare(id_value) == 0)     // Relative size
        {
            caxlib::logger << " Annotation [ " << TOO_BIG << " ] found. " << std::endl;

            glob_ann.too_big = true;
        }
        else
        if (TOO_HEAVY.compare(id_value) == 0)       // Part weight
        {
            caxlib::logger << " Annotation [ " << TOO_HEAVY << " ] found. " << std::endl;

            glob_ann.too_heavy = true;
        }
        else
        if (BAD_GEOMETRY.compare(id_value) == 0)
        {
            std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

            exitOnEmptyList(ltriangles);

            for (u_int i=0; i < ltriangles.size(); i++)
            {
                int index = atoi(ltriangles.at(i).c_str());

                exitOnInvalidElementIndex(index, triangle_ann.size());

                triangle_ann.at(index).bad_geometry = true;
            }

            caxlib::logger << BAD_GEOMETRY << ": Annotated " << ltriangles.size() << " " << TRIANGLES << std::endl;
        }
        else
        if (CLOSED_VOIDS.compare(id_value) == 0)
        {
            std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

            exitOnEmptyList(ltriangles);

            for (u_int i=0; i < ltriangles.size(); i++)
            {
                int index = atoi(ltriangles.at(i).c_str());

                exitOnInvalidElementIndex(index, triangle_ann.size());

                triangle_ann.at(index).closed_voids = closed_void_index;
            }

            closed_void_index++;

            caxlib::logger << CLOSED_VOIDS << ": Annotated " << ltriangles.size() << " " << TRIANGLES << std::endl;
        }
        else
        if (BAD_MATERIAL.compare(id_value) == 0)
        {
            std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

            exitOnEmptyList(ltriangles);

            for (u_int i=0; i < ltriangles.size(); i++)
            {
                int index = atoi(ltriangles.at(i).c_str());

                exitOnInvalidElementIndex(index, triangle_ann.size());

                triangle_ann.at(index).bad_material = true;
            }

            caxlib::logger << BAD_MATERIAL << ": Annotated " << ltriangles.size() << " " << TRIANGLES << std::endl;
        }
        else
        if (THIN_WALLS_REL.compare(id_value) == 0)
        {
            std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

            exitOnEmptyList(ltriangles);

            for (u_int i=0; i < ltriangles.size(); i++)
            {
                int index = atoi(ltriangles.at(i).c_str());

                exitOnInvalidElementIndex(index, triangle_ann.size());

                triangle_ann.at(index).thin_walls = true;
            }

            caxlib::logger << THIN_WALLS_REL << ": Annotated " << ltriangles.size() << " " << TRIANGLES << std::endl;
        }
        else
        if (THIN_CHANNELS.compare(id_value) == 0)
        {
            std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

            exitOnEmptyList(ltriangles);

            for (u_int i=0; i < ltriangles.size(); i++)
            {
                int index = atoi(ltriangles.at(i).c_str());

                exitOnInvalidElementIndex(index, triangle_ann.size());

                triangle_ann.at(index).thin_channels = true;
            }

            caxlib::logger << THIN_CHANNELS << ": Annotated " << ltriangles.size() << " " << TRIANGLES << std::endl;
        }
        else
        if (OVERHANGS.compare(id_value) == 0)
        {
            std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

            exitOnEmptyList(ltriangles);

            for (u_int i=0; i < ltriangles.size(); i++)
            {
                int index = atoi(ltriangles.at(i).c_str());

                exitOnInvalidElementIndex(index, triangle_ann.size());

                triangle_ann.at(index).overhangs = true;
            }

            caxlib::logger << OVERHANGS << ": Annotated " << ltriangles.size() << " " << TRIANGLES << std::endl;
        }
        else
        if (WEAK_FEATURES.compare(id_value) == 0)
        {
            std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

            exitOnEmptyList(ltriangles);

            for (u_int i=0; i < ltriangles.size(); i++)
            {
                int index = atoi(ltriangles.at(i).c_str());

                exitOnInvalidElementIndex(index, triangle_ann.size());

                triangle_ann.at(index).weak_feature = true;
            }

            caxlib::logger << WEAK_FEATURES << ": Annotated " << ltriangles.size() << " " << TRIANGLES << std::endl;
        }
        else
        if (MACH_ALLOWANCE.compare(id_value) == 0)
        {
            std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

            exitOnEmptyList(ltriangles);

            for (u_int i=0; i < ltriangles.size(); i++)
            {
                int index = atoi(ltriangles.at(i).c_str());

                exitOnInvalidElementIndex(index, triangle_ann.size());

                triangle_ann.at(index).mach_allowance = true;
            }

            caxlib::logger << MACH_ALLOWANCE << ": Annotated " << ltriangles.size() << " " << TRIANGLES << std::endl;
        }
        else
        {
            // Generic Annotation
            //
            caxlib::logger << " Annotation [ " << id_value << " ] found. " << std::endl;

            std::vector<std::string> lvertices = getAnnotatedLocalGeometry(annotation, VERTICES);
            std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

            if (lvertices.size()== 0 && ltriangles.size() == 0)
                glob_ann.extra_annotations.push_back(id_value);
            else
            {
                for (u_int i=0; i < lvertices.size(); i++)
                {
                    int index = atoi(lvertices.at(i).c_str());

                    exitOnInvalidElementIndex(index, vertex_ann.size());

                    vertex_ann.at(index).extra_annotations.insert(id_value);
                }

                if (lvertices.size() > 0)
                    caxlib::logger << id_value << ": Annotated " << lvertices.size() << " " << VERTICES << std::endl;

                for (u_int i=0; i < ltriangles.size(); i++)
                {
                    int index = atoi(ltriangles.at(i).c_str());

                    exitOnInvalidElementIndex(index, triangle_ann.size());

                    triangle_ann.at(index).extra_annotations.insert(id_value);
                }

                if (ltriangles.size() > 0)
                    caxlib::logger << id_value << ": Annotated " << ltriangles.size() << " " << TRIANGLES << std::endl;
            }
        }

        annotation = annotation->NextSiblingElement();
    }
}

}


