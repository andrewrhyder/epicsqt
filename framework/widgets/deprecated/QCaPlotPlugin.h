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
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Glenn Jackson
 *  Contact details:
 *    glenn.jackson@synchrotron.org.au
 */

#ifndef QCAPLOTPLUGIN_H
#define QCAPLOTPLUGIN_H

#include <QEPlot.h>

class QCaPlotPlugin : public QEPlot {
    Q_OBJECT

  public:
    QCaPlotPlugin( QWidget *parent = 0 ) : QEPlot( parent ) {}
};

#endif // QCAPLOTPLUGIN_H
