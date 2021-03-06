# QEPeriodic.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#
OTHER_FILES += \
    widgets/QEPeriodic.png

RESOURCES += \
    widgets/QEPeriodic/QEPeriodic.qrc

HEADERS += \
    widgets/QEPeriodic/QEPeriodic.h \
    widgets/QEPeriodic/QEPeriodicManager.h \
    widgets/QEPeriodic/QEPeriodicTaskMenu.h \
    widgets/QEPeriodic/PeriodicSetupDialog.h \
    widgets/QEPeriodic/PeriodicElementSetupForm.h \
    widgets/QEPeriodic/PeriodicDialog.h

SOURCES += \
    widgets/QEPeriodic/QEPeriodic.cpp \
    widgets/QEPeriodic/QEPeriodicManager.cpp \
    widgets/QEPeriodic/QEPeriodicTaskMenuExtension.cpp \
    widgets/QEPeriodic/PeriodicSetupDialog.cpp \
    widgets/QEPeriodic/PeriodicElementSetupForm.cpp \
    widgets/QEPeriodic/PeriodicDialog.cpp

INCLUDEPATH += \
    widgets/QEPeriodic

FORMS += \
    widgets/QEPeriodic/PeriodicDialog.ui \
    widgets/QEPeriodic/PeriodicSetupDialog.ui \
    widgets/QEPeriodic/PeriodicElementSetupForm.ui

# end
