#pragma once

#include "inputSys.h"
#include "windowSys.h"
#include "scene.h"
#include "kklib/sptr.h"

class Engine {
public:
	void Run();
	void Close();
	void Cleanup();

	void SetRedrawNeeded();
	InputSys* getInputSys();
	WindowSys* getWindowSys();
	Scene* getScene();

private:
	kk::sptr<InputSys> inputSys;
	kk::sptr<WindowSys> winSys;
	kk::sptr<Scene> scene;

	bool run;
	bool redraw;

	void HandleEvent(const SDL_Event& event);
};
