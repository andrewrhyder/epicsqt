# widgets.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#

INCLUDEPATH += \
    widgets/include

HEADERS += \
    widgets/include/VariableNameManager.h \
    widgets/include/UserMessage.h \
    widgets/include/contextMenu.h \
    widgets/include/QEWidget.h \
    widgets/include/managePixmaps.h \
    widgets/include/QEDragDrop.h \
    widgets/include/styleManager.h \
    widgets/include/standardProperties.h \
    widgets/include/QEStringFormattingMethods.h \
    widgets/include/QEToolTip.h \
    widgets/include/QEDesignerPlugin.h \
    widgets/include/ContainerProfile.h \
    widgets/include/QEPluginLibrary_global.h \
    widgets/include/persistanceManager.h

SOURCES += \
    widgets/src/VariableNameManager.cpp \
    widgets/src/UserMessage.cpp \
    widgets/src/contextMenu.cpp \
    widgets/src/QEWidget.cpp \
    widgets/src/managePixmaps.cpp \
    widgets/src/QEDragDrop.cpp \
    widgets/src/styleManager.cpp \
    widgets/src/standardProperties.cpp \
    widgets/src/QEStringFormattingMethods.cpp \
    widgets/src/QEToolTip.cpp \
    widgets/src/QEDesignerPlugin.cpp \
    widgets/src/ContainerProfile.cpp \
    widgets/src/persistanceManager.cpp

# end
