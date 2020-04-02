#pragma once
#include <glm/glm.h>
#include <string>
#include <vector>
struct csg_node
{
	int m_operation;
	vec3 m_data1;
	vec3 m_data2;
};

struct csg_scene
{
	csg_scene(std::string filename);
	std::vector<csg_node> nodes;
	int draw_size{-1};
	vec3 m_light_pos;
	float m_light_rad;
};