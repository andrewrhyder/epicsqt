/*  QEScratchPadItemDialog.cpp
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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 *
 */

#include <QDebug>

#include <QEScratchPadItemDialog.h>
#include <ui_QEScratchPadItemDialog.h>


//------------------------------------------------------------------------------
//
QEScratchPadItemDialog::QEScratchPadItemDialog (QWidget *parent) :
      QEDialog (parent),
      ui (new Ui::QEScratchPadItemDialog)
{
   this->ui->setupUi (this);

   this->returnIsMasked = false;

   QObject::connect (this->ui->clearButton, SIGNAL (clicked            (bool)),
                     this,                  SLOT   (clearButtonClicked (bool)));

   QObject::connect (this->ui->dataEdit,  SIGNAL (returnPressed ()),
                     this,                SLOT   (dataEditReturnPressed ()));
}

//------------------------------------------------------------------------------
//
QEScratchPadItemDialog::~QEScratchPadItemDialog ()
{
   delete ui;
}


//------------------------------------------------------------------------------
//
void QEScratchPadItemDialog::setFieldInformation (const QString dataIn)
{
   this->ui->dataEdit->setText (dataIn);
   this->ui->dataEdit->setFocus ();
}

//------------------------------------------------------------------------------
//
void QEScratchPadItemDialog::getFieldInformation (QString& dataOut)
{
   dataOut =  this->ui->dataEdit->text ().trimmed();
}

//------------------------------------------------------------------------------
//
void QEScratchPadItemDialog::dataEditReturnPressed ()
{
}


//------------------------------------------------------------------------------
// User has pressed Clear
//
void QEScratchPadItemDialog::clearButtonClicked (bool)
{
   this->ui->dataEdit->clear ();
   this->accept ();
}

//------------------------------------------------------------------------------
// User has pressed OK
//
void QEScratchPadItemDialog::on_buttonBox_accepted ()
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
void QEScratchPadItemDialog::on_buttonBox_rejected ()
{
   this->close ();
}

// end
