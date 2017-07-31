# GamepadChecker
A small GUI application for displaying the indexes and values of pushed buttons or axes on a game controller.  
You can switch between DirectInput, XInput and haptic testing if supported by the controller.  

Used libraries are SDL2 and SDL2_ttf.  
The minimum required Cmake version is 3.1.0 and your compiler needs to support at least C++11.  

Note: When setting the output directory in Qt, don’t name it ‘build’, cause it might mess up the executable’s location.  

## Linux
All dependencies need to be installed manually.  
Installing the packages "libsdl2-dev libsdl2-ttf-dev" should do the trick. (Assuming that all necessary dependencies for those are installed automatically in the process.)  
The default font is set to "Arial", so you probably need to install some kind of "ttf-ms-fonts" package.  
If you don't want to install any new fonts, you can just change the default font in the "src/prog/defaults.h" file.  

## Windows
All necessary libraries are already included in the project. However they're built for the MSVC.  
If you're using a different compiler, you'll need to replace the existing library files or link the new ones.  