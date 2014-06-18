#
#

QT -= gui
QT += xml network

DESTDIR = ./
TARGET = qerad
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

# Place all intermediate generated files in architecture specific directories
#
MOC_DIR        = O.Build/moc/$$(EPICS_HOST_ARCH)
OBJECTS_DIR    = O.Build/obj/$$(EPICS_HOST_ARCH)
UI_HEADERS_DIR = O.Build/ui_headers/$$(EPICS_HOST_ARCH)


#===========================================================
# Project files
#
HEADERS += \
   ./rad_control.h

SOURCES += \
   ./rad.cpp \
   ./rad_control.cpp


INCLUDEPATH += . \
   ../../framework/adaptation_parameters \
   ../../framework/archive \
   ../../framework/common \
   ../../framework/data/include \
   ../../framework/widgets/include

OTHER_FILES += \
   ./help_usage.txt \
   ./help_general.txt

RESOURCES +=  \
   ./QEReadArchive.qrc


# Set up EPICS
#
#unix:INCLUDEPATH += $$(EPICS_BASE)/include/os/Linux
#win32:INCLUDEPATH += $$(EPICS_BASE)/include/os/WIN32
#INCLUDEPATH += $$(EPICS_BASE)/include


LIBS += -L../../framework/designer -lQEPlugin
LIBS += -L$$(EPICS_BASE)/lib/$$(EPICS_HOST_ARCH) -lca -lCom


#===========================================================
# Check dependancies
#===========================================================
# EPICS
#
# Check EPICS appears to be present
#
_EPICS_BASE = $$(EPICS_BASE)

isEmpty( _EPICS_BASE ) {
    error( "EPICS_BASE must be defined. Ensure EPICS is installed and EPICS_BASE is set up." )
}

_EPICS_HOST_ARCH = $$(EPICS_HOST_ARCH)

isEmpty( _EPICS_HOST_ARCH ) {
    error( "EPICS_HOST_ARCH must be defined. Ensure EPICS is installed and EPICS_HOST_ARCH is set up." )
}

# end
