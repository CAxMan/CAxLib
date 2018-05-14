/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2014.
*/

#include "write_STL.h"

#include <iostream>

namespace caxlib
{

CAX_INLINE
void write_STL(const char                   * filename,
               const std::vector<double>    & xyz,
               const std::vector<u_int>     & tri)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    std::vector<double> t_norm;

    t_norm.clear();
    t_norm.resize(tri.size());

    for(int tid=0; tid<tri.size() /3; ++tid)
    {
        int tid_ptr = tid * 3;

        vec3d v0 = vec3d (xyz.at(tri[tid_ptr+0] *3), xyz.at(tri[tid_ptr+0] *3 +1), xyz.at(tri[tid_ptr+0] *3 +2));
        vec3d v1 = vec3d (xyz.at(tri[tid_ptr+1] *3), xyz.at(tri[tid_ptr+1] *3 +1), xyz.at(tri[tid_ptr+1] *3 +2));
        vec3d v2 = vec3d (xyz.at(tri[tid_ptr+2] *3), xyz.at(tri[tid_ptr+2] *3 +1), xyz.at(tri[tid_ptr+2] *3 +2));

        vec3d u = v1 - v0;    u.normalize();
        vec3d v = v2 - v0;    v.normalize();
        vec3d n = u.cross(v); n.normalize();

        t_norm[tid_ptr + 0] = n.x();
        t_norm[tid_ptr + 1] = n.y();
        t_norm[tid_ptr + 2] = n.z();
    }


    FILE *fp;

    if ((fp = fopen(filename,"w")) == NULL)
    {
      std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write_STL() : can't open " << filename << " for output" << std::endl;
      exit(-1);
    }

    fprintf(fp,"solid JMESH_STL\n");

    for (uint i=0; i < tri.size() / 3; i++)
    {
        uint tid = i*3;

        fprintf(fp," facet normal %lf %lf %lf\n", t_norm[tid], t_norm[tid+1], t_norm[tid+2]);
        fprintf(fp,"  outer loop\n");
        fprintf(fp,"   vertex %lf %lf %lf\n", xyz.at(tri.at(tid+0) * 3), xyz.at(tri.at(tid+0) * 3 +1), xyz.at(tri.at(tid+0) * 3 +2));
        fprintf(fp,"   vertex %lf %lf %lf\n", xyz.at(tri.at(tid+1) * 3), xyz.at(tri.at(tid+1) * 3 +1), xyz.at(tri.at(tid+1) * 3 +2));
        fprintf(fp,"   vertex %lf %lf %lf\n", xyz.at(tri.at(tid+2) * 3), xyz.at(tri.at(tid+2) * 3 +1), xyz.at(tri.at(tid+2) * 3 +2));
        fprintf(fp,"  endloop\n");
        fprintf(fp," endfacet\n");
     }
     fprintf(fp,"endsolid JMESH_STL\n");

     fclose(fp);
}

CAX_INLINE
void export_STL(const char                  * filename,
                const std::vector<double>   & xyz,
                const std::vector<u_int>    & tri,
                const Bbox                  & bb,
                const GlobalAnnotations     & glob_ann)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    std::vector<double> rotated_xyz;
    rotated_xyz.resize(xyz.size());

    vec3d c = bb.center();

    // Rotate geometry

    for(uint vid=0; vid < xyz.size(); vid += 3)
    {
        vec3d pos = vec3d(xyz[vid+0], xyz[vid+1], xyz[vid+2]) - c;

        vec3d tmp;
        tmp[0] = glob_ann.orientation[0] * pos.x() + glob_ann.orientation[1] * pos.y() + glob_ann.orientation[2] * pos.z();
        tmp[1] = glob_ann.orientation[3] * pos.x() + glob_ann.orientation[4] * pos.y() + glob_ann.orientation[5] * pos.z();
        tmp[2] = glob_ann.orientation[6] * pos.x() + glob_ann.orientation[7] * pos.y() + glob_ann.orientation[8] * pos.z();
        pos = tmp;

        pos += c;

        rotated_xyz.at(vid + 0) = pos.x();
        rotated_xyz.at(vid + 1) = pos.y();
        rotated_xyz.at(vid + 2) = pos.z();
    }

    write_STL(filename, rotated_xyz, tri);
}


}
