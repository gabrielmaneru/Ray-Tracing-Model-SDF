#include "raytracer.h"
#include <core/session.h>
#include <scene/scene.h>
#include <utils/math_utils.h>
#include <iostream>
#include <iomanip>
#include <omp.h>
#include <mpi.h>

c_raytracer* raytracer = new c_raytracer;

void debug_print(const char * title, int v=0, int v_max=0)
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
			<< coef(0, v_max, v)*100.f
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

	int machineCount;
	int rank;
	MPI_Comm_size(MPI_COMM_WORLD, &machineCount);

	return true;
}

void c_raytracer::update()
{
	// Get screen resolution
	int width, height;
	m_screen.get_resolution(width, height);
	int pixel_count = static_cast<int>(width*height);

	// Precompute camera data
	camera * pcam = scene->m_camera;
	vec3 eye = pcam->get_eye();
	float half_width = static_cast<float>(width)*0.5f;
	float half_height = static_cast<float>(height)*0.5f;
	vec3 u_scr = pcam->m_u_vector / half_width;
	vec3 v_scr = pcam->m_v_vector / half_height;
	vec3 p_0 = pcam->m_origin + (0.5f - half_width)*u_scr - (0.5f - half_height)*v_scr;

	// Start loop
	int debug_step = max(pixel_count/11, 1);
	int last = -1;
	#pragma omp parallel for
	for (int p = 0; p < pixel_count; p++)
	{
		int x = p % width, y = p / width;
		vec3 target = p_0 + static_cast<float>(x)*u_scr - static_cast<float>(y)*v_scr;
		vec3 color = scene->raycast({ eye,target - eye });
		m_screen.set(p, color);

		{
			if (p-debug_step >= last)
			{
				last = p;
				debug_print("Throwing Rays", p, pixel_count);
				m_screen.render();
			}

		}
	}
	debug_print("Throwing Rays", pixel_count, pixel_count);
	endline_debug_print();
	m_screen.render();

	// End Program
	session::end = true;
}

void c_raytracer::shutdown()
{
	m_screen.destroy();

	debug_print("Storing Image");
	std::string name = scene->m_scene_path;
	m_screen.output(name.substr(0, name.find_last_of('.'))+".png");
	endline_debug_print();
}
