#include "engine/world.h"

// FONT

bool FontSet::Initialize(const string& FILE) {
	TTF_Font* tmp = TTF_OpenFont(FILE.c_str(), 72);
	if (!tmp) {
		cerr << "couldn't open font " << file << endl << TTF_GetError() << endl;
		file.clear();
		return false;
	}
	TTF_CloseFont(tmp);
	file = FILE;
	return true;
}

void FontSet::Clear() {
	for (const pair<int, TTF_Font*>& it : fonts)
		TTF_CloseFont(it.second);
	fonts.clear();
}

bool FontSet::CanRun() const {
	return !file.empty();
}

TTF_Font* FontSet::Get(int size) {
	return (CanRun() && fonts.count(size) == 0) ? AddSize(size) : fonts.at(size);
}

TTF_Font* FontSet::AddSize(int size) {
	TTF_Font* font = TTF_OpenFont(file.c_str(), size);
	if (font)
		fonts.insert(make_pair(size, font));
	else
		cerr << "couldn't load font " << file << endl << TTF_GetError() << endl;
	return font;
}

vec2i FontSet::TextSize(const string& text, int size) {
	vec2i siz;
	TTF_Font* font = Get(size);
	if (font)
		TTF_SizeUTF8(font, text.c_str(), &siz.x, &siz.y);
	return siz;
}

// TEXT

Text::Text(const string& TXT, const vec2i& POS, int H, vec4c CLR, int HSCAL) :
	pos(POS),
	color(CLR),
	text(TXT)
{
	height = (HSCAL == 0) ? H : H - H/HSCAL;
}

void Text::SetPosToRect(const SDL_Rect& rect, ETextAlign align, int offset) {
	int len = size().x;

	if (align == ETextAlign::left)
		pos.x = rect.x+offset;
	else if (align == ETextAlign::center)
		pos.x = rect.x + (rect.w-len)/2;
	else if (align == ETextAlign::right)
		pos.x = rect.x+rect.w-len-offset;
	pos.y = rect.y;
}

vec2i Text::size() const {
	return World::scene()->Fonts().TextSize(text, height);
}

// TEXT EDIT

TextEdit::TextEdit(const string& TXT, ETextType TYPE, size_t CPOS) :
	type(TYPE),
	text(TXT) {
	SetCursor(CPOS);
	CheckText();
}

size_t TextEdit::CursorPos() const {
	return cpos;
}

void TextEdit::SetCursor(size_t pos) {
	cpos = (pos > text.length()) ? text.length() : pos;
}

void TextEdit::MoveCursor(bool right, bool loop) {
	if (loop) {
		if (right)
			cpos = (cpos == text.length()) ? 0 : cpos+1;
		else
			cpos = (cpos == 0) ? text.length() : cpos-1;
	} else {
		if (right && cpos != text.length())
			cpos++;
		else if (!right && cpos != 0)
			cpos--;
	}
}

string TextEdit::Text() const {
	return text;
}

void TextEdit::Text(const string& str, bool resetCpos) {
	text = str;

	if (resetCpos)
		cpos = 0;
	else
		CheckCaret();
}

void TextEdit::Add(const string& str) {
	text.insert(cpos, str);
	cpos += str.length();
	CheckText();
}

void TextEdit::Delete(bool current) {
	if (current) {
		if (cpos != text.length())
			text.erase(cpos, 1);
	} else if (cpos != 0) {
		cpos--;
		text.erase(cpos, 1);
	}
}

void TextEdit::CheckCaret() {
	if (cpos > text.length())
		cpos = text.length();
}

void TextEdit::CheckText() {
	if (type == ETextType::integer)
		CleanIntString(text);
	else if (type == ETextType::floating)
		CleanFloatString(text);
	else
		return;
	CheckCaret();
}

void TextEdit::CleanIntString(string& str) {
	for (size_t i = 0; i!=str.length(); i++)
		if (str[i] < '0' || str[i] > '9') {
			str.erase(i, 1);
			i--;
		}
}

void TextEdit::CleanFloatString(string& str) {
	bool foundDot = false;
	for (size_t i = 0; i!=str.length(); i++)
		if (str[i] < '0' || str[i] > '9') {
			if (str[i] == '.' && !foundDot)
				foundDot = true;
			else {
				str.erase(i, 1);
				i--;
			}
		}
}

// CONTROLLER

Controller::Controller() :
	joystick(nullptr),
	gamepad(nullptr),
	haptic(nullptr)
{}

bool Controller::Open(int id) {
	if (SDL_IsGameController(id))
		gamepad = SDL_GameControllerOpen(id);

	joystick = gamepad ? SDL_GameControllerGetJoystick(gamepad) : SDL_JoystickOpen(id);
	if (SDL_JoystickIsHaptic(joystick)) {
		haptic = SDL_HapticOpenFromJoystick(joystick);
		SDL_HapticRumbleInit(haptic);
	}
	return joystick;
}

void Controller::Close() {
	if (haptic) {
		SDL_HapticClose(haptic);
		haptic = nullptr;
	}
	if (gamepad) {
		SDL_GameControllerClose(gamepad);
		gamepad = nullptr;
		joystick = nullptr;
	} else if (joystick) {
		SDL_JoystickClose(joystick);
		joystick = nullptr;
	}
}

// EXCEPTION

Exception::Exception(const string& MSG, int RV) :
	message(MSG),
	retval(RV)
{}

void Exception::Display() {
	cerr << "ERROR: " << message << " (code " << retval << ")" << endl;
}
