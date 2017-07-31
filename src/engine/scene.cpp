#include "world.h"

Scene::Scene() :
	mouseOver(nullptr),
	scrollHold(nullptr)
{
	fonts.Initialize(FindFont(DEFAULT_FONT));
}

Scene::~Scene() {
	clear(objects);
	fonts.Clear();
}

void Scene::SwitchMenu(const vector<Object*>& objs) {
	// reset objects
	clear(objects);
	objects = objs;

	// reset values
	OnMouseMove(World::inputSys()->mousePos());	// update mmouse over
	scrollHold = nullptr;

	World::engine()->SetRedrawNeeded();
}

void Scene::ResizeMenu() {
	for (Object* obj : objects)
		if (ScrollArea* box = dynamic_cast<ScrollArea*>(obj))
			box->SetValues();

	World::engine()->SetRedrawNeeded();
}

void Scene::OnMouseMove(const vec2i& mPos) {
	if (scrollHold) {
		if (HorSlider* slider = dynamic_cast<HorSlider*>(scrollHold))
			slider->DragSlider(mPos.x);
		else if (ScrollArea* area = dynamic_cast<ScrollArea*>(scrollHold))
			area->DragSlider(mPos.y);
	} else
		for (Object* obj : objects)
			if (inRect(obj->getRect(), mPos)) {
				mouseOver = obj;
				break;
			}
}

void Scene::OnMouseDown(const vec2i& mPos) {
	if (!mouseOver)
		return;

	if (Button* but = dynamic_cast<Button*>(mouseOver))
		but->OnClick();
	else if (LineEditor* edt = dynamic_cast<LineEditor*>(mouseOver))
		edt->OnClick();
	else if (HorSlider* slider = dynamic_cast<HorSlider*>(mouseOver))
		CheckSliderClick(slider, mPos);
	else if (ScrollArea* area = dynamic_cast<ScrollArea*>(mouseOver))
		CheckSliderClick(area, mPos);
}

void Scene::CheckSliderClick(HorSlider* obj, const vec2i& mPos) {
	scrollHold = obj;
	if (mPos.x < obj->SliderX() || mPos.x > obj->SliderX() + obj->SliderW())
		obj->DragSlider(mPos.x - obj->SliderW() /2);
	obj->diffSliderMouseX = mPos.x - obj->SliderX();
}

void Scene::CheckSliderClick(ScrollArea* obj, const vec2i& mPos) {
	if (inRect(obj->Bar(), mPos)) {
		scrollHold = obj;
		if (mPos.y < obj->SliderY() || mPos.y > obj->SliderY() + obj->SliderH())	// if mouse outside of slider but inside bar
			obj->DragSlider(mPos.y - obj->SliderH() /2);
		obj->diffSliderMouseY = mPos.y - obj->SliderY();	// get difference between mouse y and slider y
	}
}

void Scene::OnMouseUp() {
	if (scrollHold) {
		if (HorSlider* slider = dynamic_cast<HorSlider*>(scrollHold))
			slider->diffSliderMouseX = 0;
		else if (ScrollArea* area = dynamic_cast<ScrollArea*>(scrollHold))
			area->diffSliderMouseY = 0;

		scrollHold = nullptr;
		OnMouseMove(World::inputSys()->mousePos());	// update mmouse over
	}
}

void Scene::OnMouseWheel(int ymov) {
	if (ScrollArea* box = dynamic_cast<ScrollArea*>(mouseOver))
		box->ScrollList(ymov);
}

Program* Scene::getProgram() {
	return &program;
}

FontSet& Scene::Fonts() {
	return fonts;
}

const vector<Object*>& Scene::Objects() const {
	return objects;
}
