#pragma once
#include <glm/glm.h>
#include <tuple>

struct ray
{
	vec3 m_origin;
	vec3 m_direction;
	std::pair<size_t,size_t> m_screen_coords;
};