#include "engine/world.h"

// events

Program::Program() :
	curDevice(-1)
{}

void Program::Event_OpenJoystickView() {
	if (World::inputSys()->IsRumbling())
		World::inputSys()->StopRumble(curDevice);

	if (curDevice == -1 && World::inputSys()->NumControllers() != 0)
		curDevice = 0;
	SwitchScene(EMenu::joystick);
}

void Program::Event_OpenNextJoystick() {
	if (World::inputSys()->IsRumbling())
		World::inputSys()->StopRumble(curDevice);

	curDevice = World::inputSys()->GetNextController(curDevice);
	SwitchScene();
}

void Program::Event_OpenPrevJoystick() {
	if (World::inputSys()->IsRumbling())
		World::inputSys()->StopRumble(curDevice);

	curDevice = World::inputSys()->GetPrevController(curDevice);
	SwitchScene();
}

void Program::Event_OpenGamepadView() {
	if (World::inputSys()->IsRumbling())
		World::inputSys()->StopRumble(curDevice);
	
	SwitchScene(EMenu::gamepad);
}

void Program::Event_OpenHapticView() {
	if (World::inputSys()->IsRumbling())
		World::inputSys()->StopRumble(curDevice);

	SwitchScene(EMenu::haptic);
}

void Program::Event_JoystickButton() {
	if (curMenu == EMenu::joystick && curDevice != -1) {
		vector<uint8> buttons = World::inputSys()->GetJoystickButtons(curDevice);
		vector<string> items(buttons.size());

		for (size_t i=0; i!=items.size(); i++)
			items[i] = to_string(buttons[i]);

		buttonList->Items(items);
		World::engine()->SetRedrawNeeded();
	}
}

void Program::Event_JoystickHat() {
	if (curMenu == EMenu::joystick && curDevice != -1) {
		vector<pair<uint8, uint8>> hats = World::inputSys()->GetJoystickHats(curDevice);
		vector<string> items(hats.size());

		for (size_t i=0; i!=items.size(); i++) {
			string label = to_string(hats[i].first) + ": " + hexToStr(hats[i].second) + " (";
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
				label.resize(label.size()-1);
			}
			items[i] = label + ")";
		}
		hatList->Items(items);
		World::engine()->SetRedrawNeeded();
	}
}

void Program::Event_JoystickAxis() {
	if (curMenu == EMenu::joystick && curDevice != -1) {
		vector<pair<uint8, int16>> axes = World::inputSys()->GetJoystickAxes(curDevice);
		vector<string> items(axes.size());

		for (size_t i=0; i!=items.size(); i++)
			items[i] = to_string(axes[i].first) + ": " + to_string(axes[i].second);

		axisList->Items(items);
		World::engine()->SetRedrawNeeded();
	}
}

void Program::Event_GamepadButton() {
	if (curMenu == EMenu::gamepad && curDevice != -1) {
		vector<SDL_GameControllerButton> buttons = World::inputSys()->GetGamepadButtons(curDevice);
		vector<string> items(buttons.size());

		for (size_t i=0; i!=items.size(); i++)
			items[i] = gcButtonStr(buttons[i]);

		buttonList->Items(items);
		World::engine()->SetRedrawNeeded();
	}
}

void Program::Event_GamepadAxis() {
	if (curMenu == EMenu::gamepad && curDevice != -1) {
		vector<pair<SDL_GameControllerAxis, int16>> axes = World::inputSys()->GetGamepadAxes(curDevice);
		vector<string> items(axes.size());

		for (size_t i=0; i!=items.size(); i++)
			items[i] = gcAxisStr(axes[i].first) + ": " + to_string(axes[i].second);

		axisList->Items(items);
		World::engine()->SetRedrawNeeded();
	}
}

void Program::Event_HapticTest() {
	if (World::inputSys()->IsRumbling())
		World::inputSys()->StopRumble(curDevice);
	else
		World::inputSys()->StartRumble(curDevice, curHRStrength, curHRLength);
}

void Program::Event_TestStarted() {
	hTestButton->text = "Cancel";
	World::engine()->SetRedrawNeeded();
}

void Program::Event_TestStopped() {
	hTestButton->text = "Test";
	World::engine()->SetRedrawNeeded();
}

