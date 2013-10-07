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
#include <QMap>
#include <QList>
#include <QMutex>
#include <QRegExp>
#include <QThread>
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
// NOTE: We use a map here as we want sorted keys.
//
typedef QMap <QString, SourceSpec> PVNameToSourceSpecLookUp;


//------------------------------------------------------------------------------
//
class QEArchiveThread : public QThread {
public:
   void run () { exec (); }
};

//==============================================================================
// Local Data
//==============================================================================
// We declare these items here as opposed as static members of the class because
// the later made all the EPICS plugin widgets "go away" in designer.
// I think the are issues when QObjects declared in header files.
//

// Allows only one object to be effectively created. Second and subsequent object
// do nothing, except waste space.
//
static QMutex *singletonMutex = new QMutex ();
static QEArchiveManager *singletonManager = NULL;
static QEArchiveThread *singletonThread = NULL;

static QMutex *archiveDataMutex = new QMutex ();
static ArchiveInterfaceLists archiveInterfaceList;
static PVNameToSourceSpecLookUp pvNameToSourceLookUp;

static bool allArchivesRead = false;
static int numberArchivesRead = 0;
static bool environmentErrorReported = false;


//==============================================================================
// QEArchiveManager Class Methods
//==============================================================================
//
//The singleton manager object is an orphan because we move it to singletonThread.
//
QEArchiveManager::QEArchiveManager (const QString& archivesIn,
                                    const QString& patternIn) : QObject (NULL)
{
   this->archives = archivesIn;
   this->pattern = patternIn;

   // Hard-coded message Id.
   //
   this->setSourceId (9001);

   // Register status message types.
   //
   qRegisterMetaType<QEArchiveAccess::States> ("QEArchiveAccess::States");
   qRegisterMetaType<QEArchiveAccess::Status> ("QEArchiveAccess::Status");
   qRegisterMetaType<QEArchiveAccess::StatusList> ("QEArchiveAccess::StatusList");

   qRegisterMetaType<QEArchiveAccess::PVDataRequests> ("QEArchiveAccess::PVDataRequests");
   qRegisterMetaType<QEArchiveAccess::PVDataResponses> ("QEArchiveAccess::PVDataResponses");
}

//------------------------------------------------------------------------------
//
void QEArchiveManager::setup ()
{
   QStringList archiveList;
   QString item;
   QUrl url;
   ArchiveInterfacePlus *interface;
   int j;

   // First check we are the one and only ....
   // Belts 'n' braces sanity check.
   //
   if (this != singletonManager) {
      // This is NOT the singleton object.
      //
      this->sendMessage ("QEArchiveManager::initialise - attempt to use non-singleton object",
                          message_types (MESSAGE_TYPE_ERROR));
      return;
   }

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

      interface = new ArchiveInterfacePlus (url, this);
      archiveInterfaceList.append (interface);

      connect (interface, SIGNAL (archivesResponse (const QObject*, const bool, const QEArchiveInterface::ArchiveList &)),
               this,      SLOT   (archivesResponse (const QObject*, const bool, const QEArchiveInterface::ArchiveList &)));

      connect (interface, SIGNAL (pvNamesResponse  (const QObject*, const bool, const QEArchiveInterface::PVNameList &)),
               this,      SLOT   (pvNamesResponse  (const QObject*, const bool, const QEArchiveInterface::PVNameList &)));

      connect (interface, SIGNAL (valuesResponse   (const QObject*, const bool, const QEArchiveInterface::ResponseValueList &)),
               this,      SLOT   (valuesResponse   (const QObject*, const bool, const QEArchiveInterface::ResponseValueList &)));

      interface->archivesRequest (interface);
      interface->state = QEArchiveAccess::Updating;

      this->sendMessage (QString ("requesting PV name info from ").append (interface->getName ()),
                         message_types (MESSAGE_TYPE_INFO));
   }

   this->resendStatus ();
}

//------------------------------------------------------------------------------
// slot
void QEArchiveManager::started ()
{
   this->setup ();
}

