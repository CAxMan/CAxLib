#include <caxlib/process_plan/support_structures.h>
#include <caxlib/trimesh/trimesh.h>

#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        caxlib::logger << "Usage: ./supports_service input.zip support_thickness output.zip supports_data.txt" << caxlib::endl;
        return 0;
    }

    std::vector<caxlib::VertexCumulativeSupportData> per_vertex_supports_data;

    caxlib::Trimesh m(argv[1]);
    caxlib::create_support_structures(m, atof(argv[2]), per_vertex_supports_data);

    m.save(argv[3]);

    FILE *f = fopen(argv[4], "w");
    if (f)
    {
        for(uint vid=0; vid<per_vertex_supports_data.size(); ++vid)
        {
            fprintf(f, "%f %f %f %f %f %f %f\n",
                    m.vertex(vid).x(),
                    m.vertex(vid).y(),
                    m.vertex(vid).z(),
                    per_vertex_supports_data.at(vid).ground_area_ratio,
                    per_vertex_supports_data.at(vid).vertex_area,
                    per_vertex_supports_data.at(vid).support_height,
                    per_vertex_supports_data.at(vid).lifted_area_ratio);
        }

        fclose(f);
    }

    return 0;
}
