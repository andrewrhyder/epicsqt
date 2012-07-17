# QEStripChart.pri
#
# This file is part of the EPICS QT Framework, and included into
# and as part of the overall framework plugins.pro project file.
#
_QE_ARCHIVE_LIST = $$(QE_ARCHIVE_LIST)
isEmpty( _QE_ARCHIVE_LIST ) {
    warning( "QE_ARCHIVE_LIST is not defined. Thats OK, but if you want to be able to backfill QEStripChart widgets from an archiver you should define this environment variable to be a space delimited list of your archiver URLs" )
    warning( "For example, archiver.synchrotron.org.au:80/cgi-bin/ArchiveDataServer1.cgi archiver.synchrotron.org.au:80/cgi-bin/ArchiveDataServer2.cgi" )
}
OTHER_FILES += \
    ../widgets/QEStripChart/icon.png \
    ../widgets/QEStripChart/archive.png \
    ../widgets/QEStripChart/play.png \
    ../widgets/QEStripChart/pause.png \
    ../widgets/QEStripChart/page_forward.png \
    ../widgets/QEStripChart/page_backward.png

RESOURCES += \
    ../widgets/QEStripChart/QEStripChart.qrc

HEADERS += \
    ../widgets/QEStripChart/QEStripChart.h \
    ../widgets/QEStripChart/QEStripChartItem.h \
    ../widgets/QEStripChart/QEStripChartTimeDialog.h \
    ../widgets/QEStripChart/QEStripChartItemDialog.h \
    ../widgets/QEStripChart/QEStripChartManager.h

SOURCES += \
    ../widgets/QEStripChart/QEStripChart.cpp \
    ../widgets/QEStripChart/QEStripChartItem.cpp \
    ../widgets/QEStripChart/QEStripChartTimeDialog.cpp \
    ../widgets/QEStripChart/QEStripChartItemDialog.cpp \
    ../widgets/QEStripChart/QEStripChartManager.cpp

INCLUDEPATH += \
    ../widgets/QEStripChart

FORMS += \
    ../widgets/QEStripChart/QEStripChartTimeDialog.ui \
    ../widgets/QEStripChart/QEStripChartItemDialog.ui \

# end
