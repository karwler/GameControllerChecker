#include "engine/world.h"

// SCROLL AREA

ScrollArea::ScrollArea(const Object& BASE, int IH, int SPC, int BARW) :
	Object(BASE),
	barW(BARW),
	spacing(SPC),
	itemH(IH),
	diffSliderMouseY(0),
	listY(0)
{}
ScrollArea::~ScrollArea() {}

void ScrollArea::SetValues() {
	int sizY = Size().y;

	listH = items.size() * (itemH + spacing) - spacing;
	listL = (sizY < listH) ? listH - sizY : 0;
	sliderH = (sizY < listH) ? sizY * sizY / listH : sizY;
	CheckListY();
}

void ScrollArea::DragSlider(int ypos) {
	DragList((ypos - diffSliderMouseY - Pos().y) * listH / Size().y);
}

void ScrollArea::DragList(int ypos) {
	listY = ypos;
	CheckListY();

	World::engine()->SetRedrawNeeded();
}

void ScrollArea::ScrollList(int ymov)  {
	DragList(listY + ymov);
}

const vector<string>& ScrollArea::Items() const {
	return items;
}

void ScrollArea::Items(const vector<string>& itms) {
	items = itms;
	SetValues();
}

SDL_Rect ScrollArea::ItemRect(size_t id) const {
	vec2i pos = Pos();
	SDL_Rect rect = {pos.x, pos.y - listY + id * (itemH+spacing), Size().x-BarW(), itemH};
	return cropRect(rect, getCrop(rect, getRect()));
}

SDL_Rect ScrollArea::ItemRect(size_t id, SDL_Rect& crop) const {
	vec2i pos = Pos();
	SDL_Rect rect ={pos.x, pos.y - listY + id * (itemH+spacing), Size().x-BarW(), itemH};

	crop = getCrop(rect, getRect());
	return cropRect(rect, crop);
}

vec2t ScrollArea::VisibleItems() const {
	int sizY = Size().y;
	if (items.empty() || sizY <= 0)
		return vec2t(1, 0);
	return vec2t(listY / (itemH+spacing), (listH > sizY) ? (listY+sizY) / (itemH+spacing) : items.size()-1);
}

SDL_Rect ScrollArea::Bar() const {
	int bw = BarW();
	return {End().x-bw, Pos().y, bw, Size().y};
}

SDL_Rect ScrollArea::Slider() const {
	int bw = BarW();
	return {End().x-bw, SliderY(), bw, sliderH };
}

int ScrollArea::BarW() const {
	return (sliderH == Size().y) ? 0 : barW;
}

int ScrollArea::SliderY() const {
	int sizY = Size().y;
	return listH <= sizY ? Pos().y : Pos().y + listY * sizY / listH;
}

int ScrollArea::SliderH() const {
	return sliderH;
}

int ScrollArea::ItemH() const {
	return itemH;
}

void ScrollArea::CheckListY() {
	if (listY < 0)
		listY = 0;
	else if (listY > listL)
		listY = listL;
}
