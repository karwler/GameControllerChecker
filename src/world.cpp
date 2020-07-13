#include "world.h"
#ifdef _WIN32
#include <windows.h>
#endif

InputSys* World::Input() {
	return base.Input();
}

Program* World::Prog() {
	return base.Input()->Prog();
}

WindowSys* World::Window() {
	return &base;
}

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
#else
int main() {
#endif
	return World::Window()->Start();
}
