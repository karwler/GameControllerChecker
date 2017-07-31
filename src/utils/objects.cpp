#include "engine/world.h"

// FIX

EFix operator~(EFix a) {
	return static_cast<EFix>(~static_cast<uint8>(a));
}
EFix operator&(EFix a, EFix b) {
	return static_cast<EFix>(static_cast<uint8>(a) & static_cast<uint8>(b));
}
EFix operator&=(EFix& a, EFix b) {
	return a = static_cast<EFix>(static_cast<uint8>(a) & static_cast<uint8>(b));
}
EFix operator^(EFix a, EFix b) {
	return static_cast<EFix>(static_cast<uint8>(a) ^ static_cast<uint8>(b));
}
EFix operator^=(EFix& a, EFix b) {
	return a = static_cast<EFix>(static_cast<uint8>(a) ^ static_cast<uint8>(b));
}
EFix operator|(EFix a, EFix b) {
	return static_cast<EFix>(static_cast<uint8>(a) | static_cast<uint8>(b));
}
EFix operator|=(EFix& a, EFix b) {
	return a = static_cast<EFix>(static_cast<uint8>(a) | static_cast<uint8>(b));
}

// OBJECT

Object::Object(const vec2i& ANC, vec2i POS, const vec2i& SIZ, EFix FIX, vec4c CLR) :
	color(CLR),
	fix(FIX)
{
	// set positin to anchor if set to -1
	if (POS.x == -1)
		POS.x = ANC.x;
	if (POS.y == -1)
		POS.y = ANC.y;
	Anchor(ANC);
	Pos(POS);
	Size(SIZ);
}
Object::~Object() {}

SDL_Rect Object::getRect() const {
	return {Pos().x, Pos().y, Size().x, Size().y};
}

vec2i Object::Anchor() const {
	vec2i ret;
	ret.x = (fix & FIX_X) ? anchor.x : pixX(anchor.x);
	ret.y = (fix & FIX_Y) ? anchor.y : pixY(anchor.y);
	return ret;
}

void Object::Anchor(const vec2i& newPos) {
	anchor.x = (fix & FIX_X) ? newPos.x : prcX(newPos.x);
	anchor.y = (fix & FIX_Y) ? newPos.y : prcY(newPos.y);
}

vec2i Object::Pos() const {
	vec2i ret = Anchor();
	ret.x = (fix & FIX_PX) ? pixX(pos.x) : (fix & FIX_W) ? ret.x + pos.x : ret.x + pixX(pos.x);
	ret.y = (fix & FIX_PY) ? pixY(pos.y) : (fix & FIX_H) ? ret.y + pos.y : ret.y + pixY(pos.y);
	return ret;
}

void Object::Pos(const vec2i& newPos) {
	vec2i dist = newPos - Anchor();
	pos.x = (fix & FIX_PX) ? prcX(newPos.x) : (fix & FIX_W) ? dist.x : prcX(dist.x);
	pos.y = (fix & FIX_PY) ? prcY(newPos.y) : (fix & FIX_H) ? dist.y : prcY(dist.y);
}

vec2i Object::End() const {
	vec2i ret = Anchor();
	ret.x = (fix & FIX_EX) ? pixX(end.x) : (fix & FIX_W) ? ret.x + end.x : ret.x + pixX(end.x);
	ret.y = (fix & FIX_EY) ? pixY(end.y) : (fix & FIX_H) ? ret.y + end.y : ret.y + pixY(end.y);
	return ret;
}

void Object::End(const vec2i& newPos) {
	vec2i dist = newPos - Anchor();
	end.x = (fix & FIX_EX) ? prcX(newPos.x) : (fix & FIX_W) ? dist.x : prcX(dist.x);
	end.y = (fix & FIX_EY) ? prcY(newPos.y) : (fix & FIX_H) ? dist.y : prcY(dist.y);
}

vec2i Object::Size() const {
	return End() - Pos();
}

void Object::Size(const vec2i& newSize) {
	vec2i dist = Pos() + newSize - Anchor();
	end.x = (fix & FIX_EX) ? prcX(Pos().x + newSize.x) : (fix & FIX_W) ? dist.x : prcX(dist.x);
	end.y = (fix & FIX_EY) ? prcY(Pos().y + newSize.y) : (fix & FIX_H) ? dist.y : prcY(dist.y);
}

// LABEL

