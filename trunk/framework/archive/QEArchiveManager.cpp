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
// Archive class type provides key (and name and path).
//
class KeyTimeSpec:public QEArchiveInterface::Archive {
public:
   QCaDateTime startTime;
   QCaDateTime endTime;
};

//------------------------------------------------------------------------------
// Each PV may have one or more archives available on the same
// host, e.g. a short term archive and a long term archive.
// However we expect all archives for a particlar PV to be co-hosted.
//
class SourceSpec:public QHash <int, KeyTimeSpec> {
public:
   QEArchiveInterface * interface;
};


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
static QMutex *initialiseMutex = new QMutex ();


static QMutex *archiveDataMutex = new QMutex ();

static QList <QEArchiveInterface *>archiveInterfaceList;
static QString pattern;
static QHash <QString, SourceSpec> pvNameHash;
static bool allArchivesRead = false;
static int numberArchivesRead = 0;
static bool environmentErrorReported = false;



//==============================================================================
// QEArchiveManager Class Methods
//==============================================================================
//
//
QEArchiveManager::QEArchiveManager () :
   QObject (NULL)  //The singleton manager object is an orphan.
{
   {
      QMutexLocker locker (singletonMutex);

      if (singleton == NULL) {
         // This is the first object - we are the singleton.
         //
         singleton = this;
      }
   }

   if (this != singleton) {
      // This is NOT the singleton object.
      // There can only be one.
      //
      this->sendMessage ("QEArchiveManager::QEArchiveManager - duplicate object.",
                         message_types (MESSAGE_TYPE_ERROR));
      return;
   }

   // Hard-coded message Id.
   this->setSourceId (9001);

   this->isInitialised = false;
}

//------------------------------------------------------------------------------
//
void QEArchiveManager::initialise (QString archives, QString patternIn)
{
   QStringList archiveList;
   QString item;
   QUrl url;
   QEArchiveInterface *interface;
   int j;

   // First check we are the one and only ....
   //
   if (this != singleton) {
      // This is NOT the singleton object.
      this->sendMessage ("QEArchiveManager::initialise - attempt to use non-singleton object",
                          message_types (MESSAGE_TYPE_ERROR));
      return;
   }

   // Have we already been initialised?
   // Use mutex to ensure thread safe.
   {
      QMutexLocker locker (initialiseMutex);
      if (this->isInitialised) {
         return;
      }
      this->isInitialised = true;
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

      interface = new QEArchiveInterface (url, singleton);
      archiveInterfaceList.append (interface);

      connect (interface, SIGNAL (archivesResponse (const QObject *, const bool, const QEArchiveInterface::ArchiveList &)),
               this,      SLOT   (archivesResponse (const QObject *, const bool, const QEArchiveInterface::ArchiveList &)));

      connect (interface, SIGNAL (pvNamesResponse (const QObject *, const bool, const QEArchiveInterface::PVNameList &)),
               this,      SLOT   (pvNamesResponse (const QObject *, const bool, const QEArchiveInterface::PVNameList &)));

      connect (interface, SIGNAL (valuesResponse (const QObject *, const bool, const QEArchiveInterface::ResponseValueList &)),
               this,      SLOT   (valuesResponse (const QObject *, const bool, const QEArchiveInterface::ResponseValueList &)));

      interface->archivesRequest (interface);

      this->sendMessage (QString ("requesting PV name info from ").append (interface->getName ()),
                         message_types (MESSAGE_TYPE_INFO));
   }
}


//------------------------------------------------------------------------------
//
void QEArchiveManager::initialise ()
{
   // First check we are the one and only ....
   //
   if (this != singleton) {
      // This is NOT the singleton object.
      this->sendMessage ("QEArchiveManager::initialise - attempt to use non-singleton object",
                          message_types (MESSAGE_TYPE_ERROR));
      return;
   }

   QString archives = getenv ("QE_ARCHIVE_LIST");
   QString pattern = getenv ("QE_ARCHIVE_PATTERN");

   if (archives != "") {
      if (pattern == "") {
         // Pattern environment variable undefined, use "get all" by default.
         //
         pattern = ".*";
      }

      // This is idempotent.
      //
      this->initialise (archives, pattern);

   } else {
      // Has this error already been reported??
      // Not 100% thread safe but not critical either.
      //
      if (!environmentErrorReported) {
         environmentErrorReported = true;
         this->sendMessage ("QE_ARCHIVE_LIST undefined. Required to backfill QEStripChart widgets. Define as space delimited archiver URLs",
                            message_types (MESSAGE_TYPE_WARNING));
      }
   }
}

//------------------------------------------------------------------------------
//
void QEArchiveManager::clear ()
{
   int j;

   // First check we are the one and only ....
   //
   if (this != singleton) {
      // This is NOT the singleton object.
      this->sendMessage ("QEArchiveManager::clear - attempt to use non-singleton object",
                          message_types (MESSAGE_TYPE_ERROR));
      return;
   }


   allArchivesRead = false;
   numberArchivesRead = 0;
   pvNameHash.clear ();

   // Do a deep clear of the archive interface list.
   //
   for (j = 0; j < archiveInterfaceList.count (); j++) {
      delete archiveInterfaceList.value (j);
   }
   archiveInterfaceList.clear ();
}


//==============================================================================
//
class NamesResponseContext:public QObject {
public:
   QEArchiveInterface * interface;
   QEArchiveInterface::Archive archive;
   int instance;
   int number;

   // constructor
   NamesResponseContext (QEArchiveInterface * interfaceIn,
                         QEArchiveInterface::Archive archiveIn,
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

   QEArchiveInterface *interface = (QEArchiveInterface *) userData;
   int count;
   int j;

   if (isSuccess) {

      count = archiveList.count ();
      for (j = 0; j < count; j++) {
         QEArchiveInterface::Archive archive = archiveList.value (j);
         NamesResponseContext *context;

         // Create the callback context.
         //
         context =
             new NamesResponseContext (interface, archive, j + 1, count);
         interface->namesRequest (context, archive.key, pattern);
      }
   } else {
       this->sendMessage (QString ("request failure from ").append (interface->getName ()),
                           message_types (MESSAGE_TYPE_ERROR));
   }
}


//------------------------------------------------------------------------------
//
void QEArchiveManager::pvNamesResponse (const QObject * userData,
                                        const bool isSuccess,
                                        const QEArchiveInterface::PVNameList &pvNameList)
{
   QMutexLocker locker (archiveDataMutex);

   NamesResponseContext *context = (NamesResponseContext *) userData;
   QString message;
   int j;

   if (isSuccess) {
      for (j = 0; j < pvNameList.count (); j++) {
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
   // Do we have a sigleton ojbect yet?
   // The constructor does the safe mutex locking required prior to assigin the
   // object reference to singleton. We check here in order to reduce, although
   // not absolutely remove error messages.
   //
   if (!singleton) {
      new QEArchiveManager ();
   }

   // This function is idempotent.
   //
   singleton->initialise ();
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
void QEArchiveAccess::initialise (QString archives, QString pattern)
{
   // Do we have a sigleton ojbect yet?
   //
   if (!singleton) {
      new QEArchiveManager ();
   }

   // This function is idempotent.
   //
   singleton->initialise (archives, pattern);
}

//------------------------------------------------------------------------------
//
void QEArchiveAccess::initialise ()
{
   // Do we have a sigleton ojbect yet?
   //
   if (!singleton) {
      new QEArchiveManager ();
   }

   // This function is idempotent.
   //
   singleton->initialise ();
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
   return pvNameHash.count ();
}

// end
