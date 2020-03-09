#include "ellipsoid.h"


ellipsoid::ellipsoid(vec3 center, vec3 u, vec3 v, vec3 w)
	: m_center(center), m_matrix(u,v,w)
{
	m_inv = glm::inverse(m_matrix);
}

ray_hit ellipsoid::ray_intersect(const ray & r) const
{
	ray r_unit{ m_inv*(r.m_origin - m_center), m_inv * r.m_direction };

	float a = glm::dot(r_unit.m_direction, r_unit.m_direction);
	float b = 2.0f * glm::dot(r_unit.m_origin, r_unit.m_direction);
	float c = glm::dot(r_unit.m_origin, r_unit.m_origin) - 1.0f;
	float disc = b * b - 4 * a*c;

	// No intersection
	if (disc < 0.0f)
		return {};

	// Intersection found
	else
	{
		ray_hit hit{ true };
		float t1 = (-b - sqrtf(disc)) / (2.0f*a);
		float t2 = (-b + sqrtf(disc)) / (2.0f*a);
		if (t1 < 0.0f)
			hit.m_time = (t2 < 0.0f) ? FLT_MAX : t2;
		else
			hit.m_time = t1;
		return hit;
	}
}

vec3 ellipsoid::get_normal(const ray &, const ray_hit &, const vec3 & pi) const
{
	vec3 unit_pi = m_inv * (pi - m_center);
	return glm::normalize(glm::inverse(glm::transpose(m_matrix)) * unit_pi);
}
