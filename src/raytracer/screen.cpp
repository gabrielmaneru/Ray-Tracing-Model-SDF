#include "screen.h"

void screen::setup(size_t width, size_t height)
{
	map2d<vec3>::setup(width, height);
	clear();
}

vec3 screen::get(size_t x, size_t y) const
{
	return map2d<vec3>::get(x, y);
}

void screen::set(size_t x, size_t y, vec3 value)
{
	map2d<vec3>::set(x, y,value);
}

void screen::clear()
{
	map2d<vec3>::clear(m_clear_color);
}