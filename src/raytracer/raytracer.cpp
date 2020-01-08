#include "raytracer.h"
#include <core/session.h>

c_raytracer* raytracer = new c_raytracer;

bool c_raytracer::init(int width, int height)
{
	m_screen.setup(width, height);
	m_screen.clear({});
	return true;
}

void c_raytracer::update()
{
	session::end = true;
}

void c_raytracer::shutdown()
{
}
