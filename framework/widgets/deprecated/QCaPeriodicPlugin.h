/*
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
 *  Copyright (c) 2011
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QCAPERIODICPLUGIN_H
#define QCAPERIODICPLUGIN_H

#include <QEPeriodic.h>


/// QCaPeriodicPlugin IS deprecated - use QEPeriodic.
//
class QCaPeriodicPlugin : public QEPeriodic {
    Q_OBJECT

  public:
    QCaPeriodicPlugin( QWidget *parent = 0 ) : QEPeriodic( parent ) {}

};

#endif /// QCAPERIODICPLUGIN_H
