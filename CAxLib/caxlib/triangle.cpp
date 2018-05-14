/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2017.
*/
#include "caxlib/triangle.h"
#include "caxlib/vec3.h"

namespace caxlib
{

// http://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
//
CAX_INLINE
bool triangle_barycentric_coords(const vec3d  & A,
                                 const vec3d  & B,
                                 const vec3d  & C,
                                 const vec3d  & P,
                                 std::vector<double> & wgts,
                                 const double   tol)
{
    wgts = std::vector<double>(3, 0.0);

    vec3d  u    = B - A;
    vec3d  v    = C - A;
    vec3d  w    = P - A;
    double d00  = u.dot(u);
    double d01  = u.dot(v);
    double d11  = v.dot(v);
    double d20  = w.dot(u);
    double d21  = w.dot(v);
    double den  = d00 * d11 - d01 * d01;

    if (den==0) return false; // degenerate

    wgts[2] = (d00 * d21 - d01 * d20) / den; assert(!std::isnan(wgts[2]));
    wgts[1] = (d11 * d20 - d01 * d21) / den; assert(!std::isnan(wgts[1]));
    wgts[0] = 1.0f - wgts[1] - wgts[2];      assert(!std::isnan(wgts[0]));

    for(double w : wgts) if (w < -tol || w > 1.0 + tol) return false; // outside
    return true; // inside
}


CAX_INLINE
bool triangle_point_is_inside(const vec3d  & A,
                              const vec3d  & B,
                              const vec3d  & C,
                              const vec3d  & P,
                              const double   tol)
{
    // NOTE : it assumes the four points are coplanar!
    std::vector<double> wgts;
    return triangle_barycentric_coords(A, B, C, P, wgts, tol);
}



CAX_INLINE
bool triangle_bary_is_vertex(const std::vector<double> & bary,
                             int                       & vid,
                             const double                tol)
{
    assert(bary.size()==3);
    if (bary[0]>tol && bary[1]<=tol && bary[2]<=tol) { vid = 0; return true; }
    if (bary[1]>tol && bary[0]<=tol && bary[2]<=tol) { vid = 1; return true; }
    if (bary[2]>tol && bary[0]<=tol && bary[1]<=tol) { vid = 2; return true; }
    return false;
}


CAX_INLINE
bool triangle_bary_is_edge(const std::vector<double> & bary,
                             int                     & eid,
                             const double              tol)
{
    assert(bary.size()==3);
    if (bary[0]>tol && bary[1]>tol && bary[2]<=tol) { eid = 0; return true; }
    if (bary[1]>tol && bary[2]>tol && bary[0]<=tol) { eid = 1; return true; }
    if (bary[2]>tol && bary[0]>tol && bary[1]<=tol) { eid = 2; return true; }
    return false;
}

}

