/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	scene.cpp
Purpose: Raytracing scene implementation
Author: Gabriel Ma�eru - gabriel.m
- End Header --------------------------------------------------------*/
#include "scene.h"
#include "sphere.h"
#include "box.h"
#include "polygon.h"
#include "ellipsoid.h"
#include "mesh.h"
#include <string>
#include <fstream>
#include <functional>
#include <utils/math_utils.h>

c_scene* scene = new c_scene;

vec3 parse_vec3(std::string& line);
float parse_flt(std::string& line);
material parse_mat(std::string& line);

bool c_scene::init(std::string scene_path)
{
	// Read scene file
	std::ifstream file;
	file.open(scene_path);
	if (file.is_open())
	{
		// Store path
		m_scene_path = scene_path;

		// Loop each line
		std::string line;
		while (std::getline(file, line))
		{
			if (line.size() == 0U || line[0] == '#')
				continue;

			if (line.substr(0, 7) == "SPHERE ")
			{
				vec3 center = parse_vec3(line);
				float radius = parse_flt(line);

				std::getline(file, line);
				material mat = parse_mat(line);

				m_shapes.push_back({ new sphere{ center, radius}, mat });
			}
			else if (line.substr(0, 4) == "BOX ")
			{
				vec3 corner = parse_vec3(line);
				vec3 length = parse_vec3(line);
				vec3 width = parse_vec3(line);
				vec3 height = parse_vec3(line);

				std::getline(file, line);
				material mat = parse_mat(line);

				m_shapes.push_back({ new box{ corner, length, width, height}, mat });
			}
			else if (line.substr(0, 8) == "POLYGON ")
			{
				line = line.substr(8);
				std::vector<vec3> vertices;
				int count = static_cast<int>(parse_flt(line));
				for (int i = 0; i < count; ++i)
					vertices.push_back(parse_vec3(line));

				std::getline(file, line);
				material mat = parse_mat(line);

				m_shapes.push_back({ new polygon{ vertices}, mat });
			}
			else if (line.substr(0, 10) == "ELLIPSOID ")
			{
				vec3 center = parse_vec3(line);
				vec3 u = parse_vec3(line);
				vec3 v = parse_vec3(line);
				vec3 w = parse_vec3(line);
				std::vector<vec3> vertices;
				int count = static_cast<int>(parse_flt(line));
				for (int i = 0; i < count; ++i)
					vertices.push_back(parse_vec3(line));

				std::getline(file, line);
				material mat = parse_mat(line);

				m_shapes.push_back({ new ellipsoid{ center, u, v, w}, mat });
			}
			else if (line.substr(0, 5) == "MESH ")
			{
				std::string path = line.substr(5);

				std::getline(file, line);
				vec3 pos = parse_vec3(line);
				vec3 eu_angles = parse_vec3(line);
				float scale = parse_flt(line);

				std::getline(file, line);
				material mat = parse_mat(line);

				m_shapes.push_back({ new bounded_mesh{ path, pos, eu_angles, scale}, mat });
			}
			else if (line.substr(0, 6) == "LIGHT ")
			{
				vec3 pos = parse_vec3(line);
				vec3 intensity = parse_vec3(line);
				float radius = parse_flt(line);
				m_lights.push_back(light{ pos, intensity, radius });
			}
			else if (line.substr(0, 8) == "AMBIENT ")
				m_ambient = parse_vec3(line);
			else if (line.substr(0, 4) == "AIR ")
			{
				m_air.m_electric_permittivity = parse_flt(line);
				m_air.m_magnetic_permeability = parse_flt(line);
				m_air.m_attenuation = parse_vec3(line);
			}
			else if (line.substr(0, 7) == "CAMERA ")
			{
				vec3 origin = parse_vec3(line);
				vec3 u_vector = parse_vec3(line);
				vec3 v_vector = parse_vec3(line);
				float focal_length = parse_flt(line);
				m_camera = new camera{ origin, u_vector, v_vector, focal_length };
			}
		}
		assert(m_camera != nullptr);
		file.close();
	}

	// Read config file
	file.open(".config");
	if (file.is_open())
	{
		// Loop each light
		std::string line;
		while (std::getline(file, line))
		{
			if (line.size() == 0U || line[0] == '#')
				continue;

			if (line.substr(0, 14) == "EPSILON_VALUE ")
			{
				line = line.substr(14);
				m_epsilon = parse_flt(line);
			}

			else if (line.substr(0, 15) == "SHADOW_SAMPLES ")
			{
				line = line.substr(15);
				m_shadow_samples = (int)parse_flt(line);
			}

			else if (line.substr(0, 16) == "RECURSION_DEPTH ")
			{
				line = line.substr(16);
				m_recursion_depth = (int)parse_flt(line);
			}

			else if (line.substr(0, 25) == "ROUGH_REFLECTION_SAMPLES ")
			{
				line = line.substr(25);
				m_rough_reflection_samples = (int)parse_flt(line);
			}

			else if (line.substr(0, 19) == "AA_SECTION_SAMPLES ")
			{
				line = line.substr(19);
				m_aa_section_samples = (int)parse_flt(line);
			}

			else if (line.substr(0, 19) == "AA_RECURSION_DEPTH ")
			{
				line = line.substr(19);
				m_aa_recursion_depth = (int)parse_flt(line);
			}
		}
	}
	return true;
}

vec3 c_scene::raytrace_pixel(const vec3 & px_center, const vec3 & px_width, const vec3 & px_height, const vec3 & eye) const
{
	if (m_aa_recursion_depth == 1) // Non-Adaptive
	{
		const int samples = m_aa_section_samples;
		float step = 1.0f / samples;
		vec3 color = glm::zero<vec3>();

		float s_x = 0.5f * step - 0.5f;
		for (int x = 0; x < samples; ++x, s_x += step)
		{
			float s_y = 0.5f * step - 0.5f;
			for (int y = 0; y < samples; ++y, s_y += step)
			{
				const vec3 new_target = px_center + s_x * px_width - s_y * px_height;
				color += scene->raytrace({ eye, new_target - eye });
			}
		}
		return color / static_cast<float>(samples*samples);
	}
	else
	{

		std::function<vec3(const vec3&, const vec3&,const vec3&,const int depth)>  adaptive_4x4 = [&](const vec3 & sec_center, const vec3 & sec_width, const vec3 & sec_height, const int depth)->vec3
		{
			vec3 center[]{
				px_center - .25f * px_width + .25f * px_height,
				px_center - .25f * px_width - .25f * px_height,
				px_center + .25f * px_width + .25f * px_height,
				px_center + .25f * px_width - .25f * px_height
			};

			vec3 color[4];
			for(int i=0; i<4;++i)
				color[i]= scene->raytrace({ eye, center[i] - eye });

			vec3 av_color = (color[0] + color[1] + color[2] + color[3])*.25f;

			if (depth == m_aa_recursion_depth)
				return av_color;

			vec4 diffs;
			for (int i = 0; i < 4; ++i)
			{
				float diff = glm::length2(av_color - color[i]);
				if(diff > 0.05f)
					color[i] = adaptive_4x4(center[i], sec_width*0.5f, sec_height*0.5f,depth+1);
			}
			return (color[0] + color[1] + color[2] + color[3])*.25f;

		};
		return adaptive_4x4(px_center, px_width, px_height,1);
	}
}


float compute_reflectance(const float n_ratio, const float u_ratio, const float cosI)
{
	float E_perp_ratio, E_para_ratio;
	if (n_ratio > 0.01f)
	{
		const float root = glm::sqrt(1.0f - n_ratio * n_ratio*(1 - cosI * cosI));
		E_perp_ratio = (n_ratio*cosI - u_ratio * root) / (n_ratio*cosI + u_ratio * root);
		E_para_ratio = (u_ratio*cosI - n_ratio * root) / (u_ratio*cosI + n_ratio * root);
	}
	else
		E_perp_ratio = -1.f, E_para_ratio = 1.f;

	return .5f*(E_perp_ratio*E_perp_ratio + E_para_ratio * E_para_ratio);
}

vec3 c_scene::raytrace(const ray & r) const
{
	// Check if reached maximum reflection depth
	if (r.m_depth > m_recursion_depth)
		return glm::zero<vec3>();

	// Raycast the scene
	ray_hit hit; const shape_data* shape{ nullptr };
	for (auto& s : m_shapes)
	{
		ray_hit local = s.m_shape->ray_intersect(r);
		if (local.m_has_hit && local.m_time < hit.m_time)
		{
			hit = local;
			shape = &s;
		}
	}

	// Check if not hitted anything
	if (!hit.m_has_hit)
		return glm::zero<vec3>();

	// Extract raycast data
	const float distance = glm::length(r.m_direction) * hit.m_time;
	const vec3 ray_dir = glm::normalize(r.m_direction);
	const vec3 pi = r.get_point(hit.m_time);
	vec3 normal = shape->m_shape->get_normal(r, hit, pi);
	if (glm::dot(normal, ray_dir) > 0.0f)
		normal = -normal;
	const vec3 pi_external = pi + m_epsilon * normal;

	// Extract medium data
	const float n_ratio = r.in_air ? shape->m_mat.m_inv_refractive_index : shape->m_mat.m_refractive_index;
	const float u_ratio = r.in_air ? shape->m_mat.m_inv_magnetic_permeability : shape->m_mat.m_magnetic_permeability;
	const vec3 attenuation = r.in_air ? m_air.m_attenuation : shape->m_mat.m_attenuation;

	// Compute local illumination model
	const vec3 view_vec = glm::normalize(m_camera->get_eye() - pi);
	vec3 diffuse_intensity = m_ambient;
	vec3 specular_intensity = glm::zero<vec3>();
	for (const light& light : m_lights)
	{
		// Precompute light data
		vec3 light_vec = light.m_position - pi;
		const float light_distance_2 = glm::length2(light_vec);

		// Compute shadow factor
		int hit_count = 0;
		for (int i = 0; i < m_shadow_samples; i++)
		{
			const vec3 offset = (i == 0) ? glm::zero<vec3>() : rand_ball(light.m_radius);
			ray r_shad{ pi_external, light_vec + offset };
			bool hitted{ false };
			for (auto& s : m_shapes)
			{
				const ray_hit local = s.m_shape->ray_intersect(r_shad);
				if (local.m_has_hit)
				{
					const float local_distance_2 = glm::length2(r_shad.get_point(local.m_time) - pi);
					if (local_distance_2 < light_distance_2)
					{

						hitted = true;
						break;
					}
				}
			}
			if (!hitted)
				++hit_count;
		}
		const float shadow_factor = (m_shadow_samples == 0) ? 1.0f : hit_count / (float)m_shadow_samples;

		// Compute diffuse and specular intensities
		light_vec /= glm::sqrt(light_distance_2);
		const vec3 reflect_vec = glm::reflect(-light_vec, normal);
		const vec3 light_intensity = light.m_intensity * shadow_factor;
		diffuse_intensity  += light_intensity * glm::max(glm::dot(normal, light_vec), 0.0f);
		specular_intensity += light_intensity * glm::max(glm::pow(glm::dot(reflect_vec, view_vec), shape->m_mat.m_specular_exponent), 0.0f);
	}
	diffuse_intensity *= shape->m_mat.m_diffuse_color;
	specular_intensity *= shape->m_mat.m_specular_reflection;
	vec3 color = diffuse_intensity + specular_intensity;

	// Get reflection/transmission data
	const float cosI = glm::dot(-ray_dir, normal);
	const vec3 refr_vec = glm::refract(ray_dir, normal, n_ratio);
	float reflection_coeff = glm::length2(refr_vec) == 0.0f ? 1.0f : compute_reflectance(n_ratio, u_ratio, cosI);
	const float transmission_coeff = 1.0f - reflection_coeff;
	const float reflection_loss = reflection_coeff * shape->m_mat.m_specular_reflection;
	const float transmission_loss = transmission_coeff * shape->m_mat.m_specular_reflection;
	const float absortion = 1 - reflection_loss - transmission_loss;
	color *= absortion;

	// Add reflection value
	if (reflection_loss != 0.0f && m_rough_reflection_samples > 0)
	{
		const vec3 reflect_vec = glm::reflect(ray_dir, normal);
		const int tot_samples = (shape->m_mat.m_roughness == 0.0f) ? 1 : m_rough_reflection_samples;
		vec3 reflect_value = glm::zero<vec3>();
		for (int i = 0; i < tot_samples; i++)
		{
			const vec3 offset = (i==0)?glm::zero<vec3>():rand_ball(shape->m_mat.m_roughness);
			ray r_refl{ pi_external, reflect_vec + offset };
			r_refl.m_depth = r.m_depth + 1;
			reflect_value += raytrace(r_refl);
		}
		color += reflect_value * (reflection_loss / tot_samples);
	}

	// Add transmission value
	if (transmission_loss != 0.0f) // TODO
	{
		vec3 pi_internal = pi + m_epsilon * refr_vec;
		ray r_refr{ pi_internal, refr_vec };
		r_refr.m_depth = r.m_depth + 1;
		r_refr.in_air = !r.in_air;
		vec3 transmitted_value = raytrace(r_refr);
		color += transmitted_value * transmission_loss;
	}

	// Apply medium attenuation
	color *= glm::pow(attenuation, vec3{ distance });

	// Return final color
	return color;
}
void c_scene::shutdown()
{
	for (auto& s : m_shapes)
		delete s.m_shape;
	m_shapes.clear();
}


// Parse helpers
vec3 parse_vec3(std::string& line)
{
	vec3 v;
	line = line.substr(line.find('(') + 1);
	size_t idx = line.find(',');
	v.x = static_cast<float>(atof(line.substr(0, idx).c_str()));
	line = line.substr(idx + 1);
	idx = line.find(',');
	v.y = static_cast<float>(atof(line.substr(0, idx).c_str()));
	line = line.substr(idx + 1);
	idx = line.find(')');
	v.z = static_cast<float>(atof(line.substr(0, idx).c_str()));
	if (idx + 2 <= line.size())
		line = line.substr(idx + 2);
	else
		line = {};
	return v;
}
float parse_flt(std::string& line)
{
	float v = static_cast<float>(atof(line.c_str()));
	size_t idx = line.find(' ');
	if (idx <= line.size())
		line = line.substr(idx + 1);
	else
		line = {};
	return v;
}
material parse_mat(std::string& line)
{
	vec3 dif = parse_vec3(line);
	float refl = parse_flt(line);
	float exp = parse_flt(line);
	vec3 att = parse_vec3(line);
	float elec = parse_flt(line);
	float mag = parse_flt(line);
	float rou = parse_flt(line);
	float refr = glm::sqrt(elec*mag);
	return{ dif, refl, exp, att, rou, mag, 1.0f / mag, refr, 1.0f / refr };
}