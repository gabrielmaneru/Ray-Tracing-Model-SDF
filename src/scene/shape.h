#pragma once
#include <raytracer/ray.h>
#include <glm/glm.h>

struct shape
{
	shape(vec3 color) : m_color(color) {}
	virtual float ray_intersect(const ray& r)const=0;
	vec3 m_color;
};