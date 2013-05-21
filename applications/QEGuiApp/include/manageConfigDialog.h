#ifndef MANAGECONFIGDIALOG_H
#define MANAGECONFIGDIALOG_H

#include <QDialog>

namespace Ui {
    class manageConfigDialog;
}

class manageConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit manageConfigDialog( QStringList names, QWidget *parent = 0 );
    ~manageConfigDialog();

private:
    Ui::manageConfigDialog *ui;

signals:
    void deleteConfigs( const QStringList names );

private slots:
    void on_deletePushButton_clicked();
    void on_namesListWidget_itemSelectionChanged();
};

#endif // MANAGECONFIGDIALOG_H