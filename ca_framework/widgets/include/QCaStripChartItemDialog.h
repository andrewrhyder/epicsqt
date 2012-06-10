/*  QCaStripChartItemDialog.h
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

#ifndef QCASTRIPCHARTITEMDIALOG_H
#define QCASTRIPCHARTITEMDIALOG_H

#include <QString>
#include <QColor>
#include <QDialog>

namespace Ui {
    class QCaStripChartItemDialog;
}

/*!
 * Manager class for the QCaStripChartItemDialog.ui compiled form.
 */
class QCaStripChartItemDialog : public QDialog
{
    Q_OBJECT

public:
   explicit QCaStripChartItemDialog (QWidget *parent = 0);
   ~QCaStripChartItemDialog ();

   void setPvName (QString pvNameIn);
   QString getPvName ();

   void setColor (QColor colorIn);
   QColor getColor ();

   bool isClear ();

private:
   Ui::QCaStripChartItemDialog *ui;

private slots:
    void on_buttonBox_rejected ();
    void on_buttonBox_accepted ();
    void clearButtonClicked (bool checked = false);
};

#endif  // QCASTRIPCHARTITEMDIALOG_H

