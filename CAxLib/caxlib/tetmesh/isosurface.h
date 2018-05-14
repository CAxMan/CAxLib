/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2015.
*/

#ifndef ISOSURFACE_H
#define ISOSURFACE_H

#include "../caxlib.h"
#include "tetmesh.h"


namespace caxlib
{

class Isosurface
{
    public:

        Isosurface(){}
        Isosurface(const Tetmesh & m, const float iso_value);

        Trimesh export_as_trimesh() const;

    protected:

        const Tetmesh      *m_ptr;
        float               iso_value;
        std::vector<double> coords;
        std::vector<u_int>  tris;
        std::vector<double> t_norms;
};


}

#ifndef  CAX_STATIC_LIB
#include "isosurface.cpp"
#endif

#endif // ISOSURFACE_H
