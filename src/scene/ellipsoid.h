#pragma once
#include "shape.h"

struct ellipsoid : public shape
{
	ellipsoid(vec3 center, vec3 u, vec3 v, vec3 w, material mat);
	ray_hit ray_intersect(const ray& r)const override;

	vec3 m_center;
	vec3 m_u;
	vec3 m_v;
	vec3 m_w;
};