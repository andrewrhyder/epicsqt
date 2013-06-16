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
#include <QHash>
#include <QList>
#include <QMutex>
#include <QUrl>

#include <QECommon.h>
#include <QEArchiveManager.h>

#define DEBUG  qDebug () << "QEArchiveManager::" <<  __FUNCTION__  << ":" << __LINE__


//==============================================================================
// Local Types
//==============================================================================
// Essentially just tacking on a bit of status to the basic QEArchiveInterface.
//
class ArchiveInterfacePlus : public QEArchiveInterface {
public:
   explicit ArchiveInterfacePlus (QUrl url, QObject* parent = 0);

   QEArchiveAccess::States state;
   int available;
   int read;
   int numberPVs;
};

//------------------------------------------------------------------------------
//
ArchiveInterfacePlus::ArchiveInterfacePlus (QUrl url, QObject* parent) : QEArchiveInterface (url, parent)
{
   this->state = QEArchiveAccess::Unknown;
   this->available = 0;
   this->read = 0;
   this->numberPVs = 0;
}

//------------------------------------------------------------------------------
// The archive manager can support many different interfaces.
//
typedef QList <ArchiveInterfacePlus*> ArchiveInterfaceLists;


//------------------------------------------------------------------------------
// Archive class type provides key (and name and path - these not used as suich
// but may prove to be usefull).
// For a particualar PV, we also retrieve a start and stop time.
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
// This type provides a mapping from key (sparce numbers) to KeyTimeSpec
// which contain the key itself, together with the available start/stop
// times. This allows use to choose the key that best fits the request
// time frame.
//
// Note: QHash provides faster lookups than QMap.
//       When iterating over a QMap, the items are always sorted by key.
//       With QHash, the items are arbitrarily ordered.
//
class SourceSpec {
public:
   ArchiveInterfacePlus* interface;
   QHash <int, KeyTimeSpec> keyToTimeSpecLookUp;
};

//------------------------------------------------------------------------------
// Mapping by PV name to essentially archive source to key(s) and time range(s)
// that support the PV.
//
typedef QHash <QString, SourceSpec> PVNameToSourceSpecLookUp;


//==============================================================================
// Local Data
//==============================================================================
// We declare these items here as opposed as static members of the class because
// the later made all the EPICS plugin widgets "go away" in designer.
// I think the are issues when QObjects declared in header files.
//

// Allows only one object to be effectively created. Second are subsequenct object
// do nothing, except waste space.
//
static QMutex *singletonMutex = new QMutex ();
static QEArchiveManager *singleton = NULL;

static QMutex *archiveDataMutex = new QMutex ();
static ArchiveInterfaceLists archiveInterfaceList;
static PVNameToSourceSpecLookUp pvNameToSourceLookUp;

static QString pattern;
static bool allArchivesRead = false;
static int numberArchivesRead = 0;
static bool environmentErrorReported = false;


//==============================================================================
// QEArchiveManager Class Methods
//==============================================================================
//
//The singleton manager object is an orphan.
//
QEArchiveManager::QEArchiveManager () : QObject (NULL)
{
   // Hard-coded message Id.
   //
   this->setSourceId (9001);
}

