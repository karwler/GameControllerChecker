#pragma once

#include "program.h"

class InputSys {
public:
	InputSys();
	~InputSys();

	void MouseMotionEvent(const SDL_MouseMotionEvent& motion, const vector<Object*>& objects);
	void MouseButtonDownEvent(const SDL_MouseButtonEvent& button);
	void MouseButtonUpEvent(const SDL_MouseButtonEvent& button, const vector<Object*>& objects);
	void MouseWheelEvent(const SDL_MouseWheelEvent& wheel);
	void KeypressEvent(const SDL_KeyboardEvent& key);
	void TextEvent(const SDL_TextInputEvent& text);
	void Tick(float dSec);

	void NextController();
	void PrevController();
	void FindController(size_t i);
	bool StartRumble(float strength, uint32 length);
	void StopRumble();
	vector<uint8> GetJoystickButtons();				// button id
	vector<pair<uint8, uint8>> GetJoystickHats();	// hat id, hat value
	vector<pair<uint8, int16>> GetJoystickAxes();	// axis id. axis value
	vector<SDL_GameControllerButton> GetGamepadButtons();
	vector<pair<SDL_GameControllerAxis, int16>> GetGamepadAxes();
	string JoystickName();
	string GamepadName();
	int NumJButtons();
	int NumJHats();
	int NumJAxes();
	bool IsGamepad();
	bool IsHaptic();

	SDL_JoystickID CurDevice() const;
	size_t CurIndex() const;
	size_t NumControllers() const;
	void AddController(int id);
	void DelController(SDL_JoystickID id);
	Program* Prog();
	void PostMenuSwitch(const vector<Object*>& objects);
	void SimulateMouseMove(const vector<Object*>& objects);
	LineEditor* Captured();
	void SetCapture(LineEditor* cbox);
	bool IsSelected(Object* obj);

private:
	struct Controller {
		SDL_Joystick* joystick;
		SDL_GameController* gamepad;
		SDL_Haptic* haptic;

		bool Open(int id);
		void Close();
	};

	static constexpr float scrollFactor = 50.f;

	Program* program;
	Object* mouseOver;		// object currently being hovered over by mouse
	Object* scrollHold;		// pointer to object currently being dragged by mouse (nullptr if none is being held)
	LineEditor* captured;
	umap<SDL_JoystickID, Controller> controllers;
	SDL_JoystickID curDevice;
	float curRumbling;		// rumble time left

	void CheckSliderClick(HorSlider* obj, int mx);
	void CheckSliderClick(ScrollArea* obj, SDL_Point mPos);
};
