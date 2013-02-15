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

// Provides CA to an EPICS channel.

#define epicsAlarmGLOBAL

#include <CaObjectPrivate.h>
#include <epicsEvent.h>
#include <epicsMutex.h>
#include <alarm.h>
#include <db_access.h>
#include <string.h>
#include <stdio.h>


static epicsEventId monitorEvent = NULL;
static epicsMutexId accessMutex = NULL;

static CaRef* carefListHead = NULL;
static CaRef* carefListTail = NULL;


//===============================================================================
// Initialisation and deletion
//===============================================================================

/*
    Initialise the unique ID.
*/
int CaObject::CA_UNIQUE_OBJECT_ID = 0;

/*
    Initialisation
*/
CaObject::CaObject() {
    // Construct or reuse a durable object that can be passed to CA and used as a callback argument
    myRef = CaRef::getCaRef( this );

    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = new CaObjectPrivate( this );
    priPtr = p;

    p->caConnection = new caconnection::CaConnection( this );
    initialise();
}

/*
    Shutdown
*/
CaObject::~CaObject() {
    // Flag in the durable object reference that this object has been deleted
    myRef->discard();
    myRef = NULL;

    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    shutdown();
    delete p->caConnection;
    delete p;
}

/*
    Initialise the EPICS library by creating or attaching to a context.
*/
void CaObject::initialise() {
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    p->caConnection->establishContext( p->exceptionHandler, myRef );
    CA_UNIQUE_OBJECT_ID++;
    if( CA_UNIQUE_OBJECT_ID <= 1) {
        monitorEvent = epicsEventCreate( epicsEventEmpty );
        accessMutex = epicsMutexCreate();
    }
    p->caRecord.setName( "" );
    p->caRecord.setValid( false );
}

/*
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

/*
    Establishes client side channel setup.
*/
caconnection::ca_responses CaObjectPrivate::setChannel( std::string channelName ) {
    owner->myRef->setPV( channelName );
    caRecord.setName( channelName );
    caRecord.setValid( false );
    caconnection::ca_responses ret = caConnection->establishChannel( connectionHandler, channelName );
    if( ret == caconnection::REQUEST_SUCCESSFUL )
    {
        owner->myRef->setChannelId( caConnection->getChannelId() );
    }
    return ret;
}

/*
    Begins a callback subscription with the established client side channel
    setup.
*/
caconnection::ca_responses CaObjectPrivate::startSubscription() {
    short initialDbrType = caRecord.getDbrType( carecord::CONTROL );
    short updateDbrType  = caRecord.getDbrType( carecord::TIME );
    caconnection::ca_responses status;

    if( ( initialDbrType == -1 ) || ( updateDbrType == -1 ) ) {
        status = caconnection::REQUEST_FAILED;
    } else {
        status = caConnection->establishSubscription( subscriptionHandler, owner->myRef,
                                                      initialDbrType, updateDbrType );

    }
    return status;
}

/*
    Removes any client side channel setup and accociated subscriptions.
*/
void CaObjectPrivate::removeChannel() {
    caRecord.setName( "" );
    caRecord.setValid( false );
    caConnection->removeChannel();
}

/*
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

/*
    Request one shot read callback for the channel.
*/
caconnection::ca_responses CaObjectPrivate::readChannel() {
    short readDbrType = caRecord.getDbrType( carecord::CONTROL );
    caconnection::ca_responses status;

    if(readDbrType  == -1 ) {
        status = caconnection::REQUEST_FAILED;
    } else {
        status = caConnection->readChannel( readHandler, owner->myRef, readDbrType );
    }
    return status;
}

