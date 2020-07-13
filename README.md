# GameControllerChecker
A small GUI application for displaying the indices and values of pushed buttons, hats and axes on a game controller.  
You can switch between DirectInput, XInput and haptic testing if supported by the controller.  

Used libraries are SDL2 and SDL2_ttf.  
The minimum required CMake version is 3.10.2 and your compiler needs to support at least C++17.  

## Linux
All dependencies need to be installed manually.  
Installing the packages "libsdl2-dev libsdl2-ttf-dev" should do the trick. (Assuming that all necessary dependencies for those are installed automatically in the process.)  
The default font is set to "Arial", so you probably need to install some kind of "ttf-ms-fonts" package.  
If you don't want to install any new fonts, you can just change the default font in the "src/windowSys.h" file in line 9.  

## Windows
All necessary libraries are downloaded while running CMake.  
