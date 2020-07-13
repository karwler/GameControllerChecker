#include "world.h"
#include <sstream>

// events

void Program::EventOpenJoystickView() {
	World::Input()->StopRumble();
	SwitchScene(Menu::joystick);
}

void Program::EventOpenNextJoystick() {
	World::Input()->StopRumble();
	World::Input()->NextController();
	SwitchScene(Menu::joystick);
}

void Program::EventOpenPrevJoystick() {
	World::Input()->StopRumble();
	World::Input()->PrevController();
	SwitchScene(Menu::joystick);
}

void Program::EventOpenJoystick(size_t i) {
	World::Input()->StopRumble();
	World::Input()->FindController(i);
	SwitchScene(Menu::joystick);
}

void Program::EventOpenGamepadView() {
	World::Input()->StopRumble();
	SwitchScene(Menu::gamepad);
}

void Program::EventOpenHapticView() {
	World::Input()->StopRumble();
	SwitchScene(Menu::haptic);
}

void Program::EventControllersChanged(bool reset) {
	if (reset || curMenu <= Menu::joystick)
		SwitchScene(Menu::joystick);
}

void Program::EventEnter() {
	switch (curMenu) {
	case Menu::joystick:
		if (World::Input()->IsGamepad())
			SwitchScene(Menu::gamepad);
		else if (World::Input()->IsHaptic())
			SwitchScene(Menu::haptic);
		break;
	case Menu::gamepad:
		if (World::Input()->IsHaptic())
			SwitchScene(Menu::haptic);
		break;
	case Menu::haptic:
		EventHapticTest();
	}
}

void Program::EventEscape() {
	switch (curMenu) {
	case Menu::gamepad:
		SwitchScene(Menu::joystick);
		break;
	case Menu::haptic:
		World::Input()->IsGamepad() ? SwitchScene(Menu::gamepad) : SwitchScene(Menu::joystick);
		break;
	default:
		World::Window()->Close();
	}
}

void Program::EventJoystickButton(SDL_JoystickID jid) {
	if (curMenu == Menu::joystick && World::Input()->CurDevice() == jid) {
		vector<uint8> buttons = World::Input()->GetJoystickButtons();
		vector<string> items(buttons.size());
		for (size_t i = 0; i < items.size(); i++)
			items[i] = std::to_string(buttons[i]);
		buttonList->Items(std::move(items));
	}
}

void Program::EventJoystickHat(SDL_JoystickID jid) {
	if (curMenu == Menu::joystick && World::Input()->CurDevice() == jid) {
		vector<pair<uint8, uint8>> hats = World::Input()->GetJoystickHats();
		vector<string> items(hats.size());

		for (size_t i = 0; i < items.size(); i++) {
			string label = std::to_string(hats[i].first) + ": " + HexToStr(hats[i].second) + " (";
			if (hats[i].second == SDL_HAT_CENTERED)
				label.append("centered");
			else {
				if (hats[i].second & SDL_HAT_UP)
					label.append("up ");
				if (hats[i].second & SDL_HAT_RIGHT)
					label.append("right ");
				if (hats[i].second & SDL_HAT_DOWN)
					label.append("down ");
				if (hats[i].second & SDL_HAT_LEFT)
					label.append("left ");
				label.resize(label.size() - 1);
			}
			items[i] = label + ")";
		}
		hatList->Items(std::move(items));
	}
}

void Program::EventJoystickAxis(SDL_JoystickID jid) {
	if (curMenu == Menu::joystick && World::Input()->CurDevice() == jid) {
		vector<pair<uint8, int16>> axes = World::Input()->GetJoystickAxes();
		vector<string> items(axes.size());
		for (size_t i = 0; i < items.size(); i++)
			items[i] = std::to_string(axes[i].first) + ": " + std::to_string(axes[i].second);
		axisList->Items(std::move(items));
	}
}

void Program::EventGamepadButton(SDL_JoystickID jid) {
	if (curMenu == Menu::gamepad && World::Input()->CurDevice() == jid) {
		vector<SDL_GameControllerButton> buttons = World::Input()->GetGamepadButtons();
		vector<string> items(buttons.size());
		for (size_t i = 0; i < items.size(); i++)
			items[i] = ButtonStr(buttons[i]);
		buttonList->Items(std::move(items));
	}
}

void Program::EventGamepadAxis(SDL_JoystickID jid) {
	if (curMenu == Menu::gamepad && World::Input()->CurDevice() == jid) {
		vector<pair<SDL_GameControllerAxis, int16>> axes = World::Input()->GetGamepadAxes();
		vector<string> items(axes.size());
		for (size_t i = 0; i < items.size(); i++)
			items[i] = AxisStr(axes[i].first) + ": " + std::to_string(axes[i].second);
		axisList->Items(std::move(items));
	}
}