void Program::Event_StrengthChanged(float value) {
	curHRStrength = value;
}

void Program::Event_LengthChanged(const string& text) {
	curHRLength = stoul(text);
}

void Program::Event_ControllersChanged() {
	if (World::inputSys()->NumControllers() == 0)
		curDevice = -1;
	else if (curDevice >= World::inputSys()->NumControllers())
		curDevice = 0;
	SwitchScene(EMenu::joystick);
}

// scene building

void Program::SwitchScene(EMenu newMenu) {
	curMenu = newMenu;
	SwitchScene();
}

void Program::SwitchScene() {
	World::scene()->Fonts().Clear();
	buttonList = nullptr;
	hatList = nullptr;
	axisList = nullptr;
	hTestButton = nullptr;

	vec2i res = World::winSys()->Resolution();
	vector<Object*> objects;
	
	switch (curMenu) {
	case EMenu::joystick: {
		if (curDevice == -1)
			objects.push_back(new Label(Object(res/2, vec2i(0, res.y/2-30), vec2i(res.x, 60), FIX_SIZ, DEFAULT_COLOR_BACKGROUND), "No controllers detected :(", ETextAlign::center));
		else {
			int colFac = res.x/6;
			int colWidth = res.x/3 - 5;

			int numButtons = World::inputSys()->NumJButtons(curDevice);
			int numHats = World::inputSys()->NumJHats(curDevice);
			int numAxes = World::inputSys()->NumJAxes(curDevice);

			buttonList = new ScrollArea(Object(vec2i(colFac, 110), vec2i(colFac-colWidth/2, 110), vec2i(colWidth, res.y-110), FIX_Y | FIX_EY));
			hatList = new ScrollArea(Object(vec2i(colFac*3, 110), vec2i(colFac*3-colWidth/2, 110), vec2i(colWidth, res.y-110), FIX_Y | FIX_EY));
			axisList = new ScrollArea(Object(vec2i(colFac*5, 110), vec2i(colFac*5-colWidth/2, 110), vec2i(colWidth, res.y-110), FIX_Y | FIX_EY));

			objects = {
				new Button(Object(vec2i(0, 0), -1, vec2i(30, 30), FIX_ANC | FIX_SIZ), &Program::Event_OpenPrevJoystick, "<", ETextAlign::center),
				new Label(Object(vec2i(35, 0), -1, vec2i(130, 30), FIX_ANC | FIX_SIZ, DEFAULT_COLOR_DARKENED), to_string(curDevice+1) +" / "+ to_string(World::inputSys()->NumControllers()), ETextAlign::center),
				new Button(Object(vec2i(170, 0), -1, vec2i(30, 30), FIX_ANC | FIX_SIZ), &Program::Event_OpenNextJoystick, ">", ETextAlign::center),
				new Label(Object(vec2i(210, 0), -1, vec2i(res.x-210, 30), FIX_ANC | FIX_H | FIX_EX, DEFAULT_COLOR_BACKGROUND), World::inputSys()->JoystickName(curDevice), ETextAlign::left),
				new Label(Object(vec2i(colFac, 75), vec2i(colFac-colWidth/2, 75), vec2i(colWidth, 30), FIX_Y | FIX_H), "Buttons ("+to_string(numButtons)+")", ETextAlign::center),
				new Label(Object(vec2i(colFac*3, 75), vec2i(colFac*3-colWidth/2, 75), vec2i(colWidth, 30), FIX_Y | FIX_H), "Hats ("+to_string(numHats)+")", ETextAlign::center),
				new Label(Object(vec2i(colFac*5, 75), vec2i(colFac*5-colWidth/2, 75), vec2i(colWidth, 30), FIX_Y | FIX_H), "Axes ("+to_string(numAxes)+")", ETextAlign::center),
				buttonList,
				hatList,
				axisList
			};
			if (World::inputSys()->JIsGamepad(curDevice))
				objects.push_back(new Button(Object(vec2i(0, 35), -1, vec2i(145, 30), FIX_ANC | FIX_SIZ), &Program::Event_OpenGamepadView, "Gamepad", ETextAlign::center));
			if (World::inputSys()->JIsHaptic(curDevice))
				objects.push_back(new Button(Object(vec2i(150, 35), -1, vec2i(145, 30), FIX_ANC | FIX_SIZ), &Program::Event_OpenHapticView, "Haptic", ETextAlign::center));

			Event_JoystickButton();
			Event_JoystickHat();
			Event_JoystickAxis();
		}
		break; }
	case EMenu::gamepad: {
		int colFac = res.x/4;
		int colWidth = res.x/2 - 5;

		buttonList = new ScrollArea(Object(vec2i(colFac, 110), vec2i(colFac-colWidth/2, 110), vec2i(colWidth, res.y-110), FIX_Y | FIX_EY));
		axisList = new ScrollArea(Object(vec2i(colFac*3, 110), vec2i(colFac*3-colWidth/2, 110), vec2i(colWidth, res.y-110), FIX_Y | FIX_EY));

		objects = {
			new Label(Object(vec2i(0, 0), -1, vec2i(res.x, 30), FIX_ANC | FIX_H | FIX_EX, DEFAULT_COLOR_BACKGROUND), World::inputSys()->GamepadName(curDevice), ETextAlign::left),
			new Label(Object(vec2i(colFac, 75), vec2i(colFac-colWidth/2, 75), vec2i(colWidth, 30), FIX_Y | FIX_H), "Buttons ("+to_string(SDL_CONTROLLER_BUTTON_MAX)+")", ETextAlign::center),
			new Label(Object(vec2i(colFac*3, 75), vec2i(colFac*3-colWidth/2, 75), vec2i(colWidth, 30), FIX_Y | FIX_H), "Axes ("+to_string(SDL_CONTROLLER_AXIS_MAX)+")", ETextAlign::center),
			buttonList,
			axisList
		};
		objects.push_back(new Button(Object(vec2i(0, 35), -1, vec2i(145, 30), FIX_ANC | FIX_SIZ), &Program::Event_OpenJoystickView, "Joystick", ETextAlign::center));
		if (World::inputSys()->JIsHaptic(curDevice))
			objects.push_back(new Button(Object(vec2i(150, 35), -1, vec2i(145, 30), FIX_ANC | FIX_SIZ), &Program::Event_OpenHapticView, "Haptic", ETextAlign::center));

		Event_GamepadButton();
		Event_GamepadAxis();
		break; }
	case EMenu::haptic: {
		curHRStrength = 0.5f;
		curHRLength = 1000;

		hTestButton = new Button(Object(vec2i(0, 75), -1, vec2i(100, 30), FIX_ANC | FIX_SIZ), &Program::Event_HapticTest, "Test", ETextAlign::center);

		objects = {
			new Label(Object(vec2i(0, 0), -1, vec2i(res.x, 30), FIX_ANC | FIX_H | FIX_EX, DEFAULT_COLOR_BACKGROUND), World::inputSys()->JoystickName(curDevice), ETextAlign::left),
			new Label(Object(vec2i(0, 110), -1, vec2i(155, 30), FIX_ANC | FIX_SIZ, DEFAULT_COLOR_BACKGROUND), "Strangth", ETextAlign::left),
			new HorSlider(Object(vec2i(160, 110), -1, vec2i(res.x-160, 30), FIX_ANC | FIX_H | FIX_EX), curHRStrength, &Program::Event_StrengthChanged),
			new Label(Object(vec2i(0, 145), -1, vec2i(155, 30), FIX_ANC | FIX_SIZ, DEFAULT_COLOR_BACKGROUND), "Length (ms)", ETextAlign::left),
			new LineEditor(Object(vec2i(160, 145), -1, vec2i(res.x-160, 30), FIX_ANC | FIX_H | FIX_EX), to_string(curHRLength), ETextType::integer, &Program::Event_LengthChanged),
			hTestButton
		};
		objects.push_back(new Button(Object(vec2i(0, 35), -1, vec2i(145, 30), FIX_ANC | FIX_SIZ), &Program::Event_OpenJoystickView, "Joystick", ETextAlign::center));
		if (World::inputSys()->JIsGamepad(curDevice))
			objects.push_back(new Button(Object(vec2i(150, 35), -1, vec2i(145, 30), FIX_ANC | FIX_SIZ), &Program::Event_OpenGamepadView, "Gamepad", ETextAlign::center));
		break; }
	}
	World::scene()->SwitchMenu(objects);
}
