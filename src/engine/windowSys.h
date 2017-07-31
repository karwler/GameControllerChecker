#pragma once

#include "utils/scrollArea.h"

class WindowSys {
public:
	WindowSys();
	~WindowSys();

	void CreateWindow();
	void DestroyWindow();
	void DrawObjects(const vector<Object*>& objects);
	void WindowEvent(const SDL_WindowEvent& window);
	
	vec2i Resolution() const;

private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	void PassDrawObject(Object* obj);
	void DrawObject(const SDL_Rect& bg, vec4c bgColor, const Text& text);
	void DrawObject(LineEditor* obj);
	void DrawObject(HorSlider* obj);
	void DrawObject(ScrollArea* obj);

	void DrawRect(const SDL_Rect& rect, vec4c color);
	void DrawText(const Text& txt, const SDL_Rect& crop={0, 0, 0, 0});
};
