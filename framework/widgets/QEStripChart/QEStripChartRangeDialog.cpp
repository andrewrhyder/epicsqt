/*  QEStripChartRangeDialog.cpp
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
#include <QEStripChartRangeDialog.h>
#include <ui_QEStripChartRangeDialog.h>


static const QTime zero (0, 0, 0, 0);

//------------------------------------------------------------------------------
//
QEStripChartRangeDialog::QEStripChartRangeDialog (QWidget *parent) :
      QDialog (parent),
      ui (new Ui::QEStripChartRangeDialog)
{
   this->ui->setupUi (this);


//  QObject::connect (this->ui->startTimeEdit, SIGNAL (timeChanged      (const QTime &)),
//                    this,                    SLOT   (startTimeChanged (const QTime &)));

}

//------------------------------------------------------------------------------
//
QEStripChartRangeDialog::~QEStripChartRangeDialog ()
{
   delete ui;
}


//------------------------------------------------------------------------------
//
void QEStripChartRangeDialog::setRange (const double minIn, const double maxIn)
{
    this->ui->minimumSpinBox->setValue (minIn);
    this->ui->maximumSpinBox->setValue (maxIn);
}

//------------------------------------------------------------------------------
//
double QEStripChartRangeDialog::getMinimum ()
{
    return this->ui->minimumSpinBox->value();
}

//------------------------------------------------------------------------------
//
double QEStripChartRangeDialog::getMaximum ()
{
    return this->ui->maximumSpinBox->value();
}


//==============================================================================
// Slots.
//==============================================================================
//
// Detctect changes on the fly - ensure max > min ???



//------------------------------------------------------------------------------
// User has pressed OK
//
void QEStripChartRangeDialog::on_buttonBox_accepted ()
{
   this->accept ();
}

//------------------------------------------------------------------------------
// User has pressed cancel
//
void QEStripChartRangeDialog::on_buttonBox_rejected ()
{
   this->close ();
}

// end
