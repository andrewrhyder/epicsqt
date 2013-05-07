#include <saveDialog.h>
#include <ui_saveDialog.h>
#include <QDebug>

saveDialog::saveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::saveDialog)
{
    savingStartup = true;
    enableNamedItems( false );

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
    enableNamedItems( false );
    savingStartup = true;
}

void saveDialog::on_namedRadioButton_clicked(bool checked)
{
    qDebug() << "saveDialog::on_namedRadioButton_clicked()" << checked;
    enableNamedItems( true );
    savingStartup = false;
}

void saveDialog::enableNamedItems( bool enable )
{
    ui->namesListWidget->setEnabled( enable );
    ui->nameLineEdit->setEnabled( enable );
}
