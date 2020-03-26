/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	camera.cpp
Purpose: Camera implementation
Author: Gabriel Mañeru - gabriel.m
- End Header --------------------------------------------------------*/
#include "camera.h"

// Constructor
camera::camera(vec3 plane_center, vec3 u_vector, vec3 v_vector, float focal_length)
	: m_origin(plane_center), m_u_vector(u_vector), m_v_vector(v_vector), m_focal_length(focal_length)
{
	vec3 to_eye = glm::normalize(glm::cross(m_u_vector, m_v_vector));
	m_eye = m_origin + to_eye * m_focal_length;
}
