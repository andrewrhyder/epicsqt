/*!
  \class QCaEventFilter
  \version $Revision: #1 $
  \date $DateTime: 2009/07/28 17:01:05 $
  \author andrew.rhyder
  \brief CA Date Time manager
 */
/*
 *  This file is part of the EPICS QT Framework.
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
 *    Andrew Rhyder
 *  Contact details:
 *    andrew@rhyder.org
 */

#ifndef QCADATETIME_H
#define QCADATETIME_H

#include <QDateTime>

class QCaDateTime : public QDateTime
{
public:
    QCaDateTime();
    QCaDateTime( QDateTime dt );
    void operator=( QCaDateTime& other );
    QCaDateTime( unsigned long seconds, unsigned long nanoseconds );

    QString text();

    unsigned long nSec;
};

#endif // QCADATETIME_H
