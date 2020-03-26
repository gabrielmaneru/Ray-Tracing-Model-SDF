/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	sphere.cpp
Purpose: Sphere shape
Author: Gabriel Mañeru - gabriel.m
- End Header --------------------------------------------------------*/
#include "sphere.h"

// Constructor
sphere::sphere(vec3 center, float radius)
	: m_center(center), m_radius(radius) {}

ray_hit sphere::ray_intersect(const ray & r) const
{
	vec3 ray_sphere = r.m_origin - m_center;
	float a = glm::dot(r.m_direction, r.m_direction);
	float b = 2.0f * glm::dot(ray_sphere, r.m_direction);
	float c = glm::dot(ray_sphere, ray_sphere) - m_radius*m_radius;
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

vec3 sphere::get_normal(const ray &, const ray_hit &, const vec3 & pi) const
{
	return glm::normalize(pi - m_center);
}
