# Generated from multiview.pro.

cmake_minimum_required(VERSION 3.16)
project(multiview VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

#find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)

#REF: https://doc-snapshots.qt.io/qt6-dev/cmake-qt5-and-qt6-compatibility.html
message(STATUS "Using Qt Version:  ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}")

#NOTE: qt_add_executable is Qt6 specific
#REF: https://bugreports.qt.io/browse/QTCREATORBUG-26062
#qt_add_executable(multiview WIN32 MACOSX_BUNDLE
add_executable(multiview
    src/OBJ_Loader.cpp src/OBJ_Loader.h
    src/control_ui.h
    src/control_window.cpp src/control_window.h
    src/data.cpp src/data.h
    src/geometry.cpp src/geometry.h
    src/image_io.cpp src/image_io.h
    src/image_util.cpp src/image_util.h
    src/io_util.cpp src/io_util.h
    src/iterator_util.h
    src/main.cpp src/main.h
    #src/main_egl.cpp src/main.h
    src/openglwindow.cpp src/openglwindow.h
    src/python_binding.cpp
    src/qt_gl_util.cpp
    src/qt_util.cpp src/qt_util.h
    src/rendering_view.cpp
    src/serialize.cpp src/serialize.h
    src/server.cpp src/server.h
    src/session.cpp src/session.h
    src/shader.cpp src/shader.h
    src/transformation.cpp src/transformation.h
    src/util.cpp src/util.h
    src/counting_semaphore.cpp src/counting_semaphore.h
    src/lang.cpp src/lang.h
    ui/control_ui.ui
)

target_include_directories(multiview PUBLIC
    .
    /usr/include/python3.8
    /usr/include/x86_64-linux-gnu/python3.8
)

target_compile_definitions(multiview PUBLIC
    OPENEXR
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(multiview PUBLIC
    # Remove: L/usr/include/python3.8/
    # Remove: L/usr/include/x86_64-linux-gnu/python3.8/
    EGL
    Half
    Iex
    IexMath
    IlmImf
    IlmThread
    Imath
    Qt5Widgets
    Qt5::Core
    Qt5::Gui
    boost_filesystem
    boost_graph
    boost_numpy38
    boost_python38
    boost_system
    boost_unit_test_framework
    dl
    png
    python3.8
    stdc++fs
    pthread
    GL
)

#NOTE: Added the -pthread compilation flag is Linux specific requirement
#REF: https://stackoverflow.com/questions/34143265/undefined-reference-to-symbol-pthread-createglibc-2-2-5
target_compile_options(multiview PUBLIC
    -Wall
    -Wextra
    -fopenmp
    -g
    -pedantic
    -std=c++17
    -pthread
    -msse4.1
    -mavx
)

#NOTE: Copy shader directory to build directory
#REF: https://stackoverflow.com/a/13429998
#message(STATUS "current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
add_custom_command(TARGET multiview POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/shader/ $<TARGET_FILE_DIR:multiview>/shader
                    COMMENT "Copying shader folder to build directory")

if(Release)
    message(STATUS "** multiview RELEASE build **")
    target_sources(multiview PUBLIC
        src/main.cpp
        #src/main_egl.cpp
    )
endif()

if(Debug)
    message(STATUS "** multiview DEBUG build **")
    target_sources(multiview PUBLIC
        src/main.cpp
        #src/main_egl.cpp
    )

    target_link_libraries(multiview PUBLIC
        # Remove: fsanitize=address
        # Remove: static-libasan
    )

    target_compile_options(multiview
        -fsanitize=address
        -static-libasan
    )
endif()

if(Test)
    message(STATUS "** multiview TEST build **")
    target_sources(multiview PUBLIC
        src/OBJ_Loader_test.h
        src/geometry_test.h
        src/io_util_test.h
        src/data_test.h
        src/test_main.cpp
    )

    target_link_libraries(multiview PUBLIC
        # Remove: fsanitize=address
        # Remove: static-libasan
    )

    target_compile_options(multiview
        -fsanitize=address
        -static-libasan
    )
endif()

if(Library)
    message(STATUS "** multiview LIBRARY build **")
    target_link_libraries(multiview PUBLIC
        # Remove: -shared
        # Remove: fPIC
    )
endif()
