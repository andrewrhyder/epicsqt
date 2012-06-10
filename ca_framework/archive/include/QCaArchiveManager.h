/*  QCaArchiveManager.h
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

/*!
 * Archive Manager manages access to the archives, and provides a thick binding
 * around the Archive Interface class. It's main function is to provide a PV Name
 * to interface/archive mapping.
 */

#ifndef QCAARCHIVEMANAGER_H
#define QCAARCHIVEMANAGER_H

#include <QObject>
#include <QString>

#include <QCaDateTime.h>
#include <QCaArchiveInterface.h>

/*! This is a singleton class - the single instance is declared in the .cpp file.
 *  It's only exposed in a header because the Qt framework demand that signals/slots
 *  are in headers.
 */
class QCaArchiveManager : public QObject {
   Q_OBJECT
private:
   /*! This function connects the specified the archive(s). The format of the string is
    *  space separated set of one or more hostname:port/endpoint triplets, e.g.
    *
    *  "CR01ARC01:80/cgi-bin/ArchiveDataServer.cgi CR01ARC02:80/cgi-bin/ArchiveDataServer.cgi"
    *
    *  A leading http:// is neither required nor permitted.
    *
    *  Once connected, it creates a map pf PV names to host/archive key/availalbe times
    *
    *  The pattern parameter can be used to restrict the set of extracted PVs. The same
    *  pattern applies of all archives. The pattern is a regular expression.
    */
   bool initialise (QString archives, QString patternIn = ".*");

   /*! As above, but uses the environment variables QCA_ARCHIVE_LIST and QCA_ARCHIVE_PATTERN.
    *  If QCA_ARCHIVE_PATTERN is undefined then ".*" is used.
    */
   bool initialise ();

   /*! Used to automatically run ::initialise () at elaboration time.
    */
   static const bool inited;

   void clear ();

private slots:
   void archivesResponse (const QObject * userData, const bool isSuccess, const QList<QCaArchiveInterface::Archive>& archiveList);
   void pvNamesResponse  (const QObject * userData, const bool isSuccess, const QList<QCaArchiveInterface::PVName>& pvNameList);
   void valuesResponse   (const QObject * userData, const bool isSuccess, const QList<QCaArchiveInterface::ResponseValues>& valuesList);
};


/*!
 * This class provides user access to the archives.
 * Currently only handles scaler values but can/will be extended to to
 * provide array data retrival.
 */
class QCaArchiveAccess : public QObject {
   Q_OBJECT
public:
   QCaArchiveAccess (QObject * parent = 0);
   ~QCaArchiveAccess ();

   static int getNumberInterfaces ();
   static QString getPattern ();
   static int getNumberPVs ();

   /*! Simple archive request - single scaler PV, or one arbitary element from
    *  a single array PV.  No extended meta data, just values + timestamp + alarm info.
    *  The data, if any, is sent via the setArchiveData signal.
    */
   bool readArchive (QObject * userData,        // provides call back signal context
                     const QString pvName,
                     const QCaDateTime startTime,
                     const QCaDateTime endTime,
                     const int count,
                     const QCaArchiveInterface::How how,
                     const unsigned int element = 0);

signals:
   void setArchiveData (const QObject *, const bool, const QCaDataPointList &);
   friend class QCaArchiveManager;
};

#endif  // QCAARCHIVEMANAGER_H
