/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2016.
*/

#ifndef ORIENT_H
#define ORIENT_H

#include "../caxlib.h"
#include "../sphere_coverage.h"
#include "../trimesh/trimesh.h"

namespace caxlib
{

CAX_INLINE
void define_rotation(const vec3d & build_dir, vec3d & axis, double & angle)
{
    vec3d z = vec3d(0,0,1);
    axis    = build_dir.cross(z); axis.normalize();
    angle   = acos(build_dir.dot(z));
    assert(!std::isnan(angle));
}

CAX_INLINE
bool rotated_bbox_exceeds_chamber_size(const Trimesh & m, const vec3d & build_dir)
{
    Trimesh tmp_m = m;
    vec3d   axis;
    double  angle;
    define_rotation(build_dir, axis, angle);
    tmp_m.rotate(axis, angle);

    if (tmp_m.bbox().delta_x() > m.global_annotations().printer.chamber_dimension[0])  return true;
    if (tmp_m.bbox().delta_y() > m.global_annotations().printer.chamber_dimension[1])  return true;
    if (tmp_m.bbox().delta_z() > m.global_annotations().printer.chamber_dimension[2]) return true;
    return false;
}

CAX_INLINE
double rotated_bbox_delta_z(const Trimesh & m, const vec3d & build_dir)
{
    Trimesh tmp_m = m;
    vec3d   axis;
    double  angle;
    define_rotation(build_dir, axis, angle);
    tmp_m.rotate(axis, angle);
    return tmp_m.bbox().delta_z();
}

CAX_INLINE
double cusp_height_error(const Trimesh & m, const vec3d & build_dir)
{
    Trimesh tmp_m = m;
    vec3d   axis;
    double  angle;
    define_rotation(build_dir, axis, angle);
    tmp_m.rotate(axis, angle);

    double M   = 0.0;
    double ch  = 0.0;
    for(uint tid=0; tid<tmp_m.num_triangles(); ++tid)
    {
        vec3d  n = tmp_m.triangle_normal(tid);
        double m = tmp_m.element_mass(tid);
        ch += m * (n.dot(build_dir));
        M += m;
    }
    ch/=M;
    return ch;
}

/* Choose the best orientation according to three metrics (or any combination of them).
 * Metrics are:
 *
 *  - Maximal Surface Quality (i.e. minimize cusp height error)
 *  - Minimal print time (i.e., minimize number of slices)
 *  - Minimal Support Structures (i.e., minimize overhangs)
 *
 * The influence of these metrics is defined by three scalars (wgt_srf_quality, wgt_print_time, wgt_supports).
 * This function minimizes a functional defined as the weighted sum of these three components. Weigths must
 * sum up to 1.
*/
CAX_INLINE
bool orient(Trimesh       & m,
            const double    wgt_srf_quality = 0.0,
            const double    wgt_print_time = 0.0,
            const double    wgt_supports = 1.0,
            const double    angle_thresh_deg = 30.0,
            const int       n_dirs = 100)
{
    std::vector<vec3d> dir_pool;
    sphere_coverage(n_dirs, dir_pool);

    double           best_obj = FLT_MAX; //weighted sum of cusp height,
    vec3d            best_dir;
    std::vector<int> best_supports;

    logger.disable();

    for(vec3d build_dir : dir_pool)
    {
        if (rotated_bbox_exceeds_chamber_size(m, build_dir))
        {
            logger << "Skip build dir " << build_dir << " because the part would not fit the printing chamber" << endl;
            continue;
        }

        build_dir.normalize();

        std::vector<int> overhangs;
        double sup_area    = m.get_overhangs(build_dir, angle_thresh_deg, overhangs); // \in [0,1]
        double cusp_height = cusp_height_error(m, build_dir);                         // \in [0,1]
        double height      = rotated_bbox_delta_z(m, build_dir);                      // NOT IN [0,1]!!!!!!!

        double obj = wgt_srf_quality * cusp_height +
                     wgt_print_time  * height      +
                     wgt_supports    * sup_area;

        if (obj < best_obj)
        {
            best_obj      = obj;
            best_dir      = build_dir;
            best_supports = overhangs;
        }
    }

    if (best_obj == FLT_MAX)
    {
        m.global_annotations().no_legal_orientation = true;
        logger << "The part cannot be oriented so as to fit the printing chamber!" << endl;
        return false;
    }

    logger.enable();

    for(int tid : best_supports)
    {
        m.set_overhangs(tid, true);
    }

    vec3d  axis;
    double angle;
    define_rotation(best_dir, axis, angle);
    //m.rotate(axis, angle); NO! (april 6th, 2017)

    double R[3][3];
    bake_rotation_matrix(axis, angle, R);
    // orrible - to be changed
    for(int i=0; i<3; ++i)
    for(int j=0; j<3; ++j)
    {
        m.global_annotations().orientation[3*i+j] = R[i][j];
    }

    logger << "Selected Build Orientation: " << best_dir << newl;
    logger << "Rotation Matrix: "            << newl;
    logger << R[0][0] << "\t" << R[0][1] << "\t" << R[0][2] << newl;
    logger << R[1][0] << "\t" << R[1][1] << "\t" << R[1][2] << newl;
    logger << R[2][0] << "\t" << R[2][1] << "\t" << R[2][2] << newl << flush;

    return true;
}

}

#endif // TIMER_H
