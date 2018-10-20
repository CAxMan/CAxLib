#include "AnnotationMacros.h"

/*
 * This function allow coverting a XLMElement (and all its children) into a XML string.
 * 
 */
const char * fromAnnotation2String(tinyxml2::XMLElement *element, std::string &str)
{
	if (element == NULL) {
		return "";
	}

	str.append("<");
	str.append(element->Value());
	str.append(">");

	if (element->GetText() != NULL) {
		str.append(element->GetText());
	}

	tinyxml2::XMLElement *childElement = element->FirstChildElement();
	while (childElement != NULL) {
		fromAnnotation2String(childElement, str);
		childElement = childElement->NextSiblingElement();
	}

	str.append("</");
	str.append(element->Value());
	str.append(">");

	return str.c_str();
}

bool isGlobalAnnotation(const std::string id)
{
	if (std::find(globalAnnotations.begin(), globalAnnotations.end(), id) != globalAnnotations.end())
		return true;

	return false;
}

bool isLocalTriangleAnnotation(const std::string id)
{
	if (std::find(localTriangleAnnotations.begin(), localTriangleAnnotations.end(), id) != localTriangleAnnotations.end())
		return true;

	return false;
}

bool isPrintingParameterAnnotation(const std::string id)
{
	if (std::find(printingParameterAnnotations.begin(), printingParameterAnnotations.end(), id) != printingParameterAnnotations.end())
		return true;

	return false;
}

bool isSupportStructureAnnotation(const std::string id)
{
	if (id.compare(SUPPORT_STRUCTURE) == 0)
		return true;

	return false;
}
