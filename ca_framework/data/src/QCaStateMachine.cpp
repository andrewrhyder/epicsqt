/*! 
  \class QCaStateMachine
  \version $Revision: #5 $
  \date $DateTime: 2009/07/29 14:20:04 $
  \author anthony.owen
  \brief Statemachine architecture.
 */
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
 *    Anthony Owen
 *  Contact details:
 *    anthony.owen@gmail.com
 */

#include <QCaStateMachine.h>
#include <QCaObject.h>

using namespace qcastatemachine;

/*!
    Startup default attributes.
*/
QCaStateMachine::QCaStateMachine( void *parent ) {
    myWorker = parent;
    pending = false;
    active = false;
    expired = false;
}

/*!
    Startup for the Connection statemachine.
*/
ConnectionQCaStateMachine::ConnectionQCaStateMachine( void *parent ) : QCaStateMachine( parent ) {
    currentState = DISCONNECTED;
    requestState = DISCONNECTED;
}

/*!
    Process the Connection statemachine.
*/
bool ConnectionQCaStateMachine::process( int requestedState) {
    //qDebug() << "ConnectionQCaStateMachine::process() " << requestedState;
    requestState = requestedState;
    QMutexLocker locker( &lock );
    qcaobject::QCaObject *worker = ( qcaobject::QCaObject* )( myWorker );
    switch( currentState ) {
        case DISCONNECTED :
        {
            switch( requestState) {
                case CONNECTED :
                {
                    if( active == false && pending == false ) {
                        //qDebug() << "connection: DISCONNECTED -> CONNECTED (REQUEST)";
                        if( worker->createChannel() ) //??? error not handled
                        {
                            pending = true;
                            worker->startConnectionTimer();
                        }
                    }
                    if( active == true ) {
                        //qDebug() << "connection: DISCONNECTED -> CONNECTED (CALLBACK)";
                        pending = false;
                        worker->stopConnectionTimer();
                        currentState = CONNECTED;
                    }
                    break;
                }
                case CONNECTION_EXPIRED :
                {
                    if( pending == true && expired == true ) {
                        //qDebug() << "connection: DISCONNECTED -> CONNECTION_EXPIRED";
                        pending = false;
                        expired = false;
                        worker->stopConnectionTimer();
                        worker->deleteChannel();
                    }
                    break;
                }
            } 
            break;
        }
        case CONNECTED :
        {
            switch( requestState) {
                case DISCONNECTED :
                {
                    //qDebug() << "connection: CONNECTED -> DISCONNECTED (REQUEST)";
                    if( active == true ) {
                        pending = false;
                        active = false;
                        expired = false;
                        worker->deleteChannel();
                        currentState = DISCONNECTED;
                    }
                    break;
                }
            }
            break;
        }
        case CONNECT_FAIL :
        {
            break;
        }
    }
    return true;
}

/*!
    Startup for the Subscription statemachine.
*/
SubscriptionQCaStateMachine::SubscriptionQCaStateMachine( void *parent ) : QCaStateMachine( parent ) {
    currentState = UNSUBSCRIBED;
    requestState = UNSUBSCRIBED;
}

/*!
    Process the Subscription statemachine.
*/
bool SubscriptionQCaStateMachine::process( int requestedState ) {
    //qDebug() << "SubscriptionQCaStateMachine::process() " << requestedState;
    requestState = requestedState;
    qcaobject::QCaObject *worker = ( qcaobject::QCaObject* )( myWorker );
    QMutexLocker locker( &lock );
    switch( currentState ) {
        case UNSUBSCRIBED :
        {
            switch( requestState) {
                case SUBSCRIBED :
                {
                    if( worker->isChannelConnected() && active == false && pending == false ) {
                        pending = true;
                        worker->createSubscription(); //??? error not handled
                    }
                    if( active == true ) {
                        pending = false;
                        expired = false;
                        currentState = SUBSCRIBED;
                    }
                    break;
                }
            }
            break;
        }
        case SUBSCRIBED :
        {
            switch( requestState) {
                case SUBSCRIBED_READ :
                {
                    currentState = SUBSCRIBED;
                    break;
                }
            }
            break;
        }
        case SUBSCRIBED_READ :
        {
            currentState = SUBSCRIBED;
            break;
        }
        case SUBSCRIBE_FAIL :
        {
            currentState = UNSUBSCRIBED;
            break;
        }
    }
    return true;
}

/*!
    Startup for the Reading statemachine.
*/
ReadQCaStateMachine::ReadQCaStateMachine( void *parent ) : QCaStateMachine( parent ) {
    currentState = READ_IDLE;
    requestState = READ_IDLE;
}

/*!
    Process the Reading statemachine.
*/
bool ReadQCaStateMachine::process( int requestedState ) {
    //qDebug() << "ReadQCaStateMachine::process() " << requestedState;
    requestState = requestedState;
    qcaobject::QCaObject *worker = ( qcaobject::QCaObject* )( myWorker );
    QMutexLocker locker( &lock );
    switch( currentState ) {
        case READ_IDLE :
        {
            switch( requestState ) {
                case READING :
                {
                    if( worker->isChannelConnected() && active == false ) {
                        active = true;
                        worker->getChannel(); //??? error not handled
                        currentState = READING;
                    }
                    break;
                }
            }
            break;
        }
        case READING :
        {
            switch( requestState ) {
                case READ_IDLE :
                {
                    if( active == false ) {
                        currentState = READ_IDLE;
                    }
                    break;
                }
                case READING_FAIL :
                {
                    currentState = READ_IDLE;
                    break;
                }
            }
            break;
        }
    }
    return true;
}

/*!
    Startup for the Writing statemachine.
*/
WriteQCaStateMachine::WriteQCaStateMachine( void *parent ) : QCaStateMachine( parent ) {
    currentState = WRITE_IDLE;
    requestState = WRITE_IDLE;
}

/*!
    Process the Writing statemachine.
*/
bool WriteQCaStateMachine::process( int requestedState ) {
    //qDebug() << "WriteQCaStateMachine::process() " << requestedState;
    requestState = requestedState;
    QMutexLocker locker( &lock );
    qcaobject::QCaObject *worker = ( qcaobject::QCaObject* )( myWorker );
    switch( currentState ) {
        case WRITE_IDLE :
        {
            switch( requestState ) {
                case WRITING :
                {
                    if( worker->isChannelConnected() && active == false ) {
                        active = true;
                        if( worker->putChannel() ) //??? error not handled
                            currentState = WRITING;
                        else
                            currentState = WRITE_IDLE;
                    }
                    break;
                }
            }
            break;
        }
        case WRITING :
        {
            switch( requestState ) {
                case WRITE_IDLE :
                {
                    if( active == false ) {
                        currentState = WRITE_IDLE;
                    }
                    break;
                }
                case WRITING_FAIL :
                {
                    currentState = WRITE_IDLE;
                    break;
                }
            }
            break;
        }
    }
    return true;
}
