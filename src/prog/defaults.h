#pragma once

// directory separator
#ifdef _WIN32
const char dsep = '\\';
#else
const char dsep = '/';
#endif

// video settings
#define DEFAULT_RESOLUTION	vec2i(800, 600)
#define DEFAULT_FONT		"arial"

// colors
#define DEFAULT_COLOR_BACKGROUND	vec4c(10, 10, 10, 255)
#define DEFAULT_COLOR_RECTANGLE		vec4c(90, 90, 90, 255)
#define DEFAULT_COLOR_HIGHLIGHTED	vec4c(120, 120, 120, 255)
#define DEFAULT_COLOR_DARKENED		vec4c(60, 60, 60, 255)
#define DEFAULT_COLOR_TEXT			vec4c(210, 210, 210, 255)

// program constants
const float scrollFactor = 50.f;
