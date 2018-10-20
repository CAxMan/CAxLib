#pragma once
#include "stdafx.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "stringEdit.h"

#include "OFFWriter.h"

class ANNWriter
{

public:

	OFFWriter *offWriter;

private:

	Database *_db;
	Repository *_rep;
	Model* _m;

	std::string _modelName;

	CMemoryAllocator*	_ma;

	std::ofstream	_file;
	char*			_filePath;
	char			_line[256];

public:
	ANNWriter();
	~ANNWriter();

	void setEdmiDB(Database* db, Repository *rep, Model *m, std::string modelName, CMemoryAllocator *cma){
		_db = db;
		_rep = rep;
		_ma = cma;
		_m = m;
		_modelName = modelName;
	}

	void setOffWriter(OFFWriter *writer) { offWriter = writer;  }

	void writeAnnFile(char* filePath);
	void closeFile();
};
