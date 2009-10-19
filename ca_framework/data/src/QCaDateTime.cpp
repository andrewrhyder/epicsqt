/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/src/QCaDateTime.cpp $
 * $Revision: #1 $
 * $DateTime: 2009/07/28 17:01:05 $
 * Last checked in by: $Author: rhydera $
 */

/*!
  \class QCaDatetime
  \version $Revision: #1 $
  \date $DateTime: 2009/07/28 17:01:05 $
  \author andrew.rhyder
  \brief CA Date Time manager
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
 * This class holds a date and time to an accuracy of 1 nano second, which is
 * the accuracy of an EPICS time stamp.
 * It is based on a QDateTime and all QDateTime functionality can be used where
 * an accuracy of 1mS is adequate.
 * The nanoseconds within the current millisecond is also available when required.
 *
 */

#include <QString>
#include <QTextStream>
#include <QCaDateTime.h>

/*!
  Construct an empty QCa date time
 */
QCaDateTime::QCaDateTime() {
    nSec = 0;
}

/*!
  Construct a QCa date time set to the same date/time as a conventional QDateTime
 */
QCaDateTime::QCaDateTime( QDateTime dt ) : QDateTime( dt ) {
    nSec = 0;
}

/*!
  Construct a QCa date time set to the same date/time as an EPICS time stamp
 */
QCaDateTime::QCaDateTime( unsigned long seconds, unsigned long nanoseconds ) {
    // Static count of the number of seconds between the Qt base time used when specifying
    // seconds since a base time (1/1/1970) and the EPICS base time (1/1/1990).
    static unsigned long EPICSQtEpocOffset = QDate( 1970, 1, 1 ).daysTo( QDate( 1990, 1, 1 ) ) *60 *60 *24;

    // Set the time to the second
    // Note, although the EPICS time stamp is in seconds since a base, the
    // method which takes seconds since a base time uses a different base, so an offset is added.
    setTime_t( seconds + EPICSQtEpocOffset );

    // Add the nanoseconds. Down to the millisecond goes in the Qt structure,
    // the remaining nanoseconds are saved in this class
    addMSecs( nanoseconds / 1000000 );
    nSec = nanoseconds % 1000000;
}

/*!
  Copy a QCaDateTime from another
 */
void QCaDateTime::operator=( QCaDateTime& other )
{
    setDate( other.date() );
    setTime( other.time() );
    nSec = other.nSec;
}

/*!
  Returns a string which represents the date and time
 */
QString QCaDateTime::text() {

    // Format the date and time to millisecond resolution
    QString out;
    out = toString( QString( "yyyy-MM-dd hh:mm:ss.zzz" ));

    // Add down to nanosecond resolution
    QTextStream s( &out );
    s.setFieldAlignment( QTextStream::AlignRight );
    s.setPadChar( '0' );
    s.setFieldWidth( 6 );
    s << nSec;

    return out;
}
