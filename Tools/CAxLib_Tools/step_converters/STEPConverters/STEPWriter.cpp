#include "STEPWriter.h"

STEPWriter::STEPWriter() {}
STEPWriter::~STEPWriter(){}

// Utility functions

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

// Class methods

void STEPWriter::writeStepFile()
{
	convertGeometry();
	convertAnnotations();
}

void STEPWriter::convertAnnotations()
{
	// Annotations

	if (annReader->absolute_size)
	{
		// to be done
	}

	if (annReader->part_weight)
	{
		// to be done
	}

	if (annReader->orientation_failure)
	{
		// to be done
	}

	generateVersionedActioRequest(BAD_GEOMETRY, TRIANGLES, annReader->bad_geometry);
	generateVersionedActioRequest(BAD_MATERIAL, TRIANGLES, annReader->bad_material);
	generateVersionedActioRequest(OVERHANGS, TRIANGLES, annReader->overhangs);
	generateVersionedActioRequest(CHANNEL_DIMENSION, TRIANGLES, annReader->channel_dimension);
	generateVersionedActioRequest(ABSOLUTE_THIN_WALLS, TRIANGLES, annReader->absolute_thin_walls);
	generateVersionedActioRequest(RELATIVE_THIN_WALLS, TRIANGLES, annReader->relative_thin_walls);
	generateVersionedActioRequest(WEAK_FEATURES, TRIANGLES, annReader->weak_features);

	for (int i = 0; i < annReader->closed_voids.size(); i++)
		generateVersionedActioRequest(CLOSED_VOIDS, TRIANGLES, annReader->closed_voids.at(i));
}

void STEPWriter::convertGeometry()
{
	int min_vertex_index = offReader->coords.size() / 3;
	int num_model_triangles = annReader->support_structures.at(0) -1;

	// get support structure vertices 
	if (annReader->support_structures.size() > 0)
	{
		for (int i = annReader->support_structures.at(0) * 3; i < offReader->triangles.size(); i++)
		{
			if (offReader->triangles.at(i) < min_vertex_index)
				min_vertex_index = offReader->triangles.at(i);
		}
	}
	
	coordinates_list *cl = create(coordinates_list);
	label* clName = new(_m)label("Coordinates List");
	cl->put_name(_ma->allocString(*clName));

	model = create(triangulated_surface_set);
	label* tssName = new(_m)label("Triangulated Surface Set");
	model->put_name(_ma->allocString(*tssName));
	model->put_coordinates(cl);

	tessellated_shape_representation *rep = create(tessellated_shape_representation);
	//rep->put_id("TSR_id"); - 
	rep->put_name("TSR_Name");
	//rep->put_description("Tessellated_Model");

	geometric_representation_context_global_unit_assigned_context *guac = create(geometric_representation_context_global_unit_assigned_context);

	guac->put_context_identifier("GUAC_id");
	guac->put_context_type("geometric_representation_context");
	guac->put_coordinate_space_dimension(3);

	si_unit *su = create(si_unit);
	su->put_name(si_unit_name_JOULE);

	guac->put_units_element(su);
	rep->put_context_of_items(guac);

	List<List<double> * > coord_list(_m, sdaiAGGR, 1);

	List<int> pnindex(_m, sdaiINTEGER, 1);

	std::cout << " First support structure vertex :" << min_vertex_index << std::endl;

	cl->put_position_coords(&coord_list); // Aggregates must belong to an object before one can add elements
	model->put_pnindex(&pnindex);

	// Process vertices 
	for (int i = 0; i < min_vertex_index-1; i++)
	{
		pnindex.add(i +1, _ma);

		//coords -> list -> cartesian_point
		List<double> coordinates(_m, sdaiREAL, 3);
		coord_list.add(&coordinates, _ma);

		coordinates.add(offReader->coords.at(i * 3 + 0), _ma);
		coordinates.add(offReader->coords.at(i * 3 + 1), _ma);
		coordinates.add(offReader->coords.at(i * 3 + 2), _ma);

		//cl->put_position_coords_element(&coordinates);
	}


	cl->put_npoints(cl->get_position_coords()->size());
	model->put_pnmax(model->get_pnindex()->size());

	std::cout << " Total triangles " << offReader->triangles.size() / 3 << std::endl;
	std::cout << " First support structure triangle :" << annReader->support_structures.at(0) << std::endl;
	std::cout << " Num Model triangles : " << num_model_triangles << std::endl;

	// Process triangles
	for (int i = 0; i < num_model_triangles; i++)
	{
		List<int> triangle(_m, sdaiINTEGER, 3);
		model->put_triangles_element(&triangle);

		triangle.add(offReader->triangles.at(i * 3 + 0) + 1, _ma);
		triangle.add(offReader->triangles.at(i * 3 + 1) + 1, _ma);
		triangle.add(offReader->triangles.at(i * 3 + 2) + 1, _ma);
	}

	List<double> v_normals(_m, sdaiREAL, 3);
	model->put_normals_element(&v_normals);

	v_normals.add(0.0, _ma);
	v_normals.add(0.0, _ma);
	v_normals.add(0.0, _ma);

	std::cout << "pnindex :" << model->get_pnindex()->size() << std::endl;
	std::cout << "pnmax :" << model->get_pnmax() << std::endl;

	generateSupportStructures();

	Set<MIM::tessellated_item * > tessellated_items(_m, sdaiINSTANCE, 1);
	rep->put_items(&tessellated_items);

	tessellated_items.add(model, _ma);


	if (support_structure != NULL)
		tessellated_items.add(support_structure, _ma);

	sdr = create(shape_definition_representation);
	//sdr->put_description("Shape definition representation");
	sdr->put_used_representation(rep);
}

