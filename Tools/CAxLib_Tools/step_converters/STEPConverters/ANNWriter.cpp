#pragma once
#include <string>
#include "stdafx.h"

#include "AnnotationMacros.h"
#include "ANNWriter.h"

#include "tinyxml2.h" // external library TINYXML2

ANNWriter::ANNWriter(){}
ANNWriter::~ANNWriter(){}

template<class type>
tinyxml2::XMLElement * createListOfAnnotatedElements(tinyxml2::XMLDocument *doc, const std::string elemType, std::vector<type> & list)
{
	tinyxml2::XMLElement * listElement = doc->NewElement(elemType.c_str());

	std::string slist;
	for (int i = 0; i < list.size(); i++)
	{
		std::stringstream ss;
		ss << list.at(i) << " ";
		slist += ss.str();
	}

	listElement->SetText(slist.c_str());

	return listElement;
}

tinyxml2::XMLElement * addAnnotationWithID(tinyxml2::XMLDocument *doc, const std::string ID)
{
	tinyxml2::XMLElement * annElement = doc->NewElement(ANNOTATION_NAME.c_str());
	doc->FirstChildElement()->InsertEndChild(annElement);

	tinyxml2::XMLElement * idElement = doc->NewElement(IDENTIFIER_NAME.c_str());
	idElement->SetText(ID.c_str());

	annElement->InsertEndChild(idElement);

	return annElement;
}

void ANNWriter::closeFile()
{
	_file.close();
}

void ANNWriter::writeAnnFile(char* filePath)
{
	// one more model object to work with renewed model.
	Model model2(_rep, _ma, &ap209_multidisciplinary_analysis_and_design_mim_lf_SchemaObject);
	// create model... again. actually there is no really need for that.
	//edmiCreateModelBN(rep.getRepositoryId(), (char *) db_modelName.c_str(), 
	//	"ap209_multidisciplinary_analysis_and_design_mim_lf", USE_DEFAULT_OPTIONS);
	model2.open((char *)_modelName.c_str(), sdaiRO); // reao only access

	// reserve space in memory for all neccesary objects of specified type
	//model2.defineObjectSet(MIM::et_versioned_action_request, 4, false);

	// only specified objects (and all related objects) are read into memory
	//model2.readAllObjectsToMemory();

	TIXMLASSERT(true);

	tinyxml2::XMLDocument * doc = new tinyxml2::XMLDocument();

	tinyxml2::XMLElement * root = doc->NewElement(ROOT_NAME.c_str());	
	doc->InsertFirstChild(root);

	if (offWriter->support_structure.size() > 0)
	{
		tinyxml2::XMLElement *annotation = addAnnotationWithID(doc, SUPPORT_STRUCTURE);

		tinyxml2::XMLElement *triangles = createListOfAnnotatedElements(doc, TRIANGLES, offWriter->support_structure);

		annotation->InsertEndChild(triangles);
	}

	// iterate objects of specified type.. we have only one though...
	Iterator<MIM::versioned_action_request*, MIM::entityType> iter_annotations(model2.getObjectSet(MIM::et_versioned_action_request), _m);
	for (auto* nann = iter_annotations.first(); nann; nann = iter_annotations.next())
	{
		versioned_action_request *var = reinterpret_cast<versioned_action_request *> (nann);
		
		const char *id = var->get_id();
		const char *description = var->get_description();

		std::cout << id << std::endl;

		tinyxml2::XMLDocument tmp_doc;
		tinyxml2::XMLError error = tmp_doc.Parse(description);
		
		for (tinyxml2::XMLElement* node = tmp_doc.FirstChildElement(); node; node = node->NextSiblingElement()) 
		{
			tinyxml2::XMLElement* copy = node->ShallowClone(doc)->ToElement();

			tinyxml2::XMLElement* id_child = node->FirstChildElement(IDENTIFIER_NAME.c_str());

			if (id_child != NULL)
			{
				tinyxml2::XMLElement* copy_child = id_child->ShallowClone(doc)->ToElement();
				copy_child->SetText(id_child->GetText());
				copy->InsertEndChild(copy_child);
			}

			tinyxml2::XMLElement* triangle_child = node->FirstChildElement(TRIANGLES.c_str());

			if (triangle_child != NULL)
			{
				tinyxml2::XMLElement* copy_child = triangle_child->ShallowClone(doc)->ToElement();
				copy_child->SetText(triangle_child->GetText());
				copy->InsertEndChild(copy_child);
			}

			tinyxml2::XMLElement* vertices_child = node->FirstChildElement(VERTICES.c_str());

			if (vertices_child != NULL)
			{
				tinyxml2::XMLElement* copy_child = vertices_child->ShallowClone(doc)->ToElement();
				copy_child->SetText(vertices_child->GetText());
				copy->InsertEndChild(copy_child);
			}

			doc->FirstChildElement()->InsertEndChild(copy);
		}
	}

	doc->SaveFile(filePath);
}