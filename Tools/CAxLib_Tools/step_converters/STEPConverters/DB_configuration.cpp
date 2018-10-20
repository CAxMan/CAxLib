#pragma once
#include "stdafx.h"
#include "DB_configuration.h."

void DB_configuration::terminate(char* outPath){
	//_model->writeAllObjectsToDatabase();

	edmiRemoteWriteStepFile(_db->serverContextId, _db_repName, _db_modelName, NULL, NULL,
		&outPath[0], NULL, NULL, 0, 8, &_nbWarnings, &_nbErrors, &_sdaiError, NULL);
	
	if (_sdaiError){ std::cout <<"ERROR remoteWriteToStepFile\n:"<< edmiGetErrorText(_sdaiError) << std::endl; }

	edmiCloseDatabase(_db_password);
}

void DB_configuration::createAndOpenModel(char* db_modelName){
	_db_modelName = db_modelName;

	_model = new Model(_rep, _memAlloc, &ap209_multidisciplinary_analysis_and_design_mim_lf_SchemaObject);

	_modelID = edmiCreateModelBN(_rep->getRepositoryId(), _db_modelName,
		"ap209_multidisciplinary_analysis_and_design", USE_DEFAULT_OPTIONS);
	_model->open(_db_modelName, sdaiRW);
	edmiDeleteModelContents(_model->modelId);
}

void DB_configuration::defineServerContext(){
	edmiDefineServerContext("srvCtxt", "", "", "", "LOCAL_CONTEXT", "", "", "", "", "", "", "", &(*_db).serverContextId);
}

void DB_configuration::createAndOpenRepository(char* db_repName){
	_db_repName = db_repName;
	_rep = new Repository(_db, _db_repName);
	_rep->open(sdaiRW);
}

void DB_configuration::createAndOpenDataBase(char* db_path, char* db_name, char* db_password){
	_db_path = db_path;
	_db_name = db_name;
	_db_password = db_password;

	_db = new Database(_db_path, _db_name, _db_password);
	_db->open();
}

DB_configuration::DB_configuration()
{
	_memAlloc = new CMemoryAllocator(1024);
}

DB_configuration::~DB_configuration()
{
}
