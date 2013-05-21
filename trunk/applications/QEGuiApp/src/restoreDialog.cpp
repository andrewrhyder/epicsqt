#include "restoreDialog.h"
#include "ui_restoreDialog.h"

restoreDialog::restoreDialog( QStringList names, QWidget *parent ) :
    QDialog(parent),
    ui(new Ui::restoreDialog)
{
    ui->setupUi(this);

    savingStartup = true;
    enableNamedItems( false );
    ui->namesListWidget->addItems( names );
}

restoreDialog::~restoreDialog()
{
    delete ui;
}

void restoreDialog::on_namesListWidget_clicked(QModelIndex)
{

}

void restoreDialog::on_startupRadioButton_clicked( bool )
{
    enableNamedItems( false );
    savingStartup = true;
}

void restoreDialog::on_namedRadioButton_clicked( bool )
{
    enableNamedItems( true );
    savingStartup = false;
}

void restoreDialog::enableNamedItems( bool enable )
{
    ui->namesListWidget->setEnabled( enable );
}

bool restoreDialog::getUseDefault()
{
    return ui->startupRadioButton->isChecked();
}

QString restoreDialog::getName()
{
    return ui->namesListWidget->currentItem()->text();
}

void restoreDialog::on_namesListWidget_doubleClicked( QModelIndex )
{
    accept();
}

void restoreDialog::on_buttonBox_accepted()
{

}
