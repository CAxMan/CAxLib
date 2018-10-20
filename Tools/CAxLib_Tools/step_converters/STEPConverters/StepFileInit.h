#pragma once
#include "stdafx.h"
#include "DB_configuration.h"

#ifndef createEntity(className)
#define createEntity(className) new(_m)className(_m)
#endif
#ifndef createSet(className, sdaiTYPE, n)
#define createSet(className, sdaiTYPE, n) new(_ma)Set<className*>(_ma, sdaiTYPE, n);
#endif

class StepFileInit
{

private:
	DB_configuration* _dbConfig;
	Model* _m;
	CMemoryAllocator* _ma;

public:
	StepFileInit(DB_configuration* dbConfig);

	StepFileInit(Model* m, CMemoryAllocator* ma);
	~StepFileInit();

	product_definition *prodDef;
	product_definition_shape *prodDefShape;

	std::string analysis_id = "0000"; 											//unique number or ID of Analysis
	std::string analysis_name = "Winglet Structure Analysis";					//name of the analysis
	std::string analysis_description = "Winglet 8x 5mm ribs";					//optional description of the Analysis

	std::string analysis_version = "2";											//value that represents the version
	std::string analysis_version_description = "coarse QUAD4 mesh";				//reason for the creation of the version

	std::string organization_id = "CNR";										//a unique ID for the organization
	std::string organization_name = "CNR-IMATI Genova";						//name of the organization
	std::string organization_description = "research";					//optional description of organization

	std::string person_id = "DCB";				//unique ID for the person creating the analysis / part
	std::string person_first_name = "Daniela";		//first name of for the person creating the analysis / part
	std::string person_last_name = "CABIDDU";		//last name .....

	//part/analysis creation time (should be taken from converted file)
	year_number creation_year = 2016;
	month_in_year_number creation_month = 11;
	day_in_month_number creation_day = 6;
	hour_in_day creation_hour = 10;
	int time_offset_utc = 1;
	ahead_or_behind time_offset_ahead_or_behind = ahead_or_behind_AHEAD;


	void addMandatoryEntities();
	product_definition* getProductDefinition();

};

