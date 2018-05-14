#include <stdio.h>

#include "caxlib/io/write_ANN.h"

#include "caxlib/io/read_ZIP.h"
#include "caxlib/io/write_ZIP.h"

#include "checks.h"

int main(int argc, char *argv[])
{
    std::string zip_filename = argv[1];
    std::string out_zip_filename = argv[2];

    std::string out_off_filename = out_zip_filename.substr(0, out_zip_filename.find_last_of('.')) + ".off";
    std::string out_ann_filename = out_zip_filename.substr(0, out_zip_filename.find_last_of('.')) + ".ann";

    const char * off_filename = "tmp.off";
    const char * ann_filename = "tmp.ann";

    caxlib::unzip(zip_filename.c_str(), off_filename, ann_filename);

    IMATI_STL::TriMesh tin;
    if (tin.load(off_filename) != 0)
    {
        IMATI_STL::ImatiSTL::error("Can't load input file\n");
    }

    std::cout << tin.V.numels() << " vertices." << std::endl;

    caxlib::GlobalAnnotations glob_ann;
    std::vector<caxlib::VertexAnnotations> lv_ann (tin.V.numels());
    std::vector<caxlib::TriangleAnnotations> lt_ann (tin.T.numels());

    caxlib::read_ANN(ann_filename, glob_ann, lv_ann, lt_ann);

    remove (off_filename);
    remove (ann_filename);

    int boundaries = check_boundaries (tin, lt_ann);

    caxlib::write_ANN(out_ann_filename.c_str(), glob_ann, lv_ann, lt_ann);
    tin.saveOFF(out_off_filename.c_str());

    caxlib::create_zip(out_zip_filename, out_off_filename, out_ann_filename);

    return boundaries;
}
