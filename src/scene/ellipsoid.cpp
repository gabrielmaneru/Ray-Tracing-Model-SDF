#include "ellipsoid.h"
#include "sphere.h"

ellipsoid::ellipsoid(vec3 center, vec3 u, vec3 v, vec3 w, material mat)
	:shape(mat), m_center(center), m_u(u), m_v(v), m_w(w) {}

ray_hit ellipsoid::ray_intersect(const ray & r) const
{
	sphere s{m_center, glm::length(m_u+m_v+m_w), m_material };
	return s.ray_intersect(r);
}
