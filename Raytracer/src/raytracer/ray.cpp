/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	ray.cpp
Purpose: Ray helper functions
Author: Gabriel Mañeru - gabriel.m
- End Header --------------------------------------------------------*/
#include "ray.h"

// Evaluate the ray at a certain time
vec3 ray::get_point(float t)const
{
	return m_origin + m_direction*t;
}
