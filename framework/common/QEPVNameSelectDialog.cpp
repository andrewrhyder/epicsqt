/*  QEPVNameSelectDialog.cpp
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 *
 */

#include <QDebug>
#include <QStringList>

#include <QEPVNameSelectDialog.h>
#include <ui_QEPVNameSelectDialog.h>

#include <QEArchiveManager.h>

//------------------------------------------------------------------------------
//
QEPVNameSelectDialog::QEPVNameSelectDialog (QWidget *parent) :
      QEDialog (parent),
      ui (new Ui::QEPVNameSelectDialog)
{
   this->ui->setupUi (this);

   QObject::connect (this->ui->filterEdit,  SIGNAL  (returnPressed ()),
                     this,                  SLOT    (filterEditReturnPressed ()));

   QObject::connect (this->ui->filterEdit,  SIGNAL (editingFinished       ()),
                     this,                  SLOT   (filterEditingFinished ()));

   QObject::connect (this->ui->pvNameEdit,  SIGNAL (editTextChanged (const QString&)),
                     this,                  SLOT   (editTextChanged (const QString&)));
}

//------------------------------------------------------------------------------
//
QEPVNameSelectDialog::~QEPVNameSelectDialog ()
{
   delete ui;
}

//------------------------------------------------------------------------------
//
void QEPVNameSelectDialog::setPvName (QString pvNameIn)
{
   this->originalPvName = pvNameIn.trimmed ();
   this->ui->pvNameEdit->clear ();
   this->ui->pvNameEdit->insertItem (0, this->originalPvName, QVariant ());
   this->ui->pvNameEdit->setCurrentIndex (0);

   // setPvName typically invoked just before exec () call.
   //
   this->ui->filterEdit->setFocus ();
}

//------------------------------------------------------------------------------
//
QString QEPVNameSelectDialog::getPvName ()
{
   return this->ui->pvNameEdit->currentText ().trimmed ();
}

//------------------------------------------------------------------------------
//
void QEPVNameSelectDialog::applyFilter ()
{
   QString filter = this->ui->filterEdit->text ().trimmed ();
   int n;

   this->ui->pvNameEdit->clear ();

   // QEArchiveAccess ensures the list is sorted.
   //
   this->ui->pvNameEdit->insertItems (0, QEArchiveAccess::getMatchingPVnames (filter));

   n = this->ui->pvNameEdit->count ();
   if ((n == 0) && (!this->originalPvName.isEmpty ())) {
      this->ui->pvNameEdit->insertItem (0, this->originalPvName, QVariant ());
      this->ui->pvNameEdit->setCurrentIndex (0);
   }

   this->ui->matchCountLabel->setText (QString ("%1").arg (n));
}

//------------------------------------------------------------------------------
//
void QEPVNameSelectDialog::filterEditReturnPressed ()
{
   // This will cause  filterEditingFinished to be invoked - no need
   // to apply filter here.
   //
   this->ui->pvNameEdit->setFocus ();
}

//------------------------------------------------------------------------------
//
void QEPVNameSelectDialog::filterEditingFinished ()
{
   this->applyFilter ();
}

//------------------------------------------------------------------------------
//
void QEPVNameSelectDialog::editTextChanged (const QString&)
{
   // NOTE: calling buttonBox->setStandardButtons causes a seg fault when cancel
   // eventually pressed and sometimes okay button as well, so do nothing for now.
   // Maybe do our own buttons instead of using a QDialogButtonBox.
}

//------------------------------------------------------------------------------
// User has pressed OK
//
void QEPVNameSelectDialog::on_buttonBox_accepted ()
{
   if (!this->getPvName().isEmpty ()) {
      this->accept ();
   }
}

//------------------------------------------------------------------------------
// User has pressed Cancel
//
void QEPVNameSelectDialog::on_buttonBox_rejected ()
{
   this->close ();
}

// end
