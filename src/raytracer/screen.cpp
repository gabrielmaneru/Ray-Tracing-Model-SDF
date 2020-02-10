/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	screen.cpp
Purpose: Screen implementation
Author: Gabriel Mañeru - gabriel.m
- End Header --------------------------------------------------------*/
#include "screen.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>
#include <iostream>

// Create, load & bind simple shaders and quad
void load_shaders()
{
	const char *vertexShaderSource =
	"#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"out vec2 uv;\n"
	"void main()\n"
	"{\n"
	"   uv=vec2(aPos.x,-aPos.y);\n"
	"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0"
	;
	const char *fragmentShaderSource =
	"#version 330 core\n"
	"in vec2 uv;\n"
	"out vec4 FragColor;\n"
	"uniform sampler2D textsamp;\n"
	"void main()\n"
	"{\n"
	"   FragColor = vec4(texture(textsamp,uv.xy*0.5+0.5).rgb, 1.0f);\n"
	"}\n\0"
	;

	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(shaderProgram);
}
void load_quad()
{
	float vertices[] = {
					-1.f, -1.f, 0.0f,
					 1.f, -1.f, 0.0f,
					-1.f,  1.f, 0.0f,
					-1.f,  1.f, 0.0f,
					 1.f, -1.f, 0.0f,
					 1.f,  1.f, 0.0f
	};
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void screen::setup(size_t width, size_t height)
{
	// Allocate screen
	map2d<vec3>::setup(width, height);
	clear();

	// Initialize window
	if (glfwInit())
	{
		m_window = glfwCreateWindow(min((int)width,1920), min((int)height,1080), "Raytracer", nullptr, nullptr);
		if (m_window != nullptr)
		{
			glfwMakeContextCurrent(m_window);
			if (!gl3wInit() && gl3wIsSupported(3, 3))
			{
				load_shaders();
				load_quad();

				glGenTextures(1, &m_texture);
				glBindTexture(GL_TEXTURE_2D, m_texture);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

				glBindTexture(GL_TEXTURE_2D, m_texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)m_width, (GLsizei)m_height, 0, GL_RGB, GL_FLOAT, m_values.data());

				return;
			}
			destroy_window();
		}
	}
	glfwTerminate();
}

void screen::render()
{
	if (m_window != nullptr)
	{
		// Load Texture
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)m_width, (GLsizei)m_height, 0, GL_RGB, GL_FLOAT, m_values.data());

		// Render quad
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glfwSwapBuffers(m_window);

		// Check if should close the window
		glfwPollEvents();
		if (glfwWindowShouldClose(m_window))
			destroy_window();
	}
}

void screen::destroy_window()
{
	if (m_window != nullptr)
	{
		m_window = nullptr;
		glfwTerminate();
	}
}

vec3 screen::get(size_t x, size_t y) const
{
	return map2d<vec3>::get(x, y);
}

void screen::set(size_t x, size_t y, vec3 value)
{
	map2d<vec3>::set(x, y,value);
}

void screen::set(int idx, vec3 value)
{
	m_values[idx] = value;
}

void screen::output(const std::string& path)const
{
	// Store screen onto a png
	std::vector<char> data;
	data.resize(m_values.size() * 3);
	for (size_t i = 0; i < m_values.size(); i++)
	{
		const vec3& c = m_values[i];
		data[i * 3] = static_cast<char>((c.x*255.99f));
		data[i * 3 + 1] = static_cast<char>((c.y*255.99f));
		data[i * 3 + 2] = static_cast<char>((c.z*255.99f));
	}
	stbi_write_png(path.c_str(), (int)m_width, (int)m_height, 3, data.data(), 0);
}

void screen::get_resolution(int& width, int& height)
{
	width = static_cast<int>(m_width), height = static_cast<int>(m_height);
}

void screen::clear()
{
	map2d<vec3>::clear(m_clear_color);
}