#include "tracer.h"
#include <scene/scene.h>
#include <utils/math_utils.h>

tracer::tracer(const ray & r, const ray_hit & r_hit, const shape_data * shp_data)
	:m_ray{r},m_rayhit{r_hit},m_shape{shp_data->m_shape},m_mat{shp_data->m_mat}
{ precompute_data(); }

void tracer::precompute_data()
{
	// Compute intersection data
	m_distance = glm::length(m_ray.m_direction) * m_rayhit.m_time;
	m_ray_dir = glm::normalize(m_ray.m_direction);
	m_pi = m_ray.get_point(m_rayhit.m_time);

	// Retrieve normal
	m_normal = m_shape->get_normal(m_ray, m_rayhit, m_pi);
	if(glm::dot(m_normal, m_ray_dir) > 0.0f)
		m_normal = -m_normal;

	// Compute biased intersection points
	const vec3 ep_N = scene->m_epsilon * m_normal;
	m_pi_external = m_pi + ep_N;
	m_pi_internal = m_pi - ep_N;

	// Store medium data
	if (m_ray.in_air)
	{
		n_ratio = m_mat.m_inv_refractive_index;
		u_ratio = m_mat.m_inv_magnetic_permeability;
		m_attenuation = scene->m_air_attenuation;
	}
	else
	{
		n_ratio = m_mat.m_refractive_index;
		u_ratio = m_mat.m_magnetic_permeability;
		m_attenuation = m_mat.m_attenuation;
	}

	// Compute vector to camera
	m_view_vec = glm::normalize(scene->m_camera->m_eye - m_pi);

	// Compute scattering using fresnel equations
	float reflection_coeff{1.f};
	if (n_ratio > 0.01f)
	{
		const float cosI = glm::dot(-m_ray_dir, m_normal);
		const float sinI = 1 - cosI * cosI;
		const float in_root = 1.0f - n_ratio * n_ratio * sinI;
		if (in_root > 0.f)
		{
			const float root = glm::sqrt(in_root);
			const float E_perp_ratio = (n_ratio*cosI - u_ratio * root) / (n_ratio*cosI + u_ratio * root);
			const float E_para_ratio = (u_ratio*cosI - n_ratio * root) / (u_ratio*cosI + n_ratio * root);
			reflection_coeff = .5f*(E_perp_ratio*E_perp_ratio + E_para_ratio * E_para_ratio);
		}
	}
	const float transmission_coeff = 1.0f - reflection_coeff;
	m_reflection_loss = reflection_coeff * m_mat.m_specular_reflection;
	m_transmission_loss = transmission_coeff * m_mat.m_specular_reflection;
	m_absortion = 1 - m_reflection_loss - m_transmission_loss;
}

vec3 tracer::compute_local_illumination() const
{
	// Check if no light is absorbed
	if (m_absortion == 0.0f)
		return {};

	vec3 diffuse_intensity{ scene->m_ambient };
	vec3 specular_intensity{};

	// Add light influences
	for (const light& light : scene->m_lights)
	{
		// Precompute light data
		vec3 light_vec = light.m_position - m_pi;
		const float light_distance_2 = glm::length2(light_vec);

		// Compute shadow factor
		int hit_count = 0;
		int shad_samples = scene->m_shadow_samples;
		for (int i = 0; i < scene->m_shadow_samples; i++)
		{
			// Compute the shador ray
			const vec3 offset = (i == 0) ? glm::zero<vec3>() : rand_ball(light.m_radius);
			ray r_shad{ m_pi_external, light_vec + offset };

			// Intersect the scene
			auto result = scene->ray_intersect(r_shad);

			// Check if nothing is blocking the light segment
			if (!result.first.m_has_hit)
				++hit_count;
			else
			{
				const float hit_distance_2 = glm::length2(r_shad.get_point(result.first.m_time) - m_pi);
				if (light_distance_2 < hit_distance_2)
					++hit_count;
			}
		}
		const float shadow_factor = (shad_samples == 0) ? 1.0f : hit_count / (float)shad_samples;

		// Compute diffuse and specular intensities
		light_vec /= glm::sqrt(light_distance_2);
		const vec3 light_intensity = light.m_intensity * shadow_factor;
		diffuse_intensity += light_intensity * glm::max(glm::dot(m_normal, light_vec), 0.0f);

		const vec3 reflect_vec = glm::reflect(-light_vec, m_normal);
		specular_intensity += light_intensity * glm::max(glm::pow(glm::dot(reflect_vec, m_view_vec), m_mat.m_specular_exponent), 0.0f);
	}

	// Apply material properties
	diffuse_intensity *= m_mat.m_diffuse_color;
	specular_intensity *= m_mat.m_specular_reflection;

	// Apply absortion
	return (diffuse_intensity + specular_intensity) * m_absortion;
}

vec3 tracer::compute_reflection_value() const
{
	// Check if there is a reflection
	const int reflection_samples{ scene->m_rough_reflection_samples };
	if (m_reflection_loss == 0.0f || reflection_samples <= 0)
		return {};

	// Compute reflection 
	const vec3 reflect_vec = glm::reflect(m_ray_dir, m_normal);
	const int tot_samples = (m_mat.m_roughness == 0.f) ? 1 : reflection_samples;

	vec3 reflect_value{};
	for (int i = 0; i < tot_samples; i++)
	{
		const vec3 offset = (i == 0) ? glm::zero<vec3>() : rand_ball(m_mat.m_roughness);
		ray r_refl{ m_pi_external, reflect_vec + offset };
		r_refl.m_depth = m_ray.m_depth + 1;
		r_refl.in_air = m_ray.in_air;
		reflect_value += scene->raytrace(r_refl);
	}
	return reflect_value * (m_reflection_loss / tot_samples);
}

vec3 tracer::compute_transmission_value() const
{
	// Check if there is a transmission
	if (m_transmission_loss == 0.0f)
		return {};

	const vec3 refr_vec = glm::refract(m_ray_dir, m_normal, n_ratio);
	const ray r_refr{ m_pi_internal, refr_vec, m_ray.m_depth + 1, !m_ray.in_air };
	const vec3 transmitted_value = scene->raytrace(r_refr);
	return transmitted_value * m_transmission_loss;
}

vec3 tracer::compute_attenuation() const
{
	return glm::pow(m_attenuation, vec3{ m_distance });
}
