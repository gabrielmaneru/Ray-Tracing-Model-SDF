#pragma once
#include <glm\glm.h>

struct camera
{
	camera(vec3 origin, vec3 u_vector, vec3 v_vector, float focal_length);
	
	vec3 m_eye;
	vec3 m_origin;
	vec3 m_u_vector;
	vec3 m_v_vector;
	float m_focal_length;
};