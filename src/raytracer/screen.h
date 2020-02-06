#pragma once
#include <utils/map2d.h>
#include <glm/glm.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <string>

struct screen : private map2d<vec3>
{
	void setup(size_t width, size_t height)override;
	void render();
	void destroy();
	vec3 get(size_t x, size_t y)const;
	void set(size_t x, size_t y, vec3 value);
	void set(int idx, vec3 value);
	void output(const std::string& path)const;
	void get_resolution(int& width, int& height);
	void clear();

	GLFWwindow * m_window{ nullptr };
	GLuint m_texture{ 0u };
	vec3 m_clear_color{0.5f, 0.0f, 0.5f};
};