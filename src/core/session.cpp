/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	session.cpp
Purpose: Session Controller
Author: Gabriel Mañeru - gabriel.m
- End Header --------------------------------------------------------*/

#include "session.h"
#include <scene/scene.h>
#include <raytracer/raytracer.h>

bool session::end{false};

// Initialize singletons
bool session::init(int width, int height, std::string scene_path)
{
	if (!scene->init(scene_path)) return false;
	if (!raytracer->init(width, height)) return false;
	return true;
}

// Do render loop
void session::update()
{
	do
	{
		raytracer->update();
	} while (!end);
}

// Store & Save data
void session::shutdown()
{
	raytracer->shutdown();
	scene->shutdown();
}
