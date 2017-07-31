#include "world.h"

WindowSys::WindowSys() :
	window(nullptr),
	renderer(nullptr)
{}

WindowSys::~WindowSys() {
	DestroyWindow();
}

void WindowSys::CreateWindow() {
	DestroyWindow();
	
	window = SDL_CreateWindow("Controller Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_RESOLUTION.x, DEFAULT_RESOLUTION.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	if (!window)
		throw Exception("couldn't create window\n" + string(SDL_GetError()), 3);
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
		throw Exception("couldn't create renderer\n" + string(SDL_GetError()), 4);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void WindowSys::DestroyWindow() {
	if (renderer) {
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}
	if (window) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}

void WindowSys::DrawObjects(const vector<Object*>& objects) {
	SDL_SetRenderDrawColor(renderer, DEFAULT_COLOR_BACKGROUND.r, DEFAULT_COLOR_BACKGROUND.g, DEFAULT_COLOR_BACKGROUND.b, DEFAULT_COLOR_BACKGROUND.a);
	SDL_RenderClear(renderer);

	for (Object* obj : objects)
		PassDrawObject(obj);

	SDL_RenderPresent(renderer);
}

void WindowSys::WindowEvent(const SDL_WindowEvent& winEvent) {
	if (winEvent.event == SDL_WINDOWEVENT_EXPOSED)
		World::engine()->SetRedrawNeeded();
	else if (winEvent.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		World::scene()->ResizeMenu();
}

vec2i WindowSys::Resolution() const {
	vec2i res;
	SDL_GetWindowSize(window, &res.x, &res.y);
	return res;
}

void WindowSys::PassDrawObject(Object* obj) {
	// specific drawing for each object
	if (Label* lbl = dynamic_cast<Label*>(obj))
		DrawObject(lbl->getRect(), lbl->color, lbl->getText());
	else if (LineEditor* edt = dynamic_cast<LineEditor*>(obj))
		DrawObject(edt);
	else if (HorSlider* sld = dynamic_cast<HorSlider*>(obj))
		DrawObject(sld);
	else if (ScrollArea* sca = dynamic_cast<ScrollArea*>(obj))
		DrawObject(sca);
	else
		DrawRect(obj->getRect(), obj->color);
}

void WindowSys::DrawObject(const SDL_Rect& bg, vec4c bgColor, const Text& text) {
	DrawRect(bg, bgColor);

	int len = text.size().x;
	int left = (text.pos.x < bg.x) ? bg.x-text.pos.x : 0;
	int right = (text.pos.x+len > bg.x+bg.w) ? text.pos.x+len-bg.x-bg.w : 0;
	DrawText(text, { left, 0, right, 0 });
}

void WindowSys::DrawObject(LineEditor* obj) {
	SDL_Rect bg = obj->getRect();
	DrawRect(bg, obj->color);

	// get text and calculate left and right crop
	Text text = obj->getText();
	int len = text.size().x;
	int left = (text.pos.x < bg.x) ? bg.x - text.pos.x : 0;
	int right = (len-left > bg.w) ? len - left - bg.w : 0;
	DrawText(text, {left, 0, right, 0});

	if (World::inputSys()->Captured() == obj)
		DrawRect(obj->getCaret(), DEFAULT_COLOR_HIGHLIGHTED);
}

void WindowSys::DrawObject(HorSlider* obj) {
	DrawRect(obj->getRect(), DEFAULT_COLOR_DARKENED);
	DrawRect(obj->Slider(), DEFAULT_COLOR_HIGHLIGHTED);
}

void WindowSys::DrawObject(ScrollArea* obj) {
	vec2t interval = obj->VisibleItems();
	if (interval.x > interval.y)
		return;
	
	const vector<string>& items = obj->Items();
	for (size_t i=interval.x; i<=interval.y; i++) {
		SDL_Rect crop;
		SDL_Rect rect = obj->ItemRect(i, crop);

		DrawRect(rect, DEFAULT_COLOR_BACKGROUND);
		DrawText(Text(items[i], vec2i(rect.x+5, rect.y-crop.y), obj->ItemH()), crop);
	}
	DrawRect(obj->Bar(), DEFAULT_COLOR_DARKENED);
	DrawRect(obj->Slider(), DEFAULT_COLOR_HIGHLIGHTED);
}

void WindowSys::DrawRect(const SDL_Rect& rect, vec4c color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

void WindowSys::DrawText(const Text& txt, const SDL_Rect& crop) {
	if (txt.text.empty())
		return;
	
	vec2i siz = txt.size();
	SDL_Rect rect = {txt.pos.x, txt.pos.y, siz.x, siz.y};
	SDL_Surface* surface = TTF_RenderUTF8_Blended(World::scene()->Fonts().Get(txt.height), txt.text.c_str(), {txt.color.r, txt.color.g, txt.color.b, txt.color.a});
	SDL_Texture* tex;
	if (needsCrop(crop)) {
		SDL_Surface* sheet = cropSurface(surface, rect, crop);
		tex = SDL_CreateTextureFromSurface(renderer, sheet);
		SDL_FreeSurface(sheet);
	}
	else
		tex = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_RenderCopy(renderer, tex, nullptr, &rect);
	SDL_DestroyTexture(tex);
	SDL_FreeSurface(surface);
}
