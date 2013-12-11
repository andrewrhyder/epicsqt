# api.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#

INCLUDEPATH += \
    api/include

HEADERS += \
    api/include/Generic.h \
    api/include/CaRecord.h \
    api/include/CaRef.h \
    api/include/CaObject.h \
    api/include/CaConnection.h \
    api/include/CaObjectPrivate.h

SOURCES += \
    api/src/Generic.cpp \
    api/src/CaRecord.cpp \
    api/src/CaObject.cpp \
    api/src/CaConnection.cpp \
    api/src/CaRef.cpp

# end
