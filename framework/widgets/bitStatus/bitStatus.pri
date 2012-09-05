# bitStatus.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#
OTHER_FILES += \
    ../widgets/bitStatus/icon.png

RESOURCES += \
    ../widgets/bitStatus/bitStatus.qrc

HEADERS += \
    ../widgets/bitStatus/bitStatus.h \
    ../widgets/bitStatus/bitStatusManager.h

SOURCES += \
    ../widgets/bitStatus/bitStatus.cpp \
    ../widgets/bitStatus/bitStatusManager.cpp

INCLUDEPATH += \
    ../widgets/bitStatus

# end
