#include "scene.h"
#include "sphere.h"
#include <string>
#include <fstream>

c_scene* scene = new c_scene;

bool c_scene::init(std::string scene_path)
{
	std::ifstream file;
	file.open(scene_path);

	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			if (line.size() == 0U || line[0] == '#')
				continue;

			if (line.substr(0, 6) == "SPHERE")
			{
				size_t idx;
				vec3 center;
				line = line.substr(line.find('(') + 1);
				idx = line.find(',');
				center.x = static_cast<float>(atof(line.substr(0, idx).c_str()));
				line = line.substr(idx + 1);
				idx = line.find(',');
				center.y = static_cast<float>(atof(line.substr(0, idx).c_str()));
				line = line.substr(idx + 1);
				idx = line.find(')');
				center.z = static_cast<float>(atof(line.substr(0, idx).c_str()));
				line = line.substr(idx + 2);
				float radius = static_cast<float>(atof(line.c_str()));

				std::getline(file, line);

				vec3 color;
				line = line.substr(line.find('(') + 1);
				idx = line.find(',');
				color.x = static_cast<float>(atof(line.substr(0, idx).c_str()));
				line = line.substr(idx + 1);
				idx = line.find(',');
				color.y = static_cast<float>(atof(line.substr(0, idx).c_str()));
				line = line.substr(idx + 1);
				idx = line.find(')');
				color.z = static_cast<float>(atof(line.substr(0, idx).c_str()));

				m_shapes.push_back(new sphere{center, radius, color});
			}
			else if (line.substr(0, 6) == "CAMERA")
			{

			}
		}
	}
	return true;
}

void c_scene::shutdown()
{
	for (auto*s : m_shapes)
		delete s;
	m_shapes.clear();
}
