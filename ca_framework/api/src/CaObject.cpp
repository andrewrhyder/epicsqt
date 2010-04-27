/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/api/src/CaObject.cpp $
 * $Revision: #8 $ 
 * $DateTime: 2009/11/18 10:21:48 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class CaObject
  \version $Revision: #8 $
  \date $DateTime: 2009/11/18 10:21:48 $
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
 *
 * Description
 *
 * CaObject is a non Qt C++ wrapper around the EPICS CA library.
 *
 * It is uses a void* pointer to a private class (CaObjectPrivate) to hide
 * CA definitions from users.
 */

#define epicsAlarmGLOBAL

#include <CaObjectPrivate.h>
#include <epicsEvent.h>
#include <epicsMutex.h>
#include <alarm.h>
#include <string.h>


static epicsEventId monitorEvent = NULL;
static epicsMutexId accessMutex = NULL;


//===============================================================================
// Initialisation and deletion
//===============================================================================

/*!
    Initialise the unique ID.
*/
int CaObject::CA_UNIQUE_OBJECT_ID = 0;

/*!
    Initialisation
*/
CaObject::CaObject() {
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = new CaObjectPrivate( this );
    priPtr = p;

    p->caConnection = new caconnection::CaConnection( this );
    initialise();
}

/*!
    Shutdown
*/
CaObject::~CaObject() {
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    shutdown();
    delete p->caConnection;
    delete p;
}

/*!
    Initialise the EPICS library by creating or attaching to a context.
*/
void CaObject::initialise() {
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    p->caConnection->establishContext( p->exceptionHandler, this);
    CA_UNIQUE_OBJECT_ID++;
    if( CA_UNIQUE_OBJECT_ID <= 1) {
        monitorEvent = epicsEventCreate( epicsEventEmpty );
        accessMutex = epicsMutexCreate();
    }
    p->caRecord.setName( "" );
    p->caRecord.setValid( false );
}

/*!
    Shutdown the EPICS library.
*/
void CaObject::shutdown() {
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    CA_UNIQUE_OBJECT_ID--;
    if( CA_UNIQUE_OBJECT_ID <= 0 ) {
        epicsMutexDestroy( accessMutex );
        epicsEventDestroy( monitorEvent );
    }
    p->caRecord.setName( "" );
    p->caRecord.setValid( false );
}

//===============================================================================
// Manage connections
//===============================================================================

/*!
    Establishes client side channel setup.
*/
caconnection::ca_responses CaObjectPrivate::setChannel( std::string channelName ) {
    caRecord.setName( channelName );
    caRecord.setValid( false );
    return caConnection->establishChannel( connectionHandler, channelName );
}

/*!
    Begins a callback subscription with the established client side channel
    setup.
*/
caconnection::ca_responses CaObjectPrivate::startSubscription() {
    if( caRecord.getDbrType() == -1 ) {
        return caconnection::REQUEST_FAILED;
    }
    return caConnection->establishSubscription( subscriptionHandler, owner, caRecord.getDbrType() );
}

/*!
    Removes any client side channel setup and accociated subscriptions.
*/
void CaObjectPrivate::removeChannel() {
    caRecord.setName( "" );
    caRecord.setValid( false );
    caConnection->removeChannel();
}

/*!
    Cancels active subscription but does not flush callbacks.
*/
void CaObjectPrivate::cancelSubscription() {
    //NOT IMPLEMENTED
    //cerr << "CaObject::cancelSubscription()" << endl;
    //caConnection.removeSubscription();
}

//===============================================================================
// Read and write data
//===============================================================================

/*!
    Request one shot read callback for the channel.
*/
caconnection::ca_responses CaObjectPrivate::readChannel() {
    if( caRecord.getDbrType() == -1 ) {
        return caconnection::REQUEST_FAILED;
    }
    return caConnection->readChannel( readHandler, owner, caRecord.getDbrType() );
}

