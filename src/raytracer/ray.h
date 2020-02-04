#pragma once
#include <glm/glm.h>
#include <tuple>

struct ray
{
	vec3 m_origin;
	vec3 m_direction;
	vec3 get_point(float t)const;
};