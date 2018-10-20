#pragma once
#include <string>
#include "stdafx.h"
#include "OFFReader.h"

OFFReader::OFFReader(){}
OFFReader::~OFFReader(){}

void OFFReader::closeFile(){
	_file.close();
}

void OFFReader::readOffFile(char* filePath){
	
	std::cout << "Opening " << filePath << std::endl;
	
	_filePath = filePath;
	_file.open(_filePath, std::ios::in | std::ios::out);

	size_t counter = 0;
	
	if (!_file.good())
	{
		std::cerr << "Error: " << strerror(errno);
		return;
	}
	
	// Get OFF header ("OFF")
	std::string offstring;

	_file >> offstring;

	// Get number of vertices and triangles
	int nv = 0, nt = 0, ne = 0;
	double x, y, z;
	int v0, v1, v2, num_v;

	_file >> nv >> nt >> ne;

	std::cout << nv << " Vertices" << std::endl;
	std::cout << nt << " Triangles" << std::endl;

	std::cout << std::endl << "Processing Geometry ...." << std::endl;

	// Process vertices 
	for (int i = 0; i < nv; i++)
	{
		_file >> x >> y >> z;

		coords.push_back(x);
		coords.push_back(y);
		coords.push_back(z);
	}

	// Process triangles
	for (int i = 0; i < nt; i++)
	{
		_file >> num_v >> v0 >> v1 >> v2;

		triangles.push_back(v0);
		triangles.push_back(v1);
		triangles.push_back(v2);
	}
}