#pragma once
#include "shape.h"
#include "light.h"
#include "camera.h"
#include "raytracer/ray.h"
#include <vector>

class c_scene
{
public:
	bool init(std::string scene_path);
	vec3 raytrace_pixel(const vec3& px_center, const vec3& px_width, const vec3& px_height, const vec3& eye)const;
	vec3 raytrace(const ray& r)const;
	void shutdown();


	// Scene Properties
	struct shape_data
	{
		shape* m_shape;
		material m_mat;
	};
	std::vector<shape_data> m_shapes;
	std::vector<light> m_lights;
	std::string m_scene_path;
	camera* m_camera{ nullptr };
	vec3 m_ambient{0.f};
	struct{
		float m_electric_permittivity{ 1.f };
		float m_magnetic_permeability{ 1.f };
		vec3 m_attenuation{ 1.f };
	}m_air;

	// Scene Config
	float m_epsilon{ 0.0f };
	int m_recursion_depth{ 0 };
	int m_shadow_samples{ 0 };
	int m_rough_reflection_samples{ 0 };
	int m_aa_section_samples{ 0 };
	int m_aa_recursion_depth{ 0 };
};
extern c_scene* scene;