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

// Differed declarations - avoids mutual header inclusions.
//
class QEPvLoadSave;
class QEPvLoadSaveItem;
class QEPvLoadSaveModel;

//------------------------------------------------------------------------------
// All functions are static.
//
class QEPvLoadSaveUtilities {
public:

   // This function reads xml file.
   // Example file:
   //
   // <QEPvLoadSave version="1">
   //    <!-- No need to specify top level ROOT group in file -->
   //
   //    <Group Name="Colour Values">
   //       <PV Name="REDGUM:MONITOR" Type="int" Value = "5" />
   //    </Group>
   //
   //    <Group Name="Colour Status">
   //       <!-- Empty group -->
   //    </Group>
   //
   //    <Group Name="Nested" >
   //       <Group Name="Inner" >
   //          <Array Name="WAVEFORM:MONITOR"  Type="float"  Number="8" >
   //             <Element Index="0" Value="4"     />
   //             <Element Index="1" Value="5.6"   />
   //             <Element Index="2" Value="3.5"   />
   //             <Element Index="3" Value="11.5"  />
   //             <Element Index="4" Value="-12."  />
   //             <Element Index="5" Value="55"    />
   //             <Element Index="6" Value="73.73" />
   //             <Element Index="7" Value="0"     />
   //          </Array>
   //       </Group>
   //    </Group>
   //
   //    <PV Name="FS01:BEAM_MODE" Type="string" Value = "User Beam - Top Up"  />
   //
   // </QEPvLoadSave>
   //
   static QEPvLoadSaveItem* readXmlTree (const QString& filename);

   // Reads old style .pcf files as used by Delphi PV Load/Save
   //
   static QEPvLoadSaveItem* readPcfTree (const QString& filename);

   // This is a wrapper function that calls readXmlTree/readPcfTree
   // It (currently) makes decision based on the filebane extension.
   //
   static QEPvLoadSaveItem* readTree (const QString& filename);


   // This function creates xml file.
   //
   static bool writeXmlTree (const QString& filename, const QEPvLoadSaveItem* root);

   // Creates old style .pcf files as used by Delphi PV Load/Save
   //
   static bool writePcfTree (const QString& filename, const QEPvLoadSaveItem* root);

   // This is a wrapper function that calls writeXmlTree/writePcfTree
   // It (currently) makes decision based on the filebane extension.
   //
   static bool writeTree (const QString& filename, const QEPvLoadSaveItem* root);


private:

   static QVariant readArray (QESettings* settings,
                              const QString& arrayName,
                              QString& pvName);

   static QEPvLoadSaveItem* readSection (QESettings* settings,
                                         const QString& groupName,
                                         QEPvLoadSaveItem* parent,
                                         const int level);

   // XML read write local utilitiy functions.
   //
   static QVariant convert (const QString& dataType,
                            const QString& valueImage);

   static QEPvLoadSaveItem* readXmlScalerPv (const QDomElement pvElement,
                                             QEPvLoadSaveItem* parent);

   static QEPvLoadSaveItem* readXmlArrayPv (const QDomElement pvElement,
                                            QEPvLoadSaveItem* parent);

   static void readXmlGroup (const QDomElement groupElement,
                             QEPvLoadSaveItem* parent,
                             const int level);


   static void writeXmlScalerPv (const QEPvLoadSaveItem* item,
                                 QDomElement& pvElement);

   static void writeXmlArrayPv (const QEPvLoadSaveItem* item,
                                QDomDocument& doc,
                                QDomElement& pvElement);

   static void writeXmlGroup (const QEPvLoadSaveItem* group,
                              QDomDocument& doc,
                              QDomElement& groupElement);
};

# endif  // QEPVPROPERTIES_UTILITIES_H
