INCLUDEPATH += $$PWD/src
SOURCES += $$PWD/src/openglwindow.cpp $$PWD/src/io_util.cpp $$PWD/src/geometry.cpp $$PWD/src/image_io.cpp $$PWD/src/transformation.cpp $$PWD/src/qt_util.cpp $$PWD/src/shader.cpp $$PWD/src/image_util.cpp $$PWD/src/server.cpp $$PWD/src/session.cpp $$PWD/src/data.cpp $$PWD/src/OBJ_Loader.cpp
HEADERS += $$PWD/src/openglwindow.h
FORMS += ui/control.ui
OBJECTS_DIR = ../object
UI_DIR = ../ui
LIBS += -lImath -lHalf -lIex -lIexMath -lIlmThread -lIlmImf -ldl -lboost_system -lboost_filesystem -lQt5Widgets
