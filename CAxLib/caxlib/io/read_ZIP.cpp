/**
 @author    Daniela Cabiddu (dani.cabiddu@gmail.com)
 @copyright Daniela Cabiddu 2016.

 Based on https://gist.github.com/mobius/1759816.
*/

#include "read_ZIP.h"

#include "read_OFF.h"
#include "read_ANN.h"

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
void unzip(const char *filename, const char *off, const char *ann)
{
    //Open the ZIP archive
    int err = 0;
    zip_flags_t flags;
    struct zip_stat sb;

    char buf[100];
    int len;
    int fd;
    long long sum;

    zip *zfile = zip_open(filename, 0, &err);

    if (err != 0)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ <<
                     " : read_ZIP() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    int nFiles = zip_get_num_entries(zfile, flags);

    if (nFiles == 0)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ <<
                     " : read_ZIP() : empty " << filename << endl;
        exit(-1);
    }

    if (nFiles > 2)
    {
        caxlib::logger << "ERROR : " << __FILE__ << ", line " << __LINE__ <<
                          " : read_ZIP() : " << filename << " contains more than 2 files" << endl;
        exit(-1);
    }

    struct zip_file *zf;

    for (uint i = 0; i < nFiles; i++)
    {
        if (zip_stat_index(zfile, i, 0, &sb) == 0)
        {
            std::string zfilename = sb.name;

            len = strlen(zfilename.c_str());

            caxlib::logger << "extract: " << zfilename << ", size: " << sb.size << endl;

            std::string ext = zfilename.substr(zfilename.find_last_of('.'), zfilename.length());

            if (ext.compare(".off") != 0 && ext.compare(".ann") != 0
                    && ext.compare(".OFF") != 0 && ext.compare(".ANN") != 0)
            {
                caxlib::logger << "ERROR : " << __FILE__ << ", line " << __LINE__ <<
                                  " : read_ZIP() : unknown format " << zfilename << " [" << ext << "] " << endl;
                exit(-1);
            }

            if (ext.compare(".off") == 0 || ext.compare(".OFF") == 0)
            {
                //off_filename = zfilename;

                fd = open(off, O_RDWR | O_TRUNC | O_CREAT, 0644);
            }
            else // ann file
            {
                //ann_filename = zfilename;

                fd = open(ann, O_RDWR | O_TRUNC | O_CREAT, 0644);
            }

            zf = zip_fopen_index(zfile, i, 0);

            if (!zf)
            {
                caxlib::logger << "ERROR : " << __FILE__ << ", line " << __LINE__ <<
                                  " : read_ZIP() : error reading" << zfilename << " " << endl;
                exit(1);
            }

            //fd = open(zfilename.c_str(), O_RDWR | O_TRUNC | O_CREAT, 0644);

            if (fd < 0)
            {
                caxlib::logger << "ERROR : " << __FILE__ << ", line " << __LINE__ <<
                                  " : read_ZIP() : error extracting " << zfilename << endl;
                exit(1);
            }

            sum = 0;

            while (sum != sb.size)
            {
                len = zip_fread(zf, buf, 100);

                if (len < 0)
                {
                    caxlib::logger << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_ZIP() : writing " << zfilename << endl;
                    exit(1);
                }
                write(fd, buf, len);
                sum += len;
            }

            if (close(fd) != 0)
            {
                caxlib::logger << "ERROR : " << __FILE__ << ", line " << __LINE__ <<
                                  " : read_ZIP() : impossible to close " << zfilename << endl;
                exit(1);
            }

            zip_fclose(zf);
        }
        else
        {
            caxlib::logger << "File " << __FILE__ << ", Line " << __LINE__ << endl;
        }
    }

    if (zip_close(zfile) == -1)
    {

        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ <<
                     " : read_ZIP() : impossible to close " << filename << endl;
        exit(-1);
    }
}

CAX_INLINE
void read_ZIP (const char                            * filename,
               std::vector<double>                   & xyz,
               std::vector<u_int>                    & tri,
               GlobalAnnotations                     & glob_ann,
               std::vector<VertexAnnotations>   & vertex_ann,
               std::vector<TriangleAnnotations> & triangle_ann )
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator
    const char * off_filename = "tmp.off";
    const char * ann_filename = "tmp.ann";

    unzip(filename, off_filename, ann_filename);

    read_OFF(off_filename, xyz, tri);

    vertex_ann.resize(xyz.size() / 3);
    triangle_ann.resize(tri.size() / 3);

    read_ANN(ann_filename, glob_ann, vertex_ann, triangle_ann);

    remove (off_filename);
    remove (ann_filename);
}

}
