#pragma once

class c_raytracer
{

public:
	bool init();
	void update();
	void shutdown();
};
extern c_raytracer* raytracer;