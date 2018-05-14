#include "checks.h"

int check_boundaries(IMATI_STL::TriMesh &tin, std::vector<caxlib::TriangleAnnotations> &lt_ann)
{
    Vertex *v, *w;
    Edge *e;
    Triangle *t, *s;
    Node *n;

    bool hasBoundary = false;

    for (n = tin.E.head(), e = (n)?((Edge *)n->data):NULL; n != NULL; n=n->next(), e = (n)?((Edge *)n->data):NULL)
        if (e->isOnBoundary())
        {
            hasBoundary = true;
            MARK_BIT(e->v1, 5);
            MARK_BIT(e->v2, 5);
        }

    int counter = 0;
    int tid = 0;

    for (n = tin.T.head(), t = (n)?((Triangle *)n->data):NULL; n != NULL; n=n->next(), t = (n)?((Triangle *)n->data):NULL)
    {
        if (IS_BIT (t->v1(), 5) || IS_BIT (t->v2(), 5) || IS_BIT (t->v3(), 5))
        {
            caxlib::TriangleAnnotations *tid_ann = &lt_ann.at(tid);
            tid_ann->bad_geometry = true;
            counter++;
        }

        tid++;
    }


    if (hasBoundary){
        for (n = tin.V.head(), v = (n)?((Vertex *)n->data):NULL; n != NULL; n=n->next(), v = (n)?((Vertex *)n->data):NULL)
            if (IS_BIT(v, 5))
            {
                //n_boundaries++;
                for (w = v; IS_BIT(w, 5); w = w->nextOnBoundary()) UNMARK_BIT(w, 5);
            }
    }

    return counter;

    //n_handles = (E.numels() - V.numels() - T.numels() + 2 * n_shells - n_boundaries) / 2;
    //d_boundaries = d_handles = d_shells = 0;
}
