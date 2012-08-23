/*  QEFrame.h
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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QEFRAME_H
#define QEFRAME_H

#include <QFrame>
#include <QCaWidget.h>
#include <QCaPluginLibrary_global.h>


/// The QEFrame class provide a minimalist extention to the QFrame class
/// in that it provides user level enabled and user level visibility control to
/// the frame but more significantly to all the widget enclosed within the
/// QEFrame container as well.
/// See QEGroupBox as well.
///
class QCAPLUGINLIBRARYSHARED_EXPORT QEFrame :
      public QFrame, public QCaWidget {

Q_OBJECT

#include <standardProperties.inc>

public:
    explicit QEFrame (QWidget *parent = 0);
    virtual ~QEFrame ();

};

#endif     /// QEFRAME_H
