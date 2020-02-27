/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	scene.cpp
Purpose: Raytracing scene implementation
Author: Gabriel Mañeru - gabriel.m
- End Header --------------------------------------------------------*/
#include "scene.h"
#include "sphere.h"
#include "box.h"
#include "polygon.h"
#include "ellipsoid.h"
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

				m_shapes.push_back(new sphere{ center, radius, mat });
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

				m_shapes.push_back(new box{ corner, length, width, height, mat });
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

				m_shapes.push_back(new polygon{ vertices, mat });
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

				m_shapes.push_back(new ellipsoid{ center, u, v, w, mat });
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
			else if (line.substr(0, 8) == "S_SAMPL ")
			{
				line = line.substr(8);
				m_s_samples = (int)parse_flt(line);
			}
			else if (line.substr(0, 8) == "R_DEPTH ")
			{
				line = line.substr(8);
				m_reflection_depth = (int)parse_flt(line);
			}
			else if (line.substr(0, 8) == "R_SAMPL ")
			{
				line = line.substr(8);
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
	ray_hit hit; material mat;
	for (auto& s : m_shapes)
	{
		ray_hit local = s->ray_intersect(r);
		if (local.m_hit && local.m_time < hit.m_time)
		{
			hit = local;
			mat = s->m_material;
		}
	}

	// Check if not hitted anything
	if (!hit.m_hit)
		return glm::zero<vec3>();

	// Extract raycast data
	const float distance = glm::length(r.m_direction) * hit.m_time;
	const vec3 pi = hit.m_point;
	const vec3 normal = hit.m_normal;
	const vec3 pi_external = pi + m_epsilon * normal;

	// Extract medium data
	const bool air_medium = r.m_refractive_index_i == 1.0f;
	const float refractive_index_t = (air_medium) ? mat.m_refractive_index : 1.0f;
	const vec3 attenuation = (air_medium) ? m_air.m_attenuation : mat.m_attenuation;

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
			ray r{ pi_external, light_vec + offset };
			bool hitted{ false };
			for (auto& s : m_shapes)
			{
				const ray_hit local = s->ray_intersect(r);
				const float local_distance_2 = glm::length2(local.m_point - pi);
				if (local.m_hit && local_distance_2 < light_distance_2)
				{
					hitted = true;
					break;
				}
			}
			if (!hitted)
				++hit_count;
		}
		const float shadow_factor = (m_s_samples == 0) ? 1.0f : hit_count / (float)m_s_samples;

		// Compute diffuse and specular intensities
		light_vec /= glm::sqrt(light_distance_2);
		const vec3 reflect_vec = glm::reflect(-light_vec, hit.m_normal);
		const vec3 light_intensity = light.m_intensity * shadow_factor;
		diffuse_intensity  += light_intensity * glm::max(glm::dot(hit.m_normal, light_vec), 0.0f);
		specular_intensity += light_intensity * glm::max(glm::pow(glm::dot(reflect_vec, view_vec), mat.m_specular_exponent), 0.0f);
	}
	diffuse_intensity *= mat.m_diffuse_color;
	specular_intensity *= mat.m_specular_reflection;
	vec3 color = diffuse_intensity + specular_intensity;

	// Get reflection/transmission data
	const float reflection_coeff = 1.0f;
	const float transmission_coeff = 0.0f;

	// Add reflection value
	const float reflection_factor = reflection_coeff * mat.m_specular_reflection;
	if (reflection_factor != 0.0f && m_r_samples > 0)
	{
		const vec3 reflect_vec = glm::reflect(r.m_direction, hit.m_normal);
		const int tot_samples = (mat.m_roughness == 0.0f) ? 1 : m_r_samples;
		vec3 reflect_value = glm::zero<vec3>();
		for (int i = 0; i < tot_samples; i++)
		{
			const vec3 offset = (i==0)?glm::zero<vec3>():rand_ball(mat.m_roughness);
			ray reflect{ pi_external, reflect_vec + offset };
			reflect.m_depth = r.m_depth + 1;
			reflect_value += raycast(reflect);
		}
		color += reflect_value * (reflection_factor / tot_samples);
	}

	// Add transmission value
	//const float transmission_factor = transmission_coeff * mat.m_specular_reflection;
	//if (transmission_factor != 0.0f)
	//{
	//	vec3 transmitted_value = glm::zero<vec3>();
	//	color += transmitted_value * reflection_factor;
	//}

	// Apply medium attenuation
	color *= glm::pow(attenuation, vec3{ distance });

	// Return final color
	return color;
}
void c_scene::shutdown()
{
	for (auto*s : m_shapes)
		delete s;
	m_shapes.clear();
}
