/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/api/src/CaConnection.cpp $
 * $Revision: #1 $ 
 * $DateTime: 2009/07/14 15:59:56 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class CaConnection
  \version $Revision: #1 $
  \date $DateTime: 2009/07/14 15:59:56 $
  \author anthony.owen
  \brief Low level wrapper around the EPICS library
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

#include <CaConnection.h>

using namespace caconnection;

/*!
    Initialise the unique ID.
*/
int CaConnection::CA_UNIQUE_CONNECTION_ID = 0;

/*!
    Initialise EPICS library and setup working parent. The pointer to the
    working parent is given to the EPICS thread so that any callbacks that
    occur contain the context of the parent that created this CaConnection.
*/
CaConnection::CaConnection( void* newParent ) {
    parent = newParent;
    initialise();
    reset();
}

/*!
    Shutdown EPICS library and reset internal data to defaults.
*/
CaConnection::~CaConnection() {
    shutdown();
    reset();
}

/*!
    Creates only one EPICS context and registers an exception callback.
    Use hasContext() for feedback.
    Returns: REQUEST_SUCCESSFUL or REQUEST_FAILED
*/
ca_responses CaConnection::establishContext( void (*exceptionHandler)(struct exception_handler_args), void* args ) {
    if( context.activated == false ) {
        if( CA_UNIQUE_CONNECTION_ID <= 1) {
            context.creation = ca_context_create( ca_enable_preemptive_callback );
            context.exception = ca_add_exception_event ( exceptionHandler, args );
        }
        context.activated = true;
        switch( context.creation )  {
            case ECA_NORMAL :
                return REQUEST_SUCCESSFUL;
            break;
            default :
                return REQUEST_FAILED;
            break;
        }
    } else {
        return REQUEST_FAILED;
    }
}

/*!
    Establishes client side channel and registers a connection state change
    callback.
    Use activeChannel() for feedback.
    Returns: REQUEST_SUCCESSFUL or REQUEST_FAILED
*/
ca_responses CaConnection::establishChannel( void (*connectionHandler)(struct connection_handler_args), std::string channelName ) {
    if( context.activated == true && channel.activated == false ) {
        channel.creation = ca_create_channel( channelName.c_str(), connectionHandler, this, CA_PRIORITY_DEFAULT, &channel.id );
        ca_pend_io( link.searchTimeout );
        channel.activated = true;
        switch( channel.creation ) {
            case ECA_NORMAL :
                return REQUEST_SUCCESSFUL;
            break;
            default :
                return REQUEST_FAILED;
            break;
        }
    } else {
        return REQUEST_FAILED;
    }
}

/*!
    Subscribes to the established channel and registers for data callbacks
    Use isSubscribed() for feedback.
*/
ca_responses CaConnection::establishSubscription( void (*subscriptionHandler)(struct event_handler_args), void* args, short dbrStructType ) {
    if( channel.activated == true && subscription.activated == false ) {
        subscription.creation = ca_create_subscription( dbrStructType, 1, channel.id, DBE_VALUE|DBE_ALARM, subscriptionHandler, args, NULL );
        ca_pend_io( link.searchTimeout );
        subscription.activated = true;
        switch( subscription.creation ) {
            case ECA_NORMAL :
                return REQUEST_SUCCESSFUL;
            break;
            default :
                return REQUEST_FAILED;
            break;
        }
    } else {
        return REQUEST_FAILED;
    }
}

/*!
    Removes channel and associated subscription
    Use activeChannel() for feedback.
*/
void CaConnection::removeChannel() {
    if( channel.activated == true ) {
        ca_clear_channel( channel.id );
        channel.activated = false;
        channel.creation = -1;
    }
}

/*!
    Cancels channel subscription.
*/
void CaConnection::removeSubscription() {
    //NOT IMPLEMENTED
    //ca_clear_subscription( channelId );
}

/*!
    Read channel once and register an event handler.
*/
ca_responses CaConnection::readChannel( void (*readHandler)(struct event_handler_args), void* args, short dbrStructType ) {
    if( channel.activated == true ) {
        channel.readResponse = ca_get_callback( dbrStructType, channel.id, readHandler, args);
        ca_pend_io( link.readTimeout );
        switch( channel.readResponse ) {
            case ECA_NORMAL :
                return REQUEST_SUCCESSFUL;
            break;
            case ECA_DISCONN :
                return CHANNEL_DISCONNECTED;
            break;
            default :
                return REQUEST_FAILED;
            break;
        }
    } else {
        return CHANNEL_DISCONNECTED;
    }
}

/*!
    Write to channel once and register a write handler.
*/
ca_responses CaConnection::writeChannel( void (*writeHandler)(struct event_handler_args), void* args, short dbrStructType, const void* newDbrValue ) {
    if( channel.activated == true ) {
        channel.writeResponse = ca_put_callback( dbrStructType, channel.id, newDbrValue, writeHandler, args);
        ca_pend_io( link.readTimeout );
        switch( channel.writeResponse ) {
            case ECA_NORMAL :
                return REQUEST_SUCCESSFUL;
            break;
            case ECA_DISCONN :
                return CHANNEL_DISCONNECTED;
            break;
            default :
                return REQUEST_FAILED;
            break;
        }
    } else {
        return CHANNEL_DISCONNECTED;
    }
}

/*!
    Record the connection link status.
*/
void CaConnection::setLinkState( link_states newLinkState ) {
    link.state = newLinkState;
}

/*!
    Retrieve the connection status.
*/
link_states CaConnection::getLinkState() {
    return link.state;
}

/*!
    Retrieve the channel connection state. There is no set connection state
    because this is handled by the EPICS library.
*/
channel_states CaConnection::getChannelState() {
    channel.state = ca_state( channel.id );
    switch( channel.state ) {
        case cs_never_conn :
            return NEVER_CONNECTED;
        break;
        case cs_prev_conn :
            return PREVIOUSLY_CONNECTED;
        break;
        case cs_conn :
            return CONNECTED;
        break;
        case cs_closed :
            return CLOSED; 
        break;
        default:
            return CHANNEL_UNKNOWN;
        break;
    }
}

/*!
    Retrieve the channel's database type.
*/
short CaConnection::getChannelType() {
    channel.type = ca_field_type( channel.id );
    return channel.type;
}

/*!
    Initialise with unique ID and state information
*/
void CaConnection::initialise() {
    CA_UNIQUE_CONNECTION_ID++;
}

/*!
    Shutdown and free context if last.
*/
void CaConnection::shutdown() {
    CA_UNIQUE_CONNECTION_ID--;

    if( channel.activated == true ) {
        ca_clear_channel( channel.id );
    }
    if( context.activated == true ) {
        if( CA_UNIQUE_CONNECTION_ID <= 0 ) {
            ca_context_destroy();
        }
    }
}

/*!
    Set internal data to startup conditions
*/
void CaConnection::reset() {
    link.searchTimeout = 3.0;
    link.readTimeout = 2.0;
    link.writeTimeout = 2.0;
    link.state = LINK_DOWN;

    context.activated = false;
    context.creation = -1;
    context.exception = -1;

    channel.activated = false;
    channel.creation = -1;
    channel.readResponse = -1;
    channel.writeResponse = -1;
    channel.state = cs_never_conn;
    channel.type = -1;
    channel.id = NULL;

    subscription.activated = false;
    subscription.creation = false;
}
