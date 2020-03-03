include(openglwindow.pri)

SOURCES += \
    src/main.cpp
    src/util.cpp
    src/io_util.cpp
    src/image_io.cpp

LIBS += -lImath -lHalf -lIex -lIexMath -lIlmThread -lIlmImf

target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
INSTALLS += target
