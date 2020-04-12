/**
 * @file editor.h
 * @author Gabriel Maneru, gabriel.m, gabriel.m@digipen.edu
 * @date 01/28/2020
 * @brief GUI Manager
 * @copyright Copyright (C) 2020 DigiPen Institute of Technology.
**/
#pragma once
#include <glm/glm.h>

class shader_program;
class c_editor
{
	int u_s_technique = 0;
	int u_march_it = 95;
	float u_min_dist = 0.0001f;
	float u_max_dist = 200.0f;
	int u_shadow_samples = 30;
	float u_rad0 = 0.1f;
	float u_shad_scale = 10.f;
	float u_tstep = 0.005f;

public:
	bool initialize();
	void set_uniform(shader_program* s);
	void drawGui();
	void shutdown();
};
extern c_editor* editor;