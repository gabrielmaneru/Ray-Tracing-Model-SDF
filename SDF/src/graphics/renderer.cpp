/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	renderer.cpp
Purpose: Renderer Manager
Author: Gabriel Mañeru - gabriel.m
- End Header --------------------------------------------------------*/
#include "renderer.h"
#include "shader_program.h"
#include "camera.h"
#include "gl_error.h"
#include "csg_scene.h"
#include <platform/input.h>
#include <platform/editor.h>
#include <GLFW/glfw3.h>
#include <GL/gl3w.h>
#include <iostream>

c_renderer* renderer = new c_renderer;

void load_quad()
{
	float vertices[] = {
					-1.f, -1.f,
					 3.f, -1.f,
					-1.f,  3.f,
	};
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

bool c_renderer::initialize()
{
	// Initialize GLAD
	if (gl3wInit()) return false;
	if (!gl3wIsSupported(3,3)) return false;
	//setup_gl_debug();

	load_quad();

	// Create shaders
	try {
		m_sdf_shader = new shader_program("sdf.vert", "sdf.frag");
	}
	catch (const std::string & log) { std::cout << log; }

	// Create Camera
	m_camera = new camera{};

	// Create Scenes
	m_scene = new csg_scene("Scene_1.txt");

	return true;
}

void c_renderer::draw()
{
	if (input->is_key_down(GLFW_KEY_LEFT_CONTROL) && input->is_key_triggered(GLFW_KEY_R))
	{
		m_sdf_shader->recompile();
		delete m_scene;
		m_scene = new csg_scene("Scene_1.txt");
		return;
	}

	m_camera->update();
	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	if (m_sdf_shader->is_valid())
	{
		m_sdf_shader->use();
		m_sdf_shader->set_uniform("invP", glm::inverse(m_camera->m_proj));
		m_sdf_shader->set_uniform("invV", glm::inverse(m_camera->m_view));
		m_sdf_shader->set_uniform("eye", m_camera->m_eye);
		for (int i = 0; i < m_scene->nodes.size(); ++i)
		{
			std::string s= "scene_data[";
			s += std::to_string(i);
			m_sdf_shader->set_uniform((s + "].operation").c_str(), m_scene->nodes[i].m_operation);
			m_sdf_shader->set_uniform((s + "].data1").c_str(), m_scene->nodes[i].m_data1);
			m_sdf_shader->set_uniform((s + "].data2").c_str(), m_scene->nodes[i].m_data2);

		}
		m_sdf_shader->set_uniform("draw_list", m_scene->draw_size);
		m_sdf_shader->set_uniform("light_pos", m_scene->m_light_pos);
		m_sdf_shader->set_uniform("light_rad", m_scene->m_light_rad);
		editor->set_uniform(m_sdf_shader);
		
		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 3));
	}
	
}

void c_renderer::shutdown()
{
}
