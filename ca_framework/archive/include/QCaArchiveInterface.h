/*  QCaArchiveInterface.h
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

#ifndef QARCHIVE_INTERFACE_H
#define QARCHIVE_INTERFACE_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVector>
#include <QList>
#include <QStringList>
#include <QUrl>

#include <alarm.h>

#include <QCaDataPoint.h>
#include <QCaDateTime.h>
#include <QCaAlarmInfo.h>

#include <maiaXmlRpcClient.h>

/*!
 *  This class provides a thin wrapper around the maiaXmlRpcClient specifically
 *  for handling the EPICS Channel Access Archive XML RPC procedure calls.
 *  While functionaly thin, it does provide the conversion between the types
 *  used by the XmlRpcClient (nested structure of QVarients) the types specific
 *  archive EpicsQt, e.g. time to/from QCaDateTime and QCaDataPoint.
 *
 *  This class uses the libMaia client written by
 *  Sebastian Wiedenroth <wiedi@frubar.net> and Karl Glatz.
 */
class QCaArchiveInterface : public QObject {
   Q_OBJECT

public:
   // Extends the EPICS alarm severity with archive sepcials
   //
   enum archiveAlarmSeverity {
       archSevNone       = epicsSevNone,
       archSevMinor      = epicsSevMinor,
       archSevMajor      = epicsSevMajor,
       archSevInvalid    = epicsSevInvalid,
       // From RawValue.h (arch-2.9.2)
       archSevEstRepeat  = 0x0f80,
       archSevDisconnect = 0x0f40,
       archSevStopped    = 0x0f20,    // aka Archive_Off
       archSevRepeat     = 0x0f10,
       archSevDisabled   = 0x0f08
   };

   static QString alarmSeverityName (enum archiveAlarmSeverity severity);

   enum How {
      Raw = 0,
      SpreadSheet,
      Averaged,
      PlotBinning,
      Linear
   };

   // Available archives for given host/port/end_point - key is most important.
   //
   struct Archive {
      int key;
      QString name;
      QString path;
   };

   // Available PV names (matching regular expression pattern) for
   // a given archive together with firts/last available times.
   //
   struct PVName {
      QString pvName;
      QCaDateTime startTime;
      QCaDateTime endTime;
   };

   // Returned values for a single channel.
   //
   struct ResponseValues {
      // meta data
      QString pvName;
      double displayLow;
      double displayHigh;
      int precision;
      QString units;
      unsigned int elementCount;  // number of elements per the channel value.
      // plus actual data
      QCaDataPointList dataPoints;
   };


   //---------------------------------------------------------------------------
   //
   QCaArchiveInterface (QUrl url, QObject *parent = 0);
   ~QCaArchiveInterface ();

   void setUrl (QUrl url);
   QUrl getUrl ();

   // Each of the xxxxRequest functions result in a xxxxResponse signal
   // being emitted. In each case the given userData is not used by the
   // QCaArchiveInterface class per se other it is returned in the signal
   // to provide the caller with signal context.
   //
   void infoRequest (QObject *userData);

   void archivesRequest (QObject *userData);

   void namesRequest (QObject *userData, const int key, QString pattern = ".*");

   /* The requested_element parameter specfies the (waveform) array element required.
    * This parameter applies to all the PVs requested. If different array elements
    * are needed for different PVs the separate calls to valuesRequest are required.
    * Note: element numbers start from 0. The default default value of 0 is applies
    * to scalar PVs.
    */
   void valuesRequest (QObject *userData,
                       const int key,
                       const QCaDateTime startTime,
                       const QCaDateTime endTime,
                       const int count, 
                       const How how,
                       const QStringList pvNames,
                       const unsigned int requested_element = 0);

signals:
   // The boolean (2nd) parameter is a wasSuccessfull parameter, i.e. when true
   // this indicates a successfull response, and when false indicates a fault
   // condition. For the later case, the actual value parameters are undefined.
   //
   void infoResponse     (const QObject *, const bool, const int, const QString&);
   void archivesResponse (const QObject *, const bool, const QList<QCaArchiveInterface::Archive>&);
   void pvNamesResponse  (const QObject *, const bool, const QList<QCaArchiveInterface::PVName>&);
   void valuesResponse   (const QObject *, const bool, const QList<QCaArchiveInterface::ResponseValues>&);

private:
   friend class QCaArchiveInterfaceAgent;

   enum Methods {
      Information,
      Archives,
      Names,
      Values
   };

   struct Context {
      QCaArchiveInterface::Methods method;
      QObject *userData;
      unsigned int requested_element;
   };

   enum MetaType {
      mtEnumeration = 0,
      mtNumeric = 1
   };

   enum DataType {
      dtString = 0,
      dtEnumeration = 1,
      dtInteger = 2,
      dtDouble = 3
   };

   QUrl mUrl;
   MaiaXmlRpcClient *client;

   static QCaDateTime convertArchiveToEpics (const int seconds, const int nanoSecs);
   static void convertEpicsToArchive (const QCaDateTime datetime, int& seconds, int& nanoSecs);

   void processInfo     (const QObject *userData, QVariant & response);
   void processArchives (const QObject *userData, QVariant & response);
   void processPvNames  (const QObject *userData, QVariant & response);
   void processValues   (const QObject *userData, QVariant & response, const unsigned int requested_element);

   void processOnePoint (const DataType dtype,
                         QMap<QString, QVariant> value,
                         const unsigned int requested_element,
                         QCaDataPoint & datum);

   void processOnePV (QMap<QString, QVariant> map,
                      const unsigned int requested_element,
                      struct ResponseValues& item);

private slots:
   // Used by intermediary QCaArchiveInterfaceAgent
   // Note need fully qualified QCaArchiveInterface::Context in order to match signals.
   //
   void xmlRpcResponse (const QCaArchiveInterface::Context & context, QVariant & response);
   void xmlRpcFault    (const QCaArchiveInterface::Context & context, int error, const QString & response);
};


// Essentially a private class. It provides a means to add context (method and
// original user data) to the signal callbacks from the MaiaXmlRpcClient object.
//
// The MaiaXmlRpcClient seems to be asynchronous, thus this agent class can be
// passive. If we change underlying XML RPC library then the agent class can be
// modified to inherit from QThread to provide the asynchronicity if needs be.
//
class QCaArchiveInterfaceAgent : public QObject {
   Q_OBJECT

private:
   friend class QCaArchiveInterface;

   QCaArchiveInterfaceAgent (MaiaXmlRpcClient *clientIn,
                             QCaArchiveInterface *parent);

   QNetworkReply* call (QCaArchiveInterface::Context & contextIn,
                        QString procedure,
                        QList<QVariant> args);

   MaiaXmlRpcClient *client;
   QCaArchiveInterface::Context context;

signals:
   void xmlRpcResponse (const QCaArchiveInterface::Context &, QVariant &);
   void xmlRpcFault    (const QCaArchiveInterface::Context &, int, const QString &);


private slots:
   // from maia xml_rpc client
   //
   void xmlRpcResponse (QVariant & response);
   void xmlRpcFault    (int error, const QString & response);
};

#endif // QARCHIVE_INTERFACE_H
