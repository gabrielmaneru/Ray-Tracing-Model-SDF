#include "polygon.h"

plane::plane(const std::vector<vec3>& vertices)
{
	assert(vertices.size() > 2);
	
	vec3 p0 = vertices[0];
	vec3 p1 = vertices[1];
	vec3 p2 = vertices[2];

	vec3 v0 = glm::normalize(p1 - p0);
	vec3 v1 = glm::normalize(p2 - p1);

	m_point = p0;
	m_normal = glm::normalize(glm::cross(v0, v1));

	vec3 n_abs{ glm::abs(m_normal) };
	if (n_abs.x > n_abs.y)
	{
		if (n_abs.x > n_abs.z)
			m_dominant = e_xAxis;
		else
			m_dominant = e_zAxis;
	}
	else
	{
		if (n_abs.y > n_abs.z)
			m_dominant = e_yAxis;
		else
			m_dominant = e_zAxis;
	}
}
ray_hit plane::ray_intersect(const ray & r) const
{
	float dot_ray_normal = glm::dot(r.m_direction, m_normal);
	if (std::abs(dot_ray_normal) > 0.0f)
	{
		vec3 delta = m_point - r.m_origin;
		float time = glm::dot(delta, m_normal) / dot_ray_normal;
		if (time >= 0.0f)
		{
			ray_hit hit{ true };
			hit.m_time = time;
			hit.m_point = r.get_point(hit.m_time);
			hit.m_normal = (dot_ray_normal < 0.0f) ? m_normal : -m_normal;
			return hit;
		}
	}
	return {};
}
vec2 plane::project_dominant(vec3 p)const
{
	switch (m_dominant)
	{
	case plane::e_xAxis:
		return { p.y,p.z };
	case plane::e_yAxis:
		return { p.z,p.x };
	case plane::e_zAxis:
		return { p.x,p.y };
	}
}

polygon::polygon(const std::vector<vec3>& vertices)
	: m_vertices(vertices), m_plane(vertices) {}
ray_hit polygon::ray_intersect(const ray & r) const
{
	ray_hit plane_hit = m_plane.ray_intersect(r);
	if (plane_hit.m_hit)
	{
		vec2 proj_pi = m_plane.project_dominant(plane_hit.m_point);

		std::vector<vec2> proj_vertices(m_vertices.size());
		for (size_t i = 0; i < proj_vertices.size(); ++i)
			proj_vertices[i] = m_plane.project_dominant(m_vertices[i]) - proj_pi;
		proj_vertices.push_back(proj_vertices.front());
		
		int count{ 0 };
		for (size_t i = 0; i < proj_vertices.size()-1; ++i)
		{
			vec2 s = proj_vertices[i];
			vec2 e = proj_vertices[i+1];
			
			if (e.x < s.x)
				std::swap(s, e);

			if (s.x < 0.0f && e.x >= 0.0f
			&& (s.y > 0.0f || e.y >= 0.0f))
			{
				vec2 d = e - s;
				float m = d.y / d.x;
				float b = s.y - m * s.x;
				if (b >= 0.0f)
					count++;
			}
		}
		if(count%2 == 1)
			return plane_hit;
	}
	return {};
}
