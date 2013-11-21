# common.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#

HEADERS += \
   common/QEActionRequests.h \
   common/QECommon.h \
   common/QEDialog.h \
   common/QEExpressionEvaluation.h  \
   common/QEFrameworkVersion.h \
   common/QEPVNameSelectDialog.h \
   common/QEScaling.h \
   common/PasswordDialog.h \
   common/windowCustomisation.h

SOURCES += \
   common/QEActionRequests.cpp \
   common/QECommon.cpp \
   common/QEDialog.cpp \
   common/QEExpressionEvaluation.cpp  \
   common/QEFrameworkVersion.cpp \
   common/QEPVNameSelectDialog.cpp \
   common/QEScaling.cpp \
   common/PasswordDialog.cpp \
   common/windowCustomisation.cpp

INCLUDEPATH += \
   common

# Explicity add common, and hence QEFrameworkVersion.h, to the dependacy path
# So that changes to the version/release numbers force relevent recompilations.
#
DEPENDPATH += common

FORMS += \
   common/QEPVNameSelectDialog.ui \
   common/PasswordDialog.ui

OTHER_FILES += \
   common/QEPVNameSelectFilterHelp.ui

RESOURCES += \
   common/common.qrc

# end
