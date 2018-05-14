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

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <map>
#include <set>

#ifdef CINOLIB
#include <geometry/vec3.h>
using namespace cinolib;
#else
#ifdef CAXLIB
#include <caxlib/vec3.h>
#include <caxlib/trimesh/trimesh.h>
#include <caxlib/tetmesh/tetmesh.h>
using namespace caxlib;
#endif
#endif

#include "danilib.h"

namespace danilib
{

enum PointPosition
{
    EXTERNAL_POINT,
    POINT_POINT,
    POINT_EDGE,
    POINT_EDGE_P0,
    POINT_EDGE_P1,
    POINT_TRIANGLE,
    POINT_TRIANGLE_P0,
    POINT_TRIANGLE_P1,
    POINT_TRIANGLE_P2,
    POINT_TRIANGLE_E0,
    POINT_TRIANGLE_E1,
    POINT_TRIANGLE_E2,

};

class Point
{
public:

    double x;
    double y;
    double z;

    uint id;

    bool operator<(const Point& p) const
    {
        if ( x < p.x ||
            (x == p.x && y < p.y) ||
            (x == p.x && y == p.y && z < p.z) ||
            (x == p.x && y == p.y && z == p.z && id < p.id))
            return true;

        return false;
    }

    bool operator>(const Point& p) const
    {
        if ( x > p.x ||
            (x == p.x && y > p.y) ||
            (x == p.x && y == p.y && z > p.z) ||
            (x == p.x && y == p.y && z == p.z && id > p.id))
            return true;

        return false;
    }

    struct LessThan : public std::binary_function<Point, Point, bool>
    {
        bool operator()(const Point& lhs, const Point& rhs) const
        {
            return /*!(lhs == rhs) &&*/ (lhs < rhs);
        }
    };

};

typedef std::set<Point, Point::LessThan> SortedVertexList;

DANI_INLINE
bool are_points_equal (const vec3d &p0, const vec3d &p1);

DANI_INLINE
PointPosition is_point_on_edge (const vec3d &p, const vec3d &p0, const vec3d &p1);

DANI_INLINE
PointPosition is_point_on_triangle (const vec3d &p, const vec3d &p0, const vec3d &p1, const vec3d &p2);

}

#ifndef  DANI_STATIC_LIB
#include "src/geometry.cpp"
#endif

#endif
