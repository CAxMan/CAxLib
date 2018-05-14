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

#ifndef GTE_WRAPPER_H
#define GTE_WRAPPER_H

#include "include/danilib.h"

#ifdef CAXLIB
#include "caxlib/trimesh/trimesh.h"
using namespace caxlib;
#else
#ifdef CINOLIB
#include "meshes/trimesh/trimesh.h"
using namespace cinolib;
#endif
#endif

#ifdef GEOMETRICTOOLS

    #include "Mathematics/GteDistPointSegment.h"
    #include "Mathematics/GteDistPointTriangle.h"
    #include "Mathematics/GteDistSegmentSegment.h"
    #include "Mathematics/GteDistSegment3Triangle3.h"
    #include "Mathematics/GteDistTriangle3Triangle3.h"
    #include "Mathematics/GteSegment.h"
    #include "Mathematics/GteTriangle.h"
    #include "Mathematics/GteVector3.h"

#endif


namespace danilib
{

#ifdef GEOMETRICTOOLS

    DANI_INLINE
    gte::Vector3<double> get_gte_vertex (const vec3d vertex);

    DANI_INLINE
    gte::Triangle3<double> get_gte_triangle (const vec3d vertex0, const vec3d vertex1, const vec3d vertex2);

    DANI_INLINE
    gte::DCPQuery<double, gte::Vector3<double>, gte::Triangle3<double>>::Result VT_min_distance (const vec3d vertex, const std::vector<vec3d> tri);

    DANI_INLINE
    gte::DCPQuery<double, gte::Vector3<double>, gte::Triangle3<double>>::Result VT_min_distance (const vec3d vertex, vec3d tri0, vec3d tri1, vec3d tri2);

    DANI_INLINE
    gte::DCPQuery<double, gte::Vector3<double>, gte::Segment3<double>>::Result VE_min_distance (vec3d vertex, std::vector<vec3d> edg);

    DANI_INLINE
    gte::DCPQuery<double, gte::Vector3<double>, gte::Triangle3<double>>::Result VT_min_distance (const Trimesh &m, const vec3d vertex, const uint tid);

#endif

DANI_INLINE
double VT_distance (const vec3d vertex, const std::vector<vec3d> tri);

DANI_INLINE
double VT_distance (const vec3d vertex, vec3d tri0, vec3d tri1, vec3d tri2);

DANI_INLINE
double VE_distance (vec3d vertex, std::vector<vec3d> edg);

DANI_INLINE
double VT_distance (const Trimesh &m, const vec3d vertex, const uint tid);

}


#ifndef DANI_STATIC_LIB
#include "src/wrapper_gte.cpp"
#endif

#endif // GTE_WRAPPER_H
