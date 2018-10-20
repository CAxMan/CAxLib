#pragma once
#include "stdafx.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "stringEdit.h"

#include "OFFReader.h"
#include "ANNReader.h"

class STEPWriter
{

public:

	shape_definition_representation *sdr;

	triangulated_surface_set *model = NULL;
	triangulated_surface_set *support_structure = NULL;

private:

	DB_configuration*		_dbConfig;
	CMemoryAllocator*	_ma;
	Model* _m;

	std::string _modelName;

	OFFReader *offReader;
	ANNReader *annReader;
	
public:
	STEPWriter();
	~STEPWriter();

	void setEdmiDB(DB_configuration* dbConfig)
	{
		_dbConfig = dbConfig;
		_ma = _dbConfig->getMemAlloc();
		_m = _dbConfig->getModel();
	}

	void writeStepFile();

	void convertGeometry();
	void convertAnnotations();

	void setOffReader(OFFReader *reader) { offReader = reader; }
	void setAnnReader(ANNReader *reader) { annReader = reader; }

	void generatePrintingParameters();
	void generateSupportStructures();
	void generateVersionedActioRequest(const std::string id, const std::string elemType, std::vector<int> &elements);

};
