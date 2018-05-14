/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2014.
*/

#ifndef GLCANVAS_H
#define GLCANVAS_H

#include <qglviewer.h>
#include <QGLWidget>
#include <vector>

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include "../caxlib.h"
#include "../bbox.h"
#include "../drawable_object.h"


namespace caxlib
{

class GLcanvas : public QGLViewer
{
    public:

        GLcanvas(QWidget * parent = 0);

        void init();
        void draw();
        void clear();
        void fit_scene();
        void set_clear_color(const QColor & color);

        void push_obj(const DrawableObject * obj, bool refit_scene = true);

        bool pop_first_occurrence_of(int type);
        bool pop_all_occurrences_of (int type);

    private:

        QColor clear_color;
        std::vector<const DrawableObject *> drawlist;
};


}


#ifndef  CAX_STATIC_LIB
#include "glcanvas.cpp"
#endif

#endif // GLCANVAS_H
