/*  QEStripChartTimeDialog.h
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
 */

#ifndef QE_STRIP_CHART_TIME_DIALOG_H
#define QE_STRIP_CHART_TIME_DIALOG_H

#include <QString>
#include <QColor>
#include <QDialog>
#include <QDateTime>

namespace Ui {
    class QEStripChartTimeDialog;
}

/*!
 * Manager class for the QEStripChartTimeDialog.ui compiled form.
 *
 * This dialog form allows the user to select a start datetime and
 * end datetime pair for the strip chart.
 */
class QEStripChartTimeDialog : public QDialog
{
    Q_OBJECT

public:
   explicit QEStripChartTimeDialog (QWidget *parent = 0);
   ~QEStripChartTimeDialog ();

   void setMaximumDateTime (QDateTime datetime);

   void setStartDateTime (QDateTime datetime);
   QDateTime getStartDateTime ();

   void setEndDateTime (QDateTime datetime);
   QDateTime getEndDateTime ();

private:
   Ui::QEStripChartTimeDialog *ui;

   QDateTime savedStartDateTime;
   QDateTime savedEndDateTime;

private slots:
   void startTimeChanged (const QTime &time);
   void endTimeChanged (const QTime &time);

   void startSliderValueChanged (int value);
   void endSliderValueChanged (int value);

   void on_buttonBox_rejected ();
   void on_buttonBox_accepted ();
};

#endif  // QE_STRIP_CHART_TIME_DIALOG_H

