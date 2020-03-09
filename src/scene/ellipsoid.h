#pragma once
#include "shape.h"

struct ellipsoid : public shape
{
	ellipsoid(vec3 center, vec3 u, vec3 v, vec3 w);
	ray_hit ray_intersect(const ray& r)const override;
	vec3 get_normal(const ray& r, const ray_hit& hit, const vec3& pi)const override;

	vec3 m_center;
	mat3 m_matrix;
	mat3 m_inv;
};