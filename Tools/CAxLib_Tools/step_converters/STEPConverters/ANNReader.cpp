#pragma once
#include "stdafx.h"
#include "ANNReader.h"
#include <assert.h>

#include "AnnotationMacros.h"


// Support functions 
//
std::vector<std::string> getAnnotatedLocalGeometry(tinyxml2::XMLElement * element, const std::string elementType)
{
	tinyxml2::XMLElement *elements = element->FirstChildElement(elementType.c_str());

	std::istringstream iss(elements->GetText());
	std::string token;
	std::vector<std::string> lelements;

	while (iss >> token)
		lelements.push_back(token);

	return lelements;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

ANNReader::ANNReader(){}
ANNReader::~ANNReader(){}

void ANNReader::closeFile(){
	_file.close();
}

void ANNReader::readAnnFile(char* filePath){
	
	std::cout << "Opening " << filePath << std::endl;
	
	_filePath = filePath;
	_file.open(_filePath);

	TIXMLASSERT(true);

	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	doc->LoadFile(filePath);

	if (doc->ErrorID() != 0)
	{
		std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : couldn't load input file " << filePath << std::endl;
		exit(-1);
	}

	tinyxml2::XMLElement* root = doc->FirstChildElement();

	if (root == NULL || root->Name() != ROOT_NAME)
	{
		std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : couldn't read root element " << filePath << std::endl;
		exit(-1);
	}

	tinyxml2::XMLElement* annotation = root->FirstChildElement(ANNOTATION_NAME.c_str());

	int closed_void_index = 0;


	while (annotation != NULL) {

		if (ANNOTATION_NAME.compare(annotation->Name()) != 0)
		{
			std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : couldn't read annotation element " << annotation->Name() << std::endl;
			exit(-1);
		}

		tinyxml2::XMLElement *identifier = annotation->FirstChildElement(IDENTIFIER_NAME.c_str());

		if (identifier == NULL)
		{
			std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : couldn't read annotation identifier " << annotation->Name() << std::endl;
			exit(-1);
		}

		const char *id_value = identifier->GetText();

		//if (PRINTING_PARAMETERS.compare(id_value) == 0)
		//{
		//	glob_ann.printing_parameters = get_printing_parameters(annotation);
		//}
		//else
		if (ORIENTATION_MATRIX.compare(id_value) == 0)
		{
			std::vector<std::string> lmatrix = getAnnotatedLocalGeometry(annotation, MATRIX);

			if (lmatrix.size() != 9)
			{
				std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : couldn't read orientation matrix in" << annotation->Name() << std::endl;
				exit(-1);
			}

			for (int i = 0; i < lmatrix.size(); i++)
				orientation_matrix[i] = atof(lmatrix.at(i).c_str());
		}
		else
		if (ABSOLUTE_SIZE.compare(id_value) == 0)     // Absolute size
		{
			absolute_size = true;
		}
		else
		if (RELATIVE_SIZE.compare(id_value) == 0)     // Relative size
		{
			relative_size = true;
		}
		else
		if (PART_WEIGHT.compare(id_value) == 0)       // Part weight
		{
			part_weight = true;
		}
		else
		if (ORIENTATION_FAILURE.compare(id_value) == 0)  // No orientation
		{
			orientation_failure = true;
		}
		else
		if (MATERIAL_USAGE.compare(id_value) == 0)  // Material usage
		{
			tinyxml2::XMLElement *material = annotation->FirstChildElement("MaterialValue");

			if (material == NULL)
			{
				std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : couldn't read annotation value " << annotation->Name() << std::endl;
				exit(-1);
			}
			material_usage = atof(material->GetText());
		}
		else
		if (BUILD_TIME.compare(id_value) == 0)      // Build Time
		{
			tinyxml2::XMLElement *time = annotation->FirstChildElement("Time");

			if (time == NULL)
			{
				std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ANN() : couldn't read annotation value " << annotation->Name() << std::endl;
				exit(-1);
			}
			
			build_time = atof(time->GetText());
		}
		else
		if (BAD_GEOMETRY.compare(id_value) == 0)
		{
			std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

			for (int i = 0; i < ltriangles.size(); i++)
			{
				int index = atoi(ltriangles.at(i).c_str());
				bad_geometry.push_back(index);
			}
		}
		else
		if (CLOSED_VOIDS.compare(id_value) == 0)
		{
			std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

			if (ltriangles.size() > 0)
			{
				closed_voids.push_back(std::vector<int>());

				for (int i = 0; i < ltriangles.size(); i++)
				{
					int index = atoi(ltriangles.at(i).c_str());
					closed_voids.at(closed_void_index).push_back(index);
				}

				closed_void_index++;
			}
		}
		else
		if (BAD_MATERIAL.compare(id_value) == 0)
		{
			std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

			for (int i = 0; i < ltriangles.size(); i++)
			{
				int index = atoi(ltriangles.at(i).c_str());

				bad_material.push_back(index);
			}
		}
		else
		if (ABSOLUTE_THIN_WALLS.compare(id_value) == 0)
		{
			std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

			for (int i = 0; i < ltriangles.size(); i++)
			{
				int index = atoi(ltriangles.at(i).c_str());
				absolute_thin_walls.push_back(index);
				
			}
		}
		else
		if (RELATIVE_THIN_WALLS.compare(id_value) == 0)
		{
			std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

			for (int i = 0; i < ltriangles.size(); i++)
			{
				int index = atoi(ltriangles.at(i).c_str());
				relative_thin_walls.push_back(index);
			}
		}
		else
		if (CHANNEL_DIMENSION.compare(id_value) == 0)
		{
			std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

			for (int i = 0; i < ltriangles.size(); i++)
			{
				int index = atoi(ltriangles.at(i).c_str());
				channel_dimension.push_back(index);
			}
		}
		else
		if (OVERHANGS.compare(id_value) == 0)
		{
			std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

			for (int i = 0; i < ltriangles.size(); i++)
			{
				int index = atoi(ltriangles.at(i).c_str());
				overhangs.push_back(index);
			}
		}
		else
		if (WEAK_FEATURES.compare(id_value) == 0)
		{
			std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

			for (int i = 0; i < ltriangles.size(); i++)
			{
				int index = atoi(ltriangles.at(i).c_str());
				weak_features.push_back(index);
			}
		}
		else
		if (SUPPORT_STRUCTURE.compare(id_value) == 0)
		{
			std::vector<std::string> ltriangles = getAnnotatedLocalGeometry(annotation, TRIANGLES);

			for (int i = 0; i < ltriangles.size(); i++)
			{
				int index = atoi(ltriangles.at(i).c_str());
				support_structures.push_back(index);
			}
		}

		annotation = annotation->NextSiblingElement();
	}
}