//------------------------------------------------------------------------------
// static
void QEArchiveManager::initialise (const QString& archivesIn, const QString& patternIn)
{
   QMutexLocker locker (singletonMutex);

   if (!singletonManager) {
      singletonManager = new QEArchiveManager (archivesIn, patternIn);
      singletonThread = new QEArchiveThread ();

      // Remome the singletonManager to belong to thread, connect the signal and start it.
      //
      singletonManager->moveToThread (singletonThread);

      QObject::connect (singletonThread, SIGNAL (started ()),
                        singletonManager, SLOT  (started ()));

      singletonThread->start ();
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

   emit this->archiveStatusResponse (statusList);
}

//------------------------------------------------------------------------------
//
void QEArchiveManager::archiveStatusRequest () {
   QMutexLocker locker (archiveDataMutex);

   this->resendStatus ();
}

//==============================================================================
//
class NamesResponseContext:public QObject {
public:
   ArchiveInterfacePlus* interface;
   QEArchiveInterface::Archive archive;
   int instance;

   // constructor
   NamesResponseContext (ArchiveInterfacePlus * interfaceIn,
                         ArchiveInterfacePlus::Archive archiveIn,
                         int i)
   {
      this->interface = interfaceIn;
      this->archive = archiveIn;
      this->instance = i;
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
         context = new NamesResponseContext (interface, archive, j + 1);
         interface->namesRequest (context, archive.key, pattern);
      }

   } else {
       this->sendMessage (QString ("request failure from ").append (interface->getName ()),
                          message_types (MESSAGE_TYPE_ERROR));

       interface->state = QEArchiveAccess::Error;

   }

   singletonManager->resendStatus ();
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

   if (context->instance == interface->available) {
      message = "PV name retrival from ";
      message.append (context->interface->getName ());
      message.append (" complete");
      this->sendMessage (message);

      if (interface->read == interface->available) {
         interface->state = QEArchiveAccess::Complete;

      } else {
         interface->state = QEArchiveAccess::InComplete;

      }

      numberArchivesRead++;
      allArchivesRead = (numberArchivesRead = archiveInterfaceList.count ());
   }

   delete context;
   singletonManager->resendStatus ();
}


//==============================================================================
//
class ValuesResponseContext : public QObject {
public:
   const QEArchiveAccess* archiveAccess;
   QObject* userData;

   // constructor
   ValuesResponseContext (const QEArchiveAccess* archiveAccessIn,
                          QObject* userDataIn)
   {
      this->archiveAccess = archiveAccessIn;
      this->userData = userDataIn;
   }
};


//------------------------------------------------------------------------------
//
void QEArchiveManager::readArchiveRequest (const QEArchiveAccess* archiveAccess,
                                           const QEArchiveAccess::PVDataRequests& request)
{
   QMutexLocker locker (archiveDataMutex);

   QString effectivePvName;
   bool isKnownPVName;
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
   effectivePvName = request.pvName;

   isKnownPVName = pvNameToSourceLookUp.contains (effectivePvName);
   if (!isKnownPVName) {
      // No - the PV 'as is' is not archived.
      // If user has requested XXXXXX.VAL, check if XXXXXX is archived.
      // Similarly, if user requested YYYYYY, check if YYYYYY.VAL archived.
      //
      if (effectivePvName.right (4) == ".VAL") {
         // Remove the .VAL field and try again.
         //
         effectivePvName.chop (4);
      } else {
         // Add .VAL and try again.
         //
         effectivePvName.append (".VAL");
      }
      isKnownPVName = pvNameToSourceLookUp.contains (effectivePvName);
   }

   if (isKnownPVName) {
      sourceSpec = pvNameToSourceLookUp.value (effectivePvName);

      key = -1;
      bestOverlap = -864000;

      keys = sourceSpec.keyToTimeSpecLookUp.keys ();
      for (j = 0; j < keys.count (); j++) {

         keyTimeSpec = sourceSpec.keyToTimeSpecLookUp.value (keys.value (j));

         useStart = MAX (request.startTime.toUTC (), keyTimeSpec.startTime);
         useEnd = MIN (request.endTime.toUTC (), keyTimeSpec.endTime);

         // We don't worry about calculating the overlap to an accuracy
         // of any one than one second.
         //
         overlap = useStart.secsTo (useEnd);
         if (bestOverlap < overlap) {
            bestOverlap = overlap;
            key = keyTimeSpec.key;
         }
      }

      isKnownPVName = (key >= 0);
      if (isKnownPVName) {
         pvNames.append (effectivePvName);

         // Create a reasponse context.
         //
         ValuesResponseContext* context = new  ValuesResponseContext (archiveAccess, request.userData);

         // The interface signals return data to the valuesResponse slot in the QEArchiveManager
         // object which (using supplied context) emits QEArchiveAccess setArchiveData signal on behalf
         // of this object.
         //
         sourceSpec.interface->valuesRequest (context, key,
                                              request.startTime, request.endTime,
                                              request.count, request.how,
                                              pvNames, request.element);

      } else {
         message = "Archive Manager: PV ";
         message.append (request.pvName);
         message.append (" has no matching time overlaps.");
         this->sendMessage (message, message_types (MESSAGE_TYPE_WARNING));
      }

   } else {
      message = "Archive Manager: PV ";
      message.append (request.pvName);
      message.append (" not found in archive.");
      this->sendMessage (message, message_types (MESSAGE_TYPE_WARNING));
   }
}


//------------------------------------------------------------------------------
//
void QEArchiveManager::valuesResponse (const QObject* userData,
                                       const bool isSuccess,
                                       const QEArchiveInterface::ResponseValueList& valuesList)
{
   const ValuesResponseContext* context = dynamic_cast <ValuesResponseContext*> ((QObject*) userData);

   if (context) {
      QEArchiveAccess::PVDataResponses response;

      response.userData = context->userData;
      response.isSuccess = ((isSuccess) && (valuesList.count () == 1));
      if (response.isSuccess) {
         response.pointsList = valuesList.value (0).dataPoints;
      }

      emit this->readArchiveResponse (context->archiveAccess, response);

      delete context;
   }
}


//==============================================================================
//
QEArchiveAccess::QEArchiveAccess (QObject * parent) : QObject (parent)
{
   // Construct and initialise singleton QEArchiveManager object if needs be.
   //
   QEArchiveManager::initialise ();

   // Connect status request response signals.
   //
   QObject::connect (this,             SIGNAL (archiveStatusRequest ()),
                     singletonManager, SLOT   (archiveStatusRequest ()));

   QObject::connect (singletonManager, SIGNAL (archiveStatusResponse (const QEArchiveAccess::StatusList&)),
                     this,             SLOT   (archiveStatusResponse (const QEArchiveAccess::StatusList&)));


   // Connect data request response signals.
   //
   QObject::connect (this,             SIGNAL (readArchiveRequest  (const QEArchiveAccess*, const QEArchiveAccess::PVDataRequests&)),
                     singletonManager, SLOT   (readArchiveRequest  (const QEArchiveAccess*, const QEArchiveAccess::PVDataRequests&)));

   QObject::connect (singletonManager, SIGNAL (readArchiveResponse (const QEArchiveAccess*, const QEArchiveAccess::PVDataResponses&)),
                     this,             SLOT   (readArchiveResponse (const QEArchiveAccess*, const QEArchiveAccess::PVDataResponses&)));

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
//
void QEArchiveAccess::resendStatus ()
{
   if (singletonManager) {
      emit this->archiveStatusRequest ();
   }
}

//------------------------------------------------------------------------------
//
void QEArchiveAccess::archiveStatusResponse (const QEArchiveAccess::StatusList& stringList)
{
   // Just re-broadcast status signal - no filtering.
   //
   emit this->archiveStatus (stringList);
}

//------------------------------------------------------------------------------
//
void QEArchiveAccess::readArchive (QObject* userData,
                                   const QString pvName,
                                   const QCaDateTime startTime,
                                   const QCaDateTime endTime,
                                   const int count,
                                   const QEArchiveInterface::How how,
                                   const unsigned int element)
{
   QEArchiveAccess::PVDataRequests request;

   // Set up request - aggragate parameters
   //
   request.userData = userData;
   request.pvName = pvName;
   request.startTime = startTime;
   request.endTime = endTime;
   request.count = count;
   request.how = how;
   request.element = element;

   // and hand-ball off to archiver manager thread.
   //
   emit this->readArchiveRequest (this, request);
}

//------------------------------------------------------------------------------
//
void QEArchiveAccess::readArchiveResponse (const QEArchiveAccess* archiveAccess,
                                           const QEArchiveAccess::PVDataResponses& response)
{
   // Filter and re-broadcast status signal.
   //
   if (archiveAccess == this) {
      emit this->setArchiveData (response.userData, response.isSuccess, response.pointsList);
   }
}


//------------------------------------------------------------------------------
// static functions
//------------------------------------------------------------------------------
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
   QMutexLocker locker (singletonMutex);

   return singletonManager ? singletonManager->pattern : "";
}

//------------------------------------------------------------------------------
//
int QEArchiveAccess::getNumberPVs ()
{
   QMutexLocker locker (archiveDataMutex);

   return pvNameToSourceLookUp.count ();
}

//------------------------------------------------------------------------------
//
QStringList QEArchiveAccess::getMatchingPVnames (const QString& pattern)
{
   QRegExp regExp (pattern, Qt::CaseSensitive, QRegExp::RegExp);
   QStringList existList;
   QStringList matchList;
   int n;
   int j;

   {
      QMutexLocker locker (archiveDataMutex);
      existList = pvNameToSourceLookUp.keys ();
   }

   n = existList.count ();
   for (j = 0; j < n; j++) {
      QString pvName = existList.value (j);

      if (regExp.exactMatch (pvName)) {
         matchList.append (pvName);
      }
   }

   return matchList;
}

// end
