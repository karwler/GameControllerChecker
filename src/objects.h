#pragma once

#include "utils.h"

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

class Object {
public:
	Object(SDL_Point ANC={}, SDL_Point POS={-1, -1}, SDL_Point SIZ={}, EFix FIX=FIX_NONE, SDL_Color CLR=colorRectangle);
	virtual ~Object() = default;

	virtual void Draw();
	virtual void OnResize() {}
	SDL_Rect Rect() const;
	SDL_Point Anchor() const;
	void Anchor(SDL_Point newPos);
	SDL_Point Pos() const;
	void Pos(SDL_Point newPos);
	SDL_Point End() const;
	void End(SDL_Point newPos);
	SDL_Point Size() const;
	void Size(SDL_Point newSize);	// only modifies value of end, which means that it doesn't affect pos, aka. expands the rect to the right/bottom

	static constexpr SDL_Color colorBackground = {10, 10, 10, 255};
	static constexpr SDL_Color colorRectangle = {90, 90, 90, 255};
	static constexpr SDL_Color colorLight = {120, 120, 120, 255};
	static constexpr SDL_Color colorDark = {60, 60, 60, 255};
	static constexpr SDL_Color colorText = {210, 210, 210, 255};

	SDL_Color color;
protected:
	float anchorX, anchorY;	// distance between boundaries and anchor point
	float posX, posY;
	float endX, endY;
	EFix fix;

	static SDL_Color GetColor(SDL_Color clr, bool light);

	static constexpr int offset = 5;
private:
	static constexpr float highlight = 1.2f;
};

class Label : public Object {
public:
	enum class Align : uint8 {
		left,
		center,
		right
	};

	Label(const Object& BASE=Object(), string&& TXT="", Align ALG=Align::left);
	virtual ~Label() override;

	virtual void Draw() override;
	virtual void OnResize() override;
	const string& Text() const;
	void Text(string&& newText);
	SDL_Texture* Texture() const;

	Align align;
protected:
	string text;
	SDL_Texture* tex;
};

class Button : public Label {
public:
	Button(const Object& BASE=Object(), void (Program::*CALLB)()=nullptr, string&& TXT="", Align ALG=Align::left);
	virtual ~Button() override = default;

	void OnClick();

protected:
	void (Program::*callback)();
};

class LineEditor : public Button {
public:
	enum class Type : uint8 {
		text,
		integer
	};

	LineEditor(const Object& BASE=Object(), string&& TXT="", Type TYPE=Type::text, void (Program::*KCALL)(const string&)=nullptr, void (Program::*CCALL)()=nullptr);
	virtual ~LineEditor() override = default;

	virtual void Draw() override;
	void OnClick();
	void OnKeypress(SDL_Scancode key);
	void OnText(const char* str);

	void Confirm();
	void Cancel();

private:
	void (Program::*okCall)(const string&);
	size_t cpos;
	int textPos;
	Type type;

	SDL_Rect Caret() const;	// position relative to Widget
	void MoveCursor(bool right);
	void Delete(bool current);
	void CheckCaretRight();
	void CheckCaretLeft();
	void CheckText();
};

class HorSlider : public Object {
public:
	HorSlider(const Object& BASE=Object(), float VAL=0.f, void (Program::*CAL)(float)=nullptr);
	virtual ~HorSlider() override = default;

	virtual void Draw() override;
	void DragSlider(int xpos);
	SDL_Rect Slider() const;
	int SliderX() const;

	static constexpr int sliderW = 20;

	int diffSliderMouseX;
private:
	float value;
	void (Program::*callb)(float);
};

class ScrollArea : public Object {
public:
	ScrollArea(SDL_Point ANC={}, SDL_Point POS={-1, -1}, SDL_Point SIZ={}, EFix FIX=FIX_NONE, SDL_Color CLR=colorBackground);
	virtual ~ScrollArea() override;

	virtual void Draw() override;
	virtual void OnResize() override;
	void DragSlider(int ypos);
	void DragList(int ypos);
	void ScrollList(int ymov);

	void Items(vector<string>&& newItems);
	SDL_Rect Bar() const;
	SDL_Rect Slider() const;
	int BarW() const;	// returns 0 if slider isn't needed
	int SliderY() const;
	int SliderH() const;

	static constexpr int itemH = 30;
	static constexpr int spacing = 5;

	int diffSliderMouseY;
protected:
	int listY;
	int sliderH;
	int listH, listL;
	vector<string> items;
	vector<SDL_Texture*> texs;

	static constexpr int barW = 10;

private:
	void UpdateTextures();
};
