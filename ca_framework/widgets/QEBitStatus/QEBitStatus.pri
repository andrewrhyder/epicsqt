# QEBitStatus.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#
OTHER_FILES += \
    ../widgets/QEBitStatus/icon.png

RESOURCES += \
    ../widgets/QEBitStatus/QEBitStatus.qrc

HEADERS += \
    ../widgets/QEBitStatus/QEBitStatus.h \
    ../widgets/QEBitStatus/QEBitStatusManager.h

SOURCES += \
    ../widgets/QEBitStatus/QEBitStatus.cpp \
    ../widgets/QEBitStatus/QEBitStatusManager.cpp

INCLUDEPATH += \
    ../widgets/QEBitStatus

# end
