# QEPlotter.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#
OTHER_FILES += \
    widgets/QEPlotter.png

RESOURCES += \
    widgets/QEPlotter/QEPlotter.qrc

HEADERS += \
    widgets/QEPlotter/QEExpressionEvaluation.h  \
    widgets/QEPlotter/QEPlotter.h \
    widgets/QEPlotter/QEPlotterManager.h

SOURCES += \
    widgets/QEPlotter/QEExpressionEvaluation.cpp  \
    widgets/QEPlotter/QEPlotter.cpp \
    widgets/QEPlotter/QEPlotterManager.cpp

INCLUDEPATH += \
    widgets/QEPlotter

# end
