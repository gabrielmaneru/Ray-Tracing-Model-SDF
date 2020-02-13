#include "ellipsoid.h"

ellipsoid::ellipsoid(vec3 center, vec3 u, vec3 v, vec3 w, material mat)
	:shape(mat), m_u(u), m_v(v), m_w(w) {}

ray_hit ellipsoid::ray_intersect(const ray & r) const
{
	return {};
}
