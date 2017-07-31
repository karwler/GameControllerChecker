# config
TEMPLATE = app
CONFIG += c++11

win32 {
    CONFIG(debug, debug|release) {
        CONFIG += console
    }
}

# output
TARGET = GameControllerChecker
OBJECTS_DIR = $$OUT_PWD/bin
DESTDIR = $$OUT_PWD/build

# copy data dir and dependencies
win32 {
    PWD_WIN = $$PWD
    PWD_WIN ~= s,/,\\,g
    DEST_WIN = $$DESTDIR
    DEST_WIN ~= s,/,\\,g

    contains(QT_ARCH, i386) {
        LIB_WIN = $$PWD/lib/win32
    } else {
        LIB_WIN = $$PWD/lib/win64
    }
    LIB_WIN ~= s,/,\\,g

    postbuild.commands = $$quote(cmd /c copy $$LIB_WIN\\*.dll $$DEST_WIN)
}

QMAKE_EXTRA_TARGETS += postbuild
POST_TARGETDEPS += postbuild

# includepaths
INCLUDEPATH += $$PWD/src
win32: INCLUDEPATH += $$PWD/include

# dependencies' directories
win32: LIBS += -L$$LIB_WIN

# linker flags
LIBS += -lSDL2 \
        -lSDL2_ttf

# definitions
win32: DEFINES += _UNICODE \
				  _CRT_SECURE_NO_WARNINGS

# set sources
SOURCES += src/engine/engine.cpp \
    src/engine/inputSys.cpp \
    src/engine/main.cpp \
    src/engine/scene.cpp \
    src/engine/windowSys.cpp \
    src/engine/world.cpp \
    src/prog/program.cpp \
    src/utils/objects.cpp \
    src/utils/scrollArea.cpp \
    src/utils/types.cpp \
    src/utils/utils.cpp

HEADERS += src/engine/engine.h \
    src/engine/inputSys.h \
    src/engine/scene.h \
    src/engine/windowSys.h \
    src/engine/world.h \
    src/kklib/aliases.h \
    src/kklib/sptr.h \
    src/kklib/vec2.h \
    src/kklib/vec3.h \
    src/kklib/vec4.h \
    src/prog/defaults.h \
    src/prog/program.h \
    src/utils/objects.h \
    src/utils/scrollArea.h \
    src/utils/types.h \
    src/utils/utils.h