void Program::EventHapticTest() {
	if (World::Input()->StopRumble(); World::Input()->StartRumble(curHRStrength, curHRLength))
		EventTestStarted();
}

void Program::EventTestStarted() {
	if (hTestButton)
		hTestButton->Text("Cancel");
}

void Program::EventTestStopped() {
	if (hTestButton)
		hTestButton->Text("Test");
}

void Program::EventStrengthChanged(float value) {
	curHRStrength = value;
}

void Program::EventLengthChanged(const string& text) {
	curHRLength = stoul(text);
}

// scene building

void Program::SwitchScene(Menu newMenu) {
	World::Window()->Fonts()->Clear();
	buttonList = nullptr;
	hatList = nullptr;
	axisList = nullptr;
	hTestButton = nullptr;

	SDL_Point res = World::Window()->Resolution();
	vector<Object*> objects;
	switch (curMenu = World::Input()->CurDevice() != -1 ? newMenu : Menu::none) {
	case Menu::none:
		objects = {new Label(Object({res.x / 2, res.y / 2}, {0, res.y / 2 - 30}, {res.x, 60}, FIX_SIZ, Object::colorBackground), "No controllers detected :(", Label::Align::center)};
		break;
	case Menu::joystick: {
		int colFac = res.x / 6;
		int colWidth = res.x / 3 - 5;
		int numButtons = World::Input()->NumJButtons();
		int numHats = World::Input()->NumJHats();
		int numAxes = World::Input()->NumJAxes();
		objects = {
			new Button(Object({0, 0}, {-1, -1}, {30, 30}, FIX_ANC | FIX_SIZ), &Program::EventOpenPrevJoystick, "<", Label::Align::center),
			new Label(Object({35, 0}, {-1, -1}, {130, 30}, FIX_ANC | FIX_SIZ, Object::colorDark), std::to_string(World::Input()->CurIndex() + 1) + " / " + std::to_string(World::Input()->NumControllers()), Label::Align::center),
			new Button(Object({170, 0}, {-1, -1}, {30, 30}, FIX_ANC | FIX_SIZ), &Program::EventOpenNextJoystick, ">", Label::Align::center),
			new Label(Object({210, 0}, {-1, -1}, {res.x - 210, 30}, FIX_ANC | FIX_H | FIX_EX, Object::colorBackground), World::Input()->JoystickName(), Label::Align::left),
			new Label(Object({colFac, 75}, {colFac - colWidth / 2, 75}, {colWidth, 30}, FIX_Y | FIX_H), "Buttons (" + std::to_string(numButtons) + ')', Label::Align::center),
			new Label(Object({colFac * 3, 75}, {colFac * 3 - colWidth / 2, 75}, {colWidth, 30}, FIX_Y | FIX_H), "Hats (" + std::to_string(numHats) + ')', Label::Align::center),
			new Label(Object({colFac * 5, 75}, {colFac * 5 - colWidth / 2, 75}, {colWidth, 30}, FIX_Y | FIX_H), "Axes (" + std::to_string(numAxes) + ')', Label::Align::center),
			buttonList = new ScrollArea({colFac, 110}, {colFac - colWidth / 2, 110}, {colWidth, res.y - 110}, FIX_Y | FIX_EY),
			hatList = new ScrollArea({colFac * 3, 110}, {colFac * 3 - colWidth / 2, 110}, {colWidth, res.y - 110}, FIX_Y | FIX_EY),
			axisList = new ScrollArea({colFac * 5, 110}, {colFac * 5 - colWidth / 2, 110}, {colWidth, res.y - 110}, FIX_Y | FIX_EY)
		};
		if (World::Input()->IsGamepad())
			objects.push_back(new Button(Object({0, 35}, {-1, -1}, {145, 30}, FIX_ANC | FIX_SIZ), &Program::EventOpenGamepadView, "Gamepad", Label::Align::center));
		if (World::Input()->IsHaptic())
			objects.push_back(new Button(Object({150, 35}, {-1, -1}, {145, 30}, FIX_ANC | FIX_SIZ), &Program::EventOpenHapticView, "Haptic", Label::Align::center));

		EventJoystickButton(World::Input()->CurDevice());
		EventJoystickHat(World::Input()->CurDevice());
		EventJoystickAxis(World::Input()->CurDevice());
		break; }
	case Menu::gamepad: {
		int colFac = res.x / 4;
		int colWidth = res.x / 2 - 5;
		objects = {
			new Label(Object({0, 0}, {-1, -1}, {res.x, 30}, FIX_ANC | FIX_H | FIX_EX, Object::colorBackground), World::Input()->GamepadName(), Label::Align::left),
			new Label(Object({colFac, 75}, {colFac - colWidth / 2, 75}, {colWidth, 30}, FIX_Y | FIX_H), "Buttons (" + std::to_string(SDL_CONTROLLER_BUTTON_MAX) + ')', Label::Align::center),
			new Label(Object({colFac * 3, 75}, {colFac * 3 - colWidth / 2, 75}, {colWidth, 30}, FIX_Y | FIX_H), "Axes (" + std::to_string(SDL_CONTROLLER_AXIS_MAX) + ')', Label::Align::center),
			buttonList = new ScrollArea({colFac, 110}, {colFac - colWidth / 2, 110}, {colWidth, res.y - 110}, FIX_Y | FIX_EY),
			axisList = new ScrollArea({colFac * 3, 110}, {colFac * 3 - colWidth / 2, 110}, {colWidth, res.y - 110}, FIX_Y | FIX_EY),
			new Button(Object({0, 35}, {-1, -1}, {145, 30}, FIX_ANC | FIX_SIZ), &Program::EventOpenJoystickView, "Joystick", Label::Align::center)
		};
		if (World::Input()->IsHaptic())
			objects.push_back(new Button(Object({150, 35}, {-1, -1}, {145, 30}, FIX_ANC | FIX_SIZ), &Program::EventOpenHapticView, "Haptic", Label::Align::center));

		EventGamepadButton(World::Input()->CurDevice());
		EventGamepadAxis(World::Input()->CurDevice());
		break; }
	case Menu::haptic:
		curHRStrength = 0.5f;
		curHRLength = 1000;
		objects = {
			new Label(Object({0, 0}, {-1, -1}, {res.x, 30}, FIX_ANC | FIX_H | FIX_EX, Object::colorBackground), World::Input()->JoystickName(), Label::Align::left),
			new Label(Object({0, 110}, {-1, -1}, {155, 30}, FIX_ANC | FIX_SIZ, Object::colorBackground), "Strength", Label::Align::left),
			new HorSlider(Object({160, 110}, {-1, -1}, {res.x - 160, 30}, FIX_ANC | FIX_H | FIX_EX), curHRStrength, &Program::EventStrengthChanged),
			new Label(Object({0, 145}, {-1, -1}, {155, 30}, FIX_ANC | FIX_SIZ, Object::colorBackground), "Length (ms)", Label::Align::left),
			new LineEditor(Object({160, 145}, {-1, -1}, {res.x - 160, 30}, FIX_ANC | FIX_H | FIX_EX), std::to_string(curHRLength), LineEditor::Type::integer, &Program::EventLengthChanged),
			hTestButton = new Button(Object({0, 75}, {-1, -1}, {100, 30}, FIX_ANC | FIX_SIZ), &Program::EventHapticTest, "Test", Label::Align::center),
			new Button(Object({0, 35}, {-1, -1}, {145, 30}, FIX_ANC | FIX_SIZ), &Program::EventOpenJoystickView, "Joystick", Label::Align::center)
		};
		if (World::Input()->IsGamepad())
			objects.push_back(new Button(Object({150, 35}, {-1, -1}, {145, 30}, FIX_ANC | FIX_SIZ), &Program::EventOpenGamepadView, "Gamepad", Label::Align::center));
	}
	World::Window()->SwitchMenu(std::move(objects));
}

