/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	session.cpp
Purpose: Session Controller
Author: Gabriel Mañeru - gabriel.m
- End Header --------------------------------------------------------*/

#include "session.h"
#include <platform/window.h>
#include <graphics/renderer.h>

bool session::end{false};

bool session::init()
{
	if (!window->initialize()) return false;
	if (!renderer->initialize()) return false;
	return true;
}

void session::update()
{
	do
	{
		window->update();
		renderer->draw();
		window->present();
	} while (!end);
}

void session::shutdown()
{
	renderer->shutdown();
	window->shutdown();
}
