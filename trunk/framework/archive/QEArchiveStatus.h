/*  QEArchiveStatus.h
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

#ifndef QEARCHIVESTATUS_H
#define QEARCHIVESTATUS_H

#include <QObject>

#include <QWidget>
#include <QEFrame.h>
#include <QEPluginLibrary_global.h>
#include <QEArchiveManager.h>

/// This is a non EPICS aeare widget.
/// It extracts and displays states information from the archive manager.
///
class QEPLUGINLIBRARYSHARED_EXPORT QEArchiveStatus : public QEFrame {
  Q_OBJECT
public:
   explicit QEArchiveStatus (QWidget* parent = 0);
   ~QEArchiveStatus ();
   QSize sizeHint () const;

private:
   // If internal widgets are declared at class level, there is a run time
   // exception, so they are placed in separate and private class.
   //
   class PrivateData;
   PrivateData *privateData;

private slots:
   void archiveStatus (const QEArchiveAccess::StatusList& statusList);

};

#endif  // QEARCHIVESTATUS_H 