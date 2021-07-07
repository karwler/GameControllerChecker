#include "windowSys.h"
#include "inputSys.h"
#include "objects.h"
#include "program.h"
#include <filesystem>
namespace fs = std::filesystem;

// FONT

FontSet::FontSet(const char* name) {
	TTF_Font* tmp = nullptr;
#ifdef _WIN32
	for (const fs::path& drc : {fs::path(L"."), fs::path(_wgetenv(L"SystemDrive")) / L"\\Windows\\Fonts"}) {
#else
	for (const fs::path& drc : {fs::u8path("."), fs::u8path(getenv("HOME")) / ".fonts", fs::u8path("/usr/share/fonts")}) {
#endif
		try {
			for (const fs::directory_entry& it : fs::recursive_directory_iterator(drc, fs::directory_options::follow_directory_symlink | fs::directory_options::skip_permission_denied))
				if (!SDL_strcasecmp(it.path().stem().u8string().c_str(), name)) {
					string path = it.path().u8string();
					if (tmp = TTF_OpenFont(path.c_str(), fontTestHeight); tmp) {
						file = path;
						break;
					}
				}
		} catch (...) {}
	}
	if (!tmp)
		throw std::runtime_error("Couldn't find font");

	int size;
	heightScale = !TTF_SizeUTF8(tmp, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ`~!@#$%^&*()_+-=[]{}'\\\"|;:,.<>/?", nullptr, &size) ? float(fontTestHeight) / float(size) : 1.f;
	TTF_CloseFont(tmp);
}

FontSet::~FontSet() {
	Clear();
}

void FontSet::Clear() {
	for (auto [size, font] : fonts)
		TTF_CloseFont(font);
	fonts.clear();
}

TTF_Font* FontSet::Get(int size) {
	size = int(float(size) * heightScale);
	return fonts.count(size) ? fonts.at(size) : AddSize(size);
}

TTF_Font* FontSet::AddSize(int size) {
	TTF_Font* font = TTF_OpenFont(file.c_str(), size);
	if (!font)
		throw std::runtime_error(TTF_GetError());
	return fonts.insert(pair(size, font)).first->second;
}

SDL_Point FontSet::TextSize(const string& text, int size) {
	SDL_Point siz{};
	if (TTF_Font* font = Get(size))
		TTF_SizeUTF8(font, text.c_str(), &siz.x, &siz.y);
	return siz;
}

// WINDOW SYS

WindowSys::WindowSys() = default;	// needs to be here to make unique pointers happy

WindowSys::~WindowSys() = default;	// ^

int WindowSys::Start() {
	inputSys = nullptr;
	fonts = nullptr;
	window = nullptr;
	renderer = nullptr;
	int retval = EXIT_SUCCESS;
	try {
		Run();
	} catch (const std::runtime_error& exc) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", exc.what(), window);
		retval = EXIT_FAILURE;
	} catch (...) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "unknown error", window);
		retval = EXIT_FAILURE;
	}

	for (Object* it : objects)
		delete it;
	delete inputSys;
	delete fonts;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	renderer = nullptr;
	window = nullptr;
	TTF_Quit();
	SDL_Quit();
	return retval;
}

void WindowSys::Run() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC))
		throw std::runtime_error(SDL_GetError());
	if (TTF_Init())
		throw std::runtime_error(TTF_GetError());

	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
	SDL_EventState(SDL_FINGERMOTION, SDL_DISABLE);
	SDL_EventState(SDL_TEXTEDITING, SDL_DISABLE);
	SDL_EventState(SDL_KEYMAPCHANGED, SDL_DISABLE);
	SDL_EventState(SDL_JOYBALLMOTION, SDL_DISABLE);
	SDL_EventState(SDL_CONTROLLERDEVICEADDED, SDL_DISABLE);
	SDL_EventState(SDL_CONTROLLERDEVICEREMOVED, SDL_DISABLE);
	SDL_EventState(SDL_CONTROLLERDEVICEREMAPPED, SDL_DISABLE);
	SDL_EventState(SDL_DOLLARGESTURE, SDL_DISABLE);
	SDL_EventState(SDL_DOLLARRECORD, SDL_DISABLE);
	SDL_EventState(SDL_MULTIGESTURE, SDL_DISABLE);
	SDL_EventState(SDL_CLIPBOARDUPDATE, SDL_DISABLE);
	SDL_EventState(SDL_DROPFILE, SDL_DISABLE);
	SDL_EventState(SDL_DROPTEXT, SDL_DISABLE);
	SDL_EventState(SDL_DROPBEGIN, SDL_DISABLE);
	SDL_EventState(SDL_DROPCOMPLETE, SDL_DISABLE);
	SDL_EventState(SDL_AUDIODEVICEADDED, SDL_DISABLE);
	SDL_EventState(SDL_AUDIODEVICEREMOVED, SDL_DISABLE);
	SDL_EventState(SDL_RENDER_TARGETS_RESET, SDL_DISABLE);
	SDL_EventState(SDL_RENDER_DEVICE_RESET, SDL_DISABLE);

	if (window = SDL_CreateWindow("Controller Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_RESIZABLE); !window)
		throw std::runtime_error(SDL_GetError());
	if (renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); !renderer)
		throw std::runtime_error(SDL_GetError());
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetWindowMinimumSize(window, 200, 200);

	fonts = new FontSet;
	inputSys = new InputSys(this);
	run = true;
	inputSys->Prog()->EventControllersChanged(true);
	for (uint32_t oldTime = SDL_GetTicks(); run;) {
		uint32_t newTime = SDL_GetTicks();
		uint32_t dMsec = newTime - oldTime;
		oldTime = newTime;

		SDL_SetRenderDrawColor(renderer, Object::colorBackground.r, Object::colorBackground.g, Object::colorBackground.b, Object::colorBackground.a);
		SDL_RenderClear(renderer);
		for (Object* obj : objects)
			obj->Draw();
		SDL_RenderPresent(renderer);

		inputSys->Tick(dMsec);
		SDL_Event event;
		uint32_t timeout = SDL_GetTicks() + eventTimeout;
		do {
			if (!SDL_PollEvent(&event))
				break;
			HandleEvent(event);
		} while (!SDL_TICKS_PASSED(SDL_GetTicks(), timeout));
	}
}

void WindowSys::HandleEvent(const SDL_Event& event) {
	switch (event.type) {
	case SDL_QUIT:
		Close();
		break;
	case SDL_WINDOWEVENT:
		WindowEvent(event.window);
		break;
	case SDL_KEYDOWN:
		inputSys->KeypressEvent(event.key);
		break;
	case SDL_TEXTINPUT:
		inputSys->TextEvent(event.text);
		break;
	case SDL_MOUSEMOTION:
		inputSys->MouseMotionEvent(event.motion, objects);
		break;
	case SDL_MOUSEBUTTONDOWN:
		inputSys->MouseButtonDownEvent(event.button);
		break;
	case SDL_MOUSEBUTTONUP:
		inputSys->MouseButtonUpEvent(event.button, objects);
		break;
	case SDL_MOUSEWHEEL:
		inputSys->MouseWheelEvent(event.wheel);
		break;
	case SDL_JOYAXISMOTION:
		inputSys->Prog()->EventJoystickAxis(event.jaxis.which);
		break;
	case SDL_JOYHATMOTION:
		inputSys->Prog()->EventJoystickHat(event.jhat.which);
		break;
	case SDL_JOYBUTTONDOWN: case SDL_JOYBUTTONUP:
		inputSys->Prog()->EventJoystickButton(event.jbutton.which);
		break;
	case SDL_JOYDEVICEADDED:
		inputSys->AddController(event.jdevice.which);
		break;
	case SDL_JOYDEVICEREMOVED:
		inputSys->DelController(event.jdevice.which);
		break;
	case SDL_CONTROLLERAXISMOTION:
		inputSys->Prog()->EventGamepadAxis(event.caxis.which);
		break;
	case SDL_CONTROLLERBUTTONDOWN: case SDL_CONTROLLERBUTTONUP:
		inputSys->Prog()->EventGamepadButton(event.cbutton.which);
	}
}

void WindowSys::WindowEvent(const SDL_WindowEvent& wevent) {
	switch (wevent.event) {
	case SDL_WINDOWEVENT_EXPOSED:
		inputSys->SimulateMouseMove(objects);
		break;
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		for (Object* obj : objects)
			obj->OnResize();
	}
}

void WindowSys::SwitchMenu(vector<Object*>&& objs) {
	for (Object* it : objects)
		delete it;
	objects = std::move(objs);
	inputSys->PostMenuSwitch(objects);
}

SDL_Texture* WindowSys::RenderText(const string& str, int height) {
	if (SDL_Surface* img = TTF_RenderUTF8_Blended(fonts->Get(height), str.c_str(), Object::colorText)) {
		SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, img);
		SDL_FreeSurface(img);
		return tex;
	}
	return nullptr;
}

SDL_Point WindowSys::Resolution() const {
	SDL_Point res;
	SDL_GetWindowSize(window, &res.x, &res.y);
	return res;
}

void WindowSys::DrawRect(const SDL_Rect& rect, SDL_Color color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

void WindowSys::DrawText(SDL_Texture* tex, SDL_Point pos, const SDL_Rect& frame) {
	if (!tex)
		return;

	int w, h;
	SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
	SDL_Rect dst = {pos.x, pos.y, w, h};
	SDL_Rect crop = CropRect(dst, frame);
	SDL_Rect src = {crop.x, crop.y, w - crop.w, h - crop.h};
	SDL_RenderCopy(renderer, tex, &src, &dst);
}

SDL_Rect WindowSys::CropRect(SDL_Rect& rect, const SDL_Rect& frame) {
	SDL_Rect isct;
	if (!SDL_IntersectRect(&rect, &frame, &isct))
		return rect = {};

	SDL_Point te = {rect.x + rect.w, rect.y + rect.h}, ie = {isct.x + isct.w, isct.y + isct.h};
	SDL_Rect crop;
	crop.x = isct.x > rect.x ? isct.x - rect.x : 0;
	crop.y = isct.y > rect.y ? isct.y - rect.y : 0;
	crop.w = ie.x < te.x ? te.x - ie.x + crop.x : 0;
	crop.h = ie.y < te.y ? te.y - ie.y + crop.y : 0;
	rect = isct;
	return crop;
}

void WindowSys::Close() {
	run = false;
}

InputSys* WindowSys::Input() {
	return inputSys;
}

FontSet* WindowSys::Fonts() {
	return fonts;
}
