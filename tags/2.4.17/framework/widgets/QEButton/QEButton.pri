# QEButton.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#
OTHER_FILES += \
    widgets/QEPushButton.png \
    widgets/QERadioButton.png

RESOURCES += \
    widgets/QEButton/QEButton.qrc

HEADERS += \
    widgets/QEButton/QEGenericButton.h \
    widgets/QEButton/QEPushButton.h \
    widgets/QEButton/QERadioButton.h \
    widgets/QEButton/QEPushButtonManager.h \
    widgets/QEButton/QERadioButtonManager.h

SOURCES += \
    widgets/QEButton/QEGenericButton.cpp \
    widgets/QEButton/QEPushButton.cpp \
    widgets/QEButton/QERadioButton.cpp \
    widgets/QEButton/QEPushButtonManager.cpp \
    widgets/QEButton/QERadioButtonManager.cpp

INCLUDEPATH += \
    widgets/QEButton

# end