#pragma once
#include "screen.h"
#include "ray.h"
#include <vector>

class c_raytracer
{
	void create_rays(size_t width, size_t height);
	std::vector<ray> m_rays;

public:
	bool init(size_t width, size_t height);
	void update();
	void shutdown();
	screen m_screen;
};
extern c_raytracer* raytracer;