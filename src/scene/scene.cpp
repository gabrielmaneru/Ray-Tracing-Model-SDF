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

float c_scene::compute_shadow_factor(vec3 pi, const light & l) const
{
	// Count occluding rays
	int count = 0;

	// For each sample
	for (int i = 0; i < m_s_samples; i++)
	{
		// Compute random ball vector
		vec3 offset{ 0.0f };
		if (i != 0)
			offset = rand_ball(l.m_radius);

		// Raytrace ray into the light bulb
		ray r{ pi, l.m_position + offset - pi };
		ray_hit hit;
		for (auto& s : m_shapes)
		{
			ray_hit local = s->ray_intersect(r);
			if (local.m_hit && local.m_time < hit.m_time)
				hit = local;
		}

		// If hit, count it
		if (!hit.m_hit)
			++count;
	}

	// Compute shadow factor
	return count / (float)m_s_samples;
}
vec3 c_scene::compute_reflect_value(const ray & refl, float roughness) const
{
	// No Roughness
	if (roughness == 0.0f)
		return raycast(refl);

	vec3 value{0.0f};
	for (int i = 0; i < m_r_samples; i++)
	{
		// Compute random ball vector
		vec3 offset{ 0.0f };
		if (i != 0)
			offset = rand_ball(roughness);

		ray new_refl{ refl };
		new_refl.m_direction += offset;
		value += raycast(new_refl);
	}

	return value / (float)m_r_samples;
}
vec3 c_scene::compute_phong_lightning(const ray & r, const ray_hit & hit, material mat) const
{
	// PI Extruded
	vec3 pi_extructed{ hit.m_point + m_epsilon * hit.m_normal };

	// View vec
	vec3 v_vec = glm::normalize(m_camera->get_eye() - hit.m_point);

	// Initialize Intensities
	vec3 I_diff{ m_ambient };
	vec3 I_spec{ 0.0f };

	// Loop each light
	for (const light& l : m_lights)
	{
		// Compute shadow factor
		float shad = compute_shadow_factor(pi_extructed, l);

		// Light vector
		vec3 l_vec = glm::normalize(l.m_position - hit.m_point);
		vec3 r_vec = glm::reflect(-l_vec, hit.m_normal);

		// Add Intensities
		I_diff += shad * glm::max(glm::dot(hit.m_normal, l_vec), 0.0f) * l.m_intensity;
		I_spec += shad * glm::max(glm::pow(glm::dot(r_vec, v_vec), mat.m_specular_exponent), 0.0f) * l.m_intensity;
	}

	// Apply material colors
	I_diff *= mat.m_diffuse_color;
	I_spec *= mat.m_specular_reflection;

	// Clamp the local color
	vec3 local = glm::clamp(I_diff + I_spec , 0.0f, 1.0f);
	if (mat.m_specular_reflection == 0.0f)
		return local;

	ray refl;
	refl.m_origin = pi_extructed;
	refl.m_direction = glm::reflect(r.m_direction, hit.m_normal);
	refl.m_depth = r.m_depth + 1;

	vec3 reflected_value = compute_reflect_value(refl, mat.m_roughness);
	return lerp(local, reflected_value, mat.m_specular_reflection);
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
	if (r.m_depth == 0 || r.m_depth <= m_reflection_depth)
	{
		// Ray intersect each shape on the scene
		ray_hit hit;
		material mat;
		for (auto& s : m_shapes)
		{
			ray_hit local = s->ray_intersect(r);
			if (local.m_hit && local.m_time < hit.m_time)
			{
				hit = local;
				mat = s->m_material;
			}
		}

		// Compute color if hit something
		if (hit.m_hit)
			return compute_phong_lightning(r, hit, mat);
	}
	return vec3{0.0f};
}
void c_scene::shutdown()
{
	for (auto*s : m_shapes)
		delete s;
	m_shapes.clear();
}
