#pragma once
#include <raytracer/ray.h>
#include <glm/glm.h>

struct material
{
	vec3 m_diffuse_color;
	float m_specular_reflection;
	float m_specular_exponent;
	vec3 m_attenuation;
	float m_electric_permittivity;
	float m_magnetic_permeability;
	float m_roughness;
	float m_refractive_index{ 4.0f / 3.0f };
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