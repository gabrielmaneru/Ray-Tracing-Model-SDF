#pragma once
#include "screen.h"

class c_raytracer
{
public:
	bool init(int width, int height);
	void update();
	void shutdown();

	screen m_screen;
};
extern c_raytracer* raytracer;