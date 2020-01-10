#pragma once
#include "screen.h"
#include "ray.h"
#include <vector>

class c_raytracer
{
	void create_rays(size_t width, size_t height);
	std::vector<ray> m_rays;
	size_t m_thrown_count{0u};

public:
	bool init(size_t width, size_t height);
	void update();
	void shutdown();
	screen m_screen;
};
extern c_raytracer* raytracer;