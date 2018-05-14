/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Daniela Cabiddu (daniela.cabiddu@ge.imati.cnr.it)                 *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of EpsilonShapes.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/

#include "include/geometry.h"
#include "include/wrapper_gte.h"

#ifdef CINOLIB
#include "geometry/segment.h"
#include "geometry/triangle.h"
#else
#ifdef CAXLIB
#include "caxlib/triangle.h"
#endif
#endif

namespace danilib
{

DANI_INLINE
bool are_points_equal (const vec3d &p0, const vec3d &p1)
{
    double dist = 0.0;

#ifdef GEOMETRICTOOLS

    gte::Vector3<double> diff = get_gte_vertex(p0) - get_gte_vertex(p1);
    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);

#else

#endif

    if (dist <= thresh)
        return true;

    //if (std::abs(p0.x() - p1.x()) <= thresh && std::abs(p0.y() - p1.y()) <= thresh && std::abs(p0.z() - p1.z()) <= thresh)
    //    return true;

    return false;
}

DANI_INLINE
PointPosition is_point_on_edge (const vec3d &p, const vec3d &p0, const vec3d &p1)
{
#ifdef GEOMETRICTOOLS

    if (are_points_equal(p, p0))
        return POINT_EDGE_P0;

    if (are_points_equal(p, p1))
        return POINT_EDGE_P1;

    std::vector<vec3d> edge;
    edge.push_back(p0);
    edge.push_back(p1);

    double dist = VE_distance(p, edge); //p.dist(p0) + p.dist(p1) - (p0 - p1).length();

    if (dist < thresh)
        return POINT_EDGE;

    return EXTERNAL_POINT;
#else

#endif
}

DANI_INLINE
PointPosition is_point_on_triangle (const vec3d &p, const vec3d &p0, const vec3d &p1, const vec3d &p2)
{

#ifdef GEOMETRICTOOLS

    PointPosition pp = is_point_on_edge (p, p0, p1);

    if (pp != EXTERNAL_POINT)
    {
        if (pp == POINT_EDGE_P0)
            return POINT_TRIANGLE_P0;

        if (pp == POINT_EDGE_P1)
            return POINT_TRIANGLE_P1;

        return POINT_TRIANGLE_E0;
    }

    pp = is_point_on_edge (p, p1, p2) ;

    if (pp != EXTERNAL_POINT)
    {
        if (pp == POINT_EDGE_P0)
            return POINT_TRIANGLE_P1;

        if (pp == POINT_EDGE_P1)
            return POINT_TRIANGLE_P2;

        return POINT_TRIANGLE_E1;
    }

    pp = is_point_on_edge (p, p2, p0);

    if ( pp != EXTERNAL_POINT)
    {
        if (pp == POINT_EDGE_P0)
            return POINT_TRIANGLE_P2;

        if (pp == POINT_EDGE_P1)
            return POINT_TRIANGLE_P0;

        return POINT_TRIANGLE_E2;
    }

    double dist = VT_distance(p, p0, p1, p2);

    if (dist < thresh)
        return POINT_TRIANGLE;

    //std::cout << "Distance from triangle : " << dist << std::endl;

    return EXTERNAL_POINT;

#else

    std::cerr << "Error : " << __FILE__ << " : " << __LINE__ << " : Impossible to compute point-triangle distance" << std::endl;
    exit(1);

#endif

}

}
