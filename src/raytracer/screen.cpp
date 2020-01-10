#include "screen.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>

void screen::setup(size_t width, size_t height)
{
	map2d<vec3>::setup(width, height);
	clear();
}

vec3 screen::get(size_t x, size_t y) const
{
	return map2d<vec3>::get(x, y);
}

void screen::set(std::pair<size_t,size_t> c, vec3 value)
{
	map2d<vec3>::set(c.first, c.second,value);
}

void screen::output(const std::string& path)const
{
	std::vector<char> data;
	data.resize(m_values.size() * 3);
	for (size_t i = 0; i < m_values.size(); i++)
	{
		const vec3& c = m_values[i];
		data[i * 3] = static_cast<char>((c.x*255.99f));
		data[i * 3 + 1] = static_cast<char>((c.y*255.99f));
		data[i * 3 + 2] = static_cast<char>((c.z*255.99f));
	}
	stbi_write_png(path.c_str(), (int)m_width, (int)m_height, 3, data.data(), 0);
}

void screen::clear()
{
	map2d<vec3>::clear(m_clear_color);
}