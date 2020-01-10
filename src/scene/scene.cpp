#include "scene.h"
#include "sphere.h"
#include <string>
#include <fstream>

c_scene* scene = new c_scene;

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
	return static_cast<float>(atof(line.c_str()));
}

bool c_scene::init(std::string scene_path)
{
	std::ifstream file;
	file.open(scene_path);

	if (file.is_open())
	{
		m_scene_path = scene_path;
		std::string line;
		while (std::getline(file, line))
		{
			if (line.size() == 0U || line[0] == '#')
				continue;

			if (line.substr(0, 6) == "SPHERE")
			{
				vec3 center = parse_vec3(line);
				float radius = parse_flt(line);
				std::getline(file, line);
				vec3 color = parse_vec3(line);

				m_shapes.push_back(new sphere{center, radius, color});
			}
			else if (line.substr(0, 6) == "CAMERA")
			{
				vec3 origin = parse_vec3(line);
				vec3 u_vector = parse_vec3(line);
				vec3 v_vector = parse_vec3(line);
				float focal_length = parse_flt(line);
				m_camera = new camera{ origin, u_vector, v_vector, focal_length };
			}
		}
	}
	return true;
}

vec4 c_scene::raycast(const ray & r) const
{
	float min_time{ FLT_MAX };
	vec3 color;

	for (auto& s : m_shapes)
	{
		float t = s->ray_intersect(r);
		if (t < min_time)
		{
			min_time = t;
			color = s->m_color;
		}
	}

	if (min_time < FLT_MAX)
		return vec4(color.r, color.g, color.b, 1.0f);
	return vec4{ 0.0f };
}

void c_scene::shutdown()
{
	for (auto*s : m_shapes)
		delete s;
	m_shapes.clear();
}
