# common.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#

HEADERS += \
   common/QECommon.h \
   common/QEFrameworkVersion.h \
   common/QEGuiLaunchRequests.h

SOURCES += \
   common/QECommon.cpp \
   common/QEFrameworkVersion.cpp \
   common/QEGuiLaunchRequests.cpp

INCLUDEPATH += \
   common

# Explicity add common, and hence QEFrameworkVersion.h, to the dependacy path
# So that changes to the version/release numbers force relevent recompilations.
#
DEPENDPATH += common

# end
   
