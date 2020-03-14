#pragma once
#include <scene/shape.h>

struct tracer
{
	tracer(const ray& r, const ray_hit& r_hit,const shape_data* shp_data);
	void precompute_data();

	vec3 compute_local_illumination()const;
	vec3 compute_reflection_value()const;
	vec3 compute_transmission_value()const;
	vec3 compute_attenuation()const;

	// Given data
	const ray m_ray;
	const ray_hit m_rayhit;
	const shape* m_shape;
	const material m_mat;

	// Precomputed data
	float m_distance;
	vec3 m_ray_dir;
	vec3 m_pi;
	vec3 m_normal;
	vec3 m_pi_external;
	vec3 m_pi_internal;
	float n_ratio;
	float u_ratio;
	vec3 m_attenuation;
	vec3 m_view_vec;
	float m_reflection_loss;
	float m_transmission_loss;
	float m_absortion;
};