#include "engine/world.h"
#include <sstream>
#include <locale>
#include <codecvt>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#else
namespace uni {		// necessary to prevent conflicts
#include <unistd.h>
}
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

bool strcmpCI(const string& strl, const string& strr) {
	if (strl.length() != strr.length())
		return false;

	for (size_t i = 0; i!=strl.length(); i++)
		if (tolower(strl[i]) != tolower(strr[i]))
			return false;
	return true;
}

bool findChar(const string& str, char c, size_t* id) {
	for (size_t i = 0; i!=str.length(); i++)
		if (str[i] == c) {
			if (id)
				*id = i;
			return true;
		}
	return false;
}

bool isAbsolute(const string& path) {
	return path[0] == dsep || (path[1] == ':' && path[2] == dsep);
}

string parentPath(const string& path) {
	size_t start = (path[path.length()-1] == dsep) ? path.length()-2 : path.length()-1;
	for (size_t i=start; i!=SIZE_MAX; i--)
		if (path[i] == dsep)
			return path.substr(0, i+1);
	return path;
}

string filename(const string& path) {
	for (size_t i = path.length()-1; i!=SIZE_MAX; i--)
		if (path[i] == dsep)
			return path.substr(i+1);
	return path;
}

string delExt(const string& path) {
	for (size_t i = path.length()-1; i!=SIZE_MAX; i--)
		if (path[i] == '.')
			return path.substr(0, i);
	return path;
}

string hexToStr(int num) {
	std::stringstream ss;
	ss << std::uppercase << std::hex << num;
	return ss.str();
}

string gcButtonStr(SDL_GameControllerButton button) {
	switch (button) {
	case SDL_CONTROLLER_BUTTON_A:
		return "A";
	case SDL_CONTROLLER_BUTTON_B:
		return "B";
	case SDL_CONTROLLER_BUTTON_X:
		return "X";
	case SDL_CONTROLLER_BUTTON_Y:
		return "Y";
	case SDL_CONTROLLER_BUTTON_BACK:
		return "Back";
	case SDL_CONTROLLER_BUTTON_GUIDE:
		return "Guide";
	case SDL_CONTROLLER_BUTTON_START:
		return "Start";
	case SDL_CONTROLLER_BUTTON_LEFTSTICK:
		return "Stick L";
	case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
		return "Stick R";
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		return "Shoulder L";
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		return "Shoulder R";
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		return "Up";
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		return "Down";
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		return "Left";
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		return "Right";
	}
	return "Invalid";
}

string gcAxisStr(SDL_GameControllerAxis axis) {
	switch (axis) {
	case SDL_CONTROLLER_AXIS_LEFTX:
		return "Left X";
	case SDL_CONTROLLER_AXIS_LEFTY:
		return "Left Y";
	case SDL_CONTROLLER_AXIS_RIGHTX:
		return "Right X";
	case SDL_CONTROLLER_AXIS_RIGHTY:
		return "Right Y";
	case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
		return "Trigger L";
	case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
		return "Trigger R";
	}
	return "Invalid";
}

EFileType FileType(const string& path) {
#ifdef _WIN32
	ulong attrib = GetFileAttributesW(stow(path).c_str());
	if (attrib & FILE_ATTRIBUTE_DIRECTORY)
		return EFileType::dir;
	if (attrib & FILE_ATTRIBUTE_SPARSE_FILE)
		return EFileType::link;
#else
	struct stat ps;
	stat(path.c_str(), &ps);
	if (S_ISDIR(ps.st_mode))
		return EFileType::dir;
	if (S_ISLNK(ps.st_mode))
		return EFileType::link;
#endif
	return EFileType::file;
}

vector<string> ListDirRecursively(const string& dir, size_t offs) {
	if (offs == 0)
		offs = dir.length();
	vector<string> entries;
#ifdef _WIN32
	WIN32_FIND_DATAW data;
	HANDLE hFind = FindFirstFileW(stow(dir+"*").c_str(), &data);
	if (hFind == INVALID_HANDLE_VALUE)
		return {};
	do {
		if (data.cFileName == wstring(L".") || data.cFileName == wstring(L".."))
			continue;

		string name = wtos(data.cFileName);
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			vector<string> newEs = ListDirRecursively(dir+name+dsep, offs);
			std::sort(entries.begin(), entries.end());
			entries.insert(entries.end(), newEs.begin(), newEs.end());
		} else
			entries.push_back(dir.substr(offs) + name);
	} while (FindNextFileW(hFind, &data) != 0);
	FindClose(hFind);
