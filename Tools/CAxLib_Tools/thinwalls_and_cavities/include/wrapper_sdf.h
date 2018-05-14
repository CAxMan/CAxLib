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

#ifndef SDF_WRAPPER_H
#define SDF_WRAPPER_H

#include "danilib.h"

#include <vector>

namespace danilib
{

DANI_INLINE
void shape_diameter_function (const char *filename, std::vector<double> &values);

}

#ifndef DANI_STATIC_LIB
#include "src/wrapper_sdf.cpp"
#endif

#endif // SDF_WRAPPER_H
