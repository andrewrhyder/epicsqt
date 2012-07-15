# QEImage.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#
OTHER_FILES += \
    ../widgets/QEImage/QEImage.png  \
    ../widgets/QEImage/cameraROI.png \
    ../widgets/QEImage/cameraROIreset.png \
    ../widgets/QEImage/flipRotate.png \
    ../widgets/QEImage/pause.png \
    ../widgets/QEImage/play.png \
    ../widgets/QEImage/save.png \
    ../widgets/QEImage/select.png \
    ../widgets/QEImage/zoom.png

RESOURCES += \
    ../widgets/QEImage/QEImage.qrc

HEADERS += \
    ../widgets/QEImage/imageContextMenu.h \
    ../widgets/QEImage/flipRotateMenu.h \
    ../widgets/QEImage/imageMarkup.h \
    ../widgets/QEImage/profilePlot.h \
    ../widgets/QEImage/QEImage.h \
    ../widgets/QEImage/QEImageManager.h \
    ../widgets/QEImage/selectMenu.h \
    ../widgets/QEImage/videowidget.h \
    ../widgets/QEImage/zoomMenu.h

SOURCES += \
    ../widgets/QEImage/QEImage.cpp \
    ../widgets/QEImage/QEImageManager.cpp \
    ../widgets/QEImage/imageContextMenu.cpp \
    ../widgets/QEImage/imageMarkup.cpp \
    ../widgets/QEImage/videowidget.cpp \
    ../widgets/QEImage/flipRotateMenu.cpp \
    ../widgets/QEImage/profilePlot.cpp \
    ../widgets/QEImage/selectMenu.cpp \
    ../widgets/QEImage/zoomMenu.cpp

INCLUDEPATH += \
    ../widgets/QEImage

# end
