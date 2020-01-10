#include "raytracer.h"
#include <scene/scene.h>
#include <core/session.h>

c_raytracer* raytracer = new c_raytracer;

void c_raytracer::create_rays(size_t width, size_t height)
{

}

bool c_raytracer::init(size_t width, size_t height)
{
	m_screen.setup(width, height);
	create_rays(width, height);
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
