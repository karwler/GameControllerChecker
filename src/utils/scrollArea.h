#pragma once

#include "objects.h"

class ScrollArea : public Object {
public:
	ScrollArea(const Object& BASE=Object(), int IH=30, int SPC=5, int BARW=10);
	virtual ~ScrollArea();

	virtual void SetValues();
	void DragSlider(int ypos);
	void DragList(int ypos);
	void ScrollList(int ymov);

	const vector<string>& Items() const;
	void Items(const vector<string>& itms);
	SDL_Rect ItemRect(size_t id) const;
	SDL_Rect ItemRect(size_t id, SDL_Rect& crop) const;
	vec2t VisibleItems() const;

	SDL_Rect Bar() const;
	SDL_Rect Slider() const;

	int BarW() const;					// returns 0 if slider isn't needed
	int SliderY() const;
	int SliderH() const;
	int ItemH() const;
	
	const int spacing;
	int diffSliderMouseY;
protected:
	const int barW;
	int listY;
	int sliderH;
	int listH, listL;
	int itemH;
	vector<string> items;

	void CheckListY();		// check if listY is out of limits and correct if so
};
