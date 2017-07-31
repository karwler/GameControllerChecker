#pragma once

#include "engine.h"

class World {
public:
	static Engine* engine();
	static InputSys* inputSys();
	static WindowSys* winSys();
	static Scene* scene();
	static Program* program();

private:
	static Engine base;
};
