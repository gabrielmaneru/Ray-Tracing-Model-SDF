#include "mesh.h"
#include <string> 
#include <fstream>

mesh::mesh(const std::string & path, const vec3 & pos, const vec3 & eu_angles, float scale, material mat)
	:shape(mat)
{
	std::ifstream file;
	file.open(path);
	assert(file.is_open());
	std::string line;
	std::vector<vec3> vertices;
	std::vector<std::vector<unsigned> > faces;
	while (std::getline(file, line))
	{
		if (line.size() == 0U)
			continue;
		if (line.rfind("v ") == 0)
		{
			vertices.push_back({});

			size_t s = 0;
			while (line[s] == 'v' || line[s] == ' ') { s++; }
			line = line.substr(s);

			s = line.find(' ');
			vertices.back()[0] = static_cast<float>(std::atof(line.substr(0, s).c_str()));
			while (line[s] == ' ') { s++; }
			line = line.substr(s);

			s = line.find(' ');
			vertices.back()[1] = static_cast<float>(std::atof(line.substr(0, s).c_str()));
			while (line[s] == ' ') { s++; }
			line = line.substr(s);

			vertices.back()[2] = static_cast<float>(std::atof(line.c_str()));
		}
		else if (line.rfind("f ") == 0)
		{
			std::vector<unsigned> f;
			size_t s = 0;
			while (line[s] == 'f' || line[s] == ' ') { s++; }
			line = line.substr(s);

			while (line.size() > 0)
			{
				size_t b = line.find('/');
				s = line.find(' ');
				if (s > line.length()) s = line.length();
				if (b > s) b = s;

				int i = std::atoi(line.substr(0, b).c_str()) - 1;
				f.push_back(static_cast<unsigned>(i));
				while (line[s] == ' ') { s++; }
				line = line.substr(s);
			}
			faces.push_back(f);
		}
	}
	file.close();

	glm::mat4 tr = glm::translate(glm::mat4(1.0f), pos)
		* glm::mat4_cast(quat(eu_angles))
		* glm::scale(glm::mat4(1.0f), vec3(scale));

	for (auto& f : faces)
	{
		std::vector<vec3> poly_vtx;
		for (unsigned i : f)
			poly_vtx.push_back(vec3(tr * vec4(vertices[i], 1.0f)));

		m_polygons.push_back({ poly_vtx, mat });
	}
}

ray_hit mesh::ray_intersect(const ray & r) const
{
	ray_hit best;
	for (auto& p : m_polygons)
	{
		ray_hit local = p.ray_intersect(r);
		if (!best.m_hit || (local.m_hit && local.m_time < best.m_time) )
			best = local;
	}
	return best;
}