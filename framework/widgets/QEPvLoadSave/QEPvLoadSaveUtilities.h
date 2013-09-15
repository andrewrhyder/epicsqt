/*  QEPvLoadSaveUtilities.h
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

#ifndef QEPVPROPERTIES_UTILITIES_H
#define QEPVPROPERTIES_UTILITIES_H

#include <QVariant>
#include <QESettings.h>

// Differed declaration - avoids mutual header inclusions.
//
class QEPvLoadSave;
class QEPvLoadSaveItem;
class QEPvLoadSaveModel;

//------------------------------------------------------------------------------
// All functions are static.
//
class QEPvLoadSaveUtilities {
public:
   static QEPvLoadSaveItem* readTree (const QString& filename);
private:

   static QVariant readArray (QESettings* settings,
                              const QString& arrayName,
                              QString& pvName);

   static QEPvLoadSaveItem* readSection (QESettings* settings,
                                         const QString& groupName,
                                         QEPvLoadSaveItem* parent,
                                         const int level);

   static QEPvLoadSaveItem* readPcfTree (const QString& filename);
   static QEPvLoadSaveItem* readXmlTree (const QString& filename);

};


# endif  // QEPVPROPERTIES_UTILITIES_H
