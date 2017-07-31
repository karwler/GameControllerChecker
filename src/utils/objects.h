#pragma once

#include "types.h"

enum EFix : uint8 {		// fix basically means "store value in pixels instead of a 'percentage'" (which would be a value between 0 and 1)
	FIX_NONE = 0x0,
	FIX_X    = 0x1,
	FIX_Y    = 0x2,
	FIX_ANC  = 0x3,		// fix anchor's position
	FIX_W    = 0x4,
	FIX_H    = 0x8,
	FIX_SIZ  = 0xC,		// keep distance between pos/end and anchor (don't resize)
	FIX_PX   = 0x10,
	FIX_PY   = 0x20,
	FIX_POS  = 0x30,	// start point won't be dependent on size (overrides fix_size flag when accessing start point)
	FIX_EX   = 0x40,
	FIX_EY   = 0x80,
	FIX_END  = 0xC0,	// end point won't be dependent on size (overrides fix_size flag when accessing end point)
	FIX_ALL  = 0xFF
};
EFix operator~(EFix a);
EFix operator&(EFix a, EFix b);
EFix operator&=(EFix& a, EFix b);
EFix operator^(EFix a, EFix b);
EFix operator^=(EFix& a, EFix b);
EFix operator|(EFix a, EFix b);
EFix operator|=(EFix& a, EFix b);

class Object {
public:
	Object(const vec2i& ANC=0, vec2i POS=-1, const vec2i& SIZ=0, EFix FIX=FIX_NONE, vec4c CLR=DEFAULT_COLOR_RECTANGLE);
	virtual ~Object();
	
	SDL_Rect getRect() const;
	vec2i Anchor() const;
	void Anchor(const vec2i& newPos);
	vec2i Pos() const;
	void Pos(const vec2i& newPos);
	vec2i End() const;
	void End(const vec2i& newPos);
	vec2i Size() const;
	void Size(const vec2i& newSize);	// only modifies value of end, which means that it doesn't affect pos, aka. expands the rect to the right/bottom

	vec4c color;
protected:
	EFix fix;
	vec2f anchor;
	vec2f pos, end;				// distance between boundries and anchor point
};

class Label : public Object {
public:
	Label(const Object& BASE=Object(), const string& TXT="", ETextAlign ALG=ETextAlign::left);
	virtual ~Label();

	Text getText() const;

	ETextAlign align;
	string text;
};

class Button : public Label {
public:
	Button(const Object& BASE=Object(), void (Program::*CALLB)()=nullptr, const string& TXT="", ETextAlign ALG=ETextAlign::left);
	virtual ~Button();

	void OnClick();
	void Callback(void (Program::*func)());

protected:
	void (Program::*callback)();
};

class LineEditor : public Object {
public:
	LineEditor(const Object& BASE=Object(), const string& TXT="", ETextType TYPE=ETextType::text, void (Program::*KCALL)(const string&)=nullptr, void (Program::*CCALL)()=nullptr);
	virtual ~LineEditor();

	void OnClick();
	void OnKeypress(SDL_Scancode key);
	void OnText(const char* text);

	void Confirm();
	void Cancel();
	int TextPos() const;
	void ResetTextPos();
	TextEdit& Editor();
	Text getText() const;		// warning: text doesn't have global positioning
	SDL_Rect getCaret() const;	// warning: caret doesn't have global positioning

private:
	int textPos;
	TextEdit editor;
	void (Program::*okCall)(const string&);
	void (Program::*cancelCall)();

	void CheckCaretRight();
	void CheckCaretLeft();
};

class HorSlider : public Object {
public:
	HorSlider(const Object& BASE=Object(), float VAL=0.f, void (Program::*CAL)(float)=nullptr, int SLW=20);
	virtual ~HorSlider();

	void DragSlider(int xpos);

	SDL_Rect Slider() const;
	int SliderX() const;
	int SliderW() const;

	int diffSliderMouseX;
private:
	const int sliderW;
	void (Program::*callb)(float);
	float value;
};
