#pragma once
#include <glm\glm.h>

struct camera
{
	camera(vec3 plane_center, vec3 u_vector, vec3 v_vector, float focal_length);

	vec3 m_plane_center;
	vec3 m_u_vector;
	vec3 m_v_vector;
	float m_focal_length;
};