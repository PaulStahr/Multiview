#############################################################################
# Makefile for building: Multiview
# Generated by qmake (3.1) (Qt 5.9.5)
# Project:  Multiview.pro
# Template: app
# Command: /usr/lib/qt5/bin/qmake -o Makefile Multiview.pro
#############################################################################

MAKEFILE      = Makefile

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_DEPRECATED_WARNINGS -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT -fPIC $(DEFINES)
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT -fPIC $(DEFINES)
INCPATH       = -I. -I. -isystem /usr/include/x86_64-linux-gnu/qt5 -isystem /usr/include/x86_64-linux-gnu/qt5/QtGui -isystem /usr/include/x86_64-linux-gnu/qt5/QtCore -I. -isystem /usr/include/libdrm -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++
QMAKE         = /usr/lib/qt5/bin/qmake
DEL_FILE      = rm -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
COPY          = cp -f
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
INSTALL_FILE  = install -m 644 -p
INSTALL_PROGRAM = install -m 755 -p
INSTALL_DIR   = cp -f -R
QINSTALL      = /usr/lib/qt5/bin/qmake -install qinstall
QINSTALL_PROGRAM = /usr/lib/qt5/bin/qmake -install qinstall -exe
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
TAR           = tar -cf
COMPRESS      = gzip -9f
DISTNAME      = Multiview1.0.0
DISTDIR = /mnt/Caesar/object/Multiview1.0.0
LINK          = g++
LFLAGS        = -Wl,-O1
LIBS          = $(SUBLIBS) -lImath -lHalf -lIex -lIexMath -lIlmThread -lIlmImf -ldl -lboost_system -lboost_filesystem -lQt5Widgets -lQt5Gui -lQt5Core -lGL -lpthread 
AR            = ar cqs
RANLIB        = 
SED           = sed
STRIP         = strip

####### Output directory

OBJECTS_DIR   = ../object/

####### Files

SOURCES       = src/control_window.cpp \
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
		src/util.cpp \
		src/rendering_view.cpp \
		src/qt_gl_util.cpp moc_control_window.cpp \
		moc_openglwindow.cpp
OBJECTS       = ../object/control_window.o \
		../object/data.o \
		../object/geometry.o \
		../object/image_io.o \
		../object/image_util.o \
		../object/io_util.o \
		../object/main.o \
		../object/OBJ_Loader.o \
		../object/openglwindow.o \
		../object/qt_util.o \
		../object/serialize.o \
		../object/server.o \
		../object/session.o \
		../object/shader.o \
		../object/transformation.o \
		../object/util.o \
		../object/rendering_view.o \
		../object/qt_gl_util.o \
		../object/moc_control_window.o \
		../object/moc_openglwindow.o
DIST          = /usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/spec_pre.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/unix.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/linux.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/sanitize.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/gcc-base.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/gcc-base-unix.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/g++-base.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/g++-unix.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/qconfig.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_Attica.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_Baloo.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KAuth.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KBookmarks.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KCodecs.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KCompletion.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KConfigCore.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KConfigGui.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KConfigWidgets.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KCoreAddons.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KDBusAddons.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KGlobalAccel.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KGuiAddons.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KI18n.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIconThemes.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIOCore.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIOFileWidgets.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIOGui.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIOWidgets.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KItemModels.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KItemViews.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KJobWidgets.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KNTLM.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KService.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KTextWidgets.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KWidgetsAddons.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KWindowSystem.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KXmlGui.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_accessibility_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_bootstrap_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_concurrent.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_concurrent_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_core.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_core_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_dbus.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_dbus_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_devicediscovery_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_egl_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_eglfs_kms_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_eglfsdeviceintegration_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_eventdispatcher_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_fb_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_fontdatabase_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_glx_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_gui.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_gui_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_input_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_kms_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_linuxaccessibility_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_network.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_network_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_opengl.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_opengl_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_openglextensions.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_openglextensions_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_platformcompositor_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_printsupport.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_printsupport_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_script.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_scripttools.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_service_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_sql.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_sql_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_testlib.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_testlib_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_theme_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_widgets.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_widgets_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_xcb_qpa_lib_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_xml.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_xml_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_Solid.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_SonnetCore.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_SonnetUi.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_ThreadWeaver.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qt_functions.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qt_config.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++/qmake.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/spec_post.prf \
		.qmake.stash \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/exclusive_builds.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/toolchain.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/default_pre.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/resolve_config.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/default_post.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/warn_on.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qt.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/resources.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/moc.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/unix/opengl.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/unix/thread.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qmake_use.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/file_copies.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/testcase_targets.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/exceptions.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/yacc.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/lex.prf \
		Multiview.pro src/control.h \
		src/control_ui.h \
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
		src/util.h src/control_window.cpp \
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
		src/util.cpp \
		src/rendering_view.cpp \
		src/qt_gl_util.cpp
