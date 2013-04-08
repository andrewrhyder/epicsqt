# QESimpleShape.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#
OTHER_FILES += \
    widgets/QESimpleShape/QESimpleShape.png

RESOURCES += \
    widgets/QESimpleShape/QESimpleShape.qrc

HEADERS += \
    widgets/QESimpleShape/QESimpleShape.h \
    widgets/QESimpleShape/QESimpleShapeManager.h

SOURCES += \
    widgets/QESimpleShape/QESimpleShape.cpp \
    widgets/QESimpleShape/QESimpleShapeManager.cpp

INCLUDEPATH += \
    widgets/QESimpleShape

# end
