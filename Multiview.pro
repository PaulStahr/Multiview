######################################################################
# Automatically generated by qmake (3.1) Thu Mar 5 22:33:41 2020
######################################################################

TEMPLATE = app
TARGET = Multiview
INCLUDEPATH += .
OBJECTS_DIR = ../object
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
FORMS += src/testdialog.ui ui/control_ui.ui
SOURCES += src/control_window.cpp \
           src/data.cpp \
           src/geometry.cpp \
           src/image_io.cpp \
           src/image_util.cpp \
           src/io_util.cpp \
           src/main.cpp \
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
           src/qt_gl_util.cpp
LIBS += -lImath -lHalf -lIex -lIexMath -lIlmThread -lIlmImf -ldl -lboost_system -lboost_filesystem -lQt5Widgets
