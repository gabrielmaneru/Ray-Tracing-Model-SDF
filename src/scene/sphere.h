#pragma once
#include "shape.h"

struct sphere : public shape
{
	sphere(vec3 center, float radius, vec3 color);
	float ray_intersect(const ray& r)const override;

	vec3 m_center;
	float m_radius;
};