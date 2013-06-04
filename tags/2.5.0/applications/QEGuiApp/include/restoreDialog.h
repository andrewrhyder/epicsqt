#ifndef RESTOREDIALOG_H
#define RESTOREDIALOG_H

#include <QDialog>
#include <QListWidget>

namespace Ui {
    class restoreDialog;
}

class restoreDialog : public QDialog
{
    Q_OBJECT

public:
    explicit restoreDialog( QStringList names, QWidget *parent = 0 );
    ~restoreDialog();

    bool getUseDefault();
    QString getName();

private:
    Ui::restoreDialog *ui;

    void enableNamedItems( bool enable );

    bool savingStartup;

private slots:
    void on_namesListWidget_doubleClicked(QModelIndex index);
    void on_buttonBox_accepted();
    void on_namedRadioButton_clicked(bool checked);
    void on_startupRadioButton_clicked(bool checked);
    void on_namesListWidget_clicked(QModelIndex index);
};

#endif // RESTOREDIALOG_H
