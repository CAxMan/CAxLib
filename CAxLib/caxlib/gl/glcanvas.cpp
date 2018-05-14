/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2014.
*/

#include "glcanvas.h"

namespace caxlib
{

CAX_INLINE GLcanvas::GLcanvas(QWidget * parent)
{
    setParent(parent);

    clear_color = QColor(50, 50, 50);
}

CAX_INLINE void GLcanvas::init()
{
    setFPSIsDisplayed(true);
}

CAX_INLINE void GLcanvas::clear()
{
    drawlist.clear();
}

CAX_INLINE void GLcanvas::draw()
{
    setBackgroundColor(clear_color);

    for(int i=0; i<(int)drawlist.size(); ++i)
    {
        drawlist[i]->draw();
    }
}

CAX_INLINE void GLcanvas::push_obj(const DrawableObject * obj, bool refit_scene)
{
    drawlist.push_back(obj);

    if (refit_scene)
    {
        fit_scene();
    }

    updateGL();
}

CAX_INLINE void GLcanvas::fit_scene()
{
    vec3d center(0,0,0);
    float radius = 0.0;
    int   count  = 0;

    for(int i=0; i<(int)drawlist.size(); ++i)
    {
        const DrawableObject * obj = drawlist[i];

        //if (obj->type != TRIMESH) continue;

        center += obj->scene_center();
        radius  = std::max(radius, obj->scene_radius());
        ++count;
    }

    center /= (double)count;

    setSceneCenter(qglviewer::Vec(center.x(), center.y(), center.z()));
    setSceneRadius(radius);

    showEntireScene();

    //logger << endl;
    //logger << "Update Scene Center: " << center << endl;
    //logger << "Update Scene Radius: " << radius << endl;
    //logger << endl;
}

CAX_INLINE void GLcanvas::set_clear_color(const QColor &color)
{
    clear_color = color;
    updateGL();
}


CAX_INLINE bool GLcanvas::pop_all_occurrences_of(int type)
{
    bool found = false;

    while (pop_first_occurrence_of(type))
    {
        found = true;
    };

    return found;
}

CAX_INLINE bool GLcanvas::pop_first_occurrence_of(int type)
{
    for(std::vector<const DrawableObject *>::iterator it=drawlist.begin(); it!=drawlist.end(); ++it)
    {
        const DrawableObject * obj = *it;

        if (obj->type == type)
        {
            drawlist.erase(it);
            return true;
        }
    }
    return false;
}

}
