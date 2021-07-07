#include "windowSys.h"
#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
#else
int main() {
#endif
	return WindowSys().Start();
}
