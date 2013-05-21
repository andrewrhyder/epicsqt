#include <saveDialog.h>
#include <ui_saveDialog.h>
#include <QDebug>

saveDialog::saveDialog( QStringList names, QWidget *parent ) :
    QDialog(parent),
    ui(new Ui::saveDialog)
{
    ui->setupUi(this);

    savingStartup = true;
    enableNamedItems( false );
    ui->namesListWidget->addItems( names );
}

saveDialog::~saveDialog()
{
    delete ui;
}

void saveDialog::on_startupRadioButton_clicked( bool )
{
    enableNamedItems( false );
    savingStartup = true;
}

void saveDialog::on_namedRadioButton_clicked( bool )
{
    enableNamedItems( true );
    savingStartup = false;
}

void saveDialog::enableNamedItems( bool enable )
{
    ui->namesListWidget->setEnabled( enable );
    ui->nameLineEdit->setEnabled( enable );
}

void saveDialog::on_namesListWidget_clicked(QModelIndex)
{
    ui->nameLineEdit->setText( ui->namesListWidget->currentItem()->text() );
}

bool saveDialog::getUseDefault()
{
    return ui->startupRadioButton->isChecked();
}

QString saveDialog::getName()
{
    if( ui->namedRadioButton->isChecked() )
    {
        return ui->nameLineEdit->text();
    }
    return "";
}

void saveDialog::on_namesListWidget_doubleClicked( QModelIndex )
{
    accept();
}
