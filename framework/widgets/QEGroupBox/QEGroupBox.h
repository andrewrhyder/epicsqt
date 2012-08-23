/*  QEGroupBox.h
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

#ifndef QEGROUPBOX_H
#define QEGROUPBOX_H

#include <QGroupBox>
#include <QCaWidget.h>
#include <QCaPluginLibrary_global.h>

/// The QEGroupBox class provide a minimalist extention to the QGroupBox class
/// in that it provides user level enabled and user level visibility control to
/// the group box but more significantly to all the widget enclosed within the
/// QEGroupBox container as well.
/// See QEFrame as well.
///
class QCAPLUGINLIBRARYSHARED_EXPORT QEGroupBox :
      public QGroupBox, public QCaWidget {

Q_OBJECT

#include <standardProperties.inc>

public:
    explicit QEGroupBox (QWidget *parent = 0);
    virtual ~QEGroupBox ();

};

#endif     /// QEGROUPBOX_H
