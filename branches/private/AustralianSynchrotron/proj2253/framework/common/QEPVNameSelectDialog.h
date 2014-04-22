/*  QEPVNameSelectDialog.h
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

#ifndef QEPVNAME_SELECT_DIALOG_H
#define QEPVNAME_SELECT_DIALOG_H

#include <QString>
#include <QWidget>
#include <QEDialog.h>

namespace Ui {
   class QEPVNameSelectDialog;
}

/*
 * Manager class for the QEPVNameSelectDialog.ui compiled form.
 */
class QEPVNameSelectDialog : public QEDialog
{
   Q_OBJECT

public:
    enum ListSources { Archiver, File, Database };

    explicit QEPVNameSelectDialog (QWidget *parent = 0);
    explicit QEPVNameSelectDialog (ListSources source, QString fileName, QString title, QWidget *parent = 0);
   ~QEPVNameSelectDialog ();

   void setPvName (QString pvNameIn);
   QString getPvName ();

protected:
   void closeEvent (QCloseEvent * e);
   bool getListFromFile(QString fileName);
   void getListFromDatabase(QString fileName);

private:
   Ui::QEPVNameSelectDialog *ui;
   static QWidget *helpUi;
   QString originalPvName;
   bool returnIsMasked;
   ListSources listSource;
   QStringList list;

   void applyFilter ();
   void closeHelp ();

private slots:
   void filterEditReturnPressed ();
   void filterEditingFinished ();
   void editTextChanged (const QString &);
   void helpClicked (bool checked);

   void on_buttonBox_rejected ();
   void on_buttonBox_accepted ();
};

#endif  // QEPVNAME_SELECT_DIALOG_H

