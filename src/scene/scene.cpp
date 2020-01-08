#include "scene.h"

c_scene* scene = new c_scene;

bool c_scene::init(std::string scene_path)
{
	// parse
	return true;
}

void c_scene::shutdown()
{
	for (auto*s : m_shapes)
		delete s;
	m_shapes.clear();
}
