#pragma once
#include "shape.h"
#include <glm\glm.h>

struct sphere : public shape
{
	sphere(vec3 center, float radius, vec3 color);
	void ray_intersect()const override;

	vec3 m_center;
	float m_radius;
};