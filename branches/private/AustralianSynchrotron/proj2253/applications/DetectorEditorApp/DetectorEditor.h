#ifndef DETECTOREDITOR_H
#define DETECTOREDITOR_H

#include <QDialog>
#include <QDomDocument>

namespace Ui {
class DetectorEditor;
}

class DetectorEditor : public QDialog
{
    Q_OBJECT
    
public:
    explicit DetectorEditor(QList<QStringList> detectorList, QString xmlFile, QWidget *parent = 0);
    ~DetectorEditor();

    bool isAddingDetector();
    bool isDeletingDetector();
    void enableSave();
    static bool loadDetectorData( QString xmlFile, QList<QStringList>& list );
    bool save();

private slots:
    void on_detectorName_textChanged(const QString & text  );
    void on_detectorPVPrefix_textChanged(const QString & text  );

    void on_detectorTypeComboBox_activated(int index);
    void on_detectorNameComboBox_activated(int index);

    void on_newRadioButton_clicked(bool checked);
    void on_deleteRadioButton_clicked(bool checked);
    void on_editRadioButton_clicked(bool checked);

private:
    Ui::DetectorEditor *ui;
    QList<QStringList> detectors;
    QStringList detectorNames;
    QString xmlFileName;
    QDomElement createDetectorElement( QDomDocument doc, QString name, QString type, QString pv,
                                       QString shutterPv, QString motorPv, QString rURL, QString pURL);
};

#endif // DETECTOREDITOR_H
