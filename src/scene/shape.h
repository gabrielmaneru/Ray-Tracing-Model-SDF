#pragma once
#include <glm\glm.h>

struct shape
{
	shape(vec3 color) : m_color(color) {}
	virtual void ray_intersect()const = 0;
	vec3 m_color;
};