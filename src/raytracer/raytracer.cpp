#include "raytracer.h"
#include <core/session.h>
#include <scene/scene.h>
#include <utils/math_utils.h>
#include <utils/profiler.h>
#include <iostream>
#include <iomanip>
#include <omp.h>

c_raytracer* raytracer = new c_raytracer;

void debug_print(const char * title, size_t v=0, size_t v_max=0)
{
	if (v_max > 1)
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
			<< "%  "
			<< std::flush;
	}
	else
	{
		std::cout << '\r'
			<< title << "...  "
			<< std::flush;
	}
}
void endline_debug_print()
{
	std::cout << "v/" << std::endl;
}

bool c_raytracer::init(size_t width, size_t height)
{
	debug_print("Allocating Screen");
	m_screen.setup(width, height);
	endline_debug_print();
	return true;
}

void c_raytracer::update()
{
	// Get screen resolution
	size_t width, height;
	m_screen.get_resolution(width, height);
	size_t pixel_count = width*height;

	// Precompute camera data
	camera * pCam = scene->m_camera;
	assert(pCam != nullptr);
	vec3 eye = pCam->get_eye();
	float half_width = static_cast<float>(width)*0.5f;
	float half_height = static_cast<float>(height)*0.5f;
	vec3 u_scr = pCam->m_u_vector / half_width;
	vec3 v_scr = pCam->m_v_vector / half_height;
	vec3 p_0 = pCam->m_origin + (0.5f - half_width)*u_scr - (0.5f - half_height)*v_scr;

	// Start loop
	size_t print_step = pixel_count / 10;
	START_FRAME("profiler");
	#pragma omp parallel for
	for (size_t p = 0; p < pixel_count; p++)
	{
		size_t x = p % width, y = p / width;
		vec3 target = p_0 + static_cast<float>(x)*u_scr - static_cast<float>(y)*v_scr;
		m_screen.set(x, y, scene->raycast({ eye,target - eye }));

		if(p%print_step == 0)
			debug_print("Throwing Rays", p, pixel_count);
	}
	END_FRAME();
	debug_print("Throwing Rays", pixel_count, pixel_count);
	endline_debug_print();

	// Output profiler data
	size_t cc = GET_PROFILER_DATA()[0]->m_time;
	debug_print(("Clock cycles " + std::to_string(cc) + " - Order " + std::to_string(std::to_string(cc).size())).c_str());
	endline_debug_print();

	// End Program
	session::end = true;
}

void c_raytracer::shutdown()
{
	debug_print("Storing Image");
	std::string name = scene->m_scene_path;
	m_screen.output(name.substr(0, name.find('.'))+".png");
	endline_debug_print();
}
