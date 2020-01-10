#pragma once
#include <glm/glm.h>
#include <utils/map2d.h>
#include <string>

struct screen : private map2d<vec3>
{
	void setup(size_t width, size_t height)override;
	vec3 get(size_t x, size_t y)const;
	void set(std::pair<size_t, size_t> c, vec3 value);
	void output(const std::string& path)const;
	void clear();

	vec3 m_clear_color{0.0f, 0.0f, 0.0f};
};