/*!
    Request one shot write for the channel (Generates callback).
*/
caconnection::ca_responses CaObjectPrivate::writeChannel( generic::Generic *newValue ) {

    switch( newValue->getType() ) {
        case generic::STRING :
        {
            std::string outValue = newValue->getString();
            return caConnection->writeChannel( writeHandler, owner, DBR_STRING, outValue.c_str() );
            break;
        }
        case generic::SHORT :
        {
            long outValue = newValue->getShort();
            return caConnection->writeChannel( writeHandler, owner, DBR_SHORT, &outValue );
            break;
        }
        case generic::CHAR :
        {
            char outValue = newValue->getChar();
            return caConnection->writeChannel( writeHandler, owner, DBR_CHAR, &outValue );
            break;
        }
        case generic::UNSIGNED_LONG :
        {
            unsigned long outValue = newValue->getUnsignedLong();
            return caConnection->writeChannel( writeHandler, owner, DBR_LONG, &outValue );
            break;
        }
        case generic::DOUBLE :
        {
            double outValue = newValue->getDouble();
            return caConnection->writeChannel( writeHandler, owner, DBR_DOUBLE, &outValue );
            break;
        }
        default :
        {
            return caconnection::REQUEST_FAILED;
        }
    }
    return caconnection::REQUEST_FAILED;
}

//===============================================================================
// Data record interrogation
//===============================================================================

/*!
  Return true if the current data record is from the first update after connecting
 */
bool CaObject::isFirstUpdate()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.isFirstUpdate();
}

/*!
  Return a copy of the data record.
  A void* is returned although the actual data type is carecord::CaRecord*
  NOTE: The caller is responsible for deleting the record returned.
 */
void* CaObject::getRecordCopyPtr()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return (void*)(new carecord::CaRecord( p->caRecord ));
}

/*!
  Get count of enuerated strings from the current data record
 */
int CaObject::getEnumStateCount()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getEnumStateCount();
}

/*!
  Get an enuerated string from the current data record
 */
std::string CaObject::getEnumState( int position )
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getEnumState( position );
}

/*!
  Get floating point precision from the current data record
 */
int CaObject::getPrecision()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getPrecision();
}

/*!
  Get the engineering units from the current data record
 */
std::string CaObject::getUnits()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getUnits();
}

/*!
  Get the data type from the current data record
 */
generic_types CaObject::getType()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getType();
}

/*!
  Get the seconds part of the EPICS timestamp from the current record
 */
unsigned long CaObject::getTimeStampSeconds()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getTimeStampSeconds();
}

/*!
  Get the nanoseconds part of the EPICS timestamp from the current record
 */
unsigned long CaObject::getTimeStampNanoseconds()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getTimeStampNanoseconds();
}

/*!
    Returns the alarm status from the current record
*/
short CaObject::getAlarmStatus()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getStatus();
}

/*!
    Returns the alarm severity from the current record
*/
short CaObject::getAlarmSeverity()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getAlarmSeverity();
}

/*!
    Returns the link status for the connection for the current record
*/
caconnection::link_states CaObjectPrivate::getLinkState() {
    return caConnection->getLinkState();
}

/*!
    Returns the link state of the channel for the current record
    The only 'good' state is channel_states::CONNECTED.
    Reads and writes should not be attempted until the channel is connected.
*/
caconnection::channel_states CaObjectPrivate::getChannelState() {
    return caConnection->getChannelState();
}

//===============================================================================
// CA callback handlers
//===============================================================================