//------------------------------------------------------------------------------
//
void QEArchiveManager::setup (const QString& archives, const QString& patternIn)
{
   QStringList archiveList;
   QString item;
   QUrl url;
   ArchiveInterfacePlus *interface;
   int j;

   // First check we are the one and only ....
   // Belts 'n' braces sainity check.
   //
   if (this != singleton) {
      // This is NOT the singleton object.
      this->sendMessage ("QEArchiveManager::initialise - attempt to use non-singleton object",
                          message_types (MESSAGE_TYPE_ERROR));
      return;
   }

   // All okay to go, let get started.
   //
   pattern = patternIn;

   // Split input string using space as delimiter.
   // Could extend to use regular expression and split on any white space character.
   //
   archiveList = archives.split (' ', QString::SkipEmptyParts);

   if (archiveList.count () == 0) {
      this->sendMessage ("QEArchiveManager: no archives specified",
                         message_types (MESSAGE_TYPE_INFO));
      return;
   }

   this->clear ();

   this->sendMessage (QString ("pattern: ").append (pattern),
                      message_types (MESSAGE_TYPE_INFO));

   for (j = 0; j < archiveList.count (); j++) {

      item = "http://";
      item.append (archiveList.value (j));
      url = QUrl (item);

      interface = new ArchiveInterfacePlus (url, singleton);
      archiveInterfaceList.append (interface);

      connect (interface, SIGNAL (archivesResponse (const QObject*, const bool, const QEArchiveInterface::ArchiveList &)),
               this,      SLOT   (archivesResponse (const QObject*, const bool, const QEArchiveInterface::ArchiveList &)));

      connect (interface, SIGNAL (pvNamesResponse  (const QObject*, const bool, const QEArchiveInterface::PVNameList &)),
               this,      SLOT   (pvNamesResponse  (const QObject*, const bool, const QEArchiveInterface::PVNameList &)));

      connect (interface, SIGNAL (valuesResponse   (const QObject*, const bool, const QEArchiveInterface::ResponseValueList &)),
               this,      SLOT   (valuesResponse   (const QObject*, const bool, const QEArchiveInterface::ResponseValueList &)));

      interface->archivesRequest (interface);

      this->sendMessage (QString ("requesting PV name info from ").append (interface->getName ()),
                         message_types (MESSAGE_TYPE_INFO));
   }

   this->resendStatus ();
}


//------------------------------------------------------------------------------
// static
void QEArchiveManager::initialise (const QString& archives, const QString& patternIn)
{
   QMutexLocker locker (singletonMutex);

   if (!singleton) {
      singleton = new QEArchiveManager ();
      singleton->setup (archives, patternIn);
   }
}

//------------------------------------------------------------------------------
// static
void QEArchiveManager::initialise ()
{
   QString archives = getenv ("QE_ARCHIVE_LIST");
   QString pattern = getenv ("QE_ARCHIVE_PATTERN");

   if (archives != "") {
      if (pattern.isEmpty ()) {
         // Pattern environment variable undefined, use "get all" by default.
         //
         pattern = ".*";
      }

      QEArchiveManager::initialise (archives, pattern);

   } else {
      // Has this error already been reported??
      // Not strictly 100% thread safe but not strictly critical either.
      //
      if (!environmentErrorReported) {
         environmentErrorReported = true;
         qDebug() << "QE_ARCHIVE_LIST undefined. Required to backfill QEStripChart widgets. Define as space delimited archiver URLs";
      }
   }
}

//------------------------------------------------------------------------------
//
void QEArchiveManager::clear ()
{
   int j;

   allArchivesRead = false;
   numberArchivesRead = 0;
   pvNameToSourceLookUp.clear ();

   // Do a deep clear of the archive interface list.
   //
   for (j = 0; j < archiveInterfaceList.count (); j++) {
      delete archiveInterfaceList.value (j);
   }
   archiveInterfaceList.clear ();

   this->resendStatus ();
}

//------------------------------------------------------------------------------
//
void QEArchiveManager::resendStatus ()
{
   QEArchiveAccess::StatusList statusList;
   int j;
   QUrl url;

   QEArchiveAccess::Status status;

   statusList.clear ();
   for (j = 0; j < archiveInterfaceList.count(); j++) {
      ArchiveInterfacePlus* archiveInterface = archiveInterfaceList.value (j);

      url = archiveInterface->getUrl ();
      status.hostName = url.host ();
      status.portNumber = url.port();
      status.endPoint = url.path ();

      status.state = archiveInterface->state;
      status.available = archiveInterface->available;
      status.read = archiveInterface->read;
      status.numberPVs = archiveInterface->numberPVs;

      statusList.append (status);
   }

   this->archiveStatus (statusList);
}


//==============================================================================
//
class NamesResponseContext:public QObject {
public:
   ArchiveInterfacePlus * interface;
   ArchiveInterfacePlus::Archive archive;
   int instance;
   int number;

   // constructor
   NamesResponseContext (ArchiveInterfacePlus * interfaceIn,
                         ArchiveInterfacePlus::Archive archiveIn,
                         int i,
                         int n)
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
                                         const QEArchiveInterface::ArchiveList & archiveList)
{
   QMutexLocker locker (archiveDataMutex);

   ArchiveInterfacePlus *interface = (ArchiveInterfacePlus *) userData;
   int count;
   int j;

   if (isSuccess) {

      count = archiveList.count ();
      interface->available = count;

      for (j = 0; j < count; j++) {
         QEArchiveInterface::Archive archive;  // key (name and path)
         NamesResponseContext *context;


         // Create the callback context.
         //
         archive = archiveList.value (j);
         context = new NamesResponseContext (interface, archive, j + 1, count);
         interface->namesRequest (context, archive.key, pattern);
      }

   } else {
       this->sendMessage (QString ("request failure from ").append (interface->getName ()),
                          message_types (MESSAGE_TYPE_ERROR));
   }

   singleton->resendStatus ();
}


//------------------------------------------------------------------------------
//
void QEArchiveManager::pvNamesResponse (const QObject * userData,
                                        const bool isSuccess,
                                        const QEArchiveInterface::PVNameList &pvNameList)
{
   QMutexLocker locker (archiveDataMutex);

   NamesResponseContext *context = (NamesResponseContext *) userData;
   ArchiveInterfacePlus *interface = context->interface;

   QString message;
   int j;

   if (isSuccess) {
      interface->read++;

      for (j = 0; j < pvNameList.count (); j++) {
         QEArchiveInterface::PVName pvChannel = pvNameList.value (j);
         KeyTimeSpec keyTimeSpec;
         SourceSpec sourceSpec;

         keyTimeSpec.key = context->archive.key;
         keyTimeSpec.name = context->archive.name;
         keyTimeSpec.path = context->archive.path;
         keyTimeSpec.startTime = pvChannel.startTime;
         keyTimeSpec.endTime = pvChannel.endTime;

         if (pvNameToSourceLookUp.contains (pvChannel.pvName) == false) {
            // First instance of this PV Name
            //
            interface->numberPVs++;
            sourceSpec.interface = context->interface;
            sourceSpec.keyToTimeSpecLookUp.insert (keyTimeSpec.key, keyTimeSpec);
            pvNameToSourceLookUp.insert (pvChannel.pvName, sourceSpec);

         } else {
            // Second or subsequent instance of this PV.
            //
            sourceSpec = pvNameToSourceLookUp.value (pvChannel.pvName);

            if (sourceSpec.interface == context->interface) {
               if (sourceSpec.keyToTimeSpecLookUp.contains (keyTimeSpec.key) == false) {

                  sourceSpec.keyToTimeSpecLookUp.insert (keyTimeSpec.key, keyTimeSpec);
                  // QHash: If there is already an item with the key, that item's value is replaced with value.
                  pvNameToSourceLookUp.insert (pvChannel.pvName, sourceSpec);

               } else {

                  message.sprintf ("PV %s has multiple instances of key %d",
                                   pvChannel.pvName.toAscii().data (), keyTimeSpec.key ) ;
                  this->sendMessage (message, message_types (MESSAGE_TYPE_ERROR));

               }

            } else {

               message.sprintf ("PV %s hosted on multiple interfaces. Primary %s, Secondary %s",
                                pvChannel.pvName.toAscii().data (),
                                sourceSpec.interface->getName ().toAscii().data (),
                                context->interface->getName ().toAscii().data ());
               this->sendMessage (message, message_types (MESSAGE_TYPE_ERROR));
            }
         }
      }

   } else {

      this->sendMessage (QString ("PV names failure from ").
                         append (context->interface->getName ()).
                         append (" for archive ").
                         append (context->archive.name),
                         message_types (MESSAGE_TYPE_ERROR));

   }

   if (context->instance == context->number) {
      message = "PV name retrival from ";
      message.append (context->interface->getName ());
      message.append (" complete");
      this->sendMessage (message);

      numberArchivesRead++;
      allArchivesRead = (numberArchivesRead = archiveInterfaceList.count ());
   }

   delete context;
   singleton->resendStatus ();
}


//==============================================================================
//
class ValuesResponseContext:public QObject {
public:
   QEArchiveAccess * accessor;
   QObject *userData;

   // constructor
   ValuesResponseContext (QEArchiveAccess * accessorIn,
                          QObject * userDataIn)
   {
      this->accessor = accessorIn;
      this->userData = userDataIn;
   }
};


//------------------------------------------------------------------------------
//
void QEArchiveManager::valuesResponse (const QObject * userData,
                                       const bool isSuccess,
                                       const QEArchiveInterface::ResponseValueList& valuesList)
{
   ValuesResponseContext *context = (ValuesResponseContext *) userData;

   if ((isSuccess) && (valuesList.count () == 1)) {
      emit context->accessor->setArchiveData
              (context->userData, true, valuesList.value (0).dataPoints);
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
   // Construct and initialise singleton QEArchiveManager object if needs be.
   //
   QEArchiveManager::initialise ();

   QObject::connect (singleton, SIGNAL (archiveStatus (const QEArchiveAccess::StatusList&)),
                     this,      SLOT (rxArchiveStatus (const QEArchiveAccess::StatusList&)));
}

//------------------------------------------------------------------------------
//
QEArchiveAccess::~QEArchiveAccess ()
{
}

//------------------------------------------------------------------------------
//
unsigned int QEArchiveAccess::getMessageSourceId ()
{
   return this->getSourceId ();
}

//------------------------------------------------------------------------------
//
void QEArchiveAccess::setMessageSourceId (unsigned int messageSourceIdIn)
{
   this->setSourceId (messageSourceIdIn);
}

//------------------------------------------------------------------------------
// static functions
//
void QEArchiveAccess::initialise (const QString& archives, const QString& pattern)
{
   // Construct and initialise singleton QEArchiveManager object if needs be.
   //
   QEArchiveManager::initialise (archives, pattern);
}

//------------------------------------------------------------------------------
//
void QEArchiveAccess::initialise ()
{
   // Construct and initialise singleton QEArchiveManager object if needs be.
   //
   QEArchiveManager::initialise ();
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
   QMutexLocker locker (archiveDataMutex);

   bool result;
   QString effectivePvName;
   int j;
   QStringList pvNames;
   SourceSpec sourceSpec;
   KeyTimeSpec keyTimeSpec;
   int key;
   int bestOverlap;
   QList < int >keys;
   QCaDateTime useStart;
   QCaDateTime useEnd;
   int overlap;
   QString message;

   // Is this PV currently being archived?
   //
   result = pvNameToSourceLookUp.contains (pvName);
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
         result = pvNameToSourceLookUp.contains (effectivePvName);
      } else {
         // Add .VAL and try again.
         //
         effectivePvName = pvName;
         effectivePvName = effectivePvName.append (".VAL");
         result = pvNameToSourceLookUp.contains (effectivePvName);
      }
   }

   if (result) {
      sourceSpec = pvNameToSourceLookUp.value (effectivePvName);

      key = -1;
      bestOverlap = -864000;

      keys = sourceSpec.keyToTimeSpecLookUp.keys ();
      for (j = 0; j < keys.count (); j++) {

         keyTimeSpec = sourceSpec.keyToTimeSpecLookUp.value (keys.value (j));

         useStart = MAX (startTime.toUTC (), keyTimeSpec.startTime);
         useEnd = MIN (endTime.toUTC (), keyTimeSpec.endTime);

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

         // The interface signals return data to the valuesResponse slot in the QEArchiveManager
         // object which (using supplied context) emits QEArchiveAccess setArchiveData signal on behalf
         // of this object.
         //
         sourceSpec.interface->valuesRequest (context, key, startTime, endTime, count, how, pvNames, element);

      } else {
         message = "Archive Manager: PV ";
         message.append (pvName);
         message.append (" has no matching time overlaps.");
         this->sendMessage (message, message_types (MESSAGE_TYPE_WARNING));
      }

   } else {
      message = "Archive Manager: PV ";
      message.append (pvName);
      message.append (" not found in archive.");
      this->sendMessage (message, message_types (MESSAGE_TYPE_WARNING));
   }

   return result;
}

//------------------------------------------------------------------------------
// static functions
//
bool QEArchiveAccess::isReady ()
{
   return allArchivesRead;
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
   return pvNameToSourceLookUp.count ();
}

//------------------------------------------------------------------------------
//
void QEArchiveAccess::resendStatus ()
{
   if (singleton) {
      singleton->resendStatus ();
   }
}

//------------------------------------------------------------------------------
//
void QEArchiveAccess::rxArchiveStatus (const QEArchiveAccess::StatusList& stringList)
{
   // Just re-broadcast status signal.
   //
   emit this->archiveStatus (stringList);
}

// end
