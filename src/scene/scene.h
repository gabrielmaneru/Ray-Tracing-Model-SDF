#pragma once
#include "shape.h"
#include "light.h"
#include "camera.h"
#include "raytracer/ray.h"
#include <vector>

class c_scene
{
	vec3 compute_phong_lightning(vec3 pi, vec3 n, material mat)const;
	float compute_shadow_factor(vec3 pi, const light& l)const;

public:
	bool init(std::string scene_path);
	vec3 raycast(const ray& r)const;
	void shutdown();

	std::string m_scene_path;
	std::vector<shape*> m_shapes;
	std::vector<light> m_lights;
	camera* m_camera{ nullptr };
	vec3 m_ambient;
	float m_epsilon;
	int m_samples;

};
extern c_scene* scene;