#include "geometry.h"

float intersection_ray_sphere(const ray & r, const sphere & s)
{
	vec3 ray_sphere = r.m_origin - s.m_center;
	float a = glm::dot(r.m_direction, r.m_direction);
	float b = 2.0f * glm::dot(ray_sphere, r.m_direction);
	float c = glm::dot(ray_sphere, ray_sphere) - s.m_radius*s.m_radius;
	float disc = b * b - 4 * a*c;
	if (disc < 0.0f)
		return FLT_MAX;
	else
	{
		float t1 = (-b - sqrtf(disc)) / (2.0f*a);
		float t2 = (-b + sqrtf(disc)) / (2.0f*a);
		if (t1 < 0.0f)
			return (t2 < 0.0f) ? FLT_MAX : t2;
		else
			return t1;
	}
}
