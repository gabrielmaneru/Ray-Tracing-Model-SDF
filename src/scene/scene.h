#pragma once
#include "shape.h"
#include "camera.h"
#include <vector>

class c_scene
{
	std::vector<shape*> m_shapes;
	camera* m_camera{ nullptr };

public:
	bool init(std::string scene_path);
	void shutdown();
};
extern c_scene* scene;