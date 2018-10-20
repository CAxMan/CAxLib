#pragma once
#include <string>
#include "stdafx.h"
#include "OFFWriter.h"

OFFWriter::OFFWriter(){}
OFFWriter::~OFFWriter(){}

void OFFWriter::closeFile()
{
	_file.close();
}

void OFFWriter::writeOffFile(char* filePath)
{
	// one more model object to work with renewed model.
	Model model2(_rep, _ma, &ap209_multidisciplinary_analysis_and_design_mim_lf_SchemaObject);
	// create model... again. actually there is no really need for that.
	//edmiCreateModelBN(rep.getRepositoryId(), (char *) db_modelName.c_str(), 
	//	"ap209_multidisciplinary_analysis_and_design_mim_lf", USE_DEFAULT_OPTIONS);
	model2.open((char *) _modelName.c_str(), sdaiRO); // reao only access

	// reserve space in memory for all neccesary objects of specified type
	//model2.defineObjectSet(MIM::et_coordinates_list, 4, false);
	//model2.defineObjectSet(MIM::et_triangulated_surface_set, 4, false);

	// only specified objects (and all related objects) are read into memory
	//model2.readAllObjectsToMemory();

	std::cout << "Opening " << filePath << std::endl;

	_filePath = filePath;
	_file.open(_filePath, std::ios::out);

	size_t counter = 0;

	if (!_file.good())
	{
		std::cerr << "Error: " << strerror(errno);
		return;
	}

	_file << "OFF" << std::endl;

	int nv = 0, nt = 0;

	// iterate objects of specified type.. we have only one though...
	Iterator<MIM::coordinates_list*, MIM::entityType> iter_coords(model2.getObjectSet(MIM::et_coordinates_list), _m);
	for (auto* npoint = iter_coords.first(); npoint; npoint = iter_coords.next())
	{
		coordinates_list *cpoint = reinterpret_cast<coordinates_list*>(npoint);

		nv += cpoint->get_position_coords()->size();
	}

	// iterate objects of specified type.. we have only one though...
	Iterator<MIM::triangulated_surface_set*, MIM::entityType> iter_triangle(model2.getObjectSet(MIM::et_triangulated_surface_set), _m);
	for (auto* ntriangle = iter_triangle.first(); ntriangle; ntriangle = iter_triangle.next())
	{
		triangulated_surface_set *ctriangle = reinterpret_cast<triangulated_surface_set*>(ntriangle);

		nt += ctriangle->get_triangles()->size();
	}

	_file << nv << " " << nt << " 0" << std::endl;

	std::cout << "Pocessing Vertices ... " << std::endl;

	int global_v_index = 0;

	std::map<std::string, std::vector<int> > vertex_map;
	
	for (auto* npoint = iter_coords.first(); npoint; npoint = iter_coords.next())
	{
		coordinates_list *cpoint = reinterpret_cast<coordinates_list*>(npoint);

		std::string coord_name = std::string(cpoint->get_name());

		vertex_map[coord_name] = std::vector<int>(cpoint->get_npoints());

		List<List<REAL>*>* coords = cpoint->get_position_coords();

		for (int pid = 0; pid < cpoint->get_npoints(); pid++)
		{
			double x = coords->getElement(pid)->getRealElement(0);
			double y = coords->getElement(pid)->getRealElement(1);
			double z = coords->getElement(pid)->getRealElement(2);

			_file << x << " " << y << " " << z << std::endl;

			vertex_map[coord_name].at(pid) = global_v_index;
			global_v_index++;
		}
	}

	std::cout << "Pocessing Triangles ... " << std::endl;

	int global_t_index = 0;

	// iterate objects of specified type.. we have only one though...
	for (auto* ntriangle = iter_triangle.first(); ntriangle; ntriangle = iter_triangle.next())
	{
		triangulated_surface_set *ctriangle = reinterpret_cast<triangulated_surface_set*>(ntriangle);
		std::string coord_name = std::string(ctriangle->get_coordinates()->get_name());

		std::string surface_name = std::string(ctriangle->get_name());

		if (vertex_map.find(coord_name) == vertex_map.end())
		{
			std::cerr << "[ERROR] tringulated_surface_set " << surface_name << " refers to wrong coordinate list" << std::endl;
			exit(1);
		}

		int v_size = vertex_map.find(coord_name)->second.size();

		std::cout << "Surface Name : " << surface_name << std::endl;
		std::cout << "Coordinate list size : " << v_size << std::endl;
		std::cout << "Triangle list size : " << ctriangle->get_triangles()->size() << std::endl;

		List<List<INTEGER>*>* triangles = ctriangle->get_triangles();

		for (int tid = 0; tid < triangles->size(); tid++)
		{
			int v0 = triangles->getElement(tid)->getElement(0)-1;
			int v1 = triangles->getElement(tid)->getElement(1)-1;
			int v2 = triangles->getElement(tid)->getElement(2)-1;

			int global_v0 = vertex_map[coord_name].at(v0);
			int global_v1 = vertex_map[coord_name].at(v1);
			int global_v2 = vertex_map[coord_name].at(v2);

			_file << "3 " << global_v0 << " " << global_v1 << " " << global_v2 << std::endl;

			if (surface_name.compare("Support_Structure") == 0)
			{
				support_structure.push_back(global_t_index);
			}

			global_t_index++;
		}
	}

	std::cout << "GEOMETRY COMPLETED." << std::endl;

}