/*!
    Processes EPICS callbacks, rebuilds returned data into a CaRecord.
    Database types that EPICS can return, fall into the groups:
    Basic, Status, Time, Graphic and Control.
    /note Time and Graphic database groups not implemented.
*/
bool CaObjectPrivate::processChannel( struct event_handler_args args ) {

    switch( args.type ) {
        case DBR_STS_STRING :
        {
            struct dbr_sts_string incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setString( incommingData.value );
            break;
        }
        case DBR_STS_SHORT :
        {
            struct dbr_sts_short incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setUnsignedLong( incommingData.value );
            break;
        }
        case DBR_STS_FLOAT :
        {
            struct dbr_sts_float incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setDouble( incommingData.value );
            break;
        }
        case DBR_STS_ENUM :
        {
            struct dbr_sts_enum incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setUnsignedLong( incommingData.value );
            break;
        }
        case DBR_STS_CHAR :
        {
            struct dbr_sts_char incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setRiscAlignment( incommingData.RISC_pad );
            caRecord.setChar( incommingData.value );
            break;
        }
        case DBR_STS_LONG :
        {
            struct dbr_sts_long incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setUnsignedLong( incommingData.value );
            break;
        }
        case DBR_STS_DOUBLE :
        {
            struct dbr_sts_double incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setDouble( incommingData.value );
            break;
        }
        case DBR_CTRL_STRING : //< Same as dbr_sts_string
        {
            struct dbr_sts_string incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setString( incommingData.value );
            break;
        }
        case DBR_CTRL_SHORT :
        {
            struct dbr_ctrl_int incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setUnits( std::string( incommingData.units ) );
            caRecord.setDisplayLimit( incommingData.upper_disp_limit, incommingData.lower_disp_limit );
            caRecord.setAlarmLimit( incommingData.upper_alarm_limit, incommingData.lower_alarm_limit );
            caRecord.setWarningLimit( incommingData.upper_warning_limit, incommingData.lower_warning_limit );
            caRecord.setControlLimit( incommingData.upper_ctrl_limit, incommingData.lower_ctrl_limit );
            caRecord.setUnsignedLong( incommingData.value );
            break;
        }
        case DBR_CTRL_FLOAT :
        {
            struct dbr_ctrl_float incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setPrecision( incommingData.precision );
            caRecord.setUnits( std::string( incommingData.units ) );
            caRecord.setRiscAlignment( incommingData.RISC_pad );
            caRecord.setDisplayLimit( incommingData.upper_disp_limit, incommingData.lower_disp_limit );
            caRecord.setAlarmLimit( incommingData.upper_alarm_limit, incommingData.lower_alarm_limit );
            caRecord.setWarningLimit( incommingData.upper_warning_limit, incommingData.lower_warning_limit );
            caRecord.setControlLimit( incommingData.upper_ctrl_limit, incommingData.lower_ctrl_limit );
            caRecord.setDouble( incommingData.value );
            break;
        }
        case DBR_CTRL_ENUM :
        {
            struct dbr_ctrl_enum incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            for( int i = 0; i < incommingData.no_str; i++ ) {
                caRecord.addEnumState( std::string( incommingData.strs[i] ) );
            }
            caRecord.setUnsignedLong( incommingData.value );
            break;
        }
        case DBR_CTRL_CHAR :
        {
            struct dbr_ctrl_char incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setRiscAlignment( incommingData.RISC_pad );
            caRecord.setDisplayLimit( incommingData.upper_disp_limit, incommingData.lower_disp_limit );
            caRecord.setAlarmLimit( incommingData.upper_alarm_limit, incommingData.lower_alarm_limit );
            caRecord.setWarningLimit( incommingData.upper_warning_limit, incommingData.lower_warning_limit );
            caRecord.setControlLimit( incommingData.upper_ctrl_limit, incommingData.lower_ctrl_limit );
            caRecord.setChar( incommingData.value );
            break;
        }
        case DBR_CTRL_LONG :
        {
            struct dbr_ctrl_long incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setUnits( std::string( incommingData.units ) );
            caRecord.setDisplayLimit( incommingData.upper_disp_limit, incommingData.lower_disp_limit );
            caRecord.setAlarmLimit( incommingData.upper_alarm_limit, incommingData.lower_alarm_limit );
            caRecord.setWarningLimit( incommingData.upper_warning_limit, incommingData.lower_warning_limit );
            caRecord.setControlLimit( incommingData.upper_ctrl_limit, incommingData.lower_ctrl_limit );
            caRecord.setUnsignedLong( incommingData.value );
            break;
        }
        case DBR_CTRL_DOUBLE :
        {
            struct dbr_ctrl_double incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setPrecision( incommingData.precision );
            caRecord.setUnits( std::string( incommingData.units ) );
            caRecord.setRiscAlignment( incommingData.RISC_pad0 );
            caRecord.setDisplayLimit( incommingData.upper_disp_limit, incommingData.lower_disp_limit );
            caRecord.setAlarmLimit( incommingData.upper_alarm_limit, incommingData.lower_alarm_limit );
            caRecord.setWarningLimit( incommingData.upper_warning_limit, incommingData.lower_warning_limit );
            caRecord.setControlLimit( incommingData.upper_ctrl_limit, incommingData.lower_ctrl_limit );
            caRecord.setDouble( incommingData.value );
            break;
        }
        default :
        {
            //cerr << "CaObject::processChannel(): " << ca_name( args.chid )
            //     << ", type not implemented: " << args.type << endl;
        }
    }
    return 0;
}

