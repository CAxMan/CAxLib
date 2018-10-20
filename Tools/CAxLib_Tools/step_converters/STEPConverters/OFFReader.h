#pragma once
#include "stdafx.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "stringEdit.h"
#include "DB_configuration.h"

#define str2float(x) std::atof(&x[0])

class OFFReader
{

public:

	std::vector<double> coords;
	std::vector<int> triangles;

private:

	//std::map<std::string, node*> nodeIDmap;
	//std::map<entityType, element_representation*> elementTypesMap;

	DB_configuration*		_dbConfig;
	CMemoryAllocator*	_ma;
	Model* _m;

	std::ifstream	_file;
	char*			_filePath;
	char			_line[256];

	void processVertex	(coordinates_list &cl, int id, double x, double y, double z);
	void processTriangle(triangulated_surface_set &tss, int id, int v0, int v1, int v2);
	void processTriangle(triangulated_face &tf, int id, int v0, int v1, int v2);


public:
	OFFReader();
	~OFFReader();

	void setEdmiDB(DB_configuration* dbConfig){
		_dbConfig = dbConfig;
		_ma = _dbConfig->getMemAlloc();
		_m = _dbConfig->getModel();
	}
	
	void readOffFile(char* filePath);
	void closeFile();
};
