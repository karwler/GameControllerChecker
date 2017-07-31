#pragma once

#include "prog/program.h"
#include "utils/scrollArea.h"

class Scene {
public:
	Scene();
	~Scene();
	
	void SwitchMenu(const vector<Object*>& objs);
	void ResizeMenu();
	void OnMouseMove(const vec2i& mPos);
	void OnMouseDown(const vec2i& mPos);
	void OnMouseUp();
	void OnMouseWheel(int ymov);
	
	Program* getProgram();
	FontSet& Fonts();
	const vector<Object*>& Objects() const;

private:
	Program program;
	FontSet fonts;
	vector<Object*> objects;

	Object* mouseOver;		// object currently being hovered over by mouse
	Object* scrollHold;		// pointer to object currently being dragged by mouse (nullptr if none is being held)

	void CheckSliderClick(HorSlider* obj, const vec2i& mPos);
	void CheckSliderClick(ScrollArea* obj, const vec2i& mPos);
};