string Program::HexToStr(int num) {
	std::stringstream ss;
	ss << std::uppercase << std::hex << num;
	return ss.str();
}

string Program::ButtonStr(SDL_GameControllerButton button) {
	switch (button) {
	case SDL_CONTROLLER_BUTTON_A:
		return "A";
	case SDL_CONTROLLER_BUTTON_B:
		return "B";
	case SDL_CONTROLLER_BUTTON_X:
		return "X";
	case SDL_CONTROLLER_BUTTON_Y:
		return "Y";
	case SDL_CONTROLLER_BUTTON_BACK:
		return "Back";
	case SDL_CONTROLLER_BUTTON_GUIDE:
		return "Guide";
	case SDL_CONTROLLER_BUTTON_START:
		return "Start";
	case SDL_CONTROLLER_BUTTON_LEFTSTICK:
		return "Stick L";
	case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
		return "Stick R";
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		return "Shoulder L";
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		return "Shoulder R";
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		return "Up";
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		return "Down";
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		return "Left";
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		return "Right";
	}
	return "Invalid";
}

string Program::AxisStr(SDL_GameControllerAxis axis) {
	switch (axis) {
	case SDL_CONTROLLER_AXIS_LEFTX:
		return "Left X";
	case SDL_CONTROLLER_AXIS_LEFTY:
		return "Left Y";
	case SDL_CONTROLLER_AXIS_RIGHTX:
		return "Right X";
	case SDL_CONTROLLER_AXIS_RIGHTY:
		return "Right Y";
	case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
		return "Trigger L";
	case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
		return "Trigger R";
	}
	return "Invalid";
}
