/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	window.h
Purpose: Window Manager
Author: Gabriel Mañeru - gabriel.m
- End Header --------------------------------------------------------*/
#pragma once
#include <GLFW/glfw3.h>
#include <unordered_set>
#include <glm/glm.hpp>

struct GLFWwindow;
class c_window
{
	GLFWwindow* m_window{ nullptr };

public:
	bool initialize();
	bool should_exit();
	void update();
	void present();
	void shutdown();
	GLFWwindow* get_handler();

	int m_width{ 1920 };
	int m_height{ 1080 };
};
extern c_window* window;
