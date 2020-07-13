#pragma once

#include "windowSys.h"

class World {
public:
	static InputSys* Input();
	static Program* Prog();
	static WindowSys* Window();

private:
	static inline WindowSys base;
};