/*
    Request one shot write for the channel (Generates callback).
*/
caconnection::ca_responses CaObjectPrivate::writeChannel( generic::Generic *newValue ) {

    // If not array data...
    if( newValue->getArrayCount() <= 1 )
    {

        // Write the appropriate type
        switch( newValue->getType() )
        {
            case generic::STRING :
            {
                std::string outValue = newValue->getString();
                return caConnection->writeChannel( writeHandler, owner->myRef, DBR_STRING, 0, outValue.c_str() );
                break;
            }
            case generic::SHORT :
            {
                short outValue = newValue->getShort();
                return caConnection->writeChannel( writeHandler, owner->myRef, DBR_SHORT, 0, &outValue );
                break;
            }
            case generic::UNSIGNED_SHORT :
            {
                unsigned short outValue = newValue->getUnsignedShort();
                return caConnection->writeChannel( writeHandler, owner->myRef, DBR_ENUM, 0, &outValue );
                break;
            }
            case generic::UNSIGNED_CHAR :
            {
                char outValue = newValue->getUnsignedChar();
                return caConnection->writeChannel( writeHandler, owner->myRef, DBR_CHAR, 0, &outValue );
                break;
            }
            case generic::UNSIGNED_LONG :
            {
                unsigned long outValue = newValue->getUnsignedLong();
                return caConnection->writeChannel( writeHandler, owner->myRef, DBR_LONG, 0, &outValue );
                break;
            }
            case generic::FLOAT :
            {
                float outValue = newValue->getFloat();
                return caConnection->writeChannel( writeHandler, owner->myRef, DBR_FLOAT, 0, &outValue );
                break;
            }
            case generic::DOUBLE :
            {
                double outValue = newValue->getDouble();
                return caConnection->writeChannel( writeHandler, owner->myRef, DBR_DOUBLE, 0, &outValue );
                break;
            }
            default :
            {
                return caconnection::REQUEST_FAILED;
            }
        }
    }

    // If array data...
    else
    {
        unsigned long arrayCount;
        // Write the appropriate type
        switch( newValue->getType() )
        {
            case generic::STRING :
            {
            //??? dont do arrays of strings yet
                std::string outValue = newValue->getString();
                return caConnection->writeChannel( writeHandler, owner->myRef, DBR_STRING, 0, outValue.c_str() );
                break;
            }
            case generic::SHORT :
            {
                short* outValue;
                newValue->getShort( &outValue, &arrayCount );
                return caConnection->writeChannel( writeHandler, owner->myRef, DBR_SHORT, arrayCount, outValue );
                break;
            }
            case generic::UNSIGNED_SHORT :
            {
                unsigned short* outValue;
                newValue->getUnsignedShort( &outValue, &arrayCount );
                return caConnection->writeChannel( writeHandler, owner->myRef, DBR_ENUM, arrayCount, outValue );
                break;
            }
            case generic::UNSIGNED_CHAR :
            {
                unsigned char* outValue;
                newValue->getUnsignedChar( &outValue, &arrayCount );
                return caConnection->writeChannel( writeHandler, owner->myRef, DBR_CHAR, arrayCount, outValue );
                break;
            }
            case generic::UNSIGNED_LONG :
            {
                unsigned long* outValue;
                newValue->getUnsignedLong( &outValue, &arrayCount );
                return caConnection->writeChannel( writeHandler, owner->myRef, DBR_LONG, arrayCount, outValue );
                break;
            }
            case generic::FLOAT :
            {
                float* outValue;
                newValue->getFloat( &outValue, &arrayCount );
                return caConnection->writeChannel( writeHandler, owner->myRef, DBR_FLOAT, arrayCount, outValue );
                break;
            }
            case generic::DOUBLE :
            {
                double* outValue;
                newValue->getDouble( &outValue, &arrayCount );
                return caConnection->writeChannel( writeHandler, owner->myRef, DBR_DOUBLE, arrayCount, outValue );
                break;
            }
            default :
            {
                return caconnection::REQUEST_FAILED;
            }
        }
    }
    return caconnection::REQUEST_FAILED;
}

//===============================================================================
// Data record interrogation
//===============================================================================

/*
  Return true if the current data record is from the first update after connecting
 */
bool CaObject::isFirstUpdate()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.isFirstUpdate();
}

/*
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

/*
  Get count of enuerated strings from the current data record
 */
int CaObject::getEnumStateCount()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getEnumStateCount();
}

/*
  Get an enuerated string from the current data record
 */
std::string CaObject::getEnumState( int position )
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getEnumState( position );
}

