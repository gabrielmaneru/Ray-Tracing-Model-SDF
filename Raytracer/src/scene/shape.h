#pragma once
#include <raytracer/ray.h>
#include <glm/glm.h>

struct material
{
	vec3 m_diffuse_color;
	float m_specular_reflection;
	float m_specular_exponent;
	vec3 m_attenuation;
	float m_roughness;
	float m_magnetic_permeability;
	float m_inv_magnetic_permeability;
	float m_refractive_index;
	float m_inv_refractive_index;
};
struct ray_hit
{
	bool m_has_hit{ false };
	float m_time{FLT_MAX};
	vec3 m_temp_normal;
};

struct shape
{
	virtual ray_hit ray_intersect(const ray& r)const = 0;
	virtual vec3 get_normal(const ray& r, const ray_hit& hit, const vec3& pi)const
	{
		return hit.m_temp_normal;
	}
};
struct shape_data
{
	shape* m_shape;
	material m_mat;
};