/*!
    Subscription handler callback.
*/
void CaObjectPrivate::subscriptionHandler( struct event_handler_args args ) {

    //!!!??? WARNING callbacks can occur AFTER a connection has been closed. Handle this! AJR

    epicsMutexLock( accessMutex );
    CaObject* context = (CaObject*)args.usr;

    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)(context->priPtr);

    switch( args.status ) {
        case ECA_NORMAL :
            p->processChannel( args );
            context->signalCallback( SUBSCRIPTION_SUCCESS );
        break;
        default :
            context->signalCallback( SUBSCRIPTION_FAIL );
        break;
    }
    epicsEventSignal( monitorEvent );
    epicsMutexUnlock( accessMutex );
}

/*!
    Read data handler callback.
*/
void CaObjectPrivate::readHandler( struct event_handler_args args ) {

    //!!!??? WARNING callbacks can occur AFTER a connection has been closed. Handle this! AJR

    epicsMutexLock( accessMutex );
    CaObject* context = (CaObject*)args.usr;

    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)(context->priPtr);

    switch( args.status ) {
        case ECA_NORMAL :
            p->processChannel( args );
            context->signalCallback( READ_SUCCESS );
        break;
        default :
            context->signalCallback( READ_FAIL );
        break;
    }
    epicsEventSignal( monitorEvent );
    epicsMutexUnlock( accessMutex );
}

/*!
    Write data handler callback.
*/
void CaObjectPrivate::writeHandler( struct event_handler_args args ) {

    //!!!??? WARNING callbacks can occur AFTER a connection has been closed. Handle this! AJR

    epicsMutexLock( accessMutex );
    CaObject* context = (CaObject*)args.usr;

    switch( args.status ) {
        case ECA_NORMAL :
            context->signalCallback( WRITE_SUCCESS );
        break;
        default :
            context->signalCallback( WRITE_FAIL );
        break;
    }
    epicsMutexUnlock( accessMutex );
}

/*!
    EPICS Exception handler callback.
*/
void CaObjectPrivate::exceptionHandler( struct exception_handler_args args ) {

    //!!!??? WARNING callbacks can occur AFTER a connection has been closed. Handle this! AJR

    epicsMutexLock( accessMutex );
    CaObject* context = (CaObject*)args.usr;

    switch( args.stat ) {
        case ECA_NORMAL :
            context->signalCallback( EXCEPTION );
        break;
        default :
//!!!??? crash occurs here on occasion when opening new GUI, perhaps because callbacks can occur AFTER a connection has been closed. See warning above
//       Also crash here when ioc is shutdown.
//            context->signalCallback( EXCEPTION );
        break;
    }
    epicsMutexUnlock( accessMutex );
}

/*!
    Connection handler callback.
    This is called with CaObject out of context, it is recovered in:
    "args" -> "parent" -> "grandParent".
*/
void CaObjectPrivate::connectionHandler( struct connection_handler_args args ) {

    //!!!??? WARNING callbacks can occur AFTER a connection has been closed. Handle this! AJR

    epicsMutexLock( accessMutex );
    caconnection::CaConnection* parent = (caconnection::CaConnection*)ca_puser( args.chid );

    CaObject* grandParent = (CaObject*)parent->getParent();
    switch( args.op ) {
        case CA_OP_CONN_UP :
            {
                CaObjectPrivate* grandParentPri = (CaObjectPrivate*)(grandParent->priPtr);
                grandParentPri->caRecord.setDbrType( parent->getChannelType() );
            }
            parent->setLinkState( caconnection::LINK_UP );
            grandParent->signalCallback( CONNECTION_UP );
        break;
        case CA_OP_CONN_DOWN :
            parent->setLinkState( caconnection::LINK_DOWN );
            grandParent->signalCallback( CONNECTION_DOWN );
        break;
        default :
            parent->setLinkState( caconnection::LINK_UNKNOWN );
            grandParent->signalCallback( CONNECTION_UNKNOWN );
        break;
    }   
    epicsMutexUnlock( accessMutex );
}
