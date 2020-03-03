include(openglwindow.pri)

SOURCES += \
    main.cpp
    util.cpp
    io_util.cpp
    image_io.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
INSTALLS += target
