#include "ray.h"

vec3 ray::get_point(float t)const
{
	return m_origin + m_direction*t;
}
