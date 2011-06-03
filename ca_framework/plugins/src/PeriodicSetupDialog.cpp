/*
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2011
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/*!
  This class manages a dialog for setting up a QCaPeriodicPlugin in Qt designer.
 */

#include "PeriodicSetupDialog.h"
#include "ui_PeriodicSetupDialog.h"
#include "PeriodicElementSetupForm.h"
#include <QGridLayout>
#include <QCaPeriodic.h>
#include <QLabel>
#include <QFrame>
#include <QCheckBox>
#include <QLineEdit>
#include "QCaPeriodicPlugin.h"
#include <QtDesigner>


// Create the dialog
PeriodicSetupDialog::PeriodicSetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PeriodicSetupDialog)
{
    ui->setupUi(this);

    // Populate the table
    QGridLayout* periodicGrid = this->findChild<QGridLayout *>("periodicGridLayout");
    if( periodicGrid )
    {
        // Populate the table elements
        for( int i = 0; i < NUM_ELEMENTS; i++ )
        {
            elements[i] = new PeriodicElementSetupForm( this );

            QLabel* label = elements[i]->findChild<QLabel *>("label");
            if( label )
            {
                label->setText( QCaPeriodic::elementInfo[i].symbol );
            }

            QFrame* frame = elements[i]->findChild<QFrame *>("frame");
            if( frame )
            {
                frame->setToolTip( QCaPeriodic::elementInfo[i].name );
            }

            QCaPeriodicPlugin *plugin = qobject_cast<QCaPeriodicPlugin *>(parent);
            if( plugin )
            {
                QCheckBox* enableButton = elements[i]->findChild<QCheckBox *>("checkBoxEnable");
                if( enableButton )
                {
                    enableButton->setChecked( plugin->userInfo[i].enable );
                }

                QLineEdit* value1 = elements[i]->findChild<QLineEdit *>("lineEditValue1");
                if( value1 )
                {
                    value1->setText( QString::number( plugin->userInfo[i].value1 ) );
                }

                QLineEdit* value2 = elements[i]->findChild<QLineEdit *>("lineEditValue2");
                if( value2 )
                {
                    value2->setText( QString::number( plugin->userInfo[i].value2 ) );
                }

                QLineEdit* elementText = elements[i]->findChild<QLineEdit *>("lineEditString");
                if( elementText )
                {
                    elementText->setText(  plugin->userInfo[i].elementText );
                }
            }

            periodicGrid->addWidget( elements[i], QCaPeriodic::elementInfo[i].tableRow, QCaPeriodic::elementInfo[i].tableCol );

        }

        // Populate unused rows and columns
        QLabel* label;

        label = new QLabel( this );
        label->setText( "*" );
        label->setAlignment( Qt::AlignHCenter );
        label->setAlignment( Qt::AlignVCenter );
        periodicGrid->addWidget( label, 5, 2 );

        label = new QLabel( this );
        label->setText( "**" );
        label->setAlignment( Qt::AlignHCenter );
        label->setAlignment( Qt::AlignVCenter );
        periodicGrid->addWidget( label, 6, 2 );

        label = new QLabel( this );
        label->setText( "*" );
        label->setAlignment( Qt::AlignRight );
        label->setAlignment( Qt::AlignVCenter );
        periodicGrid->addWidget( label, 8, 1 );

        label = new QLabel( this );
        label->setText( "**" );
        label->setAlignment( Qt::AlignRight );
        label->setAlignment( Qt::AlignVCenter );
        periodicGrid->addWidget( label, 9, 1 );

        // Force empty row 7 to remain
        label = new QLabel( this );
        label->setText( "" );
        periodicGrid->addWidget( label, 7, 10 );

        periodicGrid->setRowStretch( 0, 10);
        periodicGrid->setRowStretch( 1, 10);
        periodicGrid->setRowStretch( 2, 10);
        periodicGrid->setRowStretch( 3, 10);
        periodicGrid->setRowStretch( 4, 10);
        periodicGrid->setRowStretch( 5, 10);
        periodicGrid->setRowStretch( 6, 10);
        periodicGrid->setRowStretch( 7, 2);
        periodicGrid->setRowStretch( 8, 10);
        periodicGrid->setRowStretch( 9, 10);

    }
}

// Destroy the dialog
PeriodicSetupDialog::~PeriodicSetupDialog()
{
    delete ui;
}

// User has pressed OK
void PeriodicSetupDialog::on_buttonBox_accepted()
{
    QCaPeriodicPlugin *plugin = qobject_cast<QCaPeriodicPlugin *>(this->parent());
    if( plugin )
    {
        for( int i = 0; i < NUM_ELEMENTS; i++ )
        {
            QCheckBox* enableButton = elements[i]->findChild<QCheckBox *>("checkBoxEnable");
            if( enableButton )
            {
                plugin->userInfo[i].enable = enableButton->isChecked();
            }

            bool ok;
            QLineEdit* value1 = elements[i]->findChild<QLineEdit *>("lineEditValue1");
            if( value1 )
            {
                plugin->userInfo[i].value1 = value1->text().toDouble( &ok );
            }

            QLineEdit* value2 = elements[i]->findChild<QLineEdit *>("lineEditValue2");
            if( value2 )
            {
                plugin->userInfo[i].value2 = value2->text().toDouble( &ok );
            }

            QLineEdit* elementText = elements[i]->findChild<QLineEdit *>("lineEditString");
            if( elementText )
            {
                plugin->userInfo[i].elementText = elementText->text();
            }
        }

        if (QDesignerFormWindowInterface *formWindow = QDesignerFormWindowInterface::findFormWindow(plugin))
        {
            formWindow->cursor()->setProperty("userInfo", plugin->getUserInfo() );
        }

        accept();

    }
}

// User has pressed cancel
void PeriodicSetupDialog::on_buttonBox_rejected()
{
    this->close();
}
