#include "box.h"

box::box(vec3 corner, vec3 length, vec3 width, vec3 height, material mat)
	: shape(mat)
{
	m_points [0] = corner;
	m_points [1] = corner + width;
	m_points [2] = corner;
	m_points [3] = corner + length;
	m_points [4] = corner;
	m_points [5] = corner + height;

	m_normals[0] = glm::normalize(glm::cross(length,height));
	m_normals[1] = -m_normals[0];
	m_normals[2] = glm::normalize(glm::cross(height, width));
	m_normals[3] = -m_normals[2];
	m_normals[4] = glm::normalize(glm::cross(width, length));
	m_normals[5] = -m_normals[4];
}

ray_hit box::ray_intersect(const ray & r) const
{
	ray_hit enter;
	enter.m_time = 0.0f;
	ray_hit exit;

	for (size_t i = 0; i < 6; i++)
	{
		float d_dir_nor = glm::dot(r.m_direction, m_normals[i]);
		if (d_dir_nor == 0.0f)
			return {};

		float ti = -glm::dot(r.m_origin - m_points[i], m_normals[i]) / d_dir_nor;

		if (d_dir_nor < 0.0f)
		{
			if (ti > enter.m_time)
			{
				enter.m_time = ti;
				enter.m_hit |= true;
				enter.m_normal = m_normals[i];
			}
		}
		else
		{
			if(ti < exit.m_time)
			{
				exit.m_time = ti;
				exit.m_hit |= true;
				exit.m_normal = m_normals[i];
			}
		}
	}

	if (exit.m_time < enter.m_time)
		return{};

	ray_hit final = (enter.m_time > 0.0f) ? enter : exit;
	final.m_point = r.get_point(final.m_time);
	return final;
}
