#include "stdafx.h"
#include <iostream>
#include "DB_configuration.h"
//#include "AnalysisReader.h"
#include "ANNReader.h"
#include "OFFReader.h"
#include "ANNWriter.h"
#include "OFFWriter.h"
#include "STEPWriter.h"
#include "ZIPWriter.h"

//#include "ResultReader.h"
#include "StepFileInit.h"


/*================================================================================================*/
void myExceptionHandler(EDMLONG errcode, char *message, char *file, int lineNo)
/*================================================================================================*/
{
   throw new CedmError(errcode,message,file, lineNo);
}



int main(int argc, char **argv) 
{
	std::cout << ":::::::::::::::::::::::::::::::::::::::::\n";
	std::cout << "::::::::::::AP209 OFF+ANN Step Converter::::::::::::\n";
	std::cout << ":::::::::::::::::::::::::::::::::::::::::\n";
	std::cout << "Created by Daniela Cabiddu\n\n\n";

	std::string db_path, db_name, db_password, db_repName, db_modelName;
	std::string outPath;

	//INPUT
	db_path = "C:\\edm\\database";
	db_name = "caxman_db";
	db_password = "Caxman_88";
	db_repName = "DataRepository";
	db_modelName = "triangulated_surface";
	outPath = "C:\\edm\\step_out\\" + db_modelName + ".stp";

	int convert_step = 0;

	std::string input_1 = argv[1];
	std::string input_1_ext = input_1.substr(input_1.find_last_of('.'), input_1.length() - input_1.find_last_of('.'));

	std::string output_file = "";

	if (input_1_ext.compare(".stp") == 0)
	{
		convert_step = 1;
		output_file = argv[2];
	}
	else
		if (input_1_ext.compare(".off") == 0)
			convert_step = 0;
		else
			convert_step = -1;


	///###///
	try{
		defineExceptionHandler(&myExceptionHandler);

		if (convert_step == 0)	 // Convert off+ann to step
		{
			//DATABASE CONFIGURATION
			DB_configuration dbConfig;
			dbConfig.createAndOpenDataBase(&db_path[0], &db_name[0], &db_password[0]);
			dbConfig.createAndOpenRepository(&db_repName[0]);
			dbConfig.createAndOpenModel(&db_modelName[0]);
			dbConfig.defineServerContext();

			//MANDATORY ENTITIES/INSTANCES
			StepFileInit stepFile(&dbConfig);
			stepFile.addMandatoryEntities();

			std::cout << "Processing Geometry ... ";
			//READ OFF FILE
			OFFReader offReader;
			offReader.readOffFile(argv[1]);
			offReader.closeFile();

			std::cout << "COMPLETED" << std::endl;

			std::cout << "Processing Annotations ... " << std::endl;
			//READ OFF FILE
			ANNReader annReader;
			annReader.setEdmiDB(&dbConfig);
			annReader.readAnnFile(argv[2]);
			annReader.closeFile();

			std::cout << "COMPLETED" << std::endl;

			std::cout << "From OFF+ANN to STEP ... " << std::endl;

			STEPWriter stepWriter;
			stepWriter.setEdmiDB(&dbConfig);
			stepWriter.setOffReader(&offReader);
			stepWriter.setAnnReader(&annReader);
			stepWriter.writeStepFile();

			std::cout << "COMPLETED" << std::endl;

			shape_definition_representation *sdr = stepWriter.sdr;
			sdr->put_definition(stepFile.prodDefShape);

			dbConfig.terminate(&outPath[0]);
		}
		else if (convert_step == 1)	// Convert step into off + ann
		{
			std::cout << " Converting step file ... " << std::endl;
			
			std::string in_step_filename = argv[1];

			Database db ((char *) db_path.c_str(), (char *) db_name.c_str(), (char *) db_password.c_str());
			// open local database
			db.open();

			Repository rep (&db, (char *) db_repName.c_str());
			rep.open(sdaiRW); // write access

			// all objects are created using this memory allocator
			// this is used to avoid memory fragmentation. all allocated memory is released at once.
			CMemoryAllocator memAlloc(1024);
			Model model(&rep, &memAlloc, &ap209_multidisciplinary_analysis_and_design_mim_lf_SchemaObject);

			// creates model. returns error if model already exist
			edmiCreateModelBN(rep.getRepositoryId(), (char *) db_modelName.c_str(), "ap209_multidisciplinary_analysis_and_design_mim_lf", USE_DEFAULT_OPTIONS);
			model.open((char *) db_modelName.c_str(), sdaiRW); // write access
			// empty model content... just in case
			//edmiDeleteModelContents(model.modelId);

			// the context is needed to use "Semote" functions towards local database
			// "TCP" context is needed to connect to remote database via TCP
			edmiDefineServerContext("srvCtxt", "", "", "", "LOCAL_CONTEXT", "", "", "", "", "", "", "", &db.serverContextId);

			SdaiInteger	_nbWarnings;
			SdaiInteger _nbErrors;
			EdmiError	_sdaiError, rstat;

			// read data back to empy model
			rstat = edmiRemoteReadStepFile(db.serverContextId, &db_repName[0], &db_modelName[0], NULL, NULL,
				(char *) in_step_filename.c_str(), NULL,
				"ap209_multidisciplinary_analysis_and_design_mim_lf",
				NULL, DELETING_EXISTING_MODEL, &_nbWarnings, &_nbErrors, &_sdaiError, NULL);

			std::string off_filename = "off.off";
			std::string ann_filename = "ann.ann";

			//WRITE OFF FILE
			OFFWriter offWriter;
			offWriter.setEdmiDB(&db, &rep, &model, db_modelName, &memAlloc);
			offWriter.writeOffFile((char *) off_filename.c_str());
			offWriter.closeFile();

			//WRITE OFF FILE
			ANNWriter annWriter;
			annWriter.setEdmiDB(&db, &rep, &model, db_modelName, &memAlloc);
			annWriter.setOffWriter(&offWriter);
			annWriter.writeAnnFile((char *) ann_filename.c_str());
			annWriter.closeFile();

			// WRITE ZIP FILE
			ZIPWriter zipWriter (off_filename, ann_filename);
			zipWriter.write(output_file);

			// final call... probably it is better to close models before
			edmiCloseDatabase((char *)db_password.c_str());
		}

		//TERMINATE
	}
	catch (CedmError *e){
		if (e->message) {
			std::cout <<  e->message << std::endl;
		} else {
			std::cout <<  edmiGetErrorText(e->rstat) << std::endl;
		}
	}
	return 0;
}

