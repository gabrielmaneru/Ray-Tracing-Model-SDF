#pragma once
#include "shape.h"

struct ellipsoid : public shape
{
	ellipsoid(vec3 center, vec3 u, vec3 v, vec3 w, material mat);
	ray_hit ray_intersect(const ray& r)const override;

	vec3 m_center;
	mat3 m_matrix;
};