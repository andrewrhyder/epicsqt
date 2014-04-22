# deprecated QCa widgets (All repalced with QE widgets)
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#
INCLUDEPATH += \
    widgets/project_based\
    widgets/QEScratchPad

HEADERS += \
    widgets/project_based/QEDevicePad.h \
    widgets/project_based/QEDevicePadManager.h

SOURCES += \
    widgets/project_based/QEDevicePad.cpp \
    widgets/project_based/QEDevicePadManager.cpp

OTHER_FILES += \
    widgets/project_based/QEDevicePad.png

RESOURCES += \
    widgets/project_based/project_based.qrc

# end
