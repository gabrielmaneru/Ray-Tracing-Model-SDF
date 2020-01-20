/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	main.cpp
Purpose: Program base
Author: Gabriel Ma�eru - gabriel.m
- End Header --------------------------------------------------------*/

#include "session.h"
//#include <Windows.h>
#include <string>
#include <iostream>

int main(int argc, char *argv[])
//int CALLBACK WinMain(__in HINSTANCE hInstance, __in HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nCmdShow)
{
	int res_x = 500, res_y = 500;
	std::string scene = "SampleScene01.txt";

	if (argc == 1);
	else if (argc == 3)
	{
		res_x = std::atoi(argv[1]);
		res_y = std::atoi(argv[2]);
	}
	else if (argc == 4)
	{
		res_x = std::atoi(argv[1]);
		res_y = std::atoi(argv[2]);
		scene = argv[3];
	}
	else
	{
		std::cout << "Invalid Command-Line Arguments, use <width> <height> <scene.txt>" << std::endl;
		return 1;
	}

	if (!session::init(res_x, res_y, scene))
		return 1;
	session::update();
	session::shutdown();
	return 0;
}