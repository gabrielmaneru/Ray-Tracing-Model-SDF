/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	camera.h
Purpose: Camera Base
Author: Gabriel Ma�eru - gabriel.m
- End Header --------------------------------------------------------*/
#pragma once
#include <glm/glm.hpp>
class shader_program;
struct camera
{
	camera() { update(); }
	void update();

	float m_fov{ 65.0f };
	float m_near{ .001f };
	float m_far{ 100.0f };
	float m_yaw{ -120.0f };
	float m_pitch{ -30.0f };

	glm::vec3 m_eye{ 2,2,-1 };
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;

	glm::mat4 m_proj;
	glm::mat4 m_view;

	float m_angular_speed{ 0.2f };
	float m_linear_speed{ 0.05f };
};