#pragma once
#include "shape.h"
#include <vector>

struct polygon : public shape
{
	polygon(const std::vector<vec3>& vertices, material mat);
	ray_hit ray_intersect(const ray& r)const override;
	ray_hit ray_intersect_plane(const ray& r)const;

	const std::vector<vec3> m_vertices;
	vec3 m_normal;
};