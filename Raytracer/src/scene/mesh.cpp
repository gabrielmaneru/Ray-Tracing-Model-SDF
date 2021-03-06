#include "mesh.h"
#include <string> 
#include <fstream>

void load_obj(const std::string& path, std::vector<vec3>& vertices, std::vector<std::vector<unsigned> >& faces);

mesh::mesh(const std::string & path, const vec3 & pos, const vec3 & eu_angles, float scale)
{
	std::vector<vec3> vertices;
	std::vector<std::vector<unsigned> > faces;
	load_obj(path, vertices, faces);

	glm::mat4 tr = glm::translate(glm::mat4(1.0f), pos);
	tr = glm::rotate(tr, glm::radians(eu_angles.x), vec3{ 1.f,0.f,0.f });
	tr = glm::rotate(tr, glm::radians(eu_angles.y), vec3{ 0.f,1.f,0.f });
	tr = glm::rotate(tr, glm::radians(eu_angles.z), vec3{ 0.f,0.f,1.f });
	tr = glm::scale(tr, vec3(scale));

	for (auto& f : faces)
	{
		std::vector<vec3> poly_vtx;
		for (unsigned i : f)
			poly_vtx.push_back(vec3(tr * vec4(vertices[i], 1.0f)));

		m_polygons.push_back({ poly_vtx });
	}
}

ray_hit mesh::ray_intersect(const ray & r) const
{
	ray_hit best;
	const polygon* best_p{ nullptr };
	for (auto& p : m_polygons)
	{
		ray_hit local = p.ray_intersect(r);
		if (local.m_has_hit && (!best.m_has_hit || (local.m_time < best.m_time)))
			best = local, best_p = &p;
	}
	if (best.m_has_hit)
		best.m_temp_normal = best_p->get_normal(r, {}, {});
	return best;
}

void load_obj(const std::string& path, std::vector<vec3>& vertices, std::vector<std::vector<unsigned> >& faces)
{
	std::ifstream file;
	file.open(path);
	assert(file.is_open());
	std::string line;
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
}

box compute_aabb(const std::vector<polygon>& polys)
{
	glm::vec3 min{ polys[0].m_vertices[0] };
	glm::vec3 max{ polys[0].m_vertices[0] };

	for (auto& p : polys)
		for (auto&v : p.m_vertices)
			min = glm::min(min, v),
			max = glm::max(max, v);

	glm::vec3 diff = max - min;
	return box{ min, {diff.x,0.0f,0.0f}, {0.0f, diff.y, 0.0f},{0.0f, 0.0f, diff.z} };
}

bounded_mesh::bounded_mesh(const std::string & path, const vec3 & pos, const vec3 & eu_angles, float scale)
	:mesh(path, pos, eu_angles, scale), m_bounding_box(compute_aabb(m_polygons)) {}

ray_hit bounded_mesh::ray_intersect(const ray & r) const
{
	ray_hit aabb_hit = m_bounding_box.ray_intersect(r);
	if (!aabb_hit.m_has_hit)
		return {};
	return mesh::ray_intersect(r);
}
