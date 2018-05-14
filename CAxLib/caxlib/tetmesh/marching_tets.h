#ifndef MARCHING_TETS_H
#define MARCHING_TETS_H

#include "../caxlib.h"
#include "tetmesh.h"

namespace caxlib
{


CAX_INLINE
void marching_tets(const Tetmesh       & m,
                   const double          isovalue,
                   std::vector<double> & coords,
                   std::vector<u_int>  & tris,
                   std::vector<double> & norm);

}


#ifndef  CAX_STATIC_LIB
#include "marching_tets.cpp"
#endif


#endif // MARCHING_TETS_H
