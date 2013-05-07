#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <QDialog>

namespace Ui {
class saveDialog;
}

class saveDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit saveDialog(QWidget *parent = 0);
    ~saveDialog();
    
private slots:
    void on_startupRadioButton_clicked();

    void on_startupRadioButton_clicked(bool checked);

    void on_namedRadioButton_clicked(bool checked);

private:
    Ui::saveDialog *ui;

    void enableNamedItems( bool enable );

    bool savingStartup;
};

#endif // SAVEDIALOG_H
