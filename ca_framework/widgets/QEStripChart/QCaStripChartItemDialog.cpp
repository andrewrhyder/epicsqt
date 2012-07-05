/*  QCaStripChartItemDialog.cpp
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
#include <QCaStripChartItemDialog.h>
#include <ui_QCaStripChartItemDialog.h>


//------------------------------------------------------------------------------
//
QCaStripChartItemDialog::QCaStripChartItemDialog (QWidget *parent) :
      QDialog (parent),
      ui (new Ui::QCaStripChartItemDialog)
{
   this->ui->setupUi (this);
   this->colourDialog = new QColorDialog (this);

   QObject::connect (this->ui->colourButton, SIGNAL (clicked             (bool)),
                     this,                   SLOT   (colourButtonClicked (bool)));

   QObject::connect (this->ui->clearButton,  SIGNAL (clicked            (bool)),
                     this,                   SLOT   (clearButtonClicked (bool)));
}

//------------------------------------------------------------------------------
//
QCaStripChartItemDialog::~QCaStripChartItemDialog ()
{
   delete ui;
}

//------------------------------------------------------------------------------
//
void QCaStripChartItemDialog::setPvName (QString pvNameIn)
{
   this->ui->pvNameEdit->setText (pvNameIn);
}


//------------------------------------------------------------------------------
//
QString QCaStripChartItemDialog::getPvName ()
{
   return this->ui->pvNameEdit->text ().trimmed ();
}

//------------------------------------------------------------------------------
//
void QCaStripChartItemDialog::setColour (QColor colourIn)
{
   this->colour = colourIn;
}

//------------------------------------------------------------------------------
//
QColor QCaStripChartItemDialog::getColour ()
{
   return this->colour;
}

//------------------------------------------------------------------------------
//
bool QCaStripChartItemDialog::isClear ()
{
   return (this->getPvName() == "");
}

//------------------------------------------------------------------------------
// User has pressed color
//
void QCaStripChartItemDialog::colourButtonClicked (bool)
{
   this->colourDialog->setCurrentColor (this->colour);
   this->colourDialog->open (this, SLOT (colourSelected (const QColor &)));
}

//------------------------------------------------------------------------------
//
void QCaStripChartItemDialog::colourSelected (const QColor & colourIn)
{
   this->colour = colourIn;
}

//------------------------------------------------------------------------------
// User has pressed clear
//
void QCaStripChartItemDialog::clearButtonClicked (bool)
{
   this->ui->pvNameEdit->clear ();
   this->accept ();
}

//------------------------------------------------------------------------------
// User has pressed OK
//
void QCaStripChartItemDialog::on_buttonBox_accepted ()
{
   this->accept ();
}

//------------------------------------------------------------------------------
// User has pressed cancel
//
void QCaStripChartItemDialog::on_buttonBox_rejected ()
{
   this->close ();
}

// end
