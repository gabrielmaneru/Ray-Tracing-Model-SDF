/**
 * @file editor.cpp
 * @author Gabriel Maneru, gabriel.m, gabriel.m@digipen.edu
 * @date 01/28/2020
 * @brief GUI Manager
 * @copyright Copyright (C) 2020 DigiPen Institute of Technology.
**/
#include "editor.h"
#include <graphics/renderer.h>
#include <graphics/csg_scene.h>
#include <graphics/shader_program.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/ImGuizmo.h>
#include "window.h"

c_editor* editor = new c_editor;

bool c_editor::initialize()
{
	ImGui::CreateContext();
	if (!ImGui_ImplGlfw_InitForOpenGL(window->get_handler(), true))
		return false;
	const char * glsl_v = "#version 330";
	if (!ImGui_ImplOpenGL3_Init(glsl_v))
		return false;

	ImGui::StyleColorsLight();
	ImGui::GetStyle().FrameRounding = 12;
	ImGui::GetStyle().Colors[2] = ImVec4{ 1.0f, 1.0f, 1.0f, 0.6f };
	ImGui::GetStyle().Colors[10] = ImVec4{ 0.8f, 0.0f, 0.2f, 1.0f };
	ImGui::GetStyle().Colors[11] = ImVec4{ 0.8f, 0.0f, 0.2f, 1.0f };
	ImGuizmo::SetOrthographic(false);
	return true;
}
void c_editor::set_uniform(shader_program * s)
{
	s->set_uniform("s_technique", u_s_technique);
	s->set_uniform("march_it", u_march_it);
	s->set_uniform("min_dist", u_min_dist);
	s->set_uniform("max_dist", u_max_dist);
	s->set_uniform("shadow_samples", u_shadow_samples);

	s->set_uniform("rad0", u_rad0);
	s->set_uniform("min_step", u_min_step);
	s->set_uniform("factor_step", u_factor_step);

	s->set_uniform("replicate", u_replicate);
	s->set_uniform("display_shad", u_display_shadow);
}
void c_editor::drawGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (ImGui::Begin("Ray Marcher", nullptr))
	{
		if (ImGui::Button("Scene1"))
			renderer->m_next_scene = "Scene_1.txt";
		if (ImGui::Button("Scene2"))
			renderer->m_next_scene = "Scene_2.txt";

		ImGui::NewLine();

		ImGui::Text("March Info");
		ImGui::SliderInt("March It", &u_march_it, 1, 300);
		ImGui::SliderFloat("Min Dist", &u_min_dist, 0.00001f, 1.0f, "%.5f", 5.0f);
		ImGui::SliderFloat("Max Dist", &u_max_dist, 1.0f,  1000, "%.0f", 5.0f);

		ImGui::NewLine();

		ImGui::Text("Shadow Info");
		ImGui::SliderInt("Shadow Technique", &u_s_technique, 0, 3);
		if(u_s_technique == 1)
			ImGui::SliderInt("Shadow Samples", &u_shadow_samples, 1, 50);
		if (u_s_technique == 2 || u_s_technique == 3)
		{
			ImGui::SliderFloat("Rad0", &u_rad0, 0.00001f, 1.0f, "%.5f", 5.0f);
			ImGui::SliderFloat("Min Step", &u_min_step, 0.00001f, 1.0f, "%.5f", 5.0f);
			ImGui::SliderFloat("Factor Step", &u_factor_step, 0.01f, 1.0f, "%.2f", 5.0f);
		}

		ImGui::NewLine();

		ImGui::SliderFloat("Light Radius", &renderer->m_scene->m_light_rad, 0.01f, 10.0f, "%.2f", 2.0f);
		ImGui::Checkbox("Replicate", &u_replicate);
		ImGui::Checkbox("Display Shadow", &u_display_shadow);

		float framerate = ImGui::GetIO().Framerate;
		ImGui::Text(("FPS: " + std::to_string(framerate)).c_str());
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void c_editor::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
