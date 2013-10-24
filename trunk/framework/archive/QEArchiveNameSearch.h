/*  QEArchiveNameSearch.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

#ifndef QEARCHIVENAMESEARCH_H
#define QEARCHIVENAMESEARCH_H

#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QWidget>

#include <QEFrame.h>
#include <QEPluginLibrary_global.h>
#include <QEArchiveManager.h>

/// This is a non EPICS aware widget.
/// It provides a simple user means to find archived PV names.
///
class QEPLUGINLIBRARYSHARED_EXPORT QEArchiveNameSearch : public QEFrame {
  Q_OBJECT
public:
   explicit QEArchiveNameSearch (QWidget* parent = 0);
   ~QEArchiveNameSearch ();

   QStringList getSelectedNames () const;
   void clear ();

protected:
   QSize sizeHint () const;

   // Drag, no drop
   //
   void mousePressEvent (QMouseEvent *event)    { qcaMousePressEvent (event ); }
   QVariant getDrop();

   // Copy, no paste
   //
   QString copyVariable ();  // Function a widget may implement to perform a 'copy variable' operation

private:
   void createInternalWidgets ();

   void search ();

   // Utility to merge two string lists - result also sorted and no duplicates.
   //
   QStringList merge (const QStringList& a, const QStringList& b);

   QEArchiveAccess *archiveAccess;

   // Internal widgets.
   //
   QVBoxLayout *verticalLayout;
   QFrame *searchFrame;
   QHBoxLayout *horizontalLayout;
   QLineEdit *lineEdit;
   QListWidget *listWidget;

private slots:
   void searchReturnPressed ();
};

#endif  // QEARCHIVENAMESEARCH_H 
