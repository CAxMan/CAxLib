/**
 @author    Daniela Cabiddu (dani.cabiddu@gmail.com)
 @copyright Daniela Cabiddu 2016.
*/

#include "write_ANN.h"

#include "read_write_ANN.h"

#include "tinyxml2.h"

#include <iostream>
#include <map>
#include <string>

using namespace tinyxml2;

static std::string to_string(const double num)
{
    char buff[1024];
    sprintf(buff, "%f", num);
    return std::string(buff);
}


namespace caxlib
{

CAX_INLINE
XMLElement * addAnnotationWithID(XMLDocument *doc, const std::string ID)
{
    XMLElement * annElement = doc->NewElement(ANNOTATION_NAME.c_str());
    doc->FirstChildElement()->InsertEndChild(annElement);

    XMLElement * idElement = doc->NewElement(IDENTIFIER_NAME.c_str());
    idElement->SetText(ID.c_str());

    annElement->InsertEndChild(idElement);

    return annElement;
}

template<class type>
CAX_INLINE
XMLElement * createListOfAnnotatedElements (XMLDocument *doc, const std::string elemType, std::vector<type> & list)
{
    XMLElement * listElement = doc->NewElement(elemType.c_str());

    std::string slist;
    for (u_int i=0; i < list.size(); i++)
    {
        std::stringstream ss;
        ss << list.at(i) << " ";
        slist += ss.str();
    }

    listElement->SetText(slist.c_str());

    return listElement;
}

CAX_INLINE
void write_ANN(const char                              * filename,
               const GlobalAnnotations                 & glob_ann,
               const std::vector<VertexAnnotations>    & vertex_ann,
               const std::vector<TriangleAnnotations>  & triangle_ann)
{

    setlocale(LC_NUMERIC, "en_US.UTF-8");

    {
        TIXMLASSERT( true );
    }

    XMLDocument * doc = new XMLDocument();

    XMLElement * root = doc->NewElement(ROOT_NAME.c_str());
    doc->InsertFirstChild(root);

    XMLElement *ppElement = addAnnotationWithID(doc, PRINTING_PARAMETERS);

    // Printing parameters

    // Material
    //
    if (glob_ann.material.name.compare("") != 0)
    {
        XMLElement *annElement = doc->NewElement(MATERIAL.c_str());
        annElement->SetText(glob_ann.material.name.c_str());
        ppElement->InsertEndChild(annElement);
    }

    // layer_thickness
    //
    if (glob_ann.printer.layer_thickness != 0.0)
    {
        XMLElement *annElement = doc->NewElement(LAYER_THICKNESS.c_str());
        annElement->SetText(to_string(glob_ann.printer.layer_thickness).c_str());
        ppElement->InsertEndChild(annElement);
    }

    // laser_beam_diameter
    //
    if (glob_ann.printer.laser_beam_diameter != 0.0)
    {
        XMLElement *annElement = doc->NewElement(LASER_BEAM_DIAMETER.c_str());
        annElement->SetText(to_string(glob_ann.printer.laser_beam_diameter).c_str());
        ppElement->InsertEndChild(annElement);
    }

    // chamber_temperature
    //
    if (glob_ann.printer.chamber_temperature != 0.0)
    {
        XMLElement *annElement = doc->NewElement(CHAMBER_TEMPERATURE.c_str());
        annElement->SetText(to_string(glob_ann.printer.chamber_temperature).c_str());
        ppElement->InsertEndChild(annElement);
    }

    // thermal_expansion_coefficient
    //
    if (glob_ann.material.thermal_expansion_coefficient != 0.0)
    {
        XMLElement *annElement = doc->NewElement(THERMAL_EXPANSION_COEFFICIENT.c_str());
        annElement->SetText(to_string(glob_ann.material.thermal_expansion_coefficient).c_str());
        ppElement->InsertEndChild(annElement);
    }

    // thermal_shrinkage
    //
    if (glob_ann.material.thermal_shrinkage != 0.0)
    {
        XMLElement *annElement = doc->NewElement(THERMAL_SHRINKAGE.c_str());
        annElement->SetText(to_string(glob_ann.material.thermal_shrinkage).c_str());
        ppElement->InsertEndChild(annElement);
    }

    // instantaneous_elastic_modulus
    //
    if (glob_ann.material.instantaneous_elastic_modulus != 0.0)
    {
        XMLElement *annElement = doc->NewElement(INSTANTANEOUS_ELASTIC_MODULUS.c_str());
        annElement->SetText(to_string(glob_ann.material.instantaneous_elastic_modulus).c_str());
        ppElement->InsertEndChild(annElement);
    }

    // elastic_modulus_at_infinite_time
    //
    if (glob_ann.material.elastic_modulus_at_infinite_time != 0.0)
    {
        XMLElement *annElement = doc->NewElement(ELASTIC_MODULUS_AT_INFINITE_TIME.c_str());
        annElement->SetText(to_string(glob_ann.material.elastic_modulus_at_infinite_time).c_str());
        ppElement->InsertEndChild(annElement);
    }

    // poisson_ratio
    //
    if (glob_ann.material.poisson_ratio != 0.0)
    {
        XMLElement *annElement = doc->NewElement(POISSON_RATIO.c_str());
        annElement->SetText(to_string(glob_ann.material.poisson_ratio).c_str());
        ppElement->InsertEndChild(annElement);
    }

    // elastic_viscosity
    //
    if (glob_ann.material.elastic_viscosity != 0.0)
    {
        XMLElement *annElement = doc->NewElement(ELASTIC_VISCOSITY.c_str());
        annElement->SetText(to_string(glob_ann.material.elastic_viscosity).c_str());
        ppElement->InsertEndChild(annElement);
    }

    // yield_stress
    //
    if (glob_ann.material.yield_stress != 0.0)
    {
        XMLElement *annElement = doc->NewElement(YIELD_STRESS.c_str());
        annElement->SetText(to_string(glob_ann.material.yield_stress).c_str());
        ppElement->InsertEndChild(annElement);
    }

    // saturation_stress
    //
    if (glob_ann.material.saturation_stress != 0.0)
    {
        XMLElement *annElement = doc->NewElement(SATURATION_STRESS.c_str());
        annElement->SetText(to_string(glob_ann.material.saturation_stress).c_str());
        ppElement->InsertEndChild(annElement);
    }

    // isotropic_hardening_law
    //
    if (glob_ann.material.isotropic_hardening_law != "")
    {
        XMLElement *annElement = doc->NewElement(ISOTROPIC_HARDENING_LAW.c_str());
        annElement->SetText(glob_ann.material.isotropic_hardening_law.c_str());
        ppElement->InsertEndChild(annElement);
    }

    // isotropic_hardening_coefficient
    //
    if (glob_ann.material.isotropic_hardening_coefficient!= 0.0)
    {
        XMLElement *annElement = doc->NewElement(ISOTROPIC_HARDENING_COEFFICIENT.c_str());
        annElement->SetText(to_string(glob_ann.material.isotropic_hardening_coefficient).c_str());
        ppElement->InsertEndChild(annElement);
    }


    // plastic_viscosity
    //
    if (glob_ann.material.plastic_viscosity != 0.0)
    {
        XMLElement *annElement = doc->NewElement(PLASTIC_VISCOSITY.c_str());
        annElement->SetText(to_string(glob_ann.material.plastic_viscosity).c_str());
        ppElement->InsertEndChild(annElement);
    }

    // critical_temperature
    //
    if (glob_ann.material.critical_temperature != 0.0)
    {
        XMLElement *annElement = doc->NewElement(CRITICAL_TEMPERATURE.c_str());
        annElement->SetText(to_string(glob_ann.material.critical_temperature).c_str());
        ppElement->InsertEndChild(annElement);
    }

    // chamber_dimensions
    //
    if (glob_ann.printer.chamber_dimension[0] != 0.0 &&
        glob_ann.printer.chamber_dimension[1] != 0.0 &&
        glob_ann.printer.chamber_dimension[2] != 0.0 )
    {
        XMLElement *annElement = doc->NewElement(CHAMBER_DIMENSIONS.c_str());

        std::string dimensions = to_string(glob_ann.printer.chamber_dimension[0]) + " " +
                                 to_string(glob_ann.printer.chamber_dimension[1]) + " " +
                                 to_string(glob_ann.printer.chamber_dimension[2]) ;


        annElement->SetText(dimensions.c_str());
        ppElement->InsertEndChild(annElement);
    }

    // no_print_zones
    //
    if (glob_ann.printer.no_print_zones.size() > 0)
    {
        for (uint i = 0; i < glob_ann.printer.no_print_zones.size(); i++)
        {
            Bbox npz = glob_ann.printer.no_print_zones.at(i);

            XMLElement *annElement = doc->NewElement(NO_PRINT_ZONE.c_str());

            std::string dimensions = to_string(npz.min[0]) + " " + to_string(npz.min[1]) + " " + to_string(npz.min[2]) + " " +
                                     to_string(npz.max[0]) + " " + to_string(npz.max[1]) + " " + to_string(npz.max[2]);


            annElement->SetText(dimensions.c_str());
            ppElement->InsertEndChild(annElement);
        }
    }

    // Other global Annotations
    //

    // Orientation
    //
    if (glob_ann.orientation[0] != 1.0 || glob_ann.orientation[1] != 0.0 || glob_ann.orientation[2] != 0.0 ||
        glob_ann.orientation[3] != 0.0 || glob_ann.orientation[4] != 1.0 || glob_ann.orientation[5] != 0.0 ||
        glob_ann.orientation[6] != 0.0 || glob_ann.orientation[7] != 0.0 || glob_ann.orientation[8] != 1.0)
    {
        XMLElement * annElement = addAnnotationWithID(doc, ORIENTATION);

        std::vector<double> matrix(glob_ann.orientation, glob_ann.orientation + sizeof glob_ann.orientation / sizeof glob_ann.orientation[0]);
        annElement->InsertEndChild(createListOfAnnotatedElements<double>(doc, MATRIX, matrix));
    }

    // Size
    //
    if (glob_ann.too_big)
        addAnnotationWithID(doc, TOO_BIG);

    // Part Weight
    //
    if (glob_ann.too_heavy)
        addAnnotationWithID(doc, TOO_HEAVY);

    // Local Vertex Annotations
    //
    std::multimap<std::string, u_int> extra_vertex_ann;
    std::set<std::string> extra_vertex_ann_key;

    for (u_int i=0; i < vertex_ann.size(); i++)
    {
        if (vertex_ann.at(i).extra_annotations.size() > 0)
        {
            for (std::set<std::string>::iterator it = vertex_ann.at(i).extra_annotations.begin();
                 it != vertex_ann.at(i).extra_annotations.end(); it++)
            {
                std::string ann = *it;

                extra_vertex_ann_key.insert(ann);
                extra_vertex_ann.insert(std::pair<std::string, u_int> (ann, i));
            }
        }
    }

    // Extra Vertex Annotations
    //
    for (std::set<std::string>::iterator set_it = extra_vertex_ann_key.begin(); set_it != extra_vertex_ann_key.end(); set_it++)
    {
        std::string ann = *set_it;

        XMLElement *annElement = addAnnotationWithID(doc, ann);

        std::pair <std::multimap<std::string, u_int>::iterator, std::multimap<std::string, u_int>::iterator> range;
        range = extra_vertex_ann.equal_range(ann);

        std::vector<u_int> elements;

        for (std::multimap<std::string,u_int>::iterator map_it=range.first; map_it != range.second; ++map_it)
            elements.push_back(map_it->second);

        annElement->InsertEndChild(createListOfAnnotatedElements<u_int>(doc, VERTICES, elements));
    }

    extra_vertex_ann.clear();
    extra_vertex_ann_key.clear();

    // Local Triangle Annotations
    //
    std::vector<u_int> bad_geometry;
    std::vector<u_int> bad_material;
    std::vector<u_int> thin_walls;
    std::vector<u_int> thin_channels;
    std::vector<u_int> overhangs;
    std::vector<u_int> weak_features;
    std::vector<u_int> mach_allowance;

    std::vector<std::vector<u_int> > closed_voids;

    std::multimap<std::string, u_int> extra_triangle_ann;
    std::set<std::string> extra_triangle_ann_key;

    for (u_int i=0; i < triangle_ann.size(); i++)
    {
        if (triangle_ann.at(i).bad_geometry)
            bad_geometry.push_back(i);

        if (triangle_ann.at(i).bad_material)
            bad_material.push_back(i);

        if (triangle_ann.at(i).thin_walls)
            thin_walls.push_back(i);

        if (triangle_ann.at(i).thin_channels)
            thin_channels.push_back(i);

        if (triangle_ann.at(i).overhangs)
            overhangs.push_back(i);

        if (triangle_ann.at(i).weak_feature)
            weak_features.push_back(i);

        if (triangle_ann.at(i).mach_allowance)
            mach_allowance.push_back(i);

        if (triangle_ann.at(i).closed_voids > 0)
        {
            u_int void_index = triangle_ann.at(i).closed_voids;

            if (void_index > closed_voids.size())
                closed_voids.resize(void_index);

            closed_voids.at(void_index-1).push_back(i);
        }

        if (triangle_ann.at(i).extra_annotations.size() > 0)
        {
            for (std::set<std::string>::iterator it = triangle_ann.at(i).extra_annotations.begin();
                 it != triangle_ann.at(i).extra_annotations.end(); it++)
            {
                std::string ann = *it;

                extra_triangle_ann_key.insert(ann);
                extra_triangle_ann.insert(std::pair<std::string, u_int> (ann, i));
            }
        }
    }

    // Bad Geometry
    //
    if (bad_geometry.size() > 0)
    {
        XMLElement *annElement = addAnnotationWithID(doc, BAD_GEOMETRY);
        annElement->InsertEndChild(createListOfAnnotatedElements<u_int>(doc, TRIANGLES, bad_geometry));

        bad_geometry.clear();
    }

    // Closed Voids
    //
    for (u_int i=0; i < closed_voids.size(); i++)
    {
        XMLElement *annElement = addAnnotationWithID(doc, CLOSED_VOIDS);
        annElement->InsertEndChild(createListOfAnnotatedElements<u_int>(doc, TRIANGLES, closed_voids.at(i)));

        closed_voids.at(i).clear();
    }

    // Bad Material
    //
    if (bad_material.size() > 0)
    {
        XMLElement *annElement = addAnnotationWithID(doc, BAD_MATERIAL);
        annElement->InsertEndChild(createListOfAnnotatedElements<u_int>(doc, TRIANGLES, bad_material));

        bad_material.clear();
    }

    // Thin Walls
    //
    if (thin_walls.size() > 0)
    {
        XMLElement *annElement = addAnnotationWithID(doc, THIN_WALLS_REL);
        annElement->InsertEndChild(createListOfAnnotatedElements<u_int>(doc, TRIANGLES, thin_walls));

        thin_walls.clear();
    }

    // Thin Channels
    //
    if (thin_channels.size() > 0)
    {
        XMLElement *annElement = addAnnotationWithID(doc, THIN_CHANNELS);
        annElement->InsertEndChild(createListOfAnnotatedElements<u_int>(doc, TRIANGLES, thin_channels));

        thin_channels.clear();
    }

    // Overhangs
    //
    if (overhangs.size() > 0)
    {
        XMLElement *annElement = addAnnotationWithID(doc, OVERHANGS);
        annElement->InsertEndChild(createListOfAnnotatedElements<u_int>(doc, TRIANGLES, overhangs));

        overhangs.clear();
    }

    // Weak Features
    //
    if (weak_features.size() > 0)
    {
        XMLElement *annElement = addAnnotationWithID(doc, WEAK_FEATURES);
        annElement->InsertEndChild(createListOfAnnotatedElements<u_int>(doc, TRIANGLES, weak_features));

        weak_features.clear();
    }

    // Machine Allowance
    //
    if (mach_allowance.size() > 0)
    {
        XMLElement *annElement = addAnnotationWithID(doc, MACH_ALLOWANCE);
        annElement->InsertEndChild(createListOfAnnotatedElements<u_int>(doc, TRIANGLES, mach_allowance));

        mach_allowance.clear();
    }

    // Extra Triangle Annotations
    //
    for (std::set<std::string>::iterator set_it = extra_triangle_ann_key.begin(); set_it != extra_triangle_ann_key.end(); set_it++)
    {
        std::string ann = *set_it;

        XMLElement *annElement = addAnnotationWithID(doc, ann);

        std::pair <std::multimap<std::string, u_int>::iterator, std::multimap<std::string, u_int>::iterator> range;
        range = extra_triangle_ann.equal_range(ann);

        std::vector<u_int> elements;

        for (std::multimap<std::string,u_int>::iterator map_it=range.first; map_it != range.second; ++map_it)
            elements.push_back(map_it->second);

        annElement->InsertEndChild(createListOfAnnotatedElements<u_int>(doc, TRIANGLES, elements));
    }

    extra_triangle_ann.clear();
    extra_triangle_ann_key.clear();

    // Save XML on file
    //
    doc->SaveFile(filename);
}

}
