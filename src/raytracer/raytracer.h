#pragma once
#include "screen.h"

class c_raytracer
{
	void create_rays(size_t width, size_t height);


public:
	bool init(size_t width, size_t height);
	void update();
	void shutdown();

	screen m_screen;
};
extern c_raytracer* raytracer;