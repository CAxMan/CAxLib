#include "imatistl.h"
#include <stdlib.h>
#include <string.h>

using namespace IMATI_STL;

void usage()
{
	printf("\nply2off V0.1alpha - by Marco Attene\n------\n");
	printf("Usage: ply2off inmeshfile.ply\n");
	printf("  Reads 'inmeshfile.ply' and produces 'inmeshfile.off'\n");
	printf("\nHIT ENTER TO EXIT.\n");
	getchar();
	exit(0);
}

char *createFilename(const char *iname, const char *subext, const char *newextension)
{
	static char tname[2048], oname[2048];
	strcpy(tname, iname);
	for (int n = strlen(tname) - 1; n>0; n--) if (tname[n] == '.') { tname[n] = '\0'; break; }
	sprintf(oname, "%s%s%s", tname, subext, newextension);
	return oname;
}


int main(int argc, char *argv[])
{
	ImatiSTL::init(); // This is mandatory
	ImatiSTL::app_name = "Ply2off";
	ImatiSTL::app_version = "0.1alpha";
	ImatiSTL::app_year = "2017";
	ImatiSTL::app_authors = "Marco Attene";
	ImatiSTL::app_maillist = "attene@ge.imati.cnr.it";

	if (argc < 2) usage();

	TriMesh tin;
	if (tin.load(argv[1]) != 0) ImatiSTL::error("Can't open file.\n");

	tin.save(createFilename(argv[1], "", ".off"));

	return 0;
}
