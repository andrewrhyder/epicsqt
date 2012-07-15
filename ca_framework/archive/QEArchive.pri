# QEArchive.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#
QT += xml network

HEADERS += \
    ../archive/QEArchiveInterface.h \
    ../archive/QEArchiveManager.h \
    ../archive/maiaFault.h \
    ../archive/maiaObject.h \
    ../archive/maiaXmlRpcClient.h

SOURCES += \
    ../archive/QEArchiveInterface.cpp \
    ../archive/QEArchiveManager.cpp \
    ../archive/maiaFault.cpp \
    ../archive/maiaObject.cpp \
    ../archive/maiaXmlRpcClient.cpp

INCLUDEPATH += \
    ../archive

# end
