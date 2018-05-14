/**
 @author    Daniela Cabiddu (dani.cabiddu@gmail.com)
 @copyright Daniela Cabiddu 2016.

 Based on https://gist.github.com/mobius/1759816.
*/

#include "write_ZIP.h"

#include "write_OFF.h"
#include "write_ANN.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <zip.h>

#include <zconf.h>

#include <iostream>

namespace caxlib
{

CAX_INLINE
void create_zip (const std::string zip_filename, const std::string off_filename, const std::string ann_filename)
{
    int error = 0;

    caxlib::logger << " Creating archive " << zip_filename << endl;

    zip *archive = zip_open(zip_filename.c_str(), ZIP_CREATE, &error);

    if(archive == NULL || error != 0)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write_ZIP() : impossible to create " << zip_filename << endl;
        exit(-1);
    }

    // Add file OFF to the archive
    //
    caxlib::logger << " Adding " << off_filename << " to " << zip_filename << endl;

    zip_source *source = zip_source_file(archive, off_filename.c_str(), 0, 0);

    if(source == NULL)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write_ZIP() : impossible to tmp " << off_filename << endl;
        exit(-1);
    }

    int index = (int)zip_file_add(archive, off_filename.substr(off_filename.find_last_of("/\\") +1).c_str(), source, ZIP_FL_OVERWRITE);

    if(index < 0)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write_ZIP() : impossible to add " << off_filename << endl;
        exit(-1);
    }

    // Add file ANN to the archive
    //
    caxlib::logger << " Adding " << ann_filename << " to " << zip_filename << endl;

    source = zip_source_file(archive, ann_filename.c_str(), 0, 0);

    if(source == NULL)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write_ZIP() : impossible to tmp " << ann_filename << endl;
        exit(-1);
    }

    index = (int)zip_file_add(archive, ann_filename.substr(ann_filename.find_last_of("/\\") +1).c_str(), source, ZIP_FL_OVERWRITE);

    if(index < 0)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write_ZIP() : impossible to add " << off_filename << endl;
        exit(-1);
    }

    zip_close(archive);

    remove (off_filename.c_str());
    remove (ann_filename.c_str());
}

CAX_INLINE
void write_ZIP (const char                                  * filename,
                const std::vector<double>                   & xyz,
                const std::vector<u_int>                    & tri,
                const GlobalAnnotations                     & glob_ann,
                const std::vector<VertexAnnotations>   & vertex_ann,
                const std::vector<TriangleAnnotations> & triangle_ann )
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    std::string basename = filename;

    // LibZip automatically adds ".zip" extension.
    //
    if (basename.substr(basename.find_last_of(".") + 1) == "zip" ||
            basename.substr(basename.find_last_of(".") + 1) == "ZIP")
    {
        basename = basename.substr(0, basename.length() -4);
    }

    std::string off_filename = basename + "off";
    std::string ann_filename = basename + "ann";
    std::string zip_filename = basename + "zip";

    write_OFF(off_filename.c_str(), xyz, tri);

    write_ANN(ann_filename.c_str(), glob_ann, vertex_ann, triangle_ann);

    create_zip (zip_filename.c_str(), off_filename.c_str(), ann_filename.c_str());

}

}
