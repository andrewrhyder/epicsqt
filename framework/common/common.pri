# common.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#

HEADERS += \
   common/QECommon.h \
   common/QEScaling.h \
   common/QEFrameworkVersion.h \
   common/QEGuiLaunchRequests.h \
   common/QEDialog.h \
   common/QEPVNameSelectDialog.h \
   common/PasswordDialog.h

SOURCES += \
   common/QECommon.cpp \
   common/QEScaling.cpp \
   common/QEFrameworkVersion.cpp \
   common/QEGuiLaunchRequests.cpp \
   common/QEDialog.cpp \
   common/QEPVNameSelectDialog.cpp \
   common/PasswordDialog.cpp

INCLUDEPATH += \
   common

# Explicity add common, and hence QEFrameworkVersion.h, to the dependacy path
# So that changes to the version/release numbers force relevent recompilations.
#
DEPENDPATH += common

FORMS += \
   common/QEPVNameSelectDialog.ui \
   common/PasswordDialog.ui

# end
   