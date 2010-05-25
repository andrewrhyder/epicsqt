/*! 
  \class QCaObject
  \version $Revision: #7 $
  \date $DateTime: 2009/07/29 14:20:04 $
  \author anthony.owen
  \brief Provides channel access to QT.
 */
/*
 *  This file is part of the EPICS QT Framework.
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
 *    Anthony Owen
 *  Contact details:
 *    anthony.owen@gmail.com
 */

#ifndef QCAOBJECT_H
#define QCAOBJECT_H

#include <QObject>
#include <QMutex>
#include <QList>
#include <QTimer>
#include <QString>
#include <QStringList>

#include <CaObject.h>
#include <QCaStateMachine.h>
#include <QCaEventFilter.h>
#include <QCaEventUpdate.h>
#include <UserMessage.h>
#include <QCaAlarmInfo.h>
#include <QCaDateTime.h>
#include <QCaConnectionInfo.h>

namespace qcaobject {

  class QCaObject : public QObject, caobject::CaObject {
      Q_OBJECT

    public:
      QCaObject( const QString& recordName, QObject *eventObject );
      QCaObject( const QString& recordName, QObject *eventObject, UserMessage* userMessageIn );
      ~QCaObject();

      bool subscribe();
      bool singleShotRead();

      static void processEventStatic( QCaEventUpdate* dataUpdateEvent );

      bool dataTypeKnown();

      /// State machine access functions
      bool createChannel();
      void deleteChannel();
      bool createSubscription();
      bool getChannel();
      bool putChannel();
      bool isChannelConnected();
      void startConnectionTimer();
      void stopConnectionTimer();

      void setUserMessage( UserMessage* userMessageIn );

      // Get database information relating to the variable
      QString getEgu();
      QStringList getEnumerations();
      unsigned int getPrecision();

    signals:
      void dataChanged( const QVariant& value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp );
      void connectionChanged( QCaConnectionInfo& connectionInfo );

    public slots:
      bool writeData( const QVariant& value );

    protected:
      generic::generic_types getDataType();

    private:
      void initialise( const QString& newRecordName, QObject *newEventHandler, UserMessage* userMessageIn );

      long lastEventChannelState; /// Channel state from most recent update event. This is actually of type caconnection::channel_states
      long lastEventLinkState;    /// Link state from most recent update event. This is actually of type aconnection::link_states

      QString recordName;
      QVariant writingData;

      QObject* eventHandler;                  /// Event handler
      static QMutex outstandingEventsLock;    /// Used to protect access to outstandingEvents list
      static QCaEventFilter eventFilter;      /// Event filter to filter in own events
      QList<QCaEventItem> pendingEvents;      /// List of pending data events
      QTimer setChannelTimer;

      qcastatemachine::ConnectionQCaStateMachine *connectionMachine;
      qcastatemachine::SubscriptionQCaStateMachine *subscriptionMachine;
      qcastatemachine::ReadQCaStateMachine *readMachine;
      qcastatemachine::WriteQCaStateMachine *writeMachine;

      void signalCallback( caobject::callback_reasons reason ); /// CA callback function processed within an EPICS thread
      void processEvent( QCaEventUpdate* dataUpdateEvent );     /// Continue processing CA callback but within the contect of a Qt event
      void processData( void* newData );                        /// Process new CA data. newData is actually of type carecord::CaRecord*

      UserMessage* userMessage;

      // Database information relating to the variable
      QString egu;
      int precision;
      QStringList enumerations;
      bool isStatField;


    private slots:
      void setChannelExpired();
  };

}

#endif /// QCAOBJECT_H
