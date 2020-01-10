#include "raytracer.h"
#include <core/session.h>
#include <scene/scene.h>
#include <utils/math_utils.h>
#include <iostream>
#include <iomanip>

c_raytracer* raytracer = new c_raytracer;
void debug_print(const char * title, size_t v=0, size_t v_max=0)
{
	if (v_max > 0)
	{
		std::cout << '\r'
			<< title << ": "
			<< std::setw(std::to_string(v_max).size() + 1)
			<< std::to_string(v)
			<< '/'
			<< std::to_string(v_max)
			<< " - "
			<< std::fixed << std::setprecision(2)
			<< std::setw(6)
			<< coef<size_t>(0, v_max, v)*100.f
			<< "%"
			<< std::flush;
	}
	else
	{
		std::cout << '\r'
			<< title << " ..."
			<< std::flush;
	}
}

void c_raytracer::create_rays(size_t width, size_t height)
{
	camera * pCam = scene->m_camera;
	assert(pCam != nullptr);
	
	vec3 eye = pCam->get_eye();
	float half_width = static_cast<float>(width)*0.5f;
	float half_height = static_cast<float>(height)*0.5f;

	debug_print("Allocating Rays");
	m_rays.resize(width*height);

	for (size_t y = 0; y < height; y++)
	{
		vec3 v = (static_cast<float>(y) - half_height + 0.5f) / half_height * pCam->m_v_vector;
		for (size_t x = 0; x < width; x++)
		{
			vec3 u = (static_cast<float>(x) - half_width + 0.5f) / half_width * pCam->m_u_vector;
			vec3 target = pCam->m_origin + u - v;

			ray& r = m_rays[y*width + x];
			r.m_origin = eye;
			r.m_direction = target - eye;
			r.m_screen_coords = { x,y };
		}
		debug_print("Computing Rays", (y+1)*width, m_rays.size());
	}
}

bool c_raytracer::init(size_t width, size_t height)
{
	debug_print("Allocating Screen");
	m_screen.setup(width, height);
	std::cout << std::endl;

	create_rays(width, height);
	return true;
}

void c_raytracer::update()
{
	const size_t max_iteration = 9973;
	size_t target_iteration;
	if (m_rays.size() < m_thrown_count + max_iteration)
		target_iteration = m_rays.size();
	else
		target_iteration = m_thrown_count + max_iteration;

	//#pragma omp parallel for
	for (size_t i = m_thrown_count; i < target_iteration; i++)
	{
		const ray& r = m_rays[i];
		vec4 result = scene->raycast(r);
		if (result.w == 1.0f)
			m_screen.set(r.m_screen_coords, vec3(result));
	}
	m_thrown_count = target_iteration;
	debug_print("Throwing Rays", m_thrown_count, m_rays.size());

	if(m_thrown_count == m_rays.size())
		session::end = true;
}

void c_raytracer::shutdown()
{
	debug_print("Storing Image");
	std::string name = scene->m_scene_path;
	m_screen.output(name.substr(0, name.find('.'))+".png");
}