QMAKE_TARGET  = Multiview
DESTDIR       = 
TARGET        = Multiview


first: all
####### Build rules

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: Multiview.pro /usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++/qmake.conf /usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/spec_pre.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/unix.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/linux.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/sanitize.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/gcc-base.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/gcc-base-unix.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/g++-base.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/g++-unix.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/qconfig.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_Attica.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_Baloo.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KAuth.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KBookmarks.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KCodecs.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KCompletion.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KConfigCore.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KConfigGui.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KConfigWidgets.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KCoreAddons.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KDBusAddons.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KGlobalAccel.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KGuiAddons.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KI18n.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIconThemes.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIOCore.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIOFileWidgets.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIOGui.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIOWidgets.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KItemModels.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KItemViews.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KJobWidgets.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KNTLM.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KService.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KTextWidgets.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KWidgetsAddons.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KWindowSystem.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KXmlGui.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_accessibility_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_bootstrap_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_concurrent.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_concurrent_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_core.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_core_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_dbus.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_dbus_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_devicediscovery_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_egl_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_eglfs_kms_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_eglfsdeviceintegration_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_eventdispatcher_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_fb_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_fontdatabase_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_glx_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_gui.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_gui_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_input_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_kms_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_linuxaccessibility_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_network.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_network_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_opengl.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_opengl_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_openglextensions.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_openglextensions_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_platformcompositor_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_printsupport.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_printsupport_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_script.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_scripttools.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_service_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_sql.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_sql_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_testlib.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_testlib_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_theme_support_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_widgets.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_widgets_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_xcb_qpa_lib_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_xml.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_xml_private.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_Solid.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_SonnetCore.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_SonnetUi.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_ThreadWeaver.pri \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qt_functions.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qt_config.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++/qmake.conf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/spec_post.prf \
		.qmake.stash \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/exclusive_builds.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/toolchain.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/default_pre.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/resolve_config.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/default_post.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/warn_on.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qt.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/resources.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/moc.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/unix/opengl.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/unix/thread.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qmake_use.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/file_copies.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/testcase_targets.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/exceptions.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/yacc.prf \
		/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/lex.prf \
		Multiview.pro \
		/usr/lib/x86_64-linux-gnu/libQt5Widgets.prl \
		/usr/lib/x86_64-linux-gnu/libQt5Gui.prl \
		/usr/lib/x86_64-linux-gnu/libQt5Core.prl
	$(QMAKE) -o Makefile Multiview.pro
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/spec_pre.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/unix.conf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/linux.conf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/sanitize.conf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/gcc-base.conf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/gcc-base-unix.conf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/g++-base.conf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/common/g++-unix.conf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/qconfig.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_Attica.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_Baloo.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KAuth.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KBookmarks.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KCodecs.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KCompletion.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KConfigCore.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KConfigGui.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KConfigWidgets.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KCoreAddons.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KDBusAddons.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KGlobalAccel.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KGuiAddons.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KI18n.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIconThemes.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIOCore.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIOFileWidgets.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIOGui.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KIOWidgets.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KItemModels.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KItemViews.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KJobWidgets.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KNTLM.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KService.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KTextWidgets.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KWidgetsAddons.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KWindowSystem.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_KXmlGui.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_accessibility_support_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_bootstrap_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_concurrent.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_concurrent_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_core.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_core_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_dbus.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_dbus_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_devicediscovery_support_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_egl_support_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_eglfs_kms_support_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_eglfsdeviceintegration_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_eventdispatcher_support_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_fb_support_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_fontdatabase_support_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_glx_support_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_gui.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_gui_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_input_support_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_kms_support_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_linuxaccessibility_support_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_network.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_network_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_opengl.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_opengl_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_openglextensions.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_openglextensions_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_platformcompositor_support_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_printsupport.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_printsupport_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_script.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_scripttools.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_service_support_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_sql.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_sql_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_testlib.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_testlib_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_theme_support_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_widgets.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_widgets_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_xcb_qpa_lib_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_xml.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_lib_xml_private.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_Solid.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_SonnetCore.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_SonnetUi.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/modules/qt_ThreadWeaver.pri:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qt_functions.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qt_config.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++/qmake.conf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/spec_post.prf:
.qmake.stash:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/exclusive_builds.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/toolchain.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/default_pre.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/resolve_config.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/default_post.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/warn_on.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qt.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/resources.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/moc.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/unix/opengl.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/unix/thread.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qmake_use.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/file_copies.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/testcase_targets.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/exceptions.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/yacc.prf:
/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/lex.prf:
Multiview.pro:
/usr/lib/x86_64-linux-gnu/libQt5Widgets.prl:
/usr/lib/x86_64-linux-gnu/libQt5Gui.prl:
/usr/lib/x86_64-linux-gnu/libQt5Core.prl:
qmake: FORCE
	@$(QMAKE) -o Makefile Multiview.pro