#else
	DIR* directory = opendir(dir.c_str());
	if (directory) {
		dirent* data = readdir(directory);
		while (data) {
			if (data->d_name == string(".") || data->d_name == string("..")) {
				data = readdir(directory);
				continue;
			}

			if (data->d_type == DT_DIR) {
				vector<string> newEs = ListDirRecursively(dir+data->d_name+dsep, offs);
				std::sort(entries.begin(), entries.end());
				entries.insert(entries.end(), newEs.begin(), newEs.end());
			} else
				entries.push_back(dir.substr(offs) + data->d_name);

			data = readdir(directory);
		}
		closedir(directory);
	}
#endif
	return entries;
}

string FindFont(const string& font) {
	if (isAbsolute(font)) {	// check fontpath first
		if (FileType(font) == EFileType::file)
			return font;
		return CheckDirForFont(filename(font), parentPath(font));
	}

#ifdef _WIN32
	vector<string> dirFonts = {string(std::getenv("SystemDrive")) + "\\Windows\\Fonts\\"};
#else
	vector<string> dirFonts = {"/usr/share/fonts/"};
#endif
	for (string& dir : dirFonts) {	// check global font directories
		string file = CheckDirForFont(font, dir);
		if (!file.empty())
			return file;
	}
	return "";
}

string CheckDirForFont(const string& font, const string& dir) {
	for (string& it : ListDirRecursively(dir)) {
		string file = findChar(font, '.') ? filename(it) : delExt(filename(it));
		if (strcmpCI(file, font))
			return dir + it;
	}
	return "";
}

bool inRect(const SDL_Rect& rect, vec2i point) {
	return rect.w != 0 && rect.h != 0 && point.x >= rect.x && point.x <= rect.x + rect.w && point.y >= rect.y && point.y <= rect.y + rect.h;
}

bool needsCrop(const SDL_Rect& crop) {
	return crop.x > 0 || crop.y > 0 || crop.w > 0 || crop.h > 0;
}

SDL_Rect getCrop(SDL_Rect item, SDL_Rect frame) {
	vec2i siz(item.w, item.h);
	item.w += item.x;
	item.h += item.y;
	frame.w += frame.x;
	frame.h += frame.y;
	SDL_Rect crop = {0, 0, 0, 0};

	if (item.w < frame.x || item.x > frame.w || item.h < frame.y || item.y > frame.h) {
		crop.w = siz.x;
		crop.h = siz.y;
	} else {
		if (item.x < frame.x)
			crop.x = frame.x - item.x;
		if (item.w > frame.w)
			crop.w = item.w - frame.w;
		if (item.y < frame.y)
			crop.y = frame.y - item.y;
		if (item.h > frame.h)
			crop.h = item.h - frame.h;
	}
	return crop;
}

SDL_Rect cropRect(const SDL_Rect& rect, const SDL_Rect& crop) {
	return {rect.x+crop.x, rect.y+crop.y, rect.w-crop.x-crop.w, rect.h-crop.y-crop.h};
}

SDL_Surface* cropSurface(SDL_Surface* surface, SDL_Rect& rect, SDL_Rect crop) {
	vec2i temp(rect.w, rect.h);
	rect = cropRect(rect, crop);
	crop = {crop.x, crop.y, temp.x - crop.x - crop.w, temp.y - crop.y - crop.h};

	SDL_Surface* sheet = SDL_CreateRGBSurface(surface->flags, crop.w, crop.h, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);
	SDL_BlitSurface(surface, &crop, sheet, 0);
	return sheet;
}

string wtos(const wstring& wstr) {
	return std::wstring_convert<std::codecvt_utf8<wchar>, wchar>().to_bytes(wstr);
}

wstring stow(const string& str) {
	return std::wstring_convert<std::codecvt_utf8<wchar>, wchar>().from_bytes(str);
}

vec2i pix(const vec2f& p) {
	vec2i res = World::winSys()->Resolution();
	return vec2i(p.x * res.x, p.y *res.y);
}

int pixX(float p) {
	return p * World::winSys()->Resolution().x;
}

int pixY(float p) {
	return p * World::winSys()->Resolution().y;
}

vec2f prc(const vec2i& p) {
	vec2i res = World::winSys()->Resolution();
	return vec2f(float(p.x) / float(res.x), float(p.y) / float(res.y));
}

float prcX(int p) {
	return float(p) / float(World::winSys()->Resolution().x);
}

float prcY(int p) {
	return float(p) / float(World::winSys()->Resolution().y);
}
