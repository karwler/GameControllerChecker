#pragma once

#include "utils.h"

class Program {
public:
	Program(WindowSys* window);

	void EventOpenJoystickView();
	void EventOpenNextJoystick();
	void EventOpenPrevJoystick();
	void EventOpenJoystick(size_t i);
	void EventOpenGamepadView();
	void EventOpenHapticView();
	void EventControllersChanged(bool reset);
	void EventEnter();
	void EventEscape();

	void EventJoystickButton(SDL_JoystickID jid);
	void EventJoystickHat(SDL_JoystickID jid);
	void EventJoystickAxis(SDL_JoystickID jid);
	void EventGamepadButton(SDL_JoystickID jid);
	void EventGamepadAxis(SDL_JoystickID jid);

	void EventHapticTest();
	void EventTestStarted();
	void EventTestStopped();
	void EventStrengthChanged(float value);
	void EventLengthChanged(const string& text);

private:
	enum class Menu : uint8_t {
		none,
		joystick,
		gamepad,
		haptic
	} curMenu;
	WindowSys* win;
	ScrollArea* buttonList;
	ScrollArea* hatList;
	ScrollArea* axisList;

	float curHRStrength;
	uint32_t curHRLength;
	Button* hTestButton;

	void SwitchScene(Menu newMenu);
	static string HexToStr(int num);
	static string ButtonStr(SDL_GameControllerButton button);
	static string AxisStr(SDL_GameControllerAxis axis);
};
