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
#include <qdom.h>
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
QVariant QEPvLoadSaveUtilities::readArray (QESettings* settings,
                                           const QString& arrayName,
                                           QString& pvName)
{
    QVariantList result;

    pvName = "SR00TDB01";
    result << 1 << 2 << arrayName << QVariant ( (bool) (settings == NULL));

    return result;
}

//------------------------------------------------------------------------------
//
QEPvLoadSaveItem* QEPvLoadSaveUtilities::readSection (QESettings* settings,
                                                      const QString& groupName,
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
   QString pvName;
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
            DEBUG << "An attempt to rename an already renamed section ignored - first in, best dressed.";
            continue;
         }

         newName = settings->getString (key, "");
         if (newName.isEmpty()) {
            DEBUG << "An attempt to rename to empty name ignored";
            continue;
         }

         result->setNodeName (newName);
         isRenamed = true;
         continue;
      }

      // Is it a group?
      //
      if (variable.startsWith (groupPrefix, Qt::CaseInsensitive)) {
         // Valid group entry test??

         sectionName = settings->getString (key, "");

         if (sectionName.isEmpty ()) {
            DEBUG << "Unspecified group section name";
            continue;
         }

         QEPvLoadSaveUtilities::readSection (settings, sectionName, result, level + 1);
         continue;
      }

      // Is it an extented array?
      //
      if (variable.startsWith (arrayPrefix, Qt::CaseInsensitive)) {
         // Extented Array PV get ther own section.
         // Line length limits imposed by original program.
         //
         sectionName = settings->getString (key, "");

         if (sectionName.isEmpty ()) {
            DEBUG << "Unspecified array section name";
            continue;
         }

         value = QEPvLoadSaveUtilities::readArray (settings, sectionName, pvName);\

         if (pvName.isEmpty ()) {
            DEBUG << "Unspecified array PV name in section " << groupName <<  variable;
            continue;
         }

         new QEPvLoadSaveItem (pvName, true, value, result);

         continue;
      }

      // Assume just a regular PV.
      // TBD: Short array format
      //
      value = settings->getValue (key, nilValue);
      new QEPvLoadSaveItem (variable, true, value, result);

   }

   return result;
}


//------------------------------------------------------------------------------
//
QEPvLoadSaveItem* QEPvLoadSaveUtilities::readPcfTree (const QString& filename)
{
   QEPvLoadSaveItem* result = NULL;
   QESettings* settings = NULL;

   settings = new QESettings (filename.trimmed ());
   if (settings) {
      result = QEPvLoadSaveUtilities::readSection (settings, "ROOT", result, 1);
      delete settings;
   }
   return result;
}



//------------------------------------------------------------------------------
//
QEPvLoadSaveItem* QEPvLoadSaveUtilities::readXmlPv (const QDomElement pvElement,
                                                    QEPvLoadSaveItem* parent)
{
   QEPvLoadSaveItem* result = NULL;
   QVariant value (QVariant::Invalid);

   QString pvName = pvElement.attribute ("Name");
   QString dataType = pvElement.attribute ("Type", "string");
   QString elementCountImage = pvElement.attribute ("Number", "1");
   int elementCount = elementCountImage.toInt (NULL);

   if (elementCount == 1) {
      // scaler
   } else {
      // an array
   }

   result = new QEPvLoadSaveItem (pvName, true, value, parent);
   return result;
}


//------------------------------------------------------------------------------
//
QEPvLoadSaveItem* QEPvLoadSaveUtilities::readXmlGroup (const QDomElement groupElement,
                                                       const QString& groupName,
                                                       QEPvLoadSaveItem* parent,
                                                       const int level)
{
   QEPvLoadSaveItem* result = NULL;
   QVariant nilValue (QVariant::Invalid);

   if (groupElement.isNull () || groupName.isEmpty ()) {
      qWarning () << __FUNCTION__ << " null configElement and/or groupName, level => " << level;
      return result;
   }

   result = new QEPvLoadSaveItem (groupName, false, nilValue, parent);

   // Parse XML using Qt's Document Object Model.
   //
   QDomElement itemElement = groupElement.firstChildElement ("");
   while (!itemElement.isNull ())   {

      QString tagName = itemElement.tagName ();

      if (tagName == "Group") {
         QString innerGroupName = itemElement.attribute ("Name");
         QEPvLoadSaveUtilities::readXmlGroup (itemElement, innerGroupName, result, level + 1);

      } else if  (tagName == "PV") {
         QEPvLoadSaveUtilities::readXmlPv (itemElement, result);

      } else {
         qWarning () << __FUNCTION__ << " ignoring unexpected tag " << tagName;
      }

      itemElement = itemElement.nextSiblingElement ("");
   }

   return result;
}


//------------------------------------------------------------------------------
//
QEPvLoadSaveItem* QEPvLoadSaveUtilities::readXmlTree (const QString& filename, const QString& configName)
{
   QEPvLoadSaveItem* result = NULL;

   if (filename.isEmpty()) {
      qWarning () << __FUNCTION__ << " null file filename";
      return result;
   }

   QFile file (filename);
   if (!file.open (QIODevice::ReadOnly)) {
      qWarning () << __FUNCTION__ << filename  << " file open failed";
      return result;
   }

   QDomDocument doc;
   QString errorText;
   int errorLine;
   int errorCol;

   if (!doc.setContent (&file, &errorText, &errorLine, &errorCol)) {
      qWarning () << QString ("%1:%2:%3").arg (filename).arg (errorLine).arg (errorCol)
                  << " set content failed " << errorText;
      file.close ();
      return result;
   }

   QDomElement docElem = doc.documentElement ();

   // The file has been read - we can now close it.
   //
   file.close ();

   if (docElem.tagName () != "QEPvLoadSave") {
      qWarning () << filename  << " unexpected tag name " << docElem.tagName ();
      return result;
   }

   QString versionImage = docElem.attribute ("version").trimmed ();
   bool versionOkay;
   int version = versionImage.toInt (&versionOkay);

   if (!versionImage.isEmpty()) {
      // A version has been specified - it must be senible.
      //
      if (!versionOkay) {
         qWarning () << filename  << " invalid version string " << versionImage << " (integer expected)";
         return result;
      }

   } else {
      // no version - go with current version.
      //
      version = 1;
   }

   if (version != 1) {
      qWarning () << filename  << " unexpected version specified " << versionImage << " (out of range)";
      return result;
   }

   // Parse XML using Qt's Document Object Model.
   //
   QDomElement configElement = docElem.firstChildElement ("Config");
   while (!configElement.isNull ())   {
      QString readConfigName = configElement.attribute ("Name");
      if (readConfigName == configName) {

         // We have a match
         //
         result = QEPvLoadSaveUtilities::readXmlGroup (configElement, "ROOT", NULL, 1);
         break;
      }
      configElement = configElement.nextSiblingElement ("Config");
   }

   return result;
}

//==============================================================================
//
QEPvLoadSaveItem* QEPvLoadSaveUtilities::readTree (const QString& filename)
{
   QEPvLoadSaveItem* result = NULL;

   if (filename.trimmed ().endsWith (".pcf")) {
      result = readPcfTree (filename);

   } else if (filename.trimmed ().endsWith (".xml")) {
      result =  readXmlTree (filename, "Default");   // use default for now

   }

   return result;
}

// end
