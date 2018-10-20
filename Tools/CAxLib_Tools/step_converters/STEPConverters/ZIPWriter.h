
#include <iostream>
#include <string>

class ZIPWriter
{

public:

	std::string off_filename;
	std::string ann_filename;

	ZIPWriter(const std::string off_filename, const std::string ann_filename) 
	{
		this->off_filename = off_filename;
		this->ann_filename = ann_filename;
	}

	void write(const std::string output_filename);





};