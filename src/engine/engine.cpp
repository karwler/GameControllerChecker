#include "engine.h"

void Engine::Run() {
	// initialize all components
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC))
		throw Exception("couldn't initialize SDL\n" + string(SDL_GetError()), 1);
	if (TTF_Init())
		throw Exception("couldn't initialize fonts\n" + string(SDL_GetError()), 2);

	winSys = new WindowSys();
	inputSys = new InputSys();
	
	winSys->CreateWindow();
	scene = new Scene();		// initializes program and library
	
	// initialize values
	scene->getProgram()->Event_OpenJoystickView();
	run = true;
	SDL_Event event;
	uint32 oldTime = SDL_GetTicks();

	while (run) {
		// get delta seconds
		uint32 newTime = SDL_GetTicks();
		float dSec = float(newTime - oldTime) / 1000.f;
		oldTime = newTime;

		// draw scene if requested
		if (redraw) {
			redraw = false;
			winSys->DrawObjects(scene->Objects());
		}

		// handle events
		inputSys->Tick(dSec);

		uint32 timeout = SDL_GetTicks() + 50;
		while (SDL_PollEvent(&event) && timeout - SDL_GetTicks() > 0)
			HandleEvent(event);
	}
}

void Engine::Close() {
	run = false;
}

void Engine::Cleanup() {
	scene.clear();
	inputSys.clear();
	winSys.clear();

	TTF_Quit();
	SDL_Quit();
}

void Engine::HandleEvent(const SDL_Event& event) {
	// pass event to a specific handler
	if (event.type == SDL_MOUSEMOTION)
		inputSys->MouseMotionEvent(event.motion);
	else if (event.type == SDL_MOUSEBUTTONDOWN)
		inputSys->MouseButtonDownEvent(event.button);
	else if (event.type == SDL_MOUSEBUTTONUP)
		inputSys->MouseButtonUpEvent(event.button);
	else if (event.type == SDL_MOUSEWHEEL)
		inputSys->MouseWheelEvent(event.wheel);
	else if (event.type == SDL_KEYDOWN)
		inputSys->KeypressEvent(event.key);
	else if (event.type == SDL_TEXTINPUT)
		inputSys->TextEvent(event.text);
	else if (event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYBUTTONUP)
		scene->getProgram()->Event_JoystickButton();
	else if (event.type == SDL_JOYHATMOTION)
		scene->getProgram()->Event_JoystickHat();
	else if (event.type == SDL_JOYAXISMOTION)
		scene->getProgram()->Event_JoystickAxis();
	else if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP)
		scene->getProgram()->Event_GamepadButton();
	else if (event.type == SDL_CONTROLLERAXISMOTION)
		scene->getProgram()->Event_GamepadAxis();
	else if (event.type == SDL_JOYDEVICEADDED || event.type == SDL_JOYDEVICEREMOVED) {
		inputSys->UpdateControllers();
		scene->getProgram()->Event_JoystickButton();
		scene->getProgram()->Event_JoystickHat();
		scene->getProgram()->Event_JoystickAxis();
	} else if (event.type == SDL_CONTROLLERDEVICEADDED || event.type == SDL_CONTROLLERDEVICEREMOVED || event.type == SDL_CONTROLLERDEVICEREMAPPED) {
		scene->getProgram()->Event_GamepadButton();
		scene->getProgram()->Event_GamepadAxis();
	} else if (event.type == SDL_WINDOWEVENT)
		winSys->WindowEvent(event.window);
	else if (event.type == SDL_QUIT)
		Close();
}

void Engine::SetRedrawNeeded() {
	redraw = true;
}

InputSys* Engine::getInputSys() {
	return inputSys;
}

WindowSys* Engine::getWindowSys() {
	return winSys;
}

Scene* Engine::getScene() {
	return scene;
}
