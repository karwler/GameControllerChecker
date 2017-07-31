cmake_minimum_required(VERSION 3.1.0)
cmake_policy(SET CMP0015 NEW)
cmake_policy(SET CMP0043 NEW)

project(GameControllerChecker LANGUAGES CXX)
set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

# source files
set(SRC_FILES
	"src/engine/engine.cpp"
	"src/engine/engine.h"
	"src/engine/inputSys.cpp"
	"src/engine/inputSys.h"
	"src/engine/main.cpp"
	"src/engine/scene.cpp"
	"src/engine/scene.h"
	"src/engine/windowSys.cpp"
	"src/engine/windowSys.h"
	"src/engine/world.cpp"
	"src/engine/world.h"
	"src/kklib/aliases.h"
	"src/kklib/sptr.h"
	"src/kklib/vec2.h"
	"src/kklib/vec3.h"
	"src/kklib/vec4.h"
	"src/prog/defaults.h"
	"src/prog/program.cpp"
	"src/prog/program.h"
	"src/utils/objects.cpp"
	"src/utils/objects.h"
	"src/utils/scrollArea.cpp"
	"src/utils/scrollArea.h"
	"src/utils/types.cpp"
	"src/utils/types.h"
	"src/utils/utils.cpp"
	"src/utils/utils.h")

# include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

# library/framework directories
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if (${CMAKE_SIZEOF_VOID_P} MATCHES "8")
		set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib/win64)
		set(DLL_FILES
			"libfreetype-6.dll"
			"SDL2.dll"
			"SDL2_ttf.dll"
			"zlib1.dll")
    else()
        set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib/win32)
		set(DLL_FILES
			"libfreetype-6.dll"
			"SDL2.dll"
			"SDL2_ttf.dll"
			"zlib1.dll")
    endif()	
	link_directories(${LIB_DIR})
endif()

# definitions
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_definitions(-D_UNICODE -D_CRT_SECURE_NO_WARNINGS)
endif()
	
# set main target
add_executable(GameControllerChecker ${SRC_FILES})
target_compile_features(GameControllerChecker PUBLIC cxx_nullptr cxx_enum_forward_declarations cxx_variadic_templates cxx_alias_templates cxx_raw_string_literals cxx_unicode_literals cxx_long_long_type cxx_rvalue_references)

# linker flags
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_link_libraries(GameControllerChecker SDL2.lib SDL2_ttf.lib)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(GameControllerChecker SDL2 SDL2_ttf)
endif()

# target path
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

# target properties
set_target_properties(GameControllerChecker PROPERTIES
									OUTPUT_NAME GameControllerChecker
									RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
									RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_PATH}"
									RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_PATH}")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set_target_properties(GameControllerChecker PROPERTIES
									LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
									LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
endif()

# uncomment the following line for 32 bit build (unless the compiler doesn't use the "-m32" option)
# set_target_properties(GameControllerChecker PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

# post buils commands
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	foreach(dll_file ${DLL_FILES})
		add_custom_command(TARGET GameControllerChecker POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LIB_DIR}/${dll_file} ${EXECUTABLE_OUTPUT_PATH})
	endforeach()
endif()

# filter file for ms visual studio
foreach(source IN LISTS SRC_FILES)
	get_filename_component(source_dir ${source} PATH)
	string(REPLACE "/" ";" dirs "${source_dir}")
	list(GET dirs 0 dir0)
	
	if (${dir0} MATCHES "rsc")
		set(filter_path "Source")
	else()
		list(REMOVE_AT dirs 0)
		list(INSERT dirs 0 "Source")
		string(REPLACE ";" "\\" filter_path "${dirs}")
	endif()
	source_group("${filter_path}" FILES ${source})
endforeach()

