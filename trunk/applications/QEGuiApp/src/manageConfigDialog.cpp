#include "manageConfigDialog.h"
#include "ui_manageConfigDialog.h"
#include <QDebug>

manageConfigDialog::manageConfigDialog( QStringList names, QWidget *parent ) :
    QDialog(parent),
    ui(new Ui::manageConfigDialog)
{
    ui->setupUi(this);
    ui->namesListWidget->addItems( names );
    ui->deletePushButton->setEnabled( false );
}

manageConfigDialog::~manageConfigDialog()
{
    delete ui;
}

void manageConfigDialog::on_namesListWidget_itemSelectionChanged()
{
    ui->deletePushButton->setEnabled( ui->namesListWidget->selectedItems().count() );
}

void manageConfigDialog::on_deletePushButton_clicked()
{
    QStringList names;
    QList<QListWidgetItem*> list = ui->namesListWidget->selectedItems();
    for( int i = 0; i < list.count(); i++ )
    {
        names.append( list.at( i )->text() );
        qDebug() << names.last();
    }
}
