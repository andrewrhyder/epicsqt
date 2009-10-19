# $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/applications/QCaMonitorApp/QCaMonitor.pro $
# $Revision: #4 $
# $DateTime: 2009/07/28 17:01:05 $
# Last checked in by: $Author: rhydera $
#
QT -= gui
TARGET = QCaMonitor
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += \
    ./src/main.cpp \
    ./src/monitor.cpp \
    $$(QTCREATOR)/gdbmacros/gdbmacros.cpp

HEADERS += \
    ./include/monitor.h

INCLUDEPATH += \
    ./include \
    $$(QCAFRAMEWORK)/api/include \
    $$(QCAFRAMEWORK)/data/include

win32{
    INCLUDEPATH += $$(CYGWIN)/usr/include
}

LIBS += -L$$(QTDIR)/plugins/designer -lQCaPluginDebug

