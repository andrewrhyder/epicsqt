/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/include/QCaAlarmInfo.h $
 * $Revision: #3 $
 * $DateTime: 2009/07/31 15:55:17 $
 * Last checked in by: $Author: rhydera $
 */

/*!
  \class QCaEventFilter
  \version $Revision: #3 $
  \date $DateTime: 2009/07/31 15:55:17 $
  \author andrew.rhyder
  \brief Manage CA alarm and severity information
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
 */

#ifndef QCAALARMINFO_H
#define QCAALARMINFO_H

#include <QString>

#define QCAALARMINFO_SEVERITY unsigned short

class QCaAlarmInfo
{
public:
    QCaAlarmInfo();
    QCaAlarmInfo( unsigned short statusIn, unsigned short severityIn );

    QString statusName();       // Return the name of the current alarm state
    QString severityName();     // Return the name of the current alarm severity
    bool isInAlarm();           // Return true if there is an alarm
    bool isMinor();             // Return true if there is a minor alarm
    bool isMajor();             // Return true if there is a major alarm
    bool isInvalid();           // Return true if there is an invalid alarm
    QString style();            // Return a style string to update the widget's look to reflect the current alarm state
    static QCAALARMINFO_SEVERITY getInvalidSeverity(); // Return a severity that will not match any valid severity
    QCAALARMINFO_SEVERITY getSeverity(); // Return the current severity

private:
    unsigned short status;      // Alarm state
    unsigned short severity;    // Alarm severity


};

#endif // QCAALARMINFO_H
