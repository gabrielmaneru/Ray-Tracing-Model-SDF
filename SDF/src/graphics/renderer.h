/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	renderer.h
Purpose: Renderer Manager
Author: Gabriel Mañeru - gabriel.m
- End Header --------------------------------------------------------*/
#pragma once
struct camera;
class shader_program;
class c_renderer
{
	shader_program* m_sdf_shader{nullptr};
	camera* m_camera{nullptr};

public:
	bool initialize();
	void draw();
	void shutdown();

};

extern c_renderer* renderer;