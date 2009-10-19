# $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/applications/ASguiApp/ASgui.pro $
# $Revision: #4 $
# $DateTime: 2009/10/12 13:21:28 $
# Last checked in by: $Author: rhydera $
TARGET = ASgui
TEMPLATE = app
CONFIG += uitools \
    debug_and_release
SOURCES += ./src/main.cpp \
    ./src/MainWindow.cpp \
    $$(QTCREATOR)/gdbmacros/gdbmacros.cpp
HEADERS += \
    include/MainWindow.h
INCLUDEPATH += . \
    ./include \
    $$(QCAFRAMEWORK)/data/include \
    $$(QCAFRAMEWORK)/widgets/include \
    $$(QCAFRAMEWORK)/plugins/include
win32:INCLUDEPATH += $$(CYGWIN)/usr/include
LIBS += -L$$(QTDIR)/plugins/designer \
    -lQCaPluginDebug
FORMS += ./src/MainWindow.ui
