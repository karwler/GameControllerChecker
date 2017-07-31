#pragma once

#include "utils/scrollArea.h"

class Program {
public:
	Program();

	void Event_OpenJoystickView();
	void Event_OpenNextJoystick();
	void Event_OpenPrevJoystick();
	void Event_OpenGamepadView();
	void Event_OpenHapticView();

	void Event_JoystickButton();
	void Event_JoystickHat();
	void Event_JoystickAxis();

	void Event_GamepadButton();
	void Event_GamepadAxis();
	
	void Event_HapticTest();
	void Event_TestStarted();
	void Event_TestStopped();
	void Event_StrengthChanged(float value);
	void Event_LengthChanged(const string& text);

	void Event_ControllersChanged();
	
private:
	enum class EMenu : uint8 {
		joystick,
		gamepad,
		haptic
	} curMenu;
	int curDevice;
	ScrollArea *buttonList, *hatList, *axisList;

	float curHRStrength;
	uint32 curHRLength;
	Button* hTestButton;
	
	void SwitchScene(EMenu newMenu);
	void SwitchScene();
};
