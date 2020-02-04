#include "sphere.h"

sphere::sphere(vec3 center, float radius, material mat)
	: shape(mat), m_center(center), m_radius(radius) {}

ray_hit sphere::ray_intersect(const ray & r) const
{
	vec3 ray_sphere = r.m_origin - m_center;
	float a = glm::dot(r.m_direction, r.m_direction);
	float b = 2.0f * glm::dot(ray_sphere, r.m_direction);
	float c = glm::dot(ray_sphere, ray_sphere) - m_radius*m_radius;
	float disc = b * b - 4 * a*c;
	if (disc < 0.0f)
		return {};
	else
	{
		ray_hit hit{ true };
		float t1 = (-b - sqrtf(disc)) / (2.0f*a);
		float t2 = (-b + sqrtf(disc)) / (2.0f*a);
		if (t1 < 0.0f)
			hit.m_time = (t2 < 0.0f) ? FLT_MAX : t2;
		else
			hit.m_time = t1;

		hit.m_point = r.get_point(hit.m_time);
		hit.m_normal = glm::normalize(hit.m_point - m_center);
		return hit;
	}
}
