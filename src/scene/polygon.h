#pragma once
#include "shape.h"
#include <vector>

struct plane
{
	plane(const std::vector<vec3>& vertices);
	ray_hit ray_intersect(const ray& r)const;
	vec2 project_dominant(vec3 p)const;

	vec3 m_point;
	vec3 m_normal;
	enum { e_xAxis, e_yAxis, e_zAxis } m_dominant;
};

struct polygon : public shape
{
	polygon(const std::vector<vec3>& vertices);
	ray_hit ray_intersect(const ray& r)const override;
	vec3 get_normal(const ray& r, const ray_hit& hit, const vec3& pi)const override;

	std::vector<vec3> m_vertices;
	plane m_plane;
};