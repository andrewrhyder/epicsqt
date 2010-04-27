# $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/applications/ASguiApp/ASgui.pro $
# $Revision: #9 $
# $DateTime: 2010/03/23 11:07:38 $
# Last checked in by: $Author: rhydera $
TARGET = ASgui
TEMPLATE = app
CONFIG += uitools \
    debug_and_release
SOURCES += ./src/main.cpp \
    ./src/MainWindow.cpp \
    ./src/MainContext.cpp \
    ./src/StartupParams.cpp \
    $$(QTCREATOR)/gdbmacros/gdbmacros.cpp
include($$(QCAFRAMEWORK)/qtsingleapplication/src/qtsingleapplication.pri)
HEADERS += include/MainWindow.h \
    include/MainContext.h \
    include/StartupParams.h
INCLUDEPATH += . \
    ./include \
    $$(QCAFRAMEWORK)/data/include \
    $$(QCAFRAMEWORK)/widgets/include \
    $$(QCAFRAMEWORK)/plugins/include
win32:INCLUDEPATH += $$(CYGWIN)/usr/include
LIBS += -L$$(QTDIR)/plugins/designer -lQCaPluginDebug
LIBS += -L/opt/fontconfig/lib
FORMS += ./src/MainWindow.ui
