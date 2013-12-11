# QERadioGroup.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#
OTHER_FILES += \
    widgets/QERadioGroup/QERadioGroup.png

RESOURCES += \
    widgets/QERadioGroup/QERadioGroup.qrc

HEADERS += \
    widgets/QERadioGroup/QERadioGroup.h \
    widgets/QERadioGroup/QERadioGroupManager.h

SOURCES += \
    widgets/QERadioGroup/QERadioGroup.cpp \
    widgets/QERadioGroup/QERadioGroupManager.cpp

INCLUDEPATH += \
    widgets/QERadioGroup

# end
