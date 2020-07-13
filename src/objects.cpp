#include "world.h"

// OBJECT

Object::Object(SDL_Point ANC, SDL_Point POS, SDL_Point SIZ, EFix FIX, SDL_Color CLR) :
	color(CLR),
	fix(FIX)
{
	if (POS.x == -1)
		POS.x = ANC.x;
	if (POS.y == -1)
		POS.y = ANC.y;
	Anchor(ANC);
	Pos(POS);
	Size(SIZ);
}

void Object::Draw() {
	World::Window()->DrawRect(Rect(), color);
}

SDL_Rect Object::Rect() const {
	SDL_Point p = Pos(), s = Size();
	return {p.x, p.y, s.x, s.y};
}

SDL_Point Object::Anchor() const {
	SDL_Point ret, res = World::Window()->Resolution();
	ret.x = int(fix & FIX_X ? anchorX : anchorX * float(res.x));
	ret.y = int(fix & FIX_Y ? anchorY : anchorY * float(res.y));
	return ret;
}

void Object::Anchor(SDL_Point newPos) {
	SDL_Point res = World::Window()->Resolution();
	anchorX = fix & FIX_X ? float(newPos.x) : float(newPos.x) / float(res.x);
	anchorY = fix & FIX_Y ? float(newPos.y) : float(newPos.y) / float(res.y);
}

SDL_Point Object::Pos() const {
	SDL_Point ret = Anchor(), res = World::Window()->Resolution();
	ret.x = fix & FIX_PX ? int(posX * float(res.x)) : fix & FIX_W ? ret.x + int(posX) : ret.x + int(posX * float(res.x));
	ret.y = fix & FIX_PY ? int(posY * float(res.y)) : fix & FIX_H ? ret.y + int(posY) : ret.y + int(posY * float(res.y));
	return ret;
}

void Object::Pos(SDL_Point newPos) {
	SDL_Point dist = newPos - Anchor(), res = World::Window()->Resolution();
	posX = fix & FIX_PX ? float(newPos.x) / float(res.x) : fix & FIX_W ? float(dist.x) : float(dist.x) / float(res.x);
	posY = fix & FIX_PY ? float(newPos.y) / float(res.y) : fix & FIX_H ? float(dist.y) : float(dist.y) / float(res.y);
}

SDL_Point Object::End() const {
	SDL_Point ret = Anchor(), res = World::Window()->Resolution();
	ret.x = fix & FIX_EX ? int(endX * float(res.x)) : fix & FIX_W ? ret.x + int(endX) : ret.x + int(endX * float(res.x));
	ret.y = fix & FIX_EY ? int(endY * float(res.y)) : fix & FIX_H ? ret.y + int(endY) : ret.y + int(endY * float(res.y));
	return ret;
}

void Object::End(SDL_Point newPos) {
	SDL_Point dist = newPos - Anchor(), res = World::Window()->Resolution();;
	endX = fix & FIX_EX ? float(newPos.x) / float(res.x) : fix & FIX_W ? float(dist.x) : float(dist.x) / float(res.x);
	endY = fix & FIX_EY ? float(newPos.y) / float(res.y) : fix & FIX_H ? float(dist.y) : float(dist.y) / float(res.y);
}

SDL_Point Object::Size() const {
	return End() - Pos();
}

void Object::Size(SDL_Point newSize) {
	SDL_Point dist = Pos() + newSize - Anchor(), res = World::Window()->Resolution();;;
	endX = fix & FIX_EX ? float(Pos().x + newSize.x) / float(res.x) : fix & FIX_W ? float(dist.x) : float(dist.x) / float(res.x);
	endY = fix & FIX_EY ? float(Pos().y + newSize.y) / float(res.y) : fix & FIX_H ? float(dist.y) : float(dist.y) / float(res.y);
}

SDL_Color Object::GetColor(SDL_Color clr, bool light) {
	return !light ? clr : SDL_Color{
		uint8(std::clamp(float(clr.r) * highlight, 0.f, 255.f)),
		uint8(std::clamp(float(clr.g) * highlight, 0.f, 255.f)),
		uint8(std::clamp(float(clr.b) * highlight, 0.f, 255.f)),
		clr.a
	};
}

// LABEL

