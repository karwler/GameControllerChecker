#include "world.h"

Engine World::base;

Engine* World::engine() {
	return &base;
}

InputSys* World::inputSys() {
	return base.getInputSys();
}

WindowSys* World::winSys() {
	return base.getWindowSys();
}

Scene* World::scene() {
	return base.getScene();
}

Program* World::program() {
	return base.getScene()->getProgram();
}
