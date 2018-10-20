#pragma once
#include "stdafx.h"
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "stringEdit.h"
#include "DB_configuration.h"

#include "AnnotationMacros.h"

class ANNReader
{
private:

	//std::map<std::string, node*> nodeIDmap;
	//std::map<entityType, element_representation*> elementTypesMap;

	DB_configuration*		_dbConfig;
	CMemoryAllocator*	_ma;
	Model* _m;

	std::ifstream	_file;
	char*			_filePath;
	//char			_line[256];

	void processAnnotation(tinyxml2::XMLElement * annotation);

	void processLocalAnnotation(tinyxml2::XMLElement * annotation);

public:
	
	std::vector<int> bad_geometry;
	std::vector<int> bad_material;
	std::vector<int> overhangs;
	std::vector<int> channel_dimension;
	std::vector<int> absolute_thin_walls;
	std::vector<int> relative_thin_walls;
	std::vector<int> weak_features;
	std::vector<int> support_structures;

	std::vector<std::vector<int>> closed_voids;


	double orientation_matrix[9];
	double material_usage = 0.0;
	double build_time = 0.0;

	bool absolute_size = false;
	bool relative_size = false;
	bool part_weight = false;
	bool orientation_failure = false;
	
	ANNReader();
	~ANNReader();

	void setEdmiDB(DB_configuration* dbConfig){
		_dbConfig = dbConfig;
		_ma = _dbConfig->getMemAlloc();
		_m = _dbConfig->getModel();
	}

	void readAnnFile(char* filePath);
	void closeFile();

};