Label::Label(const Object& BASE, string&& TXT, Align ALG) :
	Object(BASE),
	align(ALG),
	text(std::move(TXT)),
	tex(World::Window()->RenderText(text, Size().y))
{}

Label::~Label() {
	if (tex)
		SDL_DestroyTexture(tex);
}

void Label::Draw() {
	SDL_Rect bg = Rect();
	int ofs = 0;
	switch (int w; align) {
	case Align::left:
		ofs = offset;
		break;
	case Align::center:
		SDL_QueryTexture(tex, nullptr, nullptr, &w, nullptr);
		ofs = (bg.w - w) / 2;
		break;
	case Align::right:
		SDL_QueryTexture(tex, nullptr, nullptr, &w, nullptr);
		ofs = bg.w - w - offset;
	}
	World::Window()->DrawRect(bg, GetColor(color, World::Input()->IsSelected(this)));
	World::Window()->DrawText(tex, {bg.x + ofs, bg.y}, bg);
}

void Label::OnResize() {
	if (tex)
		SDL_DestroyTexture(tex);
	tex = World::Window()->RenderText(text, Size().y);
}

const string& Label::Text() const {
	return text;
}

void Label::Text(string&& newText) {
	text = std::move(newText);
	OnResize();
}

SDL_Texture* Label::Texture() const {
	return tex;
}

// BUTTON

Button::Button(const Object& BASE, void (Program::*CALLB)(), string&& TXT, Align ALG) :
	Label(BASE, std::move(TXT), ALG),
	callback(CALLB)
{}

void Button::OnClick() {
	if (callback)
		(World::Prog()->*callback)();
}

// LINE EDITOR

LineEditor::LineEditor(const Object& BASE, string&& TXT, Type TYPE, void (Program::*KCALL)(const string&), void (Program::*CCALL)()) :
	Button(BASE, CCALL, std::move(TXT)),
	okCall(KCALL),
	textPos(0),
	type(TYPE)
{
	CheckText();
}

void LineEditor::Draw() {
	SDL_Rect bg = Rect();
	World::Window()->DrawRect(bg, GetColor(color, World::Input()->IsSelected(this)));
	World::Window()->DrawText(tex, {bg.x - textPos, bg.y}, bg);
	if (World::Input()->Captured() == this)
		World::Window()->DrawRect(Caret(), Object::colorLight);
}

void LineEditor::OnClick() {
	World::Input()->SetCapture(this);
	cpos = text.length();
}

void LineEditor::OnKeypress(SDL_Scancode key) {
	switch (key) {
	case SDL_SCANCODE_RIGHT:
		MoveCursor(true);
		CheckCaretRight();
		break;
	case SDL_SCANCODE_LEFT:
		MoveCursor(false);
		CheckCaretLeft();
		break;
	case SDL_SCANCODE_BACKSPACE:
		Delete(false);
		CheckCaretLeft();
		break;
	case SDL_SCANCODE_DELETE:
		Delete(true);
		break;
	case SDL_SCANCODE_RETURN:
		Confirm();
		break;
	case SDL_SCANCODE_ESCAPE:
		Cancel();
	}
}

void LineEditor::OnText(const char* str) {
	text.insert(cpos, str);
	cpos += strlen(str);
	CheckText();
	CheckCaretRight();
}

void LineEditor::Confirm() {
	if (textPos = 0; okCall)
		(World::Prog()->*okCall)(text);
	World::Input()->SetCapture(nullptr);
}

void LineEditor::Cancel() {
	if (textPos = 0; callback)
		(World::Prog()->*callback)();
	World::Input()->SetCapture(nullptr);
}

SDL_Rect LineEditor::Caret() const {
	SDL_Point pos = Pos();
	int height = Size().y;
	return {World::Window()->Fonts()->TextSize(text.substr(0, cpos), height).x - textPos + pos.x, pos.y, 5, height};
}

void LineEditor::MoveCursor(bool right) {
	if (right && cpos != text.length())
		cpos++;
	else if (!right && cpos)
		cpos--;
}

void LineEditor::Delete(bool current) {
	if (current) {
		if (cpos != text.length()) {
			text.erase(cpos, 1);
			OnResize();
		}
	} else if (cpos) {
		cpos--;
		text.erase(cpos, 1);
		OnResize();
	}
}

void LineEditor::CheckCaretRight() {
	SDL_Rect caret = Caret();
	if (int diff = caret.x + caret.w - End().x; diff > 0)
		textPos += diff;
}

void LineEditor::CheckCaretLeft() {
	SDL_Rect caret = Caret();
	if (int diff = Pos().x - caret.x; diff > 0)
		textPos -= diff;
}

void LineEditor::CheckText() {
	if (type == Type::integer)
		if (text.erase(std::remove_if(text.begin(), text.end(), [](char c) -> bool { return !isdigit(c); }), text.end()); cpos > text.length())
			cpos = text.length();
	OnResize();
}

// HOR SLIDER

HorSlider::HorSlider(const Object& BASE, float VAL, void (Program::*CAL)(float)) :
	Object(BASE),
	diffSliderMouseX(0),
	value(VAL),
	callb(CAL)
{}

void HorSlider::Draw() {
	World::Window()->DrawRect(Rect(), GetColor(Object::colorDark, World::Input()->IsSelected(this)));
	World::Window()->DrawRect(Slider(), Object::colorLight);
}

void HorSlider::DragSlider(int xpos) {
	if (value = std::clamp(float(xpos - diffSliderMouseX - Pos().x) / float(Size().x - sliderW), 0.f, 1.f); callb)
		(World::Prog()->*callb)(value);
}

SDL_Rect HorSlider::Slider() const {
	return {SliderX(), Pos().y, sliderW, Size().y};
}

int HorSlider::SliderX() const {
	return int(float(Pos().x) + float(Size().x - sliderW) * value);
}

// SCROLL AREA

ScrollArea::ScrollArea(SDL_Point ANC, SDL_Point POS, SDL_Point SIZ, EFix FIX, SDL_Color CLR) :
	Object(ANC, POS, SIZ, FIX, CLR),
	diffSliderMouseY(0),
	listY(0)
{}

ScrollArea::~ScrollArea() {
	for (SDL_Texture* it : texs)
		SDL_DestroyTexture(it);
}

void ScrollArea::Draw() {
	if (SDL_Rect bg = Rect(); !items.empty() && bg.h > 0) {
		SDL_Point ipos = {bg.x + offset, bg.y - listY};
		pair<size_t, size_t> vis(listY / (itemH + spacing), listH > bg.h ? (listY + bg.h) / (itemH + spacing) : items.size() - 1);
		for (size_t i = vis.first; i <= vis.second; i++, ipos.y += itemH + spacing)
			World::Window()->DrawText(texs[i], ipos, bg);
	}
	World::Window()->DrawRect(Bar(), Object::colorDark);
	World::Window()->DrawRect(Slider(), Object::colorLight);
}

void ScrollArea::OnResize() {
	int sizY = Size().y;
	listH = int(items.size()) * (itemH + spacing) - spacing;
	listL = sizY < listH ? listH - sizY : 0;
	sliderH = sizY < listH ? sizY * sizY / listH : sizY;
	listY = std::clamp(listY, 0, listL);
}

void ScrollArea::DragSlider(int ypos) {
	DragList((ypos - diffSliderMouseY - Pos().y) * listH / Size().y);
}

void ScrollArea::DragList(int ypos) {
	listY = std::clamp(ypos, 0, listL);
}

void ScrollArea::ScrollList(int ymov) {
	DragList(listY + ymov);
}

void ScrollArea::Items(vector<string>&& newItems) {
	items = std::move(newItems);
	OnResize();
	UpdateTextures();
}

void ScrollArea::UpdateTextures() {
	for (SDL_Texture* it : texs)
		SDL_DestroyTexture(it);

	texs.resize(items.size());
	for (size_t i = 0; i < items.size(); i++)
		texs[i] = World::Window()->RenderText(items[i], itemH);
}

SDL_Rect ScrollArea::Bar() const {
	int bw = BarW();
	return {End().x - bw, Pos().y, bw, Size().y};
}

SDL_Rect ScrollArea::Slider() const {
	int bw = BarW();
	return {End().x - bw, SliderY(), bw, sliderH};
}

int ScrollArea::BarW() const {
	return sliderH == Size().y ? 0 : barW;
}

int ScrollArea::SliderY() const {
	int sizY = Size().y;
	return listH <= sizY ? Pos().y : Pos().y + listY * sizY / listH;
}

int ScrollArea::SliderH() const {
	return sliderH;
}
