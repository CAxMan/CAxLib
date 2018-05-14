#ifndef DRAWABLE_ISOSURFACE_H
#define DRAWABLE_ISOSURFACE_H

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include "../caxlib.h"
#include "../drawable_object.h"
#include "isosurface.h"

namespace caxlib
{

class DrawableIsosurface : public Isosurface, public DrawableObject
{
    public:

        DrawableIsosurface();
        DrawableIsosurface(const Tetmesh & m, const float iso_value);

        void   draw() const;
        vec3d scene_center() const;
        float scene_radius() const;
};

}

#ifndef  CAX_STATIC_LIB
#include "drawable_isosurface.cpp"
#endif

#endif // DRAWABLE_ISOSURFACE_H
