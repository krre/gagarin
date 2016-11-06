QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OctreeFarm
TEMPLATE = app
CONFIG += c++11
win32: LIBS += -L$$(GLEW_HOME)/lib -lglew32 -lopengl32
linux: LIBS += -lGLEW

INCLUDEPATH += "../../ThirdParty" $$(GLEW_HOME)/include

SOURCES += main.cpp\
    MainWindow.cpp \
    Octree.cpp \
    Camera.cpp \
    Viewport.cpp \
    Properties.cpp \
    Source.cpp \
    ../../Source/Core/Utils.cpp \
    ../../Source/Core/Object.cpp \
    Command.cpp \
    Options.cpp

HEADERS  += MainWindow.h \
    Octree.h \
    Camera.h \
    Viewport.h \
    Properties.h \
    ../../Source/Core/Common.h \
    Source.h \
    ../../Source/Core/Utils.h \
    ../../Source/Core/Object.h \
    Defines.h \
    Command.h \
    Options.h
