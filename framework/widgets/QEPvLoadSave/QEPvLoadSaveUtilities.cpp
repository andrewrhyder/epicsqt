/*  QEPvLoadSaveUtilities.cpp
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

#include <stdlib.h>

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QVariant>
#include <QVariantList>

#include "QEPvLoadSave.h"
#include "QEPvLoadSaveItem.h"
#include "QEPvLoadSaveModel.h"

#include "QEPvLoadSaveUtilities.h"

#define DEBUG qDebug() << "QEPvLoadSaveUtilities::" << __FUNCTION__ << ":" << __LINE__


//==============================================================================
//
QEPvLoadSaveItem* QEPvLoadSaveUtilities::readTree (QFile* file)
{
   QVariant nil (QVariant::Invalid);
   QEPvLoadSaveItem* result;
   QEPvLoadSaveItem* g1, *g2;

   result = new QEPvLoadSaveItem ("Root", false, nil, NULL);

   g1 = new QEPvLoadSaveItem ("GROUPS", false, nil, result);
   g2 = new QEPvLoadSaveItem ("GROUP2", false, nil, result);


   new QEPvLoadSaveItem ("COUNTER:MONITOR", true, QVariant ((double) 0.0), g1);
   new QEPvLoadSaveItem ("TS01EVR01:TTL01_DELAY_SP", true, QVariant ((double) 0.0), g1);
   new QEPvLoadSaveItem ("TS01EVR01:TTL02_DELAY_SP", true, QVariant ((double) 0.0), g1);
   new QEPvLoadSaveItem ("BLUEGUM:MONITOR", true, QVariant ((double) 0.0), g1);
   new QEPvLoadSaveItem ("GREENGUM:MONITOR", true, QVariant ((double) 0.0), g1);
   new QEPvLoadSaveItem ("POWER:MONITOR", true, QVariant ((double) 0.0), g1);
   new QEPvLoadSaveItem ("REDGUM:MONITOR", true, QVariant ((double) 0.0), g1);
   new QEPvLoadSaveItem ("SR11BCM01:CURRENT_MONITOR", true, QVariant ((double) 0.0), g1);
   new QEPvLoadSaveItem ("SR11BCM01:LIFETIME_CURRENT_PRODUCT", true, QVariant ((double) 0.0), g1);
   new QEPvLoadSaveItem ("SR11BCM01:LIFETIME_MONITOR", true, QVariant ((double) 0.0), g1);

   new QEPvLoadSaveItem ("YELLOWGUM:MONITOR", true, QVariant ((double) 0.0), g2);
   new QEPvLoadSaveItem ("BLUEGUM:MONITOR_STATUS", true, QVariant ((double) 0.0), g2);
   new QEPvLoadSaveItem ("COUNTER:MONITOR_STATUS", true, QVariant ((double) 0.0), g2);
   new QEPvLoadSaveItem ("GREENGUM:MONITOR_STATUS", true, QVariant ((double) 0.0), g2);
   new QEPvLoadSaveItem ("POWER:MONITOR_STATUS", true, QVariant ((double) 0.0), g2);
   new QEPvLoadSaveItem ("REDGUM:MONITOR_STATUS", true, QVariant ((double) 0.0), g2);
   new QEPvLoadSaveItem ("YELLOWGUM:MONITOR_STATUS", true, QVariant ((double) 0.0), g2);
   new QEPvLoadSaveItem ("BLACKGUM:MONITOR_STATUS", true, QVariant ((double) 0.0), g2);
   new QEPvLoadSaveItem ("WAVEFORM:MONITOR", true, QVariant ((double) 0.0), g2);


   return result;
}

// end
