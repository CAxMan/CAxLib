#pragma once
#include "stdafx.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "stringEdit.h"

class OFFWriter
{

public:

	std::vector<int> support_structure;

private:

	//std::map<std::string, node*> nodeIDmap;
	//std::map<entityType, element_representation*> elementTypesMap;

	Database *_db;
	Repository *_rep;
	Model* _m;

	std::string _modelName;

	CMemoryAllocator*	_ma;

	std::ofstream	_file;
	char*			_filePath;
	char			_line[256];

public:
	OFFWriter();
	~OFFWriter();

	void setEdmiDB(Database* db, Repository *rep, Model *m, std::string modelName, CMemoryAllocator *cma){
		_db = db;
		_rep = rep;
		_ma = cma;
		_m = m;
		_modelName = modelName;
	}

	void writeOffFile(char* filePath);
	void closeFile();
};
