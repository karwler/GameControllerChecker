#pragma once

// include SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// include other useful stuff
#include "kklib/aliases.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;
using std::string;
using std::wstring;
using std::to_string;

using kk::vec2i;
using kk::vec2f;
using kk::vec4c;
using vec2t = kk::vec2<size_t>;

#ifdef main
#undef main
#endif

class Engine;
class InputSys;
class WinSys;

class Scene;
class Program;

// string related stuff
bool strcmpCI(const string& strl, const string& strr);
bool findChar(const string& str, char c, size_t* id = nullptr);
bool isAbsolute(const string& path);
string parentPath(const string& path);
string filename(const string& path);
string delExt(const string& path);
string hexToStr(int num);
string gcButtonStr(SDL_GameControllerButton button);
string gcAxisStr(SDL_GameControllerAxis axis);

// file handling
enum EFileType : uint8 {
	file,
	dir,
	link
};

EFileType FileType(const string& path);
vector<string> ListDirRecursively(const string& dir, size_t offs=0);
string FindFont(const string& font);	// on success returns absolute path to font file, otherwise returns empty path
string CheckDirForFont(const string& font, const string& dir);	// returns same as FindFont

// graphics
bool inRect(const SDL_Rect& rect, vec2i point);
bool needsCrop(const SDL_Rect& crop);
SDL_Rect getCrop(SDL_Rect item, SDL_Rect frame);
SDL_Rect cropRect(const SDL_Rect& rect, const SDL_Rect& crop);
SDL_Surface* cropSurface(SDL_Surface* surface, SDL_Rect& rect, SDL_Rect crop);

// convertions
string wtos(const wstring& wstr);
wstring stow(const string& str);

vec2i pix(const vec2f& p);
int pixX(float p);
int pixY(float p);
vec2f prc(const vec2i& p);
float prcX(int p);
float prcY(int p);

template <typename T>
void clear(vector<T*>& vec) {
	for (T* it : vec)
		delete it;
	vec.clear();
}
