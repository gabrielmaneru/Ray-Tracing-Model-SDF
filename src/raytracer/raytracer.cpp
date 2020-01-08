#include "raytracer.h"
#include <core/session.h>

c_raytracer* raytracer = new c_raytracer;

bool c_raytracer::init()
{
	return true;
}

void c_raytracer::update()
{
	session::end = true;
}

void c_raytracer::shutdown()
{
}
