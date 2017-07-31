#pragma once

#include "utils.h"
#include "prog/defaults.h"
#include "kklib/vec2.h"
#include "kklib/vec3.h"
#include "kklib/vec4.h"

// enumerations

enum class ETextAlign : uint8 {
	left,
	center,
	right
};

enum class ETextType : uint8 {
	text,
	integer,
	floating
};

// text related stuff

class FontSet {
public:
	bool Initialize(const string& FILE="");
	void Clear();

	bool CanRun() const;
	TTF_Font* Get(int size);
	vec2i TextSize(const string& text, int size);

private:
	string file;
	map<int, TTF_Font*> fonts;

	TTF_Font* AddSize(int size);
};

struct Text {
	Text(const string& TXT="", const vec2i& POS=0, int H=50, vec4c CLR=DEFAULT_COLOR_TEXT, int HSCAL=8);

	vec2i pos;
	int height;
	vec4c color;
	string text;

	void SetPosToRect(const SDL_Rect& rect, ETextAlign align, int offset=5);
	vec2i size() const;
};

class TextEdit {
public:
	TextEdit(const string& TXT = "", ETextType TYPE = ETextType::text, size_t CPOS = 0);

	size_t CursorPos() const;
	void SetCursor(size_t pos);
	void MoveCursor(bool right, bool loop = false);

	string Text() const;
	void Text(const string& str, bool resetCpos = true);
	void Add(const string& str);
	void Delete(bool current);

private:
	size_t cpos;
	ETextType type;
	string text;

	void CheckCaret();
	void CheckText();

	void CleanIntString(string& str);
	void CleanFloatString(string& str);
};

// some random types

struct Controller {
	Controller();

	SDL_Joystick* joystick;
	SDL_GameController* gamepad;
	SDL_Haptic* haptic;

	bool Open(int id);
	void Close();
};

struct Exception {
	Exception(const string& MSG="", int RV=-1);

	string message;
	int retval;

	void Display();
};
