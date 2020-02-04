#pragma once
#include "screen.h"
#include "ray.h"
#include <vector>
#include <thread>

struct thread_data
{
	int width, height;
	vec3 eye, p_0, u_scr, v_scr;
	int start, end;
};

class c_raytracer
{
	bool m_parallel{ true };
	std::vector<std::thread> m_threads;
	std::vector<bool> m_thread_bb;
	static void thread_job(int id, thread_data data);

public:
	bool init(size_t width, size_t height);
	void update();
	void shutdown();
	screen m_screen;
};
extern c_raytracer* raytracer;