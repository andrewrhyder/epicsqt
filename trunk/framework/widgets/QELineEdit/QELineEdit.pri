# QELineEdit.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#
OTHER_FILES += \
    widgets/QELineEdit/QELineEdit.png \
    widgets/QELineEdit/QENumericEdit.png \

RESOURCES += \
    widgets/QELineEdit/QELineEdit.qrc

HEADERS += \
    widgets/QELineEdit/QEGenericEdit.h \
    widgets/QELineEdit/QELineEdit.h \
    widgets/QELineEdit/QELineEditManager.h \
    widgets/QELineEdit/QENumericEdit.h \
    widgets/QELineEdit/QENumericEditManager.h

SOURCES += \
    widgets/QELineEdit/QEGenericEdit.cpp \
    widgets/QELineEdit/QELineEdit.cpp \
    widgets/QELineEdit/QELineEditManager.cpp \
    widgets/QELineEdit/QENumericEdit.cpp \
    widgets/QELineEdit/QENumericEditManager.cpp

INCLUDEPATH += \
    widgets/QELineEdit

# end
