/*  QEStripChartItemDialog.h
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

#ifndef QESTRIPCHARTITEMDIALOG_H
#define QESTRIPCHARTITEMDIALOG_H

#include <QString>
#include <QColor>
#include <QDialog>
#include <QColorDialog>

namespace Ui {
    class QEStripChartItemDialog;
}

/*!
 * Manager class for the QEStripChartItemDialog.ui compiled form.
 */
class QEStripChartItemDialog : public QDialog
{
    Q_OBJECT

public:
   explicit QEStripChartItemDialog (QWidget *parent = 0);
   ~QEStripChartItemDialog ();

   void setPvName (QString pvNameIn);
   QString getPvName ();

   void setColour (QColor colourIn);
   QColor getColour ();

   bool isClear ();

private:
   Ui::QEStripChartItemDialog *ui;
   QColorDialog *colourDialog;
   QColor colour;

private slots:
   void colourButtonClicked (bool checked = false);
   void colourSelected (const QColor & colourIn);

   void on_buttonBox_rejected ();
   void on_buttonBox_accepted ();
   void clearButtonClicked (bool checked = false);
};

#endif  // QESTRIPCHARTITEMDIALOG_H