/*
  Get floating point precision from the current data record
 */
int CaObject::getPrecision()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getPrecision();
}

/*
  Get the engineering units from the current data record
 */
std::string CaObject::getUnits()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getUnits();
}

/*
  Get the data type from the current data record
 */
generic_types CaObject::getType()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getType();
}

/*
  Get the seconds part of the EPICS timestamp from the current record
 */
unsigned long CaObject::getTimeStampSeconds()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getTimeStampSeconds();
}

/*
  Get the nanoseconds part of the EPICS timestamp from the current record
 */
unsigned long CaObject::getTimeStampNanoseconds()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getTimeStampNanoseconds();
}

/*
    Returns the alarm status from the current record
*/
short CaObject::getAlarmStatus()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getStatus();
}

/*
    Returns the alarm severity from the current record
*/
short CaObject::getAlarmSeverity()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getAlarmSeverity();
}

/*
    Returns the display upper limit
*/
double CaObject::getDisplayUpper()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getDisplayLimit();
    return limit.upper;
}

/*
    Returns the display lower limit
*/
double CaObject::getDisplayLower()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getDisplayLimit();
    return limit.lower;
}

/*
    Returns the alarm upper limit
*/
double CaObject::getAlarmUpper()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getAlarmLimit();
    return limit.upper;
}

/*
    Returns the alarm lower limit
*/
double CaObject::getAlarmLower()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getAlarmLimit();
    return limit.lower;
}

/*
    Returns the warning upper limit
*/
double CaObject::getWarningUpper()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getWarningLimit();
    return limit.upper;
}

/*
    Returns the warning lower limit
*/
double CaObject::getWarningLower()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getWarningLimit();
    return limit.lower;
}

/*
    Returns the control upper limit
*/
double CaObject::getControlUpper()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getControlLimit();
    return limit.upper;
}

/*
    Returns the control lower limit
*/
double CaObject::getControlLower()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getControlLimit();
    return limit.lower;
}

/*
    Returns the link status for the connection for the current record
*/
caconnection::link_states CaObjectPrivate::getLinkState() {
    return caConnection->getLinkState();
}

/*
    Returns the link state of the channel for the current record
    The only 'good' state is channel_states::CONNECTED.
    Reads and writes should not be attempted until the channel is connected.
*/
caconnection::channel_states CaObjectPrivate::getChannelState() {
    return caConnection->getChannelState();
}

/*
  Get the the host name of the current connection.
 */
std::string CaObject::getHostName()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caConnection->getHostName();
}

/*
  Get the the field type of the current connection.
*/
std::string  CaObject::getFieldType ()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caConnection->getFieldType();
}

/*
   Get number of elemenst served for the current connection.
*/
unsigned long CaObject::getElementCount()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caConnection->getElementCount();
}


//===============================================================================
// CA callback handlers
//===============================================================================

