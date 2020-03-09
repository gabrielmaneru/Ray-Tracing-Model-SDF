#pragma once
#include "shape.h"
#include "polygon.h"

struct mesh : public shape
{
	mesh(const std::string& path, const vec3& pos, const vec3& eu_angls, float scale, material mat);
	ray_hit ray_intersect(const ray& r)const override;

	std::vector<polygon> m_polygons;
};