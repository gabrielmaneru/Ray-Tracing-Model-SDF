#include "polygon.h"

polygon::polygon(const std::vector<vec3>& vertices, material mat)
	:shape(mat), m_vertices(vertices)
{
	assert(m_vertices.size() > 2);

	vec3 p0 = m_vertices[0];
	vec3 p1 = m_vertices[1];
	vec3 p2 = m_vertices[2];

	vec3 v0 = glm::normalize(p1 - p0);
	vec3 v1 = glm::normalize(p2 - p1);

	m_normal = glm::normalize(glm::cross(v0, v1));
}

ray_hit polygon::ray_intersect(const ray & r) const
{
	ray_hit plane_hit = ray_intersect_plane(r);
	if (plane_hit.m_hit)
	{
		vec3 p0 = m_vertices[0];
		for (size_t tri = 1u; tri < m_vertices.size() - 1u; ++tri)
		{
			vec3 p1 = m_vertices[tri];
			vec3 p2 = m_vertices[tri+1];

			vec3 v1 = p1 - p0;
			vec3 v2 = p2 - p0;

			float d_v1 = glm::dot(v1, v1);
			float d_v2 = glm::dot(v2, v2);
			float d_v1_v2 = glm::dot(v1, v2);

			// Check the affine coordinates to see if the ray
			// lies inside the triangle
			float d = d_v1 * d_v2 - d_v1_v2 * d_v1_v2;
			if (glm::abs(d) > 0.0f)
			{
				glm::vec3 p = plane_hit.m_point - p0;
				float d_p_v1 = glm::dot(v1, p);
				float d_p_v2 = glm::dot(v2, p);

				float s = (d_v2*d_p_v1 - d_v1_v2 * d_p_v2) / d;
				float t = (d_v1*d_p_v2 - d_v1_v2 * d_p_v1) / d;

				if (s + t <= 1.0f && 0.0f <= s && 0.0f <= t)
				{
					plane_hit.m_hit = true;
					return plane_hit;
				}
			}
		}
	}
	return {};
}

ray_hit polygon::ray_intersect_plane(const ray & r) const
{
	float dot_ray_normal = glm::dot(r.m_direction, m_normal);
	if (std::abs(dot_ray_normal) > 0.0f)
	{
		vec3 delta = m_vertices[0] - r.m_origin;
		float time = glm::dot(delta, m_normal) / dot_ray_normal;
		if (time >= 0.0f)
		{
			ray_hit hit{true};
			hit.m_time = time;
			hit.m_point = r.get_point(hit.m_time);
			hit.m_normal = m_normal;
			return hit;
		}
	}
	return {};
}
