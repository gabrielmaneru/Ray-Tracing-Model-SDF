#include "camera.h"

camera::camera(vec3 plane_center, vec3 u_vector, vec3 v_vector, float focal_length)
	: m_origin(plane_center), m_u_vector(u_vector), m_v_vector(v_vector), m_focal_length(focal_length) {}

vec3 camera::get_eye()
{
	vec3 to_eye = glm::normalize(glm::cross(m_u_vector, m_v_vector));
	return m_origin + to_eye * m_focal_length;
}