/*
    Processes EPICS callbacks, rebuilds returned data into a CaRecord.
    Database types that EPICS can return, fall into the groups:
    Basic, Status, Time, Graphic and Control.
    /note Time and Graphic database groups not implemented.
*/
bool CaObjectPrivate::processChannel( struct event_handler_args args ) {

    // NOTE: The caRecord type dos not handle arrays of String (yet).
    //       So we use the single value instance of setString.
    //
    switch( args.type ) {
        // STS (status) formats - pprivides values and alarm severity/status.
        // Not currentl;y used - but remains included for completeness
        //
        case DBR_STS_STRING :
        {
            struct dbr_sts_string *incommingData = (struct dbr_sts_string *) (args.dbr);
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setString( incommingData->value );
            break;
        }
        case DBR_STS_SHORT :
        {
            struct dbr_sts_short *incommingData = (struct dbr_sts_short *) (args.dbr);
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setShort( &incommingData->value, args.count );
            break;
        }
        case DBR_STS_FLOAT :
        {
            struct dbr_sts_float *incommingData = (struct dbr_sts_float *) (args.dbr);
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setFloat( &incommingData->value, args.count );
            break;
        }
        case DBR_STS_ENUM :
        {
            struct dbr_sts_enum *incommingData = (struct dbr_sts_enum *) (args.dbr);
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setUnsignedShort( &incommingData->value, args.count );
            break;
        }
        case DBR_STS_CHAR :
        {
            struct dbr_sts_char *incommingData = (struct dbr_sts_char *) (args.dbr);
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setUnsignedChar( &incommingData->value, args.count );
            break;
        }
        case DBR_STS_LONG :
        {
            struct dbr_sts_long *incommingData = (struct dbr_sts_long *) (args.dbr);
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setLong( &incommingData->value, args.count );
            break;
        }
        case DBR_STS_DOUBLE :
        {
            struct dbr_sts_double *incommingData = (struct dbr_sts_double *) (args.dbr);
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setDouble( &incommingData->value, args.count);
            break;
        }

        // TIME formats - likes status but with the time, which is nice.
        //
        case DBR_TIME_STRING :
        {
            struct dbr_time_string *incommingData = (struct dbr_time_string*) (args.dbr);
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setString( incommingData->value );
            caRecord.setTimeStamp( incommingData->stamp.secPastEpoch, incommingData->stamp.nsec );
            break;
        }
        case DBR_TIME_SHORT :
        {
            struct dbr_time_short *incommingData = (struct dbr_time_short *) (args.dbr);
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setShort( &incommingData->value, args.count );
            caRecord.setTimeStamp( incommingData->stamp.secPastEpoch, incommingData->stamp.nsec );
            break;
        }
        case DBR_TIME_FLOAT :
        {
            struct dbr_time_float *incommingData = (struct dbr_time_float *) (args.dbr);
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setFloat( &incommingData->value, args.count );
            caRecord.setTimeStamp( incommingData->stamp.secPastEpoch, incommingData->stamp.nsec );
            break;
        }
        case DBR_TIME_ENUM :
        {
            struct dbr_time_enum *incommingData = (struct dbr_time_enum *) (args.dbr);
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setUnsignedShort( &incommingData->value, args.count );
            caRecord.setTimeStamp( incommingData->stamp.secPastEpoch, incommingData->stamp.nsec );
            break;
        }
        case DBR_TIME_CHAR :
        {
            struct dbr_time_char *incommingData = (struct dbr_time_char *) (args.dbr);
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setUnsignedChar( &incommingData->value, args.count );
            caRecord.setTimeStamp( incommingData->stamp.secPastEpoch, incommingData->stamp.nsec );
            break;
        }
        case DBR_TIME_LONG :
        {
            struct dbr_time_long *incommingData = (struct dbr_time_long *) (args.dbr);
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setLong( &incommingData->value, args.count );
            caRecord.setTimeStamp( incommingData->stamp.secPastEpoch, incommingData->stamp.nsec );
            break;
        }
        case DBR_TIME_DOUBLE :
        {
            struct dbr_time_double * incommingData = (struct dbr_time_double *) (args.dbr);
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setDouble( &incommingData->value, args.count );
            caRecord.setTimeStamp( incommingData->stamp.secPastEpoch, incommingData->stamp.nsec );
            break;
        }

        // CTRL (control) formats
        //
        case DBR_CTRL_STRING : //< Same as dbr_sts_string
        {
            struct dbr_sts_string *incommingData = (struct dbr_sts_string *) (args.dbr);
//          caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setString( incommingData->value );
            break;
        }
        case DBR_CTRL_SHORT :
        {
            struct dbr_ctrl_int* incommingData = (dbr_ctrl_int*)(args.dbr);
//          caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setUnits( std::string( incommingData->units ) );
            caRecord.setDisplayLimit( incommingData->upper_disp_limit, incommingData->lower_disp_limit );
            caRecord.setAlarmLimit( incommingData->upper_alarm_limit, incommingData->lower_alarm_limit );
            caRecord.setWarningLimit( incommingData->upper_warning_limit, incommingData->lower_warning_limit );
            caRecord.setControlLimit( incommingData->upper_ctrl_limit, incommingData->lower_ctrl_limit );
            caRecord.setShort( &incommingData->value, args.count );
            break;
        }
        case DBR_CTRL_FLOAT :
        {
            struct dbr_ctrl_float* incommingData = (dbr_ctrl_float*)(args.dbr);
//          caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setPrecision( incommingData->precision );
            caRecord.setUnits( std::string( incommingData->units ) );
            caRecord.setDisplayLimit( incommingData->upper_disp_limit, incommingData->lower_disp_limit );
            caRecord.setAlarmLimit( incommingData->upper_alarm_limit, incommingData->lower_alarm_limit );
            caRecord.setWarningLimit( incommingData->upper_warning_limit, incommingData->lower_warning_limit );
            caRecord.setControlLimit( incommingData->upper_ctrl_limit, incommingData->lower_ctrl_limit );
            caRecord.setFloat( &incommingData->value, args.count );
            break;
        }
        case DBR_CTRL_ENUM :
        {
            struct dbr_ctrl_enum* incommingData = (dbr_ctrl_enum*)(args.dbr);
            caRecord.setName( ca_name( args.chid ) );
//          caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            if( caRecord.getEnumStateCount() == 0 )
            {
                for( int i = 0; i < incommingData->no_str; i++ )
                {
                    caRecord.addEnumState( std::string( incommingData->strs[i] ) );
                }
            }
            caRecord.setUnsignedShort( &incommingData->value, args.count );
            break;
        }
        case DBR_CTRL_CHAR :
        {
            struct dbr_ctrl_char* incommingData = (dbr_ctrl_char*)(args.dbr);
//          caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setDisplayLimit( incommingData->upper_disp_limit, incommingData->lower_disp_limit );
            caRecord.setAlarmLimit( incommingData->upper_alarm_limit, incommingData->lower_alarm_limit );
            caRecord.setWarningLimit( incommingData->upper_warning_limit, incommingData->lower_warning_limit );
            caRecord.setControlLimit( incommingData->upper_ctrl_limit, incommingData->lower_ctrl_limit );
            caRecord.setUnsignedChar( &incommingData->value, args.count );
            break;
        }
        case DBR_CTRL_LONG :
        {
            struct dbr_ctrl_long* incommingData = (dbr_ctrl_long*)(args.dbr);
//          caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setUnits( std::string( incommingData->units ) );
            caRecord.setDisplayLimit( incommingData->upper_disp_limit, incommingData->lower_disp_limit );
            caRecord.setAlarmLimit( incommingData->upper_alarm_limit, incommingData->lower_alarm_limit );
            caRecord.setWarningLimit( incommingData->upper_warning_limit, incommingData->lower_warning_limit );
            caRecord.setControlLimit( incommingData->upper_ctrl_limit, incommingData->lower_ctrl_limit );
            caRecord.setLong( &incommingData->value, args.count );
            break;
        }
        case DBR_CTRL_DOUBLE :
        {
            struct dbr_ctrl_double* incommingData = (dbr_ctrl_double*)(args.dbr);
//          caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setPrecision( incommingData->precision );
            caRecord.setUnits( std::string( incommingData->units ) );
            caRecord.setDisplayLimit( incommingData->upper_disp_limit, incommingData->lower_disp_limit );
            caRecord.setAlarmLimit( incommingData->upper_alarm_limit, incommingData->lower_alarm_limit );
            caRecord.setWarningLimit( incommingData->upper_warning_limit, incommingData->lower_warning_limit );
            caRecord.setControlLimit( incommingData->upper_ctrl_limit, incommingData->lower_ctrl_limit );
            caRecord.setDouble( &incommingData->value, args.count );
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

CaObject* CaObjectPrivate::contextFromCaUsr( void* usr, void* id )
{
    CaRef* ref = (CaRef*)(usr);
    return (CaObject*)(ref->getRef( id ));
}

/*
    Subscription handler callback.
*/
void CaObjectPrivate::subscriptionHandler( struct event_handler_args args ) {
    epicsMutexLock( accessMutex );
    CaObject* context = contextFromCaUsr( args.usr, args.chid );
    if( !context )
    {
        epicsMutexUnlock( accessMutex );
        return;
    }

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

/*
    Read data handler callback.
*/
void CaObjectPrivate::readHandler( struct event_handler_args args ) {
    epicsMutexLock( accessMutex );
    CaObject* context = contextFromCaUsr( args.usr, args.chid );
    if( !context )
    {
        epicsMutexUnlock( accessMutex );
        return;
    }

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

/*
    Write data handler callback.
*/
void CaObjectPrivate::writeHandler( struct event_handler_args args ) {
    epicsMutexLock( accessMutex );
    CaObject* context = contextFromCaUsr( args.usr, args.chid );
    if( !context )
    {
        epicsMutexUnlock( accessMutex );
        return;
    }

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

/*
    EPICS Exception handler callback.
*/
void CaObjectPrivate::exceptionHandler( struct exception_handler_args args ) {
    epicsMutexLock( accessMutex );
    CaObject* context = contextFromCaUsr( args.usr, args.chid );
    if( !context )
    {
        epicsMutexUnlock( accessMutex );
        return;
    }

    switch( args.stat ) {
        case ECA_NORMAL :
            context->signalCallback( EXCEPTION );
        break;
        default :
            context->signalCallback( EXCEPTION );
        break;
    }
    epicsMutexUnlock( accessMutex );
}

/*
    Connection handler callback.
    This is called with CaObject out of context, it is recovered in:
    "args" -> "parent" -> "grandParent".
*/
void CaObjectPrivate::connectionHandler( struct connection_handler_args args ) {


    // Since "connectionHandler" is a callback method (and, therefore, called in an asynchronous way), sometimes, it
    // generates a SEGFAULT. Therefore, a workaround based on a 1 second delay was introduced in order to give time
    // for "things" to complete before continuing executing this method (thus avoiding a SEGFAULT).
    // NOTE 1: a more solid solution should be implemented but this requires a deep understanding of the QE core
    // NOTE 2: without this workaround, a SEGFAULT appears ~4 out of 10 executions; with this workaround, there is no SEGFAULT
    //     usleep(1000);
    // A. R. Above workaround removed from repository as it is only for one specific GUI on one IOC, and this work around won't build on Windows.
    // Note, the use of getRef() below, which checks for three error conditions where a callback occurs after a connection has been closed.
    // With a bit of luck, this problem relates to a fourth error condition which can be added to getRef()(or perhaps will shed light on why CA callbacks
    // can occur after a channel is closed at all).

    epicsMutexLock( accessMutex );
    CaRef* ref = (CaRef*)(ca_puser( args.chid ));
    caconnection::CaConnection* parent = (caconnection::CaConnection*)(ref->getRef( args.chid ));
    if( !parent )
    {
        epicsMutexUnlock( accessMutex );
        return;
    }




    CaObject* grandParent = (CaObject*)parent->getParent();
    switch( args.op ) {
        case CA_OP_CONN_UP :
            {
                CaObjectPrivate* grandParentPri = (CaObjectPrivate*)(grandParent->priPtr);
                grandParentPri->caRecord.setDbrType( parent->getChannelType() );
            }
            parent->setChannelElementCount();
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

/*
  Set if callbacks are required on write completion. (default is write with no callback)
  Note, this is not just for better write status, if affects the behaviour of the write as follows:
  When using write with callback, then record will finish processing before accepting next write.
  Writing with callback may be required when writing code that is tightly integrated with record
  processing and code nneds to know processing has completed.
  Writing with no callback is more desirable when a detachement from record processing is required, for
  example in a GUI after issuing a motor record move a motor stop command will take effect immedietly
  if writing without callback, but will only take affect after the move has finished if writing with callback.
  */
void CaObject::setWriteWithCallback( bool writeWithCallbackIn )
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    // Set the write callback requirements
    p->caConnection->setWriteWithCallback( writeWithCallbackIn );
}

/*
  Determine if callbacks are delivered on write completion.
  */
bool CaObject::getWriteWithCallback()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    // return the write callback requirements
    return p->caConnection->getWriteWithCallback();
}

//===========================================================
// CaRef methods

// Provide a new or reused instance. Call instead of constructor.
CaRef* CaRef::getCaRef( void* ownerIn )
{
    // If there is any previous CaRef  instances discarded over 5 seconds ago, return the first.
    if( carefListHead )
    {
        CaRef* firstRef = carefListHead;
        if( difftime( time( NULL ), firstRef->idleTime ) > 5.0 )
        {
            // Move the list head to the next (possibly NULL) object
            carefListHead = firstRef->next;

            // If end of queue reached, clear the end of list reference.
            if( firstRef == carefListTail )
            {
                carefListTail = NULL;
            }

            // Re-initialise and return the recycled object
            firstRef->init( ownerIn );
//            printf("reuse\n");
            return firstRef;
        }
    }

    // There are no old instances to reuse - create a new one
//    printf("create\n");
    return new CaRef( ownerIn );
}

// Construction.
// Don't use directly. Called by getCaRef() if none available for reuse
CaRef::CaRef( void* ownerIn )
{
    init( ownerIn );
}

// Initialisation. Used for construction and reuse
void CaRef::init( void* ownerIn )
{
    magic = CAREF_MAGIC;
    owner = ownerIn;
    discarded = false;
    channel = NULL;
    next = NULL;
    idleTime = 0;
//    dumpList();
}

// Destrution
// This should never be called. Present just to log an error
CaRef::~CaRef()
{
    printf( "CaRef destructor called. This should never occur.");
}

// Mark as discarded and queue for reuse when no further CA callbacks are expected
void CaRef::discard()
{
    // Flag no longer in use
    discarded = true;

    // Note the time discarded
    idleTime = time( NULL );

    // Place the disused item on the discarded queue
    if( !carefListHead )
    {
        carefListHead = this;
    }

    if( carefListTail )
    {
        carefListTail->next = this;
    }
    carefListTail = this;
//    dumpList();
}

// Return the object referenced, if it is still around.
// Returns NULL if the object is no longer in use.
void* CaRef::getRef( void* channelIn )
{
    // Sanity check - was the CA user data really a CaRef pointer
    if( magic != CAREF_MAGIC )
    {
        printf( "CaRef::getRef() called the CA user data was not really a CaRef pointer. (magic number is bad).  CA user data: %ld\n", (long)this );
        return NULL;
    }

    // If discarded, then a late callback has occured
    if( discarded )
    {
        printf( "Late CA callback. CaRef::getRef() called after associated object has been discarded.  object reference: %ld  variable: %s  expected channel: %ld received channel %ld\n",
                (long)owner, variable.c_str(), (long)channel, (long)channelIn );
        return NULL;
    }

    // If a channel has been recorded, but the current channel doesn't match, it is likely due to a late callback calling with a reference to a now re-used CaRef
    if( channel && (channel != channelIn) )
    {
        printf( "Very late CA callback. CaRef::getRef() called with incorrect channel ID.  object reference: %ld  variable: %s  expected channel: %ld received channel %ld\n",
                (long)owner, variable.c_str(), (long)channel, (long)channelIn );
        return NULL;
    }

    // Return the referenced object
    return owner;
}

// set the variable - for logging only
void CaRef::setPV( std::string variableIn )
{
    variable = variableIn;
}

// set the channel - for checking and logging
void CaRef::setChannelId ( void* channelIn )
{
    channel = channelIn;
}

// Dump the current list - for debugging only
//void CaRef::dumpList()
//{
//    printf( "head: %lu\n", (unsigned long)carefListHead );
//    CaRef* obj = carefListHead;
//    int count = 0;
//    while( obj )
//    {
//        count++;
//// Include the following line if full dump of current list is required
////        printf("   obj: %lu next: %lu PV: %s\n", (unsigned long)obj, (unsigned long)(obj->next), obj->variable.c_str() );
//        obj = obj->next;
//    }
//    printf( "count: %d\n", count );
//    printf( "tail: %lu\n", (unsigned long)carefListTail );
//    fflush(stdout);
//}
