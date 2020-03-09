#pragma once
#include "shape.h"

struct sphere : public shape
{
	sphere(vec3 center, float radius);
	ray_hit ray_intersect(const ray& r)const override;
	vec3 get_normal(const ray& r, const ray_hit& hit, const vec3& pi)const override;

	vec3 m_center;
	float m_radius;
};