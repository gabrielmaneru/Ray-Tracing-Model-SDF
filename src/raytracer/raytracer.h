#pragma once
#include <utils/map2d.h>
#include <glm/glm.h>

using screen = map2d<vec3>;
class c_raytracer
{
public:
	bool init(int width, int height);
	void update();
	void shutdown();

	screen m_screen;
};
extern c_raytracer* raytracer;