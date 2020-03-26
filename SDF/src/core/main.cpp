/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	main.cpp
Purpose: Program base
Author: Gabriel Ma�eru - gabriel.m
- End Header --------------------------------------------------------*/

#include "session.h"
#ifdef _DEBUG
int main(int argc, char *argv[])
#else
#include <Windows.h>
int CALLBACK WinMain(__in HINSTANCE hInstance, __in HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nCmdShow)
#endif // _DEBUG
{
	// Run Session
	if (!session::init())
		return 1;
	session::update();
	session::shutdown();
	return 0;
}