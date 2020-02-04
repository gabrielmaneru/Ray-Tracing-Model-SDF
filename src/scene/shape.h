#pragma once
#include <raytracer/ray.h>
#include <glm/glm.h>

struct material
{
	vec3 m_diffuse;
	float m_spec_refl;
	float m_spec_exp;
};
struct ray_hit
{
	bool m_hit{ false };
	float m_time{FLT_MAX};
	vec3 m_point;
	vec3 m_normal;
};

struct shape
{
	shape(material m) : m_material(m) {}
	virtual ray_hit ray_intersect(const ray& r)const=0;
	material m_material;
};