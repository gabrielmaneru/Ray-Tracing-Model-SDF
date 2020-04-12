/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	window.cpp
Purpose: Window Manager
Author: Gabriel Mañeru - gabriel.m
- End Header --------------------------------------------------------*/
#include "window.h"
#include "input.h"
#include <core/session.h>
#include <GLFW/glfw3.h>

c_window* window = new c_window;

bool c_window::initialize()
{
	// Initialize GLFW
	if (!glfwInit()) return false;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_window = glfwCreateWindow(m_width, m_height, "Physics System", nullptr, nullptr);
	if (!m_window) return false;
	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);
	input->setup_callback(m_window);
	return true;
}
bool c_window::should_exit()
{
	return glfwWindowShouldClose(m_window);
}
void c_window::update()
{
	input->clear_triggers();
	glfwPollEvents();
	glfwGetFramebufferSize(m_window, &m_width, &m_height);
	m_width = glm::max(m_width, 1);
	m_height = glm::max(m_height, 1);
}
void c_window::present()
{
	glfwSwapBuffers(m_window);
	if (should_exit())
		session::end = true;
}
void c_window::shutdown()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}
GLFWwindow * c_window::get_handler()
{
	return m_window;
}
