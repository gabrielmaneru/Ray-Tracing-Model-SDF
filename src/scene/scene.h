#pragma once
#include "shape.h"
#include "camera.h"
#include "raytracer/ray.h"
#include <vector>

class c_scene
{
public:
	bool init(std::string scene_path);
	vec4 raycast(const ray& r)const;
	void shutdown();

	std::vector<shape*> m_shapes;
	camera* m_camera{ nullptr };
	std::string m_scene_path;
};
extern c_scene* scene;