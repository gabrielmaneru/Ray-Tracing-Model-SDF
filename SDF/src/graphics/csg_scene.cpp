#include "csg_scene.h"
#include <fstream>
#include <map>

csg_scene::csg_scene(std::string filename)
{
	const char * path = "../resources/scenes/";
	std::string filepath = path + filename;
	std::ifstream file;
	file.open(filepath);
	if (!file.is_open())
		return;

	std::string line;
	while (std::getline(file,line))
	{
		if (line.size() == 0U || line[0] == '#')
			continue;

		if (line == "END_DRAWLIST")
		{
			draw_size = static_cast<int>(nodes.size());
			continue;
		}

		size_t s = line.find("OP=");
		if (s < line.size())
		{
			csg_node node{};

			line = line.substr(s+3);
			s = line.find(' ');
			node.m_operation = std::atoi(line.substr(0, s).c_str());
			
			
			s = line.find("DATA=");
			line = line.substr(s + 5);
			for(int i = 0; i < 3; ++i)
			{
				if (line.empty()) break;
				s = line.find(',');
				node.m_data1[i]= std::atof(line.substr(0, s).c_str());
				if (s < line.size())
					line = line.substr(s + 1);
				else
					line = "";
			}
			for (int i = 0; i < 3; ++i)
			{
				if (line.empty()) break;
				s = line.find(',');
				node.m_data2[i] = std::atof(line.substr(0, s).c_str());
				if (s < line.size())
					line = line.substr(s + 1);
				else
					line = "";
			}
			nodes.push_back(node);
			continue;
		}

		s = line.find("LIGHT=");
		if (s < line.size())
		{
			line = line.substr(s + 6);

			s = line.find(',');
			m_light_pos.x = std::atof(line.substr(0, s).c_str());
			line = line.substr(s + 1);

			s = line.find(',');
			m_light_pos.y = std::atof(line.substr(0, s).c_str());
			line = line.substr(s + 1);

			s = line.find(',');
			m_light_pos.z = std::atof(line.substr(0, s).c_str());
			line = line.substr(s + 1);

			s = line.find(',');
			m_light_rad = std::atof(line.substr(0, s).c_str());
			continue;
		}
	}

	file.close();
}
