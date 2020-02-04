#pragma once
#include "shape.h"

struct sphere : public shape
{
	sphere(vec3 center, float radius, material mat);
	ray_hit ray_intersect(const ray& r)const override;

	vec3 m_center;
	float m_radius;
};