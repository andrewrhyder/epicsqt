# adaptation_parameters.pri
#

AP=adaptation_parameters

INCLUDEPATH += \
   $$AP

HEADERS += \
   $$AP/QEEnvironmentVariables.h \
   $$AP/QEOptions.h  \
   $$AP/QESettings.h  \
   $$AP/QEAdaptationParameters.h

SOURCES += \
   $$AP/QEEnvironmentVariables.cpp \
   $$AP/QEOptions.cpp  \
   $$AP/QESettings.cpp  \
   $$AP/QEAdaptationParameters.cpp

# end
