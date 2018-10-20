#pragma once

#include <string>

#include "tinyxml2.h"

#include <vector>

// List of element IDs used in the .ANN codification
//
const std::string ROOT_NAME = "Annotations";
const std::string ANNOTATION_NAME = "Annotation";
const std::string IDENTIFIER_NAME = "Identifier";

const std::string ORIENTATION_MATRIX = "Orientation_matrix";
const std::string ABSOLUTE_SIZE = "Absolute_Size";
const std::string RELATIVE_SIZE = "Relative_Size";
const std::string PART_WEIGHT = "Part_Weight";
const std::string ORIENTATION_FAILURE = "Orientation_Failure";
const std::string MATERIAL_USAGE = "Material_Usage";
const std::string BUILD_TIME = "Build_Time";

const std::string BAD_GEOMETRY = "Bad_Geometry";       // Bad geometry
const std::string CLOSED_VOIDS = "Closed_Voids";       // Closed voidss
const std::string BAD_MATERIAL = "Bad_Material";       // Material
const std::string ABSOLUTE_THIN_WALLS = "Absolute_Thin_Walls";     // Absolute thin walls
const std::string RELATIVE_THIN_WALLS = "Relative_Thin_Walls";     // Relative thin walls
const std::string CHANNEL_DIMENSION = "Channel_Dimension";       // Channel dimensions
const std::string OVERHANGS = "Overhangs";          // Overhangs
const std::string WEAK_FEATURES = "Weak_Features";      // Weak feature
const std::string MACH_ALLOWANCE = "Mach_Allowance";     // Machining allowance

const std::string SUPPORT_STRUCTURE = "Support_Structure";

const std::string TRIANGLES = "Triangles";          // Machining allowance
const std::string VERTICES = "Vertices";           // Machining allowance
const std::string MATRIX = "Matrix";

const std::vector<std::string> globalAnnotations =
{
	ORIENTATION_MATRIX,
	ABSOLUTE_SIZE,
	RELATIVE_SIZE,
	ORIENTATION_FAILURE,
};

const std::vector<std::string> localTriangleAnnotations = 
{
	BAD_GEOMETRY,
	BAD_MATERIAL,
	CLOSED_VOIDS,
	OVERHANGS,
	CHANNEL_DIMENSION,
	ABSOLUTE_THIN_WALLS,
	RELATIVE_THIN_WALLS,
	WEAK_FEATURES,	
};

const std::vector<std::string> localVertexAnnotations = {};

const std::vector<std::string> printingParameterAnnotations =
{

};


const char * fromAnnotation2String	(tinyxml2::XMLElement *element, std::string &str);

bool isGlobalAnnotation				(const std::string id);
bool isLocalVertexAnnotation		(const std::string id);
bool isLocalTriangleAnnotation		(const std::string id);
bool isPrintingParameterAnnotation	(const std::string id);
bool isSupportStructureAnnotation	(const std::string id);
