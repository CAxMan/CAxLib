#include "drawable_isosurface.h"

namespace caxlib
{


CAX_INLINE
DrawableIsosurface::DrawableIsosurface()
{
    type = ISOSURFACE;
}

CAX_INLINE
DrawableIsosurface::DrawableIsosurface(const Tetmesh & m, const float iso_value) : Isosurface(m, iso_value)
{
    type = ISOSURFACE;
}

CAX_INLINE
void DrawableIsosurface::draw() const
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    for(size_t i=0; i<tris.size(); i+=3)
    {
        int vid0     = tris[i+0];
        int vid1     = tris[i+1];
        int vid2     = tris[i+2];
        int vid0_ptr = 3 * vid0;
        int vid1_ptr = 3 * vid1;
        int vid2_ptr = 3 * vid2;

        glBegin(GL_TRIANGLES);
        glColor3f(1.0,0.0,0.0);
        glNormal3dv(&(t_norms[i]));
        glVertex3dv(&(coords[vid0_ptr]));
        glVertex3dv(&(coords[vid1_ptr]));
        glVertex3dv(&(coords[vid2_ptr]));
        glEnd();
    }
}

CAX_INLINE
vec3d DrawableIsosurface::scene_center() const
{
    return m_ptr->bb.center();
}

CAX_INLINE
float DrawableIsosurface::scene_radius() const
{
    return m_ptr->bb.diag();
}

}
