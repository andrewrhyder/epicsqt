/*  QEPvPropertiesUtilities.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2013 Australian Synchrotron
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

/* The QEPvPropertiesUtilities class provides utilities for the main QEPvProperties widget.
 */

#ifndef QEPVPROPERTIES_UTILITIES_H
#define QEPVPROPERTIES_UTILITIES_H

#include <QList>
#include <QString>
#include <QStringList>


//------------------------------------------------------------------------------
// This class provides a named (by record type: ai, bo, calc etc) list of record field names.
// No other info about the field (DBF_INLINK, DBF_DOUBLE, DBF_MENU etc.) stored here.
//
class QERecordSpec : public QStringList {
public:
   explicit QERecordSpec (const QString recordType);

   QString getRecordType ();

   // If index is out of bounds, this function returns ""
   //
   QString getFieldName (const int index);

private:
   QString recordType;
};


//------------------------------------------------------------------------------
// This class provides a list of RecordSpec specs, with additional find functions.
//
class QERecordSpecList : public QList< QERecordSpec *> {
public:
   QERecordSpecList ();
   QERecordSpec *find (const QString recordType);
   void appendOrReplace (QERecordSpec *recordSpec);

   bool processRecordSpecFile (const QString& filename);

private:
   int findSlot (const QString recordType);
};


//------------------------------------------------------------------------------
// All functions are static.
//
class QERecordFieldName {
public:
   // Converts PV name to record name, e.g.:
   //
   // SR11BCM01:CURRENT_MONITOR.PREC => SR11BCM01:CURRENT_MONITOR
   // SR11BCM01:CURRENT_MONITOR.VAL  => SR11BCM01:CURRENT_MONITOR
   // SR11BCM01:CURRENT_MONITOR      => SR11BCM01:CURRENT_MONITOR
   //
   static QString recordName (const QString & pvName);

   // Converts PV name to field name, e.g.:
   //
   // SR11BCM01:CURRENT_MONITOR.PREC => PREC
   // SR11BCM01:CURRENT_MONITOR.VAL  => VAL
   // SR11BCM01:CURRENT_MONITOR      => VAL (it's the default)
   //
   static QString fieldName (const QString & pvName);

   // From field PV name
   // (SR11BCM01:CURRENT_MONITOR.PREC, EGU) => SR11BCM01:CURRENT_MONITOR.EGU
   // (SR11BCM01:CURRENT_MONITOR,      EGU) => SR11BCM01:CURRENT_MONITOR.EGU
   //
   static QString fieldPvName (const QString & pvName, const QString & field);

   // Form pseudo field record type PV name.
   // SR11BCM01:CURRENT_MONITOR.PREC => SR11BCM01:CURRENT_MONITOR.RTYP
   //
   static QString rtypePvName (const QString & pvName);

   // This function indicates if the given string is a valid PV name,
   // not ony in the pureset EPICS sense, but that it also conforms to
   // the naming standard.
   //
   static bool pvNameIsValid (const QString & pvName);

   // Remove any qualifiers (e.g. " CPP") prior to validaing name.
   // Modified name set in pvName is valid, otherwise cleared.
   //
   static bool extractPvName (const QString & item, QString & pvName);
};


# endif  // QEPVPROPERTIES_UTILITIES_H