qmake_all: FORCE


all: Makefile $(TARGET)

dist: distdir FORCE
	(cd `dirname $(DISTDIR)` && $(TAR) $(DISTNAME).tar $(DISTNAME) && $(COMPRESS) $(DISTNAME).tar) && $(MOVE) `dirname $(DISTDIR)`/$(DISTNAME).tar.gz . && $(DEL_FILE) -r $(DISTDIR)

distdir: FORCE
	@test -d $(DISTDIR) || mkdir -p $(DISTDIR)
	$(COPY_FILE) --parents $(DIST) $(DISTDIR)/
	$(COPY_FILE) --parents /usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/data/dummy.cpp $(DISTDIR)/
	$(COPY_FILE) --parents src/control.h src/control_ui.h src/control_window.h src/data.h src/geometry.h src/image_io.h src/image_util.h src/io_util.h src/iterator_util.h src/main.h src/OBJ_Loader.h src/openglwindow.h src/qt_util.h src/serialize.h src/server.h src/session.h src/shader.h src/transformation.h src/util.h $(DISTDIR)/
	$(COPY_FILE) --parents src/control_window.cpp src/data.cpp src/geometry.cpp src/image_io.cpp src/image_util.cpp src/io_util.cpp src/main.cpp src/OBJ_Loader.cpp src/openglwindow.cpp src/qt_util.cpp src/serialize.cpp src/server.cpp src/session.cpp src/shader.cpp src/transformation.cpp src/util.cpp src/rendering_view.cpp src/qt_gl_util.cpp $(DISTDIR)/


clean: compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


distclean: clean 
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) .qmake.stash
	-$(DEL_FILE) Makefile


####### Sub-libraries

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

check: first

benchmark: first

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_moc_predefs_make_all: moc_predefs.h
compiler_moc_predefs_clean:
	-$(DEL_FILE) moc_predefs.h
moc_predefs.h: /usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/data/dummy.cpp
	g++ -pipe -O2 -Wall -W -dM -E -o moc_predefs.h /usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/data/dummy.cpp

compiler_moc_header_make_all: moc_control_window.cpp moc_openglwindow.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_control_window.cpp moc_openglwindow.cpp
moc_control_window.cpp: src/session.h \
		src/image_util.h \
		src/util.h \
		src/iterator_util.h \
		src/data.h \
		src/OBJ_Loader.h \
		src/geometry.h \
		src/io_util.h \
		src/qt_util.h \
		src/control_ui.h \
		src/control_window.h \
		moc_predefs.h \
		/usr/lib/qt5/bin/moc
	/usr/lib/qt5/bin/moc $(DEFINES) --include ./moc_predefs.h -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++ -I/mnt/Caesar/Multiview -I/mnt/Caesar/Multiview -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/include/c++/9 -I/usr/include/x86_64-linux-gnu/c++/9 -I/usr/include/c++/9/backward -I/usr/lib/gcc/x86_64-linux-gnu/9/include -I/usr/local/include -I/usr/lib/gcc/x86_64-linux-gnu/9/include-fixed -I/usr/include/x86_64-linux-gnu -I/usr/include src/control_window.h -o moc_control_window.cpp

moc_openglwindow.cpp: src/openglwindow.h \
		moc_predefs.h \
		/usr/lib/qt5/bin/moc
	/usr/lib/qt5/bin/moc $(DEFINES) --include ./moc_predefs.h -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++ -I/mnt/Caesar/Multiview -I/mnt/Caesar/Multiview -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/include/c++/9 -I/usr/include/x86_64-linux-gnu/c++/9 -I/usr/include/c++/9/backward -I/usr/lib/gcc/x86_64-linux-gnu/9/include -I/usr/local/include -I/usr/lib/gcc/x86_64-linux-gnu/9/include-fixed -I/usr/include/x86_64-linux-gnu -I/usr/include src/openglwindow.h -o moc_openglwindow.cpp

compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_predefs_clean compiler_moc_header_clean 

####### Compile

../object/control_window.o: src/control_window.cpp src/control_window.h \
		src/session.h \
		src/image_util.h \
		src/util.h \
		src/iterator_util.h \
		src/data.h \
		src/OBJ_Loader.h \
		src/geometry.h \
		src/io_util.h \
		src/qt_util.h \
		src/control_ui.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/control_window.o src/control_window.cpp

../object/data.o: src/data.cpp src/data.h \
		src/OBJ_Loader.h \
		src/geometry.h \
		src/io_util.h \
		src/util.h \
		src/iterator_util.h \
		src/image_util.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/data.o src/data.cpp

../object/geometry.o: src/geometry.cpp src/geometry.h \
		src/io_util.h \
		src/util.h \
		src/iterator_util.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/geometry.o src/geometry.cpp

../object/image_io.o: src/image_io.cpp src/image_io.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/image_io.o src/image_io.cpp

../object/image_util.o: src/image_util.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/image_util.o src/image_util.cpp

../object/io_util.o: src/io_util.cpp src/io_util.h \
		src/util.h \
		src/iterator_util.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/io_util.o src/io_util.cpp

../object/main.o: src/main.cpp src/serialize.h \
		src/main.h \
		src/image_util.h \
		src/util.h \
		src/iterator_util.h \
		src/server.h \
		src/session.h \
		src/data.h \
		src/OBJ_Loader.h \
		src/geometry.h \
		src/io_util.h \
		src/qt_util.h \
		src/control_window.h \
		src/control_ui.h \
		src/rendering_view.h \
		src/transformation.h \
		src/image_io.h \
		src/shader.h \
		src/openglwindow.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/main.o src/main.cpp

../object/OBJ_Loader.o: src/OBJ_Loader.cpp src/OBJ_Loader.h \
		src/geometry.h \
		src/io_util.h \
		src/util.h \
		src/iterator_util.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/OBJ_Loader.o src/OBJ_Loader.cpp

../object/openglwindow.o: src/openglwindow.cpp src/openglwindow.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/openglwindow.o src/openglwindow.cpp

../object/qt_util.o: src/qt_util.cpp src/qt_util.h \
		src/data.h \
		src/OBJ_Loader.h \
		src/geometry.h \
		src/io_util.h \
		src/util.h \
		src/iterator_util.h \
		src/image_util.h \
		src/image_io.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/qt_util.o src/qt_util.cpp

../object/serialize.o: src/serialize.cpp src/serialize.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/serialize.o src/serialize.cpp

../object/server.o: src/server.cpp src/server.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/server.o src/server.cpp

../object/session.o: src/session.cpp src/session.h \
		src/image_util.h \
		src/util.h \
		src/iterator_util.h \
		src/data.h \
		src/OBJ_Loader.h \
		src/geometry.h \
		src/io_util.h \
		src/qt_util.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/session.o src/session.cpp

../object/shader.o: src/shader.cpp src/shader.h \
		src/io_util.h \
		src/util.h \
		src/iterator_util.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/shader.o src/shader.cpp

../object/transformation.o: src/transformation.cpp src/transformation.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/transformation.o src/transformation.cpp

../object/util.o: src/util.cpp src/util.h \
		src/iterator_util.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/util.o src/util.cpp

../object/rendering_view.o: src/rendering_view.cpp src/rendering_view.h \
		src/OBJ_Loader.h \
		src/geometry.h \
		src/io_util.h \
		src/util.h \
		src/iterator_util.h \
		src/session.h \
		src/image_util.h \
		src/data.h \
		src/qt_util.h \
		src/transformation.h \
		src/image_io.h \
		src/shader.h \
		src/openglwindow.h \
		src/qt_gl_util.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/rendering_view.o src/rendering_view.cpp

../object/qt_gl_util.o: src/qt_gl_util.cpp src/qt_gl_util.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/qt_gl_util.o src/qt_gl_util.cpp

../object/moc_control_window.o: moc_control_window.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/moc_control_window.o moc_control_window.cpp

../object/moc_openglwindow.o: moc_openglwindow.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../object/moc_openglwindow.o moc_openglwindow.cpp

####### Install

install:  FORCE

uninstall:  FORCE

FORCE:

