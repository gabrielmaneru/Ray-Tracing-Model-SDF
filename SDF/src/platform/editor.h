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
	int u_march_it = 115;
	float u_min_dist = 0.001f;
	float u_max_dist = 25.0f;

	int u_s_technique = 3;
	int u_shadow_samples = 3;
	float u_rad0 = 0.05f;
	float u_min_step = 0.01f;
	float u_factor_step = 0.25f;

	bool u_replicate{ false };
	bool u_display_shadow{false};

public:
	bool initialize();
	void set_uniform(shader_program* s);
	void drawGui();
	void shutdown();
};
extern c_editor* editor;