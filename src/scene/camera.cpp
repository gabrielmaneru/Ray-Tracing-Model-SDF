#include "camera.h"

camera::camera(vec3 plane_center, vec3 u_vector, vec3 v_vector, float focal_length)
	: m_plane_center(plane_center), m_u_vector(u_vector), m_v_vector(v_vector), m_focal_length(focal_length) {}
