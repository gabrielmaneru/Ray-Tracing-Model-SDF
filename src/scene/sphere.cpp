#include "sphere.h"
#include <raytracer/geometry.h>

sphere::sphere(vec3 center, float radius, vec3 color)
	: shape(color), m_center(center), m_radius(radius) {}

float sphere::ray_intersect(const ray & r) const
{
	return intersection_ray_sphere(r, *this);
}
