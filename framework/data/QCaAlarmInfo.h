/*  QCaAlarmInfo.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2009, 2010 Australian Synchrotron
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

// Manage CA alarm and severity information

#ifndef QCAALARMINFO_H
#define QCAALARMINFO_H

#include <QString>
#include <QEPluginLibrary_global.h>

#define QCAALARMINFO_SEVERITY unsigned short

class QEPLUGINLIBRARYSHARED_EXPORT QCaAlarmInfo
{
public:
    QCaAlarmInfo();
    QCaAlarmInfo( unsigned short statusIn, unsigned short severityIn );

    bool operator==(const QCaAlarmInfo& other) const;   // Return true if equal
    bool operator!=(const QCaAlarmInfo& other) const;   // Return true if not equal

    QString statusName() const;       // Return the name of the current alarm state
    QString severityName() const;     // Return the name of the current alarm severity
    bool isInAlarm() const;           // Return true if there is an alarm
    bool isMinor() const;             // Return true if there is a minor alarm
    bool isMajor() const;             // Return true if there is a major alarm
    bool isInvalid() const;           // Return true if there is an invalid alarm
    QString style() const;            // Return a style string to update the widget's look to reflect the current alarm state

    // getStyleColorName/getColorName return standard color for the alarm state.
    // The former is paler/less solid, suitable e.g. label backgrounds. The later
    // is more solid suitable for graphics. In both cases the reurned format is
    // of the form of a 6 digit hex string, e.g.: "#0080ff"
    //
    QString getStyleColorName() const; // Return 'standard' style colour for the alarm state.
    QString getColorName() const;      // Return 'standard' colour for the alarm state.


    static QCAALARMINFO_SEVERITY getInvalidSeverity(); // Return a severity that will not match any valid severity
    QCAALARMINFO_SEVERITY getSeverity() const; // Return the current severity

private:
    unsigned short status;      // Alarm state
    unsigned short severity;    // Alarm severity

};

#endif // QCAALARMINFO_H
