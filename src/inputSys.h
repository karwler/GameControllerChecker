#pragma once

#include "utils.h"

class InputSys {
public:
	InputSys(WindowSys* window);
	~InputSys();

	void MouseMotionEvent(const SDL_MouseMotionEvent& motion, const vector<Object*>& objects);
	void MouseButtonDownEvent(const SDL_MouseButtonEvent& button);
	void MouseButtonUpEvent(const SDL_MouseButtonEvent& button, const vector<Object*>& objects);
	void MouseWheelEvent(const SDL_MouseWheelEvent& wheel);
	void KeypressEvent(const SDL_KeyboardEvent& key);
	void TextEvent(const SDL_TextInputEvent& text);
	void Tick(uint32_t dMsec);

	void NextController();
	void PrevController();
	void FindController(size_t i);
	bool StartRumble(float strength, uint32_t length);
	void StopRumble();
	vector<uint8_t> GetJoystickButtons() const;				// button id
	vector<pair<uint8_t, uint8_t>> GetJoystickHats() const;	// hat id, hat value
	vector<pair<uint8_t, int16_t>> GetJoystickAxes() const;	// axis id. axis value
	vector<SDL_GameControllerButton> GetGamepadButtons() const;
	vector<pair<SDL_GameControllerAxis, int16_t>> GetGamepadAxes() const;
	const char* JoystickName() const;
	const char* GamepadName() const;
	int NumJButtons() const;
	int NumJHats() const;
	int NumJAxes() const;
	bool IsGamepad() const;
	bool IsHaptic() const;

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
	bool IsSelected(const Object* obj) const;

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
	Object* mouseOver = nullptr;	// object currently being hovered over by mouse
	Object* scrollHold = nullptr;	// pointer to object currently being dragged by mouse (nullptr if none is being held)
	LineEditor* captured;
	umap<SDL_JoystickID, Controller> controllers;
	SDL_JoystickID curDevice;
	uint32_t curRumbling = 0;		// rumble time left in ms

	void CheckSliderClick(HorSlider* obj, int mx);
	void CheckSliderClick(ScrollArea* obj, SDL_Point mPos);
};
