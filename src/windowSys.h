#pragma once

#include "inputSys.h"

class FontSet {
public:
	FontSet(const string& name="arial");
	~FontSet();

	void Clear();
	TTF_Font* Get(int size);
	SDL_Point TextSize(const string& text, int size);

private:
	static constexpr int fontTestHeight = 100;

	string file;
	umap<int, TTF_Font*> fonts;
	float heightScale;	// for scaling down font size to fit requested height

	TTF_Font* AddSize(int size);
};

class WindowSys {
public:
	int Start();
	void Close();
	void SwitchMenu(vector<Object*>&& objs);

	InputSys* Input();
	FontSet* Fonts();
	SDL_Texture* RenderText(const string& str, int height);
	SDL_Point Resolution() const;
	void DrawRect(const SDL_Rect& rect, SDL_Color color);
	void DrawText(SDL_Texture* tex, SDL_Point pos, const SDL_Rect& frame);

private:
	InputSys* inputSys;
	FontSet* fonts;
	SDL_Window* window;
	SDL_Renderer* renderer;
	vector<Object*> objects;
	bool run;

	void Run();
	void HandleEvent(const SDL_Event& event);
	void WindowEvent(const SDL_WindowEvent& wevent);
	static SDL_Rect CropRect(SDL_Rect& rect, const SDL_Rect& frame);
};
