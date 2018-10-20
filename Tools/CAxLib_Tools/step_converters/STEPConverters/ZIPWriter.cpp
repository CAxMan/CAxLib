#include "ZIPWriter.h"

#include "zlib.h"
#include "zip.h"

void ZIPWriter::write(const std::string output_filename)
{
	
	int error = 0;

	zip *archive = zip_open(output_filename.c_str(), ZIP_CREATE, &error);

	if (archive == NULL || error != 0)
	{
		std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write_ZIP() : impossible to create " << output_filename << std::endl;
		exit(-1);
	}

	// Add file OFF to the archive
	//
	zip_source *source = zip_source_file(archive, off_filename.c_str(), 0, 0);

	if (source == NULL)
	{
		std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write_ZIP() : impossible to tmp " << off_filename <<std:: endl;
		exit(-1);
	}

	int index = (int)zip_file_add(archive, off_filename.substr(off_filename.find_last_of("/\\") + 1).c_str(), source, ZIP_FL_OVERWRITE);

	if (index < 0)
	{
		std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write_ZIP() : impossible to add " << off_filename << std::endl;
		exit(-1);
	}

	// Add file ANN to the archive
	//

	source = zip_source_file(archive, ann_filename.c_str(), 0, 0);

	if (source == NULL)
	{
		std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write_ZIP() : impossible to tmp " << ann_filename << std::endl;
		exit(-1);
	}

	index = (int)zip_file_add(archive, ann_filename.substr(ann_filename.find_last_of("/\\") + 1).c_str(), source, ZIP_FL_OVERWRITE);

	if (index < 0)
	{
		std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write_ZIP() : impossible to add " << off_filename << std::endl;
		exit(-1);
	}

	zip_close(archive);

	remove(off_filename.c_str());
	remove(ann_filename.c_str());

	off_filename = "";
	ann_filename = "";
}