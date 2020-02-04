#pragma once
#include "shape.h"
#include <array>

struct box : public shape
{
	box(vec3 corner, vec3 length, vec3 width, vec3 height, material mat);
	ray_hit ray_intersect(const ray& r)const override;

	std::array<vec3, 6u> m_points;
	std::array<vec3, 6u> m_normals;
};