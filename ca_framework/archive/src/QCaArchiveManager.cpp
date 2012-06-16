/*  QCaArchiveManager.cpp
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

#include <QCaArchiveManager.h>


#define DEBUG qDebug () << __FILE__  << ":" << __LINE__ << "(" << __FUNCTION__ << ")"

#define MAX(a, b)           ((a) >= (b) ? (a) : (b))
#define MIN(a, b)           ((a) <= (b) ? (a) : (b))


//==============================================================================
// Archive class type provides key (and name and path).
//
class KeyTimeSpec : public QCaArchiveInterface::Archive {
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
   QCaArchiveInterface * interface;
};


//==============================================================================
// Local Data
//==============================================================================
// We declare these ietms here as opposed as stict items in the class because
// the later made all the epicd plugin widgets "go away" in designer.
//
static QMutex *mutex = new QMutex ();

static QList<QCaArchiveInterface *> archiveInterfaceList;
static QString pattern;
static QHash<QString, SourceSpec> pvNameHash;

static QCaArchiveManager singleton;
const bool QCaArchiveManager::inited = singleton.initialise ();



//==============================================================================
// Class Methods
//==============================================================================
//
bool QCaArchiveManager::initialise (QString archives, QString patternIn)
{
   int sp_posn;
   QString item;
   QUrl url;
   QCaArchiveInterface * interface;
   int j;

   if (archives == "") {
      DEBUG << "no archives specified";
      return false;
   }

   this->clear ();

   while (archives != "") {
      archives = archives.trimmed ();
      sp_posn = archives.indexOf (" ");
      if (sp_posn > 0) {
         // found one
         //
         item = archives.left (sp_posn);
         archives.remove(0, sp_posn);

      } else {
         item = archives;
         archives.clear ();
      }

      item.prepend("http://");
      url = QUrl (item);

      interface = new QCaArchiveInterface (url, &singleton);
      archiveInterfaceList.append (interface);

      connect (interface, SIGNAL (archivesResponse (const QObject *, const bool, const QList<QCaArchiveInterface::Archive>&)),
               this,      SLOT   (archivesResponse (const QObject *, const bool, const QList<QCaArchiveInterface::Archive>&)));

      connect (interface, SIGNAL (pvNamesResponse  (const QObject *, const bool, const QList<QCaArchiveInterface::PVName>&)),
               this,      SLOT   (pvNamesResponse  (const QObject *, const bool, const QList<QCaArchiveInterface::PVName>&)));

      connect (interface, SIGNAL (valuesResponse   (const QObject *, const bool, const QList<QCaArchiveInterface::ResponseValues>&)),
               this,      SLOT   (valuesResponse   (const QObject *, const bool, const QList<QCaArchiveInterface::ResponseValues>&)));
   }

   pattern = patternIn;

   if (archiveInterfaceList.count() > 0) {
      for (j = 0; j < archiveInterfaceList.count(); j++) {
         interface = archiveInterfaceList.value (j);
         interface->archivesRequest (interface);
         qDebug () << "Archive info:" << interface->getUrl ().toString ();
      }
      qDebug () << "Archive info:  pattern:" << pattern;
   }

   return true;
}


//------------------------------------------------------------------------------
//
bool QCaArchiveManager::initialise ()
{
   bool result;
   QString archives = getenv ("QCA_ARCHIVE_LIST");
   QString pattern = getenv ("QCA_ARCHIVE_PATTERN");

   if (archives != "") {
      if (pattern == "") {
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
void QCaArchiveManager::clear ()
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
   QCaArchiveInterface * interface;
   QCaArchiveInterface::Archive archive;
   int instance;
   int number;
   // constructor
   NamesResponseContext (QCaArchiveInterface * interfaceIn, QCaArchiveInterface::Archive archiveIn, int i, int n)
   {
      this->interface = interfaceIn;
      this->archive = archiveIn;
      this->instance = i;
      this->number = n;
   }
};


//------------------------------------------------------------------------------
//
void QCaArchiveManager::archivesResponse (const QObject * userData,
                                          const bool isSuccess,
                                          const QList<QCaArchiveInterface::Archive>& archiveList)
{
   QMutexLocker locker (mutex);

   QCaArchiveInterface * interface = (QCaArchiveInterface *) userData;
   int count;
   int j;

   if (isSuccess) {

      count = archiveList.count ();
      for (j = 0; j < count; j++) {
         QCaArchiveInterface::Archive archive = archiveList.value (j);
         NamesResponseContext *context;

         // Create the callback context.
         //
         context = new NamesResponseContext (interface, archive, j+1, count);
         interface->namesRequest (context, archive.key, pattern);
      }
   } else {
      DEBUG << "failure" << interface->getUrl().toString ();
   }
}


//------------------------------------------------------------------------------
//
void QCaArchiveManager::pvNamesResponse  (const QObject * userData,
                                          const bool isSuccess,
                                          const QList<QCaArchiveInterface::PVName>& pvNameList)
{
   QMutexLocker locker (mutex);

   NamesResponseContext *context = (NamesResponseContext *) userData;
   int j;

   if (isSuccess) {
      for (j = 0; j < pvNameList.count (); j++ ) {
         QCaArchiveInterface::PVName pvChannel = pvNameList.value (j);
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
   QCaArchiveAccess * accessor;
   QObject * userData;
      // constructor
      ValuesResponseContext (QCaArchiveAccess * accessorIn, QObject * userDataIn) {
      this->accessor = accessorIn;
      this->userData = userDataIn;
   }
};


//------------------------------------------------------------------------------
//
void QCaArchiveManager::valuesResponse (const QObject * userData,
                                        const bool isSuccess,
                                        const QList<QCaArchiveInterface::ResponseValues>& valuesList)
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
QCaArchiveAccess::QCaArchiveAccess (QObject * parent) : QObject (parent)
{
}

//------------------------------------------------------------------------------
//
QCaArchiveAccess:: ~QCaArchiveAccess ()
{
}


//------------------------------------------------------------------------------
//
bool QCaArchiveAccess::readArchive (QObject * userData,
                                    const QString pvName,
                                    const QCaDateTime startTime,
                                    const QCaDateTime endTime,
                                    const int count,
                                    const QCaArchiveInterface::How how,
                                    const unsigned int element)
{
   QMutexLocker locker (mutex);

   bool result;
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

   // TODO: check for pvName +/- .VAL
   //
   result = pvNameHash.contains (pvName);
   if (result) {
      sourceSpec = pvNameHash.value (pvName);

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
int QCaArchiveAccess::getNumberInterfaces ()
{
   return archiveInterfaceList.count ();
}

//------------------------------------------------------------------------------
//
QString QCaArchiveAccess::getPattern ()
{
   return pattern;
}

//------------------------------------------------------------------------------
//
int QCaArchiveAccess::getNumberPVs ()
{
   return pvNameHash.count ();
}

// end

