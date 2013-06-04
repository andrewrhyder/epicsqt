/*  QEStripChartAdjustPVDialog.h
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

#ifndef QE_STRIP_CHART_ADJUST_DIALOG_H
#define QE_STRIP_CHART_ADJUST_DIALOG_H

#include <QString>
#include <QColor>
#include <QDialog>
#include <QDateTime>

#include <QEStripChartUtilities.h>

namespace Ui {
    class QEStripChartAdjustPVDialog;
}

/*
 * Manager class for the QEStripChartAdjustPVDialog.ui compiled form.
 *
 * This dialog form allows the user to select a start datetime and
 * end datetime pair for the strip chart.
 */
class QEStripChartAdjustPVDialog : public QDialog
{
    Q_OBJECT

public:
   explicit QEStripChartAdjustPVDialog (QWidget *parent = 0);
   ~QEStripChartAdjustPVDialog ();

    void setValueScaling (const ValueScaling & valueScale);
    ValueScaling getValueScaling ();

    // Support data for scaling scalculations.
    //
    void setSupport (const double min, const double max,
                     const TrackRange& loprHopr,
                     const TrackRange& plotted,
                     const TrackRange& buffered);

private:
   Ui::QEStripChartAdjustPVDialog *ui;

   ValueScaling valueScale;

   TrackRange loprHopr;
   TrackRange plotted;
   TrackRange buffered;

   double chartMinimum;
   double chartMaximum;
   bool returnIsMasked;

   // local utility conveniance function
   void useSelectedRange (const TrackRange& selectedRange);

private slots:
   void originReturnPressed ();
   void slopeReturnPressed ();
   void offsetReturnPressed ();

   void resetButtonClicked (bool checked = false);
   void loprHoprButtonClicked (bool checked = false);
   void plottedButtonClicked (bool checked = false);
   void bufferedButtonClicked (bool checked = false);

   // These conection are made by QDialog and associates.
   //
   void on_buttonBox_rejected ();
   void on_buttonBox_accepted ();
};

#endif  // QE_STRIP_CHART_ADJUST_DIALOG_H

