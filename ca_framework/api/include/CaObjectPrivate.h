/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/api/include/CaObjectPrivate.h $
 * $Revision: #1 $ 
 * $DateTime: 2009/07/29 14:20:04 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class CaObject
  \version $Revision: #1 $
  \date $DateTime: 2009/07/29 14:20:04 $
  \author anthony.owen
  \brief Provides CA to an EPICS channel.
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
 * Description:
 *
 * This class should be considered an extension of the CaObject class.
 * It contains any definititions that require the inclusino of EPICS include files.
 * It should be included by CaObject.cpp (part of the QCa framework) but not
 * by any application code.
 */

#ifndef CAOBJECTPRIVATE_H_
#define CAOBJECTPRIVATE_H_

#include <cadef.h>
#include <CaConnection.h>
#include <CaRecord.h>
#include <CaObject.h>
#include <Generic.h>

using namespace caobject;
using namespace generic;

class CaObjectPrivate {
    public:
        CaObjectPrivate( CaObject* ownerIn ) { owner = ownerIn; }

        // Manage connections
        caconnection::ca_responses setChannel( std::string channelName );       // Return type actually caconnection::ca_responses
        caconnection::ca_responses startSubscription();                         // Return type actually caconnection::ca_responses
        void                       removeChannel();
        void                       cancelSubscription(); //< NOT IMPLEMENTED

        // Read and write data
        caconnection::ca_responses   readChannel();                               // Return type actually caconnection::ca_responses
        caconnection::ca_responses   writeChannel( generic::Generic *newValue );

        // Data record interrogation
        caconnection::link_states    getLinkState();                              // Return type actually caconnection::link_states
        caconnection::channel_states getChannelState();                           // Return type actually caconnection::channel_states
//        bool                         isChannelConnected(); // < NOT IMPLEMENTED

        // CA callback handlers
        bool        processChannel( struct event_handler_args args );
        static void subscriptionHandler( struct event_handler_args args );
        static void readHandler( struct event_handler_args args );
        static void writeHandler( struct event_handler_args args );
        static void exceptionHandler( struct exception_handler_args args );
        static void connectionHandler( struct connection_handler_args args );

        // CA data
        caconnection::CaConnection *caConnection;
        carecord::CaRecord caRecord;

        // CaObject of which this instance is a part of
    private:
        CaObject* owner;

};

#endif  // CAOBJECTPRIVATE_H_
