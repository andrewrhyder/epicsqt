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

#include "PeriodicDialog.h"
#include "ui_PeriodicDialog.h"
#include <QtDebug>


PeriodicDialog::PeriodicDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::PeriodicDialog)
{
    m_ui->setupUi(this);
}

PeriodicDialog::~PeriodicDialog()
{
    delete m_ui;
}

void PeriodicDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QString PeriodicDialog::getElement()
{
    return elementSelected;
}

void PeriodicDialog::setElement( QString elementIn, QList<bool>& enabledList, QList<QString>& elementList )
{
    QList<QPushButton *> allPButtons = this->findChildren<QPushButton *>();
    for( int i = 0; i < allPButtons.size(); i++ )
    {
        // Enable or disable the button as directed by the 'enabled' list
        QString element = allPButtons[i]->text();
        for( int j = 0; j < elementList.size(); j++ )
        {
            if( element.compare( elementList[j]) == 0 )
            {
                allPButtons[i]->setEnabled( enabledList[j] );
                break;
            }
        }

        // Set the button focus if it is enabled and matching the provided symbol
        if( allPButtons[i]->isEnabled() && allPButtons[i]->text() == elementIn )
            allPButtons[i]->setFocus();
    }
}

void PeriodicDialog::noteElementSelected()
{
    if( this->focusWidget()->inherits("QPushButton") )
    {
        QPushButton* btn;
        btn = (QPushButton*)(this->focusWidget());
        elementSelected = btn->text();
    }
    this->close();
}
