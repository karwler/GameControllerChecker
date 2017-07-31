#pragma once

#include "utils/objects.h"

class InputSys {
public:
	InputSys();
	~InputSys();

	void MouseMotionEvent(const SDL_MouseMotionEvent& motion);
	void MouseButtonDownEvent(const SDL_MouseButtonEvent& button);
	void MouseButtonUpEvent(const SDL_MouseButtonEvent& button);
	void MouseWheelEvent(const SDL_MouseWheelEvent& wheel);
	void KeypressEvent(const SDL_KeyboardEvent& key);
	void TextEvent(const SDL_TextInputEvent& text);
	
	void Tick(float dSec);
	static vec2i mousePos();	// get mouse poition
	
	int GetNextController(int curID);
	int GetPrevController(int curID);
	
	bool IsRumbling() const;
	bool StartRumble(int curID, float strength, uint32 length);
	bool StopRumble(int curID);

	vector<uint8> GetJoystickButtons(int curID);			// button id
	vector<pair<uint8, uint8>> GetJoystickHats(int curID);	// hat id, hat value
	vector<pair<uint8, int16>> GetJoystickAxes(int curID);	// axis id. axis value
	vector<SDL_GameControllerButton> GetGamepadButtons(int curID);
	vector<pair<SDL_GameControllerAxis, int16>> GetGamepadAxes(int curID);

	string JoystickName(int curID);
	string GamepadName(int curID);
	int NumJButtons(int curID);
	int NumJHats(int curID);
	int NumJAxes(int curID);
	bool JIsGamepad(int curID);
	bool JIsHaptic(int curID);

	size_t NumControllers() const;
	void UpdateControllers();

	LineEditor* Captured();
	void SetCapture(LineEditor* cbox);

private:
	LineEditor* captured;
	float curRumbling;	// time left
	vector<Controller> controllers;

	void ClearControllers();
};
