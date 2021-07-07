cmake_minimum_required(VERSION 3.10.2)
project(GameControllerChecker LANGUAGES CXX)
if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# source files
set(SRC_FILES
	"src/inputSys.cpp"
	"src/inputSys.h"
	"src/main.cpp"
	"src/objects.cpp"
	"src/objects.h"
	"src/program.cpp"
	"src/program.h"
	"src/utils.h"
	"src/windowSys.cpp"
	"src/windowSys.h")

# dependencies
set(VER_SDL "2.0.12" CACHE STRING "SDL2 version.")
set(VER_TTF "2.0.15" CACHE STRING "SDL2_ttf version.")
set(DIR_LIB "${CMAKE_SOURCE_DIR}/lib")

function(downloadLib URL DST LIBDIR)
	get_filename_component(LIBDIR "${LIBDIR}" NAME)
	if(EXISTS "${DST}/${LIBDIR}")
		message(STATUS "Found ${DST}/${LIBDIR}")
		return()
	endif()

	get_filename_component(FILE "${URL}" NAME)
	if(EXISTS "${DST}/${FILE}")
		message(STATUS "Found ${DST}/${FILE}")
	else()
		message(STATUS "Downloading ${URL}")
		file(DOWNLOAD "${URL}" "${DST}/${FILE}")
	endif()

	if(FILE MATCHES ".zip$")
		execute_process(COMMAND "${CMAKE_COMMAND}" -E tar xf "${FILE}" WORKING_DIRECTORY "${DST}")
	elseif(FILE MATCHES ".tar.gz$")
		execute_process(COMMAND "${CMAKE_COMMAND}" -E tar xzf "${FILE}" WORKING_DIRECTORY "${DST}")
	elseif(FILE MATCHES ".dmg$")
		execute_process(COMMAND hdiutil attach "${DST}/${FILE}")
		string(FIND "${FILE}" "-" FID)
		string(SUBSTRING "${FILE}" 0 ${FID} FNAME)
		file(COPY "/Volumes/${FNAME}/${FNAME}.framework" DESTINATION "${DST}")
	endif()
endfunction()

include_directories("${CMAKE_SOURCE_DIR}/src")
if(NOT EMSCRIPTEN)
	file(MAKE_DIRECTORY "${DIR_LIB}")
	if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
		if(MSVC)
			set(DIR_SDL "${DIR_LIB}/SDL2-${VER_SDL}")
			set(DIR_TTF "${DIR_LIB}/SDL2_ttf-${VER_TTF}")
			if(CMAKE_SIZEOF_VOID_P EQUAL 8)
				set(BIN_SDL "${DIR_SDL}/lib/x64")
				set(BIN_TTF "${DIR_TTF}/lib/x64")
			else()
				set(BIN_SDL "${DIR_SDL}/lib/x86")
				set(BIN_TTF "${DIR_TTF}/lib/x86")
			endif()
			set(LIB_SDL "${BIN_SDL}")
			set(LIB_TTF "${BIN_TTF}")
			set(INC_SDL "${DIR_SDL}/include")
			set(INC_TTF "${DIR_TTF}/include")
			set(URL_SDL "https://www.libsdl.org/release/SDL2-devel-${VER_SDL}-VC.zip")
			set(URL_TTF "https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-${VER_TTF}-VC.zip")
		else()
			if(CMAKE_SIZEOF_VOID_P EQUAL 8)
				set(DIR_SDL "${DIR_LIB}/SDL2-${VER_SDL}/x86_64-w64-mingw32")
				set(DIR_TTF "${DIR_LIB}/SDL2_ttf-${VER_TTF}/x86_64-w64-mingw32")
			else()
				set(DIR_SDL "${DIR_LIB}/SDL2-${VER_SDL}/i686-w64-mingw32")
				set(DIR_TTF "${DIR_LIB}/SDL2_ttf-${VER_TTF}/i686-w64-mingw32")
			endif()
			set(DIR_GLEW "${DIR_LIB}/glew-${VER_GLEW}")
			set(BIN_SDL "${DIR_SDL}/bin")
			set(BIN_TTF "${DIR_TTF}/bin")
			set(LIB_SDL "${DIR_SDL}/lib")
			set(LIB_TTF "${DIR_TTF}/lib")
			set(INC_SDL "${DIR_SDL}/include/SDL2")
			set(INC_TTF "${DIR_TTF}/include/SDL2")
			set(URL_SDL "https://www.libsdl.org/release/SDL2-devel-${VER_SDL}-mingw.tar.gz")
			set(URL_TTF "https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-${VER_TTF}-mingw.tar.gz")
		endif()
		downloadLib("${URL_SDL}" "${DIR_LIB}" "${DIR_SDL}")
		downloadLib("${URL_TTF}" "${DIR_LIB}" "${DIR_TTF}")
		link_directories("${LIB_SDL}" "${LIB_TTF}")
		include_directories("${INC_SDL}" "${INC_TTF}")
	endif()
