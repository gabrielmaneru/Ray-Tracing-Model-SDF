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
	vec3 raycast(const ray& r)const;
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
	int m_s_samples{ 0 };
	int m_reflection_depth{ 0 };
	int m_r_samples{ 0 };
};
extern c_scene* scene;