#include "raytracer.h"
#include <core/session.h>
#include <scene/scene.h>
#include <utils/math_utils.h>

c_raytracer* raytracer = new c_raytracer;

void c_raytracer::create_rays(size_t width, size_t height)
{
	camera * pCam = scene->m_camera;
	assert(pCam != nullptr);

	vec3 eye = pCam->get_eye();
	float half_width = static_cast<float>(width)*0.5f;
	float half_height = static_cast<float>(height)*0.5f;
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
	}
}

bool c_raytracer::init(size_t width, size_t height)
{
	m_screen.setup(width, height);
	create_rays(width, height);
	return true;
}

void c_raytracer::update()
{
	//#pragma omp parallel for
	for (const ray& r : m_rays)
	{
		vec4 result = scene->raycast(r);
		if (result.w == 1.0f)
			m_screen.set(r.m_screen_coords, vec3(result));
	}
	session::end = true;
}

void c_raytracer::shutdown()
{
	std::string name = scene->m_scene_path;
	m_screen.output(name.substr(0, name.find('.'))+".png");
}
