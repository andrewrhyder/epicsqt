/*  QEScratchPadItemDialog.h
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
 */

#ifndef QESCRATCHPADITEMDIALOG_H
#define QESCRATCHPADITEMDIALOG_H

#include <QString>
#include <QDialog>

namespace Ui {
    class QEScratchPadItemDialog;
}

/*
 * Manager class for the QEScratchPadItemDialog.ui compiled form.
 */
class QEScratchPadItemDialog : public QDialog
{
    Q_OBJECT

public:
   explicit QEScratchPadItemDialog (QWidget *parent = 0);
   ~QEScratchPadItemDialog ();

   void setFieldInformation (const QString dataIn);
   void getFieldInformation (QString& dataOut);

private:
   Ui::QEScratchPadItemDialog *ui;
   bool returnIsMasked;

private slots:
   void dataEditReturnPressed ();

   void on_buttonBox_rejected ();
   void on_buttonBox_accepted ();
   void clearButtonClicked (bool checked = false);
};

#endif  // QESCRATCHPADITEMDIALOG_H

