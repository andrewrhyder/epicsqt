/*  QEStripChartItemDialog.cpp
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
#include <QEStripChartItemDialog.h>
#include <ui_QEStripChartItemDialog.h>


//------------------------------------------------------------------------------
//
QEStripChartItemDialog::QEStripChartItemDialog (QWidget *parent) :
      QDialog (parent),
      ui (new Ui::QEStripChartItemDialog)
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
QEStripChartItemDialog::~QEStripChartItemDialog ()
{
   delete ui;
}

//------------------------------------------------------------------------------
//
void QEStripChartItemDialog::setPvName (QString pvNameIn)
{
   this->ui->pvNameEdit->setText (pvNameIn);
}


//------------------------------------------------------------------------------
//
QString QEStripChartItemDialog::getPvName ()
{
   return this->ui->pvNameEdit->text ().trimmed ();
}

//------------------------------------------------------------------------------
//
void QEStripChartItemDialog::setColour (QColor colourIn)
{
   int r, g, b;
   QString style;

   this->colour = colourIn;
   r = this->colour.red ();
   g = this->colour.green ();
   b = this->colour.blue ();

   style.sprintf ("QWidget { background-color: #%02x%02x%02x; }", r, g, b);

   this->ui->colourIndicator->setStyleSheet (style);

}

//------------------------------------------------------------------------------
//
QColor QEStripChartItemDialog::getColour ()
{
   return this->colour;
}

//------------------------------------------------------------------------------
//
bool QEStripChartItemDialog::isClear ()
{
   return (this->getPvName() == "");
}

//------------------------------------------------------------------------------
// User has pressed color
//
void QEStripChartItemDialog::colourButtonClicked (bool)
{
   this->colourDialog->setCurrentColor (this->colour);
   this->colourDialog->open (this, SLOT (colourSelected (const QColor &)));
}

//------------------------------------------------------------------------------
//
void QEStripChartItemDialog::colourSelected (const QColor & colourIn)
{
   this->setColour(colourIn);
}

//------------------------------------------------------------------------------
// User has pressed clear
//
void QEStripChartItemDialog::clearButtonClicked (bool)
{
   this->ui->pvNameEdit->clear ();
   this->accept ();
}

//------------------------------------------------------------------------------
// User has pressed OK
//
void QEStripChartItemDialog::on_buttonBox_accepted ()
{
   this->accept ();
}

//------------------------------------------------------------------------------
// User has pressed cancel
//
void QEStripChartItemDialog::on_buttonBox_rejected ()
{
   this->close ();
}

// end
