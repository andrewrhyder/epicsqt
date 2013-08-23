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
#include <QESettings.h>
#include <QStringList>
#include <QVariant>
#include <QVariantList>

#include "QEPvLoadSave.h"
#include "QEPvLoadSaveItem.h"
#include "QEPvLoadSaveModel.h"

#include "QEPvLoadSaveUtilities.h"

#define DEBUG qDebug() << "QEPvLoadSaveUtilities::" << __FUNCTION__ << ":" << __LINE__

static const int maxDepth        = 10;
static const QString namePrefix  = "*NAME";
static const QString groupPrefix = "*GROUP";
static const QString arrayPrefix = "*ARRAY";


//------------------------------------------------------------------------------
//
QEPvLoadSaveItem* QEPvLoadSaveUtilities::readSection (const QString& groupName,
                                                      QESettings* settings,
                                                      QEPvLoadSaveItem* parent,
                                                      const int level)
{
   QEPvLoadSaveItem* result = NULL;
   QStringList theKeys;
   int j;
   bool isRenamed;
   QString variable;
   QString key;
   QString newName;
   QString sectionName;
   QVariant value;
   QVariant nilValue (QVariant::Invalid);

   if (!settings) {
      DEBUG << "bad input";
      return result;
   }

   if (level >= maxDepth) {
      DEBUG << "Nesting too deep (" << level << "), group: " << groupName;
      return result;
   }

   result = new QEPvLoadSaveItem (groupName, false, nilValue, parent);

   isRenamed = false;
   theKeys = settings->groupKeys (groupName);
   for (j = 0; j < theKeys.count(); j++) {
      variable = theKeys.value (j);
      key = groupName + "/" + variable;

//      DEBUG << j << variable << " full" << key;

      if (variable.startsWith ("#")) continue;   // is a comment";

      if (variable.startsWith (namePrefix, Qt::CaseInsensitive)) {

         if (level == 1) {
            DEBUG << "An attempt to rename the root node section ignored";
            continue;
         }

         if (isRenamed) {
            DEBUG << "An attempt to rename an already renamed section ignored";
            continue;
         }

         newName = settings->getString (key, "");
         if (newName.isEmpty()) {
            DEBUG << "An attempt to rename to empty name ignored";
            continue;
         }

         result->setNodeName (newName);
         isRenamed = true;

      } else if (variable.startsWith (groupPrefix, Qt::CaseInsensitive)) {
         // Valid group entry test??

         sectionName = settings->getString (key, "");

         if (sectionName.isEmpty()) {
            DEBUG << "Unspecified group section name";
            continue;
         }

         QEPvLoadSaveUtilities::readSection (sectionName, settings, result, level + 1);


      } else if (variable.startsWith (arrayPrefix, Qt::CaseInsensitive)) {
         DEBUG << "skipping array for now";

      } else {
         // Assume just a regular PV.
         //

         value = settings->getValue (key, nilValue );
         new QEPvLoadSaveItem (variable, true, value, result);
      }
   }

   return result;
}

//==============================================================================
//
QEPvLoadSaveItem* QEPvLoadSaveUtilities::readTree (const QString& filename)
{
   QEPvLoadSaveItem* result = NULL;
   QESettings* settings = NULL;

   settings = new QESettings (filename.trimmed ());
   if (settings) {
      result = QEPvLoadSaveUtilities::readSection ("ROOT", settings, result, 1);
      delete settings;
   }
   return result;
}

// end
