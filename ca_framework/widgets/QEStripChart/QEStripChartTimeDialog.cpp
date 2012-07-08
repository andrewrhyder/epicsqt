/*  QEStripChartTimeDialog.cpp
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
#include <QEStripChartTimeDialog.h>
#include <ui_QEStripChartTimeDialog.h>


static const QTime zero (0, 0, 0, 0);

//------------------------------------------------------------------------------
//
QEStripChartTimeDialog::QEStripChartTimeDialog (QWidget *parent) :
      QDialog (parent),
      ui (new Ui::QEStripChartTimeDialog)
{
   this->ui->setupUi (this);

   this->ui->startTimeSlider->setTracking (true);
   this->ui->endTimeSlider->setTracking (true);

   QObject::connect (this->ui->startTimeEdit, SIGNAL (timeChanged      (const QTime &)),
                     this,                    SLOT   (startTimeChanged (const QTime &)));

   QObject::connect (this->ui->endTimeEdit,   SIGNAL (timeChanged      (const QTime &)),
                     this,                    SLOT   (endTimeChanged   (const QTime &)));

   QObject::connect (this->ui->startTimeSlider, SIGNAL (valueChanged            (int)),
                     this,                      SLOT   (startSliderValueChanged (int)));

   QObject::connect (this->ui->endTimeSlider,   SIGNAL (valueChanged            (int)),
                     this,                      SLOT   (endSliderValueChanged   (int)));
}

//------------------------------------------------------------------------------
//
QEStripChartTimeDialog::~QEStripChartTimeDialog ()
{
   delete ui;
}

//------------------------------------------------------------------------------
//
void QEStripChartTimeDialog::setMaximumDateTime (QDateTime datetime)
{
   QDate date = datetime.date ();

   this->ui->startDate->setMaximumDate (date);
   this->ui->endDate->setMaximumDate   (date);
}

//------------------------------------------------------------------------------
//
void QEStripChartTimeDialog::setStartDateTime (QDateTime datetime)
{
   QDate date = datetime.date ();
   QTime time = datetime.time ();
   int t;

   this->savedStartDateTime = datetime;

   // 240 steps - each 6 minutes.
   t = QTime ().secsTo (time) / 360;

   this->ui->startDate->setSelectedDate (date);
   this->ui->startTimeSlider->setValue (t);
   this->ui->startTimeEdit->setTime (time);
}

//------------------------------------------------------------------------------
//
QDateTime QEStripChartTimeDialog::getStartDateTime ()
{
   // By useing the saved time - we preserve the time infor.
   //
   this->savedStartDateTime.setDate (this->ui->startDate->selectedDate ());
   this->savedStartDateTime.setTime (this->ui->startTimeEdit->time ());
   return this->savedStartDateTime;
}

//------------------------------------------------------------------------------
//
void QEStripChartTimeDialog::setEndDateTime (QDateTime datetime)
{
   QDate date = datetime.date ();
   QTime time = datetime.time ();
   int t;

   this->savedEndDateTime = datetime;

   // 240 steps - each 6 minutes.
   t = QTime ().secsTo (time) / 360;

   this->ui->endDate->setSelectedDate (date);
   this->ui->endTimeSlider->setValue (t);
   this->ui->endTimeEdit->setTime (time);
}

//------------------------------------------------------------------------------
//
QDateTime QEStripChartTimeDialog::getEndDateTime ()
{
   this->savedEndDateTime.setDate (this->ui->endDate->selectedDate ());
   this->savedEndDateTime.setTime (this->ui->endTimeEdit->time ());
   return this->savedEndDateTime;
}


//==============================================================================
// Slots.
//==============================================================================
//
void QEStripChartTimeDialog::startTimeChanged (const QTime &time)
{
   int t;

   t = QTime ().secsTo (time) / 360;
   this->ui->startTimeSlider->setValue (t);

}

//------------------------------------------------------------------------------
//
void QEStripChartTimeDialog::endTimeChanged (const QTime &time)
{
   int t;

   t = QTime ().secsTo (time) / 360;
   this->ui->endTimeSlider->setValue (t);
}


//------------------------------------------------------------------------------
//
void QEStripChartTimeDialog::startSliderValueChanged (int value)
{
    QTime time;

    time = QTime ().addSecs (360 * value);
    this->ui->startTimeEdit->setTime (time);
}

//------------------------------------------------------------------------------
//
void QEStripChartTimeDialog::endSliderValueChanged (int value)
{
   QTime time;

   time = QTime ().addSecs (360 * value);
   this->ui->endTimeEdit->setTime (time);
}


//------------------------------------------------------------------------------
// User has pressed OK
//
void QEStripChartTimeDialog::on_buttonBox_accepted ()
{
   this->accept ();
}

//------------------------------------------------------------------------------
// User has pressed cancel
//
void QEStripChartTimeDialog::on_buttonBox_rejected ()
{
   this->close ();
}

// end
