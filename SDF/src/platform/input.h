/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	input.h
Purpose: Input Manager
Author: Gabriel Mañeru - gabriel.m
- End Header --------------------------------------------------------*/
#pragma once
#include <unordered_set>

struct GLFWwindow;
struct s_input
{
	void setup_callback(GLFWwindow*)const;
	void clear_triggers();
	bool is_key_up(int key)const;
	bool is_key_down(int key)const;
	bool is_key_triggered(int key)const;
	bool is_key_released(int key)const;

	std::unordered_set<int> m_triggered_keys{};
	int m_keyboard[348]{0};
	double m_mouse_pos[2]{0.0};
	double m_mouse_offset[2]{0.0};
	bool m_mouse_pressed[2]{false};
	bool m_mouse_triggered[2]{false};
};
extern s_input* input;
