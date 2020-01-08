#pragma once
#include "shape.h"
#include <vector>

class c_scene
{
	std::vector<shape*> m_shapes;

public:
	bool init(std::string scene_path);
	void shutdown();
};
extern c_scene* scene;