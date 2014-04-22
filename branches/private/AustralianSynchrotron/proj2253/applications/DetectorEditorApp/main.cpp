#include <QtGui/QApplication>
#include "DetectorEditor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QList<QStringList> detectorList;
    // read customisation file and get detector list
    DetectorEditor::loadDetectorData("DetectorMainCustomisations.xml", detectorList);
    DetectorEditor dEditor(detectorList, "DetectorMainCustomisations.xml");

    while (true){
        if ( dEditor.exec() == QDialog::Rejected )
        {
            break;
        }
        // update the system customisation: reload
        if (dEditor.save()) break;
    }

    return a.exec();
}