Label::Label(const Object& BASE, const string& TXT, ETextAlign ALG) :
	Object(BASE),
	align(ALG),
	text(TXT)
{}
Label::~Label() {}

Text Label::getText() const {
	Text txt(text, 0, Size().y);
	txt.SetPosToRect(getRect(), align);
	return txt;
}

// BUTTON

Button::Button(const Object& BASE, void (Program::*CALLB)(), const string& TXT, ETextAlign ALG) :
	Label(BASE, TXT, ALG),
	callback(CALLB)
{}
Button::~Button() {}

void Button::OnClick() {
	if (callback)
		(World::program()->*callback)();
}

void Button::Callback(void (Program::*func)()) {
	callback = func;
}

// LINE EDITOR

LineEditor::LineEditor(const Object& BASE, const string& TXT, ETextType TYPE, void (Program::*KCALL)(const string&), void (Program::*CCALL)()) :
	Object(BASE),
	editor(TXT, TYPE),
	okCall(KCALL),
	cancelCall(CCALL),
	textPos(0)
{}
LineEditor::~LineEditor() {}

void LineEditor::OnClick() {
	World::inputSys()->SetCapture(this);
	editor.SetCursor(editor.Text().length());
}

void LineEditor::OnKeypress(SDL_Scancode key) {
	bool redraw = true;
	if (key == SDL_SCANCODE_RIGHT) {
		editor.MoveCursor(true);
		CheckCaretRight();
	} else if (key == SDL_SCANCODE_LEFT) {
		editor.MoveCursor(false);
		CheckCaretLeft();
	} else if (key == SDL_SCANCODE_BACKSPACE) {
		editor.Delete(false);
		CheckCaretLeft();
	} else if (key == SDL_SCANCODE_DELETE)
		editor.Delete(true);
	else if (key == SDL_SCANCODE_RETURN)
		Confirm();
	else if (key == SDL_SCANCODE_ESCAPE)
		Cancel();
	else
		redraw = false;

	if (redraw)
		World::engine()->SetRedrawNeeded();
}

void LineEditor::OnText(const char* text) {
	editor.Add(text);
	CheckCaretRight();
}

void LineEditor::Confirm() {
	ResetTextPos();
	if (okCall)
		(World::program()->*okCall)(editor.Text());
	World::inputSys()->SetCapture(nullptr);
}

void LineEditor::Cancel() {
	ResetTextPos();
	if (cancelCall)
		(World::program()->*cancelCall)();
	World::inputSys()->SetCapture(nullptr);
}

int LineEditor::TextPos() const {
	return textPos;
}

void LineEditor::ResetTextPos() {
	textPos = 0;
}

TextEdit& LineEditor::Editor() {
	return editor;
}

Text LineEditor::getText() const {
	return Text(editor.Text(), Pos()-vec2i(textPos, 0), Size().y);
}

SDL_Rect LineEditor::getCaret() const {
	vec2i pos = Pos();
	int height = Size().y;

	return {Text(editor.Text().substr(0, editor.CursorPos()), 0, height).size().x - textPos + pos.x, pos.y, 5, height};
}

void LineEditor::CheckCaretRight() {
	SDL_Rect caret = getCaret();
	int diff = caret.x + caret.w - End().x;
	if (diff > 0)
		textPos += diff;
}

void LineEditor::CheckCaretLeft() {
	SDL_Rect caret = getCaret();
	int diff = Pos().x - caret.x;
	if (diff > 0)
		textPos -= diff;
}

// HOR SLIDER

HorSlider::HorSlider(const Object& BASE, float VAL, void (Program::*CAL)(float), int SLW) :
	Object(BASE),
	sliderW(SLW),
	diffSliderMouseX(0),
	callb(CAL),
	value(VAL)
{}
HorSlider::~HorSlider() {}

void HorSlider::DragSlider(int xpos) {
	value = float(xpos - diffSliderMouseX - Pos().x) / float(Size().x - sliderW);
	if (value < 0.f)
		value = 0.f;
	else if (value > 1.f)
		value = 1.f;

	if (callb)
		(World::program()->*callb)(value);
	World::engine()->SetRedrawNeeded();
}

SDL_Rect HorSlider::Slider() const {
	return {SliderX(), Pos().y, sliderW, Size().y};
}

int HorSlider::SliderX() const {
	return Pos().x + float(Size().x-sliderW) * value;
}

int HorSlider::SliderW() const {
	return sliderW;
}
