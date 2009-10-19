/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/src/QCaAlarmInfo.cpp $
 * $Revision: #4 $
 * $DateTime: 2009/07/31 15:55:17 $
 * Last checked in by: $Author: rhydera $
 */

/*!
  \class QCaDatetime
  \version $Revision: #4 $
  \date $DateTime: 2009/07/31 15:55:17 $
  \author andrew.rhyder
  \brief CA alarm info manager
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
 * andrew.rhyder@synchrotron.org.au
 * 800 Blackburn Road, Clayton, Victoria 3168, Australia.
 *
 * Description:
 *
 * This class holds the alarm state of a data update.
 * It provides methods for checking for alarm conditions and the type of alarm
 * conditions without an awareness of EPICS alarm enumerations.
 */


#include <alarm.h>
#include <QCaAlarmInfo.h>

/*!
  Construct an empty instance.
  By default there is no alarm present.
 */
QCaAlarmInfo::QCaAlarmInfo()
{
    status = NO_ALARM;
    severity = NO_ALARM;
}

/*!
  Construct an instance given an alarm state and severity
 */
QCaAlarmInfo::QCaAlarmInfo( unsigned short statusIn, unsigned short severityIn ) {
    status = statusIn;
    severity = severityIn;
}

/*!
  Return a string identifying the alarm state
 */
QString QCaAlarmInfo::statusName() {

    if( status <= lastEpicsAlarmCond )
        return QString( epicsAlarmConditionStrings[status] );
    else
        return QString( "" );
}

/*!
  Return a string identifying the alarm severity
 */
QString QCaAlarmInfo::severityName() {

    if( severity <= lastEpicsAlarmSev )
        return QString( epicsAlarmSeverityStrings[severity] );
    else
        return QString( "" );
}

/*!
  Return true if there is an alarm
 */
bool QCaAlarmInfo::isInAlarm() {
    return( status != NO_ALARM );
}

/*!
  Return true if there is a minor alarm
 */
bool QCaAlarmInfo::isMinor() {
    return( severity == MINOR_ALARM );
}

/*!
  Return true if there is a major alarm
 */
bool QCaAlarmInfo::isMajor() {
    return( severity == MAJOR_ALARM );
}

/*!
  Return true if there is an invalid alarm
 */
bool QCaAlarmInfo::isInvalid() {
    return( severity == INVALID_ALARM );
}

/*!
  Return a style string to update the widget's look to reflect the current alarm state
 */
QString QCaAlarmInfo::style()
{
    switch( severity )
    {
        case NO_ALARM:      return "";
        case MINOR_ALARM:   return "QWidget { background-color: #ffff80; }";
        case MAJOR_ALARM:   return "QWidget { background-color: #ff8080; }";
        case INVALID_ALARM: return "QWidget { background-color: #ffffff; }";
        default:            return "";
    }
}

/*!
  Return a severity that will not match any valid severity
 */
QCAALARMINFO_SEVERITY QCaAlarmInfo::getInvalidSeverity()
{
    return ALARM_NSEV;
}

/*!
  Return the severity
  The caller is only expected to compare this to
 */
QCAALARMINFO_SEVERITY QCaAlarmInfo::getSeverity()
{
    return severity;
}
