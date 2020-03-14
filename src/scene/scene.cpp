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
#include <raytracer/tracer.h>
#include <string>
#include <fstream>
#include <functional>

c_scene* scene = new c_scene;

vec3 parse_vec3(std::string& line);
float parse_flt(std::string& line);
material parse_mat(std::string& line);

std::pair<ray_hit, const shape_data*> c_scene::ray_intersect(const ray & r)const
{
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
	return { hit,shape };
}

bool c_scene::init(std::string scene_path)
{
	float air_electric{ 1.0f }, air_magnetic{ 1.0f };

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
				parse_flt(line);
				air_electric = parse_flt(line);
				air_magnetic = parse_flt(line);
				m_air_attenuation = parse_vec3(line);
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

	// Apply air properties (usually useless)
	float refr_air = glm::sqrt(air_electric*air_magnetic);
	for (auto& s : m_shapes)
	{
		s.m_mat.m_magnetic_permeability /= air_magnetic;
		s.m_mat.m_refractive_index /= refr_air;

		s.m_mat.m_inv_magnetic_permeability = 1.0f / s.m_mat.m_magnetic_permeability;
		s.m_mat.m_inv_refractive_index = 1.0f / s.m_mat.m_refractive_index;
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
		if (m_aa_section_samples == 1)
			return scene->raytrace({ eye,px_center - eye });

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

vec3 c_scene::raytrace(const ray & r) const
{
	// Check if reached maximum reflection depth
	if (r.m_depth > m_recursion_depth)
		return glm::zero<vec3>();

	// Raycast the scene
	auto result = ray_intersect(r);

	// Check if not hitted anything
	if (!result.first.m_has_hit)
		return glm::zero<vec3>();

	// Create the tracer
	const tracer tr{ r,result.first, result.second };

	// Compute the pixel color
	vec3 color = tr.compute_local_illumination();
	color     += tr.compute_reflection_value();
	color     += tr.compute_transmission_value();
	color     *= tr.compute_attenuation();
	
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
	return{ dif, refl, exp, att, rou, mag, 0.0f, refr, 0.0f };
}