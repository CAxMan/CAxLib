#include "isosurface.h"

#include "marching_tets.h"

namespace caxlib
{

CAX_INLINE
Isosurface::Isosurface(const Tetmesh & m, const float iso_value) : m_ptr(&m), iso_value(iso_value)
{
    marching_tets(m, iso_value, coords, tris, t_norms);
}

CAX_INLINE
Trimesh Isosurface::export_as_trimesh() const
{
    return Trimesh(coords, tris);
}

}