endif()

# flags
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_definitions(-D_UNICODE -D_CRT_SECURE_NO_WARNINGS)
endif()

if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
	set(CXX_FLG_ANY "-Wall -Wextra -pedantic-errors -Wdouble-promotion -Wformat=2 -Wmissing-declarations -Wold-style-cast -Wshadow -Wunreachable-code -Wno-implicit-fallthrough -Wno-switch")
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(CXX_FLG_ANY "${CXX_FLG_ANY} -Wcast-align=strict -Wduplicated-branches -Wduplicated-cond -Wlogical-op -Wuseless-cast")
	else()
		set(CXX_FLG_ANY "${CXX_FLG_ANY} -Wcast-align -Wnull-dereference -Wshadow-all -Wno-final-dtor-non-final-class -Wno-unused-private-field")
	endif()

	if(EMSCRIPTEN)
		set(CXX_FLG_ANY "${CXX_FLG_ANY} --preload-file ${CMAKE_BINARY_DIR}/data@/ -s TOTAL_MEMORY=536870912 -s WASM=1 -s USE_SDL=2 -s USE_SDL_TTF=2 -lidbfs.js -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-parameter")
		set(FLG_DEBUG "-g4 -s DISABLE_EXCEPTION_CATCHING=0")
		set(FLG_RELEASE "-O3")
	else()
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(CXX_FLG_ANY "${CXX_FLG_ANY} -mmmx -msse -msse2")
		endif()
		set(FLG_DEBUG "-g")
		set(FLG_RELEASE "-O3")
		if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			set(FLG_RELEASE "${FLG_RELEASE} -flto -fuse-linker-plugin -s")
		endif()
	endif()
elseif(MSVC)
	set(CXX_FLG_ANY "/W4 /wd4100 /wd4201 /wd4244 /wd4458 /wd4706 /wd4805")
	set(FLG_DEBUG "/Od /Ob0")
	set(FLG_RELEASE "/O2 /Ob2 /Oi /Ot /GL")
endif()

if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_FLG_ANY} ${FLG_DEBUG}")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLG_ANY} ${FLG_RELEASE}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLG_ANY} ${FLG_DEBUG}")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLG_ANY} ${FLG_RELEASE}")
endif()

# main target
add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES})
if(EMSCRIPTEN)
	return()
endif()
target_link_libraries(${PROJECT_NAME} SDL2 SDL2_ttf)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set_target_properties(${PROJECT_NAME} PROPERTIES
									RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
									RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_PATH}"
									RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_PATH}")

# post buils commands
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
						COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${BIN_SDL}/SDL2.dll" "${BIN_TTF}/SDL2_ttf.dll" "${BIN_TTF}/libfreetype-6.dll" "${BIN_TTF}/zlib1.dll" "${EXECUTABLE_OUTPUT_PATH}"
						COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${DIR_SDL}/COPYING.txt" "${EXECUTABLE_OUTPUT_PATH}/COPYING.SDL.txt"
						COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${BIN_TTF}/LICENSE.freetype.txt" "${EXECUTABLE_OUTPUT_PATH}"
						COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${BIN_TTF}/LICENSE.zlib.txt" "${EXECUTABLE_OUTPUT_PATH}")
endif()

if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") AND (NOT CMAKE_BUILD_TYPE STREQUAL "Debug"))
	find_program(STRIPPER "strip")
	if(STRIPPER)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND "${STRIPPER}" -s "${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}")
	endif()
endif()

# filter file for ms visual studio
foreach(FSRC IN LISTS SRC_FILES)
	get_filename_component(FGRP "${FSRC}" DIRECTORY)
	source_group("${FGRP}" FILES "${FSRC}")
endforeach()