void STEPWriter::generatePrintingParameters()
{

}

void STEPWriter::generateSupportStructures()
{
	if (annReader->support_structures.size() == 0)
		return;

	std::cout << "Support_structure " << std::endl;

	int min_vertex_index = offReader->coords.size() / 3;
	int num_model_triangles = annReader->support_structures.at(0) - 1;

	for (int i = num_model_triangles * 3; i < offReader->triangles.size(); i++)
	{
		if (offReader->triangles.at(i) < min_vertex_index)
			min_vertex_index = offReader->triangles.at(i);
	}

	std::cout << " -- a new coordinates_list .... " << std::endl;

	coordinates_list *cl_ss = create(coordinates_list);
	label* clName = new(_m)label("Coordinates List Support Structure");
	cl_ss->put_name(_ma->allocString(*clName));

	std::cout << " -- a new triangulated_surface set .... " << std::endl;

	support_structure = create(triangulated_surface_set);
	label* tssName = new(_m)label("Support_Structure");
	support_structure->put_name(_ma->allocString(*tssName));
	support_structure->put_coordinates(cl_ss);

	std::cout << " -- filling the new coordinates_list .... " << std::endl;

	List<int> pnindex(_m, sdaiINTEGER, 1);

	support_structure->put_pnindex(&pnindex);
	// Process vertices 
	for (int i = min_vertex_index; i < offReader->coords.size() / 3; i++)
	{
		pnindex.add(i - min_vertex_index +1, _ma);

		//coords -> list -> cartesian_point
		List<double> coordinates(_m, sdaiREAL, 3);
		cl_ss->put_position_coords_element(&coordinates);

		coordinates.add(offReader->coords.at(i * 3 + 0), _ma);
		coordinates.add(offReader->coords.at(i * 3 + 1), _ma);
		coordinates.add(offReader->coords.at(i * 3 + 2), _ma);

		//cl->put_position_coords_element(&coordinates);

      support_structure->put_pnindex_element(i - min_vertex_index);

	}

	cl_ss->put_npoints(cl_ss->get_position_coords()->size());

	support_structure->put_pnmax(support_structure->get_pnindex()->size());


	std::cout << " -- filling the new triangulated_surface set .... " << std::endl;

	// Process triangles
	for (int i = num_model_triangles; i < offReader->triangles.size() / 3; i++)
	{
		List<int> triangle(_m, sdaiINTEGER, 3);
		support_structure->put_triangles_element(&triangle);

		triangle.add(offReader->triangles.at(i * 3 + 0) - min_vertex_index +1, _ma);
		triangle.add(offReader->triangles.at(i * 3 + 1) - min_vertex_index +1, _ma);
		triangle.add(offReader->triangles.at(i * 3 + 2) - min_vertex_index +1, _ma);
	}

	List<double> v_normals(_m, sdaiREAL, 3);
	support_structure->put_normals_element(&v_normals);

	v_normals.add(0.0, _ma);
	v_normals.add(0.0, _ma);
	v_normals.add(0.0, _ma);

	std::cout << "DONE" << std::endl;
}

void STEPWriter::generateVersionedActioRequest(const std::string id, const std::string elemType, std::vector<int> &elements)
{
	if (elements.size() == 0)
		return;

	tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument();
	tinyxml2::XMLElement *request = doc->NewElement(ANNOTATION_NAME.c_str());

	tinyxml2::XMLElement *identifier = doc->NewElement(IDENTIFIER_NAME.c_str());
	identifier->SetText(id.c_str());

	tinyxml2::XMLElement *annotated_elements = createListOfAnnotatedElements(doc, elemType, elements);

	request->InsertFirstChild(identifier);
	request->InsertEndChild(annotated_elements);

	std::string description;

	fromAnnotation2String(request, description);

	versioned_action_request *var = create(versioned_action_request);
	var->put_id(_ma->allocString(const_cast<char *> (id.c_str())));
	var->put_description(_ma->allocString(const_cast<char *> (description.c_str())));
	var->put_purpose(_ma->allocString(const_cast<char *> (id.c_str())));
}
