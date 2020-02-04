#include "raytracer.h"
#include <core/session.h>
#include <scene/scene.h>
#include <utils/math_utils.h>
#include <iostream>
#include <iomanip>

c_raytracer* raytracer = new c_raytracer;

void c_raytracer::thread_job(int id, thread_data data)
{
	for (int p = data.start; p < data.end; p++)
	{
		int x = p % data.width, y = p / data.width;
		vec3 target = data.p_0 + static_cast<float>(x)*data.u_scr - static_cast<float>(y)*data.v_scr;
		vec3 color = scene->raycast({ data.eye,target - data.eye });
		raytracer->m_screen.set(p, color);
	}
	if (id >= 0)
		raytracer->m_thread_bb[id] = true;
}

bool c_raytracer::init(size_t width, size_t height)
{
	std::cout << "\r" << "Allocating Screen..." << std::flush;
	m_screen.setup(width, height);
	std::cout << "v/" << std::endl;

	// Prepare data for threads
	thread_data data;
	data.width = static_cast<int>(width);
	data.height = static_cast<int>(height);

	// Precompute camera data
	camera * pcam = scene->m_camera;
	float half_width = static_cast<float>(width)*0.5f;
	float half_height = static_cast<float>(height)*0.5f;
	data.eye = pcam->get_eye();
	data.u_scr = pcam->m_u_vector / half_width;
	data.v_scr = pcam->m_v_vector / half_height;
	data.p_0 = pcam->m_origin + (0.5f - half_width)*data.u_scr - (0.5f - half_height)*data.v_scr;
	int total_count = static_cast<int>(width*height);

	// Distribute screen to threads
	if (m_parallel)
	{
		int thread_count = static_cast<int>(std::thread::hardware_concurrency())-1;
		for (int i = 0; i < thread_count; i++)
		{
			thread_data local_data{ data };
			local_data.start = total_count / thread_count * i;
			local_data.end = total_count / thread_count * (i+1);
			m_threads.emplace_back(std::thread{ thread_job, i, local_data });
			m_thread_bb.push_back(false);
		}
	}
	else
	{
		data.start = 0;
		data.end = total_count;
		thread_job(-1, data);
	}
	return true;
}

void c_raytracer::update()
{

	// Check if threads have finished
	bool finished = true;
	for (size_t i = 0; i < m_threads.size(); i++)
	{
		if (m_thread_bb[i])
		{
			if(m_threads[i].joinable())
				m_threads[i].join();
		}
		else
			finished = false;
	}

	// Render texture
	m_screen.render();

	// End Program
	if(finished)
		session::end = true;
}

void c_raytracer::shutdown()
{
	m_screen.destroy();

	std::string name = scene->m_scene_path;
	std::cout << "\r" << "Storing Image..." << std::flush;
	m_screen.output(name.substr(0, name.find_last_of('.'))+".png");
	std::cout << "v/" << std::endl;
}
