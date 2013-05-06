#include <saveDialog.h>
#include <ui_saveDialog.h>
#include <QDebug>

saveDialog::saveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::saveDialog)
{
    savingStartup = true;

    ui->setupUi(this);
}

saveDialog::~saveDialog()
{
    delete ui;
}

void saveDialog::on_startupRadioButton_clicked()
{
}

void saveDialog::on_startupRadioButton_clicked(bool checked)
{
    qDebug() << "saveDialog::on_startupRadioButton_clicked()" << checked;
    ui->namesListWidget->setEnabled( false );
    ui->nameLineEdit->setEnabled( false );
    savingStartup = checked;
}

void saveDialog::on_namedRadioButton_clicked(bool checked)
{
    qDebug() << "saveDialog::on_namedRadioButton_clicked()" << checked;
    ui->namesListWidget->setEnabled( true );
    ui->nameLineEdit->setEnabled( true );
    savingStartup = !checked;
}
