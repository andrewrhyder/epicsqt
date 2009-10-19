/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/api/src/CaRecord.cpp $
 * $Revision: #4 $ 
 * $DateTime: 2009/07/27 16:34:58 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class CaRecord
  \version $Revision: #4 $
  \date $DateTime: 2009/07/27 16:34:58 $
  \author anthony.owen
  \brief Provides a data holder for EPICS types.
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

#include <CaRecord.h>

using namespace std;
using namespace generic;
using namespace carecord;

/*!
    Starts up disconnected.
*/
CaRecord::CaRecord() {
    reset();
}

/*!
    Cleans out object.
*/
CaRecord::~CaRecord() {
    reset();
}

/*!
    Provides creation based on an existing CaRecord.
*/
CaRecord::CaRecord( CaRecord &param ) : Generic() {
    cloneValue( &param );
}

/*!
    Provides = operator for assignment.
*/
CaRecord& CaRecord::operator= ( CaRecord &param ) {
    cloneValue( &param );
    return *this;
}

/*!
    Set the Record name.
*/
void CaRecord::setName( string newName ) {
    name = newName;
}

/*!
    Set the translated database type for this record.
*/
void CaRecord::setDbrType( short newDbrType ) {
    dbrType = newDbrType;
}

/*!
    Set database record structure type.
*/
void CaRecord::setDbrTranslation( dbr_translation_type newType ) {
    dbrStruct = newType;
}

/*!
    Set record to be in a valid or invalid state.
*/
void CaRecord::setValid( bool newValid ) {
    valid = newValid;
}

/*!
    Flag record as processed first time.
*/
void CaRecord::updateProcessState() {
    switch( processState ) {
        case NO_UPDATE:
            processState = FIRST_UPDATE;
        break;
        case FIRST_UPDATE:
            processState = UPDATE;
        break;
        case UPDATE:

        break;
    }
}

/*!
    Set the connection link status for the record.
*/
void CaRecord::setStatus( short newStatus ) {
    status = newStatus;
}

/*!
    Set the alarm severity for the record.
*/
void CaRecord::setAlarmSeverity( short newSeverity ) {
    alarmSeverity = newSeverity;
}

/*!
    Set the precision for the record.
*/
void CaRecord::setPrecision( short newPrecision ) {
    precision = newPrecision;
}

/*!
    Set the units for the record.
    The test is for protection against EPICS library. The library returns the
    units the first time around then "" for every other callback.
*/
void CaRecord::setUnits( string newUnits ) {
    if( newUnits != "" )
        units = newUnits;
}

/*!
    Set the timestamp for the record.
*/
void CaRecord::setTimeStamp(  unsigned long timeStampSecondsIn, unsigned long timeStampNanosecondsIn ) {
    timeStampSeconds = timeStampSecondsIn;
    timeStampNanoseconds = timeStampNanosecondsIn;
}

/*!
    Set the RISC alignment for the record.
*/
void CaRecord::setRiscAlignment( double newRiscAlignment ) {
    riscAlignment = newRiscAlignment;
}

/*!
    Set the enumeration string states for the record.
*/
void CaRecord::addEnumState( string newEnumState ) {
    enumStates.push_back( newEnumState );
}

/*!
    Set the display limits for the record.
*/
void CaRecord::setDisplayLimit( double newUpper, double newLower ) {
    display.upper = newUpper;
    display.lower = newLower;
}

/*!
    Set the alarm limits for the record.
*/
void CaRecord::setAlarmLimit( double newUpper, double newLower ) {
    alarm.upper = newUpper;
    alarm.lower = newLower;
}

/*!
    Set the warning limits for the record.
*/
void CaRecord::setWarningLimit( double newUpper, double newLower ) {
    warning.upper = newUpper;
    warning.lower = newLower;
}

/*!
    Set the control limits for the record.
*/
void CaRecord::setControlLimit( double newUpper, double newLower ) {
    control.upper = newUpper;
    control.lower = newLower;
}

/*!
    .
*/
bool CaRecord::isFirstUpdate() {
    if( processState == FIRST_UPDATE ) {
        return true;
    } else {
        return false;
    }
}

/*!
    Gets the database structure type based on the information mode of the
    CaRecord.
*/
short CaRecord::getDbrType() {
    switch( dbrStruct ) {
        case BASIC :
            return dbrType;
        break;
        case STATUS :
            return getDbrTranslation( statusTranslationMatrix, dbrType );
        break;
        case TIME :
            return getDbrTranslation( timeTranslationMatrix, dbrType );
        break;
        case GRAPHIC :
            return getDbrTranslation( graphicTranslationMatrix, dbrType );
        break;
        case CONTROL :
            return getDbrTranslation( controlTranslationMatrix, dbrType );
        break;
    }
    return -1;
}

/*!
    Get the matrix translated database structure type for the record.
*/
short CaRecord::getDbrTranslation( const short translationMatrix[TYPE_COUNT][2], short type ) {
    for( int i=0; i < TYPE_COUNT; i++ ) {
        if( translationMatrix[i][0] == type ) {
            return translationMatrix[i][1];
        }
    }
    return -1;
}

/*!
    Reset configurational data
*/
void CaRecord::reset() {
    name = "";
    dbrType = -1;
    dbrStruct = CONTROL;
    valid = false;
    processState = NO_UPDATE;
    status = 0;
    alarmSeverity = 0;
    precision = 0;
    units = "";

    timeStampSeconds = 0;
    timeStampNanoseconds = 0;

    riscAlignment = 0;

    display.upper = 0;
    display.lower = 0;
    alarm.upper = 0;
    alarm.lower = 0;
    warning.upper = 0;
    warning.lower = 0;
    control.upper = 0;
    control.lower = 0;
}
