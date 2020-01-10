#include "raytracer.h"
#include <core/session.h>

c_raytracer* raytracer = new c_raytracer;

bool c_raytracer::init(int width, int height)
{
	m_screen.setup(width, height);
	// Create Rays
	return true;
}

void c_raytracer::update()
{
	// Throw some rays
	// If finished
	session::end = true;
}

void c_raytracer::shutdown()
{
	// Store Image
}
