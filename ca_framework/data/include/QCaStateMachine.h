/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/include/QCaStateMachine.h $
 * $Revision: #3 $
 * $DateTime: 2009/07/22 13:02:25 $
 * Last checked in by: $Author: owena $
 */

/*!
  \class QCaStateMachine
  \version $Revision: #3 $
  \date $DateTime: 2009/07/22 13:02:25 $
  \author anthony.owen
  \brief Statemachine architecture.
 */

/* Copyright (c) 2009 Australian Synchrotron
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * Licence as published by the Free Software Foundation; either
 * version 2.1 of the Licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public Licence for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * Licence along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Contact details:
 * anthony.owen@synchrotron.org.au
 * 800 Blackburn Road, Clayton, Victoria 3168, Australia.
 *
 */

#ifndef QCASTATEMACHINE_H_
#define QCASTATEMACHINE_H_

#include <QtDebug>
#include <QMutex>

class StateMachineTemplate {
  public:
    int currentState;
    int requestState;
    virtual bool process( int requestedState ) = 0;
};

namespace qcastatemachine {

  enum connection_states { DISCONNECTED, CONNECTED, CONNECT_FAIL, CONNECTION_EXPIRED };
  enum subscription_states { UNSUBSCRIBED, SUBSCRIBED, SUBSCRIBED_READ, SUBSCRIBE_SUCCESS, SUBSCRIBE_FAIL };
  enum read_states { READ_IDLE, READING, READING_FAIL };
  enum writing_states { WRITE_IDLE, WRITING, WRITING_FAIL };

  class QCaStateMachine : public StateMachineTemplate {
    public:
      QCaStateMachine( void *parent );
      //virtual ~QCaStateMachine();
      QMutex lock;
      bool pending;
      bool active;
      bool expired;
      void *myWorker;
      virtual bool process( int requestedState ) = 0;
  };

  class ConnectionQCaStateMachine : public QCaStateMachine {
    public:
      ConnectionQCaStateMachine( void *parent );
      ~ConnectionQCaStateMachine();
      bool process( int requestedState );
  };

  class SubscriptionQCaStateMachine : public QCaStateMachine {
    public:
      SubscriptionQCaStateMachine( void *parent );
      ~SubscriptionQCaStateMachine();
      bool process( int requestedState );
  };

  class ReadQCaStateMachine : public QCaStateMachine {
    public:
      ReadQCaStateMachine( void *parent );
      ~ReadQCaStateMachine();
      bool process( int requestedState );
  };

  class WriteQCaStateMachine : public QCaStateMachine {
    public:
      WriteQCaStateMachine( void *parent );
      ~WriteQCaStateMachine();
      bool process( int requestedState );
  };

}

#endif /// QCASTATEMACHINE_H_
