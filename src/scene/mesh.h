#pragma once
#include "shape.h"
#include "polygon.h"
#include "box.h"

struct mesh : public shape
{
	mesh(const std::string& path, const vec3& pos, const vec3& eu_angls, float scale);
	ray_hit ray_intersect(const ray& r)const override;

	std::vector<polygon> m_polygons;
};

struct bounded_mesh : public mesh
{
	bounded_mesh(const std::string& path, const vec3& pos, const vec3& eu_angls, float scale);
	ray_hit ray_intersect(const ray& r)const override;

	box m_bounding_box;
};