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
#include <utils/math_utils.h>

c_scene* scene = new c_scene;

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
	return{ dif, refl, exp, att, elec, mag, rou };
}

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

				std::getline(file, line);
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

				m_shapes.push_back({ new mesh{ path, pos, eu_angles, scale}, mat });
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

			if (line.substr(0, 8) == "EPSILON ")
			{
				line = line.substr(8);
				m_epsilon = parse_flt(line);
			}
			else if (line.substr(0, 15) == "SHADOW_SAMPLES ")
			{
				line = line.substr(15);
				m_s_samples = (int)parse_flt(line);
			}
			else if (line.substr(0, 17) == "REFLECTION_DEPTH ")
			{
				line = line.substr(17);
				m_reflection_depth = (int)parse_flt(line);
			}
			else if (line.substr(0, 19) == "REFLECTION_SAMPLES ")
			{
				line = line.substr(19);
				m_r_samples = (int)parse_flt(line);
			}
		}
	}
	return true;
}
vec3 c_scene::raycast(const ray & r) const
{
	// Check if reached maximum reflection depth
	if (r.m_depth > m_reflection_depth)
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
	const vec3 pi = r.get_point(hit.m_time);
	const vec3 normal = shape->m_shape->get_normal(r, hit, pi);
	const vec3 pi_external = pi + m_epsilon * normal;

	// Extract medium data
	const bool air_medium = r.m_refractive_index_i == 1.0f;
	const float refractive_index_t = (air_medium) ? shape->m_mat.m_refractive_index : 1.0f;
	const vec3 attenuation = (air_medium) ? m_air.m_attenuation : shape->m_mat.m_attenuation;

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
		for (int i = 0; i < m_s_samples; i++)
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
		const float shadow_factor = (m_s_samples == 0) ? 1.0f : hit_count / (float)m_s_samples;

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
	const float reflection_coeff = 1.0f;	// TODO
	const float transmission_coeff = 0.0f;	// TODO
	const float reflection_loss = reflection_coeff * shape->m_mat.m_specular_reflection;
	const float transmission_loss = transmission_coeff * shape->m_mat.m_specular_reflection;
	const float absortion = 1 - reflection_loss - transmission_loss;
	color *= absortion;

	// Add reflection value
	if (reflection_loss != 0.0f && m_r_samples > 0)
	{
		const vec3 reflect_vec = glm::reflect(r.m_direction, normal);
		const int tot_samples = (shape->m_mat.m_roughness == 0.0f) ? 1 : m_r_samples;
		vec3 reflect_value = glm::zero<vec3>();
		for (int i = 0; i < tot_samples; i++)
		{
			const vec3 offset = (i==0)?glm::zero<vec3>():rand_ball(shape->m_mat.m_roughness);
			ray r_refl{ pi_external, reflect_vec + offset };
			r_refl.m_depth = r.m_depth + 1;
			reflect_value += raycast(r_refl);
		}
		color += reflect_value * (reflection_loss / tot_samples);
	}

	// Add transmission value
	if (transmission_loss != 0.0f) // TODO
	{
		vec3 transmitted_value = glm::zero<vec3>();
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
