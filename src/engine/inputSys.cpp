#include "world.h"

InputSys::InputSys() :
	curRumbling(0.f)
{
	SetCapture(nullptr);
	UpdateControllers();
}

InputSys::~InputSys() {
	ClearControllers();
}

void InputSys::MouseMotionEvent(const SDL_MouseMotionEvent& motion) {
	World::scene()->OnMouseMove(vec2i(motion.x, motion.y));
}

void InputSys::MouseButtonDownEvent(const SDL_MouseButtonEvent& button) {
	if (captured)
		captured->Confirm();

	if (button.button == SDL_BUTTON_LEFT)
		World::scene()->OnMouseDown(mousePos());
}

void InputSys::MouseButtonUpEvent(const SDL_MouseButtonEvent& button) {
	if (button.button == SDL_BUTTON_LEFT)
		World::scene()->OnMouseUp();
}

void InputSys::MouseWheelEvent(const SDL_MouseWheelEvent& wheel) {
	World::scene()->OnMouseWheel(float(wheel.y) * -scrollFactor);
}

void InputSys::KeypressEvent(const SDL_KeyboardEvent& key) {
	if (captured)
		captured->OnKeypress(key.keysym.scancode);
}

void InputSys::TextEvent(const SDL_TextInputEvent& text) {
	captured->OnText(text.text);
	World::engine()->SetRedrawNeeded();
}

void InputSys::Tick(float dSec) {
	if (curRumbling > 0.f) {
		curRumbling -= dSec;
		if (curRumbling <= 0.f)
			World::program()->Event_TestStopped();
	}
}

vec2i InputSys::mousePos() {
	vec2i pos;
	SDL_GetMouseState(&pos.x, &pos.y);
	return pos;
}

int InputSys::GetNextController(int curID) {
	return (curID == controllers.size()-1) ? 0 : curID + 1;
}

int InputSys::GetPrevController(int curID) {
	return (curID == 0) ? controllers.size()-1 : curID - 1;
}

bool InputSys::IsRumbling() const {
	return curRumbling > 0.f;
}

bool InputSys::StartRumble(int curID, float strength, uint32 length) {
	if (SDL_HapticRumblePlay(controllers[curID].haptic, strength, length) == -1)
		return false;
	
	curRumbling = float(length) / 1000.f;
	World::program()->Event_TestStarted();
	return true;
}

bool InputSys::StopRumble(int curID) {
	curRumbling = 0.f;
	World::program()->Event_TestStopped();
	return SDL_HapticRumbleStop(controllers[curID].haptic) == 0;
}

vector<uint8> InputSys::GetJoystickButtons(int curID) {
	vector<uint8> buttons;
	for (int i=0; i!=SDL_JoystickNumButtons(controllers[curID].joystick); i++)
		if (SDL_JoystickGetButton(controllers[curID].joystick, i))
			buttons.push_back(i);
	return buttons;
}

vector<pair<uint8, uint8>> InputSys::GetJoystickHats(int curID) {
	vector<pair<uint8, uint8>> hats(SDL_JoystickNumHats(controllers[curID].joystick));
	for (int i=0; i!=hats.size(); i++)
		hats[i] = make_pair(i, SDL_JoystickGetHat(controllers[curID].joystick, i));
	return hats;
}

vector<pair<uint8, int16>> InputSys::GetJoystickAxes(int curID) {
	vector<pair<uint8, int16>> axes(SDL_JoystickNumAxes(controllers[curID].joystick));
	for (int i=0; i!=axes.size(); i++)
		axes[i] = make_pair(i, SDL_JoystickGetAxis(controllers[curID].joystick, i));
	return axes;
}

vector<SDL_GameControllerButton> InputSys::GetGamepadButtons(int curID) {
	vector<SDL_GameControllerButton> buttons;
	for (uint8 i=0; i!=SDL_CONTROLLER_BUTTON_MAX; i++) {
		SDL_GameControllerButton but = static_cast<SDL_GameControllerButton>(i);
		if (SDL_GameControllerGetButton(controllers[curID].gamepad, but))
			buttons.push_back(but);
	}
	return buttons;
}

vector<pair<SDL_GameControllerAxis, int16>> InputSys::GetGamepadAxes(int curID) {
	const uint8 numAxes = SDL_CONTROLLER_AXIS_MAX;
	vector<pair<SDL_GameControllerAxis, int16>> axes(numAxes);

	for (uint8 i=0; i!=numAxes; i++) {
		SDL_GameControllerAxis axs = static_cast<SDL_GameControllerAxis>(i);
		axes[i] = make_pair(axs, SDL_GameControllerGetAxis(controllers[curID].gamepad, axs));
	}
	return axes;
}

string InputSys::JoystickName(int curID) {
	return SDL_JoystickName(controllers[curID].joystick);
}

string InputSys::GamepadName(int curID) {
	return SDL_GameControllerName(controllers[curID].gamepad);
}

int InputSys::NumJButtons(int curID) {
	return SDL_JoystickNumButtons(controllers[curID].joystick);
}

int InputSys::NumJHats(int curID) {
	return SDL_JoystickNumHats(controllers[curID].joystick);
}

int InputSys::NumJAxes(int curID) {
	return SDL_JoystickNumAxes(controllers[curID].joystick);
}

bool InputSys::JIsGamepad(int curID) {
	return controllers[curID].gamepad != nullptr;
}

bool InputSys::JIsHaptic(int curID) {
	return controllers[curID].haptic != nullptr;
}

size_t InputSys::NumControllers() const {
	return controllers.size();
}

void InputSys::UpdateControllers() {
	ClearControllers();

	for (int i=0; i!=SDL_NumJoysticks(); i++) {
		Controller dev;
		if (dev.Open(i))
			controllers.push_back(dev);
	}
	if (World::program())
		World::program()->Event_ControllersChanged();
}

void InputSys::ClearControllers() {
	for (Controller& it : controllers)
		it.Close();
	controllers.clear();
}

LineEditor* InputSys::Captured() {
	return captured;
}

void InputSys::SetCapture(LineEditor* cbox) {
	captured = cbox;
	if (captured)
		SDL_StartTextInput();
	else
		SDL_StopTextInput();

	World::engine()->SetRedrawNeeded();
}
