/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	session.cpp
Purpose: Session Controller
Author: Gabriel Mañeru - gabriel.m
- End Header --------------------------------------------------------*/

#include "session.h"
#include <raytracer/raytracer.h>

bool session::end{false};

bool session::init()
{
	if (!raytracer->init()) return false;
	return true;
}

void session::update()
{
	do
	{
		raytracer->update();
	} while (!end);
}

void session::shutdown()
{
	raytracer->shutdown();
}
