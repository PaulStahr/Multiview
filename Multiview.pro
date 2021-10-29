######################################################################
# Automatically generated by qmake (3.1) Thu Mar 5 22:33:41 2020
######################################################################

TEMPLATE = app
TARGET = Multiview

QMAKE_CXXFLAGS+= -fopenmp -g -pedantic -Wextra -Wall
QMAKE_LFLAGS +=  -fopenmp -g

INCLUDEPATH += .

CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17
# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += OPENEXR
# Input
HEADERS += src/control_ui.h \
           src/control_window.h \
           src/data.h \
           src/geometry.h \
           src/image_io.h \
           src/image_util.h \
           src/io_util.h \
           src/iterator_util.h \
           src/main.h \
           src/OBJ_Loader.h \
           src/openglwindow.h \
           src/qt_util.h \
           src/serialize.h \
           src/server.h \
           src/session.h \
           src/shader.h \
           src/transformation.h \
           src/util.h
FORMS += ui/control_ui.ui
SOURCES += src/control_window.cpp \
           src/data.cpp \
           src/geometry.cpp \
           src/image_io.cpp \
           src/image_util.cpp \
           src/io_util.cpp \
           src/OBJ_Loader.cpp \
           src/openglwindow.cpp \
           src/qt_util.cpp \
           src/serialize.cpp \
           src/server.cpp \
           src/session.cpp \
           src/shader.cpp \
           src/transformation.cpp \
           src/util.cpp\
           src/rendering_view.cpp\
           src/qt_gl_util.cpp\
           src/python_binding.cpp
LIBS +=  -L/usr/include/x86_64-linux-gnu/python3.8/ -L/usr/include/python3.8/ -lImath -lHalf -lIex -lIexMath -lIlmThread -lIlmImf -ldl -lboost_system -lboost_filesystem -lQt5Widgets -lstdc++fs -lpng -lEGL -lpython3.8 -lboost_graph -lboost_numpy38 -lboost_python38 -lboost_system -lboost_filesystem  -lboost_unit_test_framework
INCLUDEPATH += /usr/include/python3.8/ /usr/include/x86_64-linux-gnu/python3.8/

Release {
    TARGET = Multiview
    SOURCES += src/main.cpp    
    OBJECTS_DIR = ./object
}

Debug {
    TARGET = Multiview_debug
    SOURCES += src/main.cpp
    QMAKE_CXXFLAGS+=-fsanitize=address -static-libasan
    LIBS+=-fsanitize=address -static-libasan
    OBJECTS_DIR = ./object_debug
}

Test {
    TARGET = unit_test
    SOURCES += src/test_main.cpp
    HEADERS += src/io_util_test.h src/OBJ_Loader_test.h src/geometry_test.h
    QMAKE_CXXFLAGS+=-fsanitize=address -static-libasan
    LIBS+=-fsanitize=address -static-libasan
    OBJECTS_DIR = ./object_debug
}

Library {
    TARGET = Multiview.so
    OBJECTS_DIR = ./object
    LIBS+=--shared -fPIC
}
