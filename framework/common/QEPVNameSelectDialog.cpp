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

   this->returnIsMasked = false;

   QObject::connect (this->ui->clearButton, SIGNAL (clicked            (bool)),
                     this,                  SLOT   (clearButtonClicked (bool)));

   QObject::connect (this->ui->filterEdit,  SIGNAL  (returnPressed ()),
                     this,                  SLOT (filterEditReturnPressed ()));


   QObject::connect (this->ui->filterEdit,  SIGNAL (editingFinished       ()),
                     this,                  SLOT   (filterEditingFinished ()));

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
   this->applyFilter ();

   if (!pvNameIn.isEmpty()) {
      this->ui->pvNameEdit->insertItem (0, pvNameIn, QVariant ());
      this->ui->pvNameEdit->setCurrentIndex (0);
   }
}


//------------------------------------------------------------------------------
//
QString QEPVNameSelectDialog::getPvName ()
{
   return this->ui->pvNameEdit->currentText().trimmed ();
}

//------------------------------------------------------------------------------
//
bool QEPVNameSelectDialog::isClear ()
{
   return (this->getPvName() == "");
}

//------------------------------------------------------------------------------
//
void QEPVNameSelectDialog::applyFilter ()
{
   QString filter = this->ui->filterEdit->text().trimmed ();
   int n;

   this->ui->pvNameEdit->clear ();

   // QEArchiveAccess ensures thelist is sorted.
   //
   this->ui->pvNameEdit->insertItems (0, QEArchiveAccess::getMatchingPVnames (filter));

   n =  this->ui->pvNameEdit->count ();

   this->ui->matchCountLabel->setText (QString ("%1").arg (n));
}

//------------------------------------------------------------------------------
//
void QEPVNameSelectDialog::filterEditReturnPressed ()
{
   this->returnIsMasked = true;

   // this will cause  filterEditingFinished to be invoked - no need
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
// User has pressed Clear
//
void QEPVNameSelectDialog::clearButtonClicked (bool)
{
   this->ui->pvNameEdit->clear ();
   this->accept ();
}

//------------------------------------------------------------------------------
// User has pressed OK
//
void QEPVNameSelectDialog::on_buttonBox_accepted ()
{
   if (this->returnIsMasked) {
      this->returnIsMasked = false;
      return;
   }

   this->accept ();
}

//------------------------------------------------------------------------------
// User has pressed Cancel
//
void QEPVNameSelectDialog::on_buttonBox_rejected ()
{
   this->close ();
}

// end
