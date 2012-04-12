/*
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
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QCABYTEARRAY_H
#define QCABYTEARRAY_H

#include <QtDebug>
#include <QVariant>
#include <QCaObject.h>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QCaByteArray : public qcaobject::QCaObject {
    Q_OBJECT

  public:
    QCaByteArray( QString recordName, QObject *eventObject, unsigned int variableIndexIn );
    QCaByteArray( QString recordName, QObject *eventObject, unsigned int variableIndexIn, UserMessage* userMessageIn );

  signals:
    void byteArrayConnectionChanged( QCaConnectionInfo& connectionInfo, const unsigned int &variableIndex );
    void byteArrayChanged( const QByteArray& value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp, const unsigned int& variableIndex );

  public slots:
    void writeByteArray( const QByteArray& data );

  private:
    void initialise( unsigned int variableIndexIn );
    unsigned int variableIndex;

  private slots:
    void forwardDataChanged( const QByteArray &value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp );
    void forwardConnectionChanged( QCaConnectionInfo& connectionInfo);
};

#endif /// QCABYTEARRAY_H
