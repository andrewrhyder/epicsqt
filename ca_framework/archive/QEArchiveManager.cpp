/*  QEArchiveManager.cpp
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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <stdlib.h>
#include <QDebug>
#include <QUrl>
#include <QMutex>
#include <QHash>

#include <QEArchiveManager.h>


#define DEBUG qDebug () << __FILE__  << ":" << __LINE__ << "(" << __FUNCTION__ << ")"

#define MAX(a, b)    ((a) >= (b) ? (a) : (b))
#define MIN(a, b)    ((a) <= (b) ? (a) : (b))


//==============================================================================
// Archive class type provides key (and name and path).
//
class KeyTimeSpec : public QEArchiveInterface::Archive {
public:
   QCaDateTime startTime;
   QCaDateTime endTime;
};

//------------------------------------------------------------------------------
// Each PV may have one or more archives available on the same
// host, e.g. a short term archive and a long term archive.
// However we expect all archives for a particlar PV to be co-hosted.
//
class SourceSpec : public QHash<int, KeyTimeSpec> {
public:
   QEArchiveInterface * interface;
};


//==============================================================================
// Local Data
//==============================================================================
// We declare these items here as opposed as static members of the class because
// the later made all the EPICS plugin widgets "go away" in designer.
//
static QMutex *mutex = new QMutex ();

static QList<QEArchiveInterface *> archiveInterfaceList;
static QString pattern;
static QHash<QString, SourceSpec> pvNameHash;

static QEArchiveManager singleton;
const bool QEArchiveManager::initialised = singleton.initialise ();


//==============================================================================
// QEArchiveManager Class Methods
//==============================================================================
//
bool QEArchiveManager::initialise (QString archives, QString patternIn)
{
   QStringList archiveList;
   QString item;
   QUrl url;
   QEArchiveInterface * interface;
   int j;

   pattern = patternIn;

   // Split input string using space.
   // Could extend to use regular expression and split on any white space character.
   //
   archiveList = archives.split (' ', QString::SkipEmptyParts);

   if (archiveList.count () == 0) {
      DEBUG << "no archives specified";
      return false;
   }

   this->clear ();

   for (j = 0; j < archiveList.count (); j++) {

      item = "http://";
      item.append (archiveList.value (j));
      url = QUrl (item);

      interface = new QEArchiveInterface (url, &singleton);
      archiveInterfaceList.append (interface);

      connect (interface, SIGNAL (archivesResponse (const QObject *, const bool, const QList<QEArchiveInterface::Archive>&)),
               this,      SLOT   (archivesResponse (const QObject *, const bool, const QList<QEArchiveInterface::Archive>&)));

      connect (interface, SIGNAL (pvNamesResponse  (const QObject *, const bool, const QList<QEArchiveInterface::PVName>&)),
               this,      SLOT   (pvNamesResponse  (const QObject *, const bool, const QList<QEArchiveInterface::PVName>&)));

      connect (interface, SIGNAL (valuesResponse   (const QObject *, const bool, const QList<QEArchiveInterface::ResponseValues>&)),
               this,      SLOT   (valuesResponse   (const QObject *, const bool, const QList<QEArchiveInterface::ResponseValues>&)));

      interface->archivesRequest (interface);
      qDebug () << "Archive info:  url:" << interface->getUrl ().toString ();
   }

   qDebug () << "Archive info:  pattern:" << pattern;

   return true;
}


//------------------------------------------------------------------------------
//
bool QEArchiveManager::initialise ()
{
   bool result;
   QString archives = getenv ("QCA_ARCHIVE_LIST");
   QString pattern = getenv ("QCA_ARCHIVE_PATTERN");

   if (archives != "") {
      if (pattern == "") {
         // Pattern environment variable undefined use "get all" by default.
         pattern = ".*";
      }
      result = this->initialise (archives, pattern);
   } else {
      qDebug () << "Archive info:  QCA_ARCHIVE_LIST undefined";
      result = false;
   }
   return result;
}

//------------------------------------------------------------------------------
//
void QEArchiveManager::clear ()
{
   int j;

   pvNameHash.clear ();

   // Do a deep clear of the archive interface list.
   //
   for (j = 0; j < archiveInterfaceList.count(); j++) {
      delete archiveInterfaceList.value(j);
   }
   archiveInterfaceList.clear ();
}


//==============================================================================
//
class NamesResponseContext : public QObject {
public:
   QEArchiveInterface * interface;
   QEArchiveInterface::Archive archive;
   int instance;
   int number;
   // constructor
   NamesResponseContext (QEArchiveInterface * interfaceIn, QEArchiveInterface::Archive archiveIn, int i, int n)
   {
      this->interface = interfaceIn;
      this->archive = archiveIn;
      this->instance = i;
      this->number = n;
   }
};


//------------------------------------------------------------------------------
//
void QEArchiveManager::archivesResponse (const QObject * userData,
                                         const bool isSuccess,
                                         const QList<QEArchiveInterface::Archive>& archiveList)
{
   QMutexLocker locker (mutex);

   QEArchiveInterface * interface = (QEArchiveInterface *) userData;
   int count;
   int j;

   if (isSuccess) {

      count = archiveList.count ();
      for (j = 0; j < count; j++) {
         QEArchiveInterface::Archive archive = archiveList.value (j);
         NamesResponseContext *context;

         // Create the callback context.
         //
         context = new NamesResponseContext (interface, archive, j+1, count);
         interface->namesRequest (context, archive.key, pattern);
      }
   } else {
      qDebug () << "request archives failure from " << interface->getUrl().toString ();
   }
}


//------------------------------------------------------------------------------
//
void QEArchiveManager::pvNamesResponse  (const QObject * userData,
                                         const bool isSuccess,
                                         const QList<QEArchiveInterface::PVName>& pvNameList)
{
   QMutexLocker locker (mutex);

   NamesResponseContext *context = (NamesResponseContext *) userData;
   int j;

   if (isSuccess) {
      for (j = 0; j < pvNameList.count (); j++ ) {
         QEArchiveInterface::PVName pvChannel = pvNameList.value (j);
         KeyTimeSpec keyTimeSpec;
         SourceSpec sourceSpec;

         keyTimeSpec.key = context->archive.key;
         keyTimeSpec.name = context->archive.name;
         keyTimeSpec.path = context->archive.path;
         keyTimeSpec.startTime = pvChannel.startTime;
         keyTimeSpec.endTime = pvChannel.endTime;

         if (pvNameHash.contains (pvChannel.pvName) == false) {
            // First instance of this PV Name
            //
            sourceSpec.interface = context->interface;
            sourceSpec.insert (keyTimeSpec.key, keyTimeSpec);
            pvNameHash.insert (pvChannel.pvName, sourceSpec);

         } else {

            sourceSpec = pvNameHash.value (pvChannel.pvName);

            if (sourceSpec.interface == context->interface) {
               if (sourceSpec.contains (keyTimeSpec.key) == false) {

                  sourceSpec.insert (keyTimeSpec.key, keyTimeSpec);
                  // QHash: If there is already an item with the key, that item's value is replaced with value.
                  pvNameHash.insert (pvChannel.pvName, sourceSpec);

               } else {
                  DEBUG << "PV " << pvChannel.pvName << " multiple instances of key" << keyTimeSpec.key;
               }

            } else {
               DEBUG << "PV " << pvChannel.pvName << " hosted on multiple interfaces";
               DEBUG << "primarry: " << sourceSpec.interface->getUrl().toString ();
               DEBUG << "secondary:" << context->interface->getUrl().toString ();
            }
         }
      }

   } else {
      DEBUG << "failure" << context->interface->getUrl().toString () << context->archive.key << context->archive.name;
   }

   if (context->instance == context->number) {
      qDebug () << context->interface->getUrl ().toString () << " complete";
   }

   delete context;
}


//==============================================================================
//
class ValuesResponseContext : public QObject {
public:
   QEArchiveAccess * accessor;
   QObject * userData;
      // constructor
      ValuesResponseContext (QEArchiveAccess * accessorIn, QObject * userDataIn) {
      this->accessor = accessorIn;
      this->userData = userDataIn;
   }
};


//------------------------------------------------------------------------------
//
void QEArchiveManager::valuesResponse (const QObject * userData,
                                       const bool isSuccess,
                                       const QList<QEArchiveInterface::ResponseValues>& valuesList)
{
    ValuesResponseContext *context = (ValuesResponseContext *) userData;

    if ((isSuccess) && (valuesList.count () == 1)) {
       emit context->accessor->setArchiveData (context->userData, true, valuesList.value (0).dataPoints);
    } else {
       QCaDataPointList empty;
       emit context->accessor->setArchiveData (context->userData, false, empty);
    }

   delete context;
}


//==============================================================================
//
QEArchiveAccess::QEArchiveAccess (QObject * parent) : QObject (parent)
{
}

//------------------------------------------------------------------------------
//
QEArchiveAccess:: ~QEArchiveAccess ()
{
}


//------------------------------------------------------------------------------
//
bool QEArchiveAccess::readArchive (QObject * userData,
                                   const QString pvName,
                                   const QCaDateTime startTime,
                                   const QCaDateTime endTime,
                                   const int count,
                                   const QEArchiveInterface::How how,
                                   const unsigned int element)
{
   QMutexLocker locker (mutex);

   bool result;
   QString effectivePvName;
   int j;
   QStringList pvNames;
   SourceSpec sourceSpec;
   KeyTimeSpec keyTimeSpec;
   int key;
   int bestOverlap;
   QList<int> keys;
   QCaDateTime useStart;
   QCaDateTime useEnd;
   int overlap;

   // Is this PV currently being archived?
   //
   result = pvNameHash.contains (pvName);
   if (result) {
       // Yes - save this exact name.
       //
       effectivePvName = pvName;
   } else {
       // No - the PV 'as is' is not archived.
       // If user has requested XXXXXX.VAL, check if XXXXXX is archived.
       // Similarly, if user requested YYYYYY, check if YYYYYY.VAL archived.
       //
       if (pvName.right (4) == ".VAL") {
           // Remove the .VAL field and try again.
           //
           effectivePvName = pvName;
           effectivePvName.chop (4);
           result = pvNameHash.contains (effectivePvName);
       } else {
           // Add .VAL and try again.
           //
           effectivePvName = pvName;
           effectivePvName = effectivePvName.append (".VAL");
           result = pvNameHash.contains (effectivePvName);
       }
   }

   if (result) {
      sourceSpec = pvNameHash.value (effectivePvName);

      key = -1;
      bestOverlap = -864000;

      keys = sourceSpec.keys ();
      for (j = 0; j < keys.count (); j++) {

         keyTimeSpec = sourceSpec.value (keys.value (j));

         useStart = MAX (startTime.toUTC (), keyTimeSpec.startTime);
         useEnd   = MIN (endTime.toUTC (),   keyTimeSpec.endTime);

         // We don't worry about calculating the overlap to an accuracy
         // of any one than one second.
         //
         overlap = useStart.secsTo (useEnd);
         if (bestOverlap < overlap) {
            bestOverlap = overlap;
            key = keyTimeSpec.key;
         }
      }

      result = (key >= 0);
      if (result) {
         ValuesResponseContext *context;

         // Create the callback context.
         //
         context = new ValuesResponseContext (this, userData);
         pvNames.append (pvName);
         sourceSpec.interface->valuesRequest (context, key, startTime, endTime, count, how, pvNames, element);

      } else {
         DEBUG " PV" << pvName <<  "has no matching time overlaps";
      }

   } else {
      DEBUG " PV" << pvName <<  "not found in archive";
   }
   return result;
}

//------------------------------------------------------------------------------
//
int QEArchiveAccess::getNumberInterfaces ()
{
   return archiveInterfaceList.count ();
}

//------------------------------------------------------------------------------
//
QString QEArchiveAccess::getPattern ()
{
   return pattern;
}

//------------------------------------------------------------------------------
//
int QEArchiveAccess::getNumberPVs ()
{
   return pvNameHash.count ();
}

// end

