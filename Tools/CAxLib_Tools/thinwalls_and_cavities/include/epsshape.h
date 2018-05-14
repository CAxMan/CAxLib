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

#ifndef EPSSHAPE_H
#define EPSSHAPE_H
#include "geometry.h"


namespace danilib {

enum InOut {

    INNER,
    OUTER,
    BORDER
};


enum VertexType {

    ORIGINAL_BOUNDARY_POINT,
    ADDED_BOUNDARY_POINT,
    EXTRA_POINT

};

enum EdgeType {

    ORIGINAL_BOUNDARY_EDGE,
    ADDED_BOUNDARY_SUBEDGE,
    EXTRA_EDGE

};

enum TriType {

    ORIGINAL_BOUNDARY_TRIANGLE,
    ADDED_BOUNDARY_TRIANGLE,
    EXTRA_TRIANGLE

};

class EpsilonShape
{

public:

    double epsilon;

    uint epsilon_center_tin;

    std::vector<double> v_epsilon;

    bool is_epsilon_known;

    //Methods
    //virtual std::tuple<uint, uint, uint> compute_betti_numbers ();

    //virtual double compute_epsilon ();

    //virtual void update_ext_link (const uint vid, std::multimap<double, Point> &link, const Point &p);
};

}

#endif // EPSSHAPE_H
