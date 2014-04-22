#-------------------------------------------------
#
# Project created by QtCreator 2014-03-25T01:08:59
#
#-------------------------------------------------

QT       += core gui

TARGET = DetectorEditorApp
TEMPLATE = app
CONFIG += uitools \
    debug_and_release


SOURCES += main.cpp\
        DetectorEditor.cpp

HEADERS  += DetectorEditor.h

FORMS    += DetectorEditor.ui

INCLUDEPATH += . \
    ./include \
    ../../framework/widgets/include \
    ../../framework/widgets/QEForm \
    ../../framework/widgets/QELogin \
    ../../framework/data/include \
    ../../framework/api/include \
    ../../framework/common \
    ../../framework/include

# Explicity add ../../framework/common, and hence QEFrameworkVersion.h, to the dependacy path
# So that changes to the version/release numbers forece revent recompilations.
#
DEPENDPATH += ../../framework/common

LIBS += -L../../framework/designer -lQEPlugin
