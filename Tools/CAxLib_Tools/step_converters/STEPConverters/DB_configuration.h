#pragma once
#include "stdafx.h"

class DB_configuration
{
private:
	CMemoryAllocator*	_memAlloc;
	Database*			_db;
	Repository*			_rep;
	Model*				_model;
	SdaiModel			_modelID;

	SdaiInteger	_nbWarnings; 
	SdaiInteger _nbErrors;
	EdmiError	_sdaiError;

	char* _db_path;
	char* _db_name;
	char* _db_password;
	char* _db_repName;
	char* _db_modelName;
	char* _outPath;

public:
	DB_configuration();
	~DB_configuration();

	//Initialize
	void createAndOpenDataBase(char* db_path, char* db_name, char* db_password);
	void createAndOpenRepository(char* db_repName);
	void createAndOpenModel(char* db_modelName);
	void defineServerContext();

	void terminate(char* outPath);
	
	//Getters
	CMemoryAllocator*	getMemAlloc(){ return _memAlloc; }
	Database*			getDB(){ return _db; }
	Repository*			getRep(){ return _rep; }
	Model*				getModel(){ return _model; }

};

