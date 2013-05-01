# data.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#

INCLUDEPATH += \
   data/include

HEADERS += \
   data/include/QCaAlarmInfo.h \
   data/include/QCaConnectionInfo.h \
   data/include/QCaDataPoint.h \
   data/include/QCaDateTime.h \
   data/include/QCaEventFilter.h \
   data/include/QCaEventUpdate.h \
   data/include/QCaObject.h \
   data/include/QCaStateMachine.h \
   data/include/QCaVariableNamePropertyManager.h \
   data/include/QEByteArray.h \
   data/include/QEFloating.h \
   data/include/QEFloatingFormatting.h \
   data/include/QEInteger.h \
   data/include/QEIntegerFormatting.h \
   data/include/QELocalEnumeration.h \
   data/include/QEString.h \
   data/include/QEStringFormatting.h \
   data/include/qepicspv.h


SOURCES += \
   data/src/QCaAlarmInfo.cpp \
   data/src/QCaConnectionInfo.cpp \
   data/src/QCaDateTime.cpp \
   data/src/QCaEventFilter.cpp \
   data/src/QCaEventUpdate.cpp \
   data/src/QCaObject.cpp \
   data/src/QCaStateMachine.cpp \
   data/src/QCaVariableNamePropertyManager.cpp \
   data/src/QEByteArray.cpp \
   data/src/QEFloating.cpp \
   data/src/QEFloatingFormatting.cpp \
   data/src/QEInteger.cpp \
   data/src/QEIntegerFormatting.cpp \
   data/src/QELocalEnumeration.cpp \
   data/src/QEString.cpp \
   data/src/QEStringFormatting.cpp \
   data/src/qepicspv.cpp

# end
