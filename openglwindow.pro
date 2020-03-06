include(openglwindow.pri)

SOURCES += \
    src/main.cpp
    src/util.cpp
    src/io_util.cpp
    src/image_io.cpp

FORMS += ui/control.ui \
    src/controlwindow.ui
HEADERS += control_window.h
LIBS += -lImath -lHalf -lIex -lIexMath -lIlmThread -lIlmImf -ldl -lboost_system -lboost_filesystem -lmainwindow

target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
INSTALLS += target
