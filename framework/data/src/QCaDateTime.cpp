/*  QCaDateTime.cpp
 *
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
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#include <QString>
#include <QTextStream>
#include <QCaDateTime.h>
#include <QDebug>


static const QDateTime qtEpoch    (QDate( 1970, 1, 1 ), QTime( 0, 0, 0, 0 ), Qt::UTC );
static const QDateTime epicsEpoch (QDate( 1990, 1, 1 ), QTime( 0, 0, 0, 0 ), Qt::UTC );
static unsigned long EPICSQtEpocOffset = qtEpoch.secsTo ( epicsEpoch );


/*
  Construct an empty QCa date time
 */
QCaDateTime::QCaDateTime() {
    nSec = 0;
}

/*
  Construct a QCa date time set to the same date/time as a conventional QDateTime
 */
QCaDateTime::QCaDateTime( QDateTime dt ) : QDateTime( dt ) {
    nSec = 0;
}

/*
  Construct a QCa date time set to the same date/time as an EPICS time stamp
 */
QCaDateTime::QCaDateTime( unsigned long seconds, unsigned long nanoseconds ) {
    // Set the time to the second
    // Note, although the EPICS time stamp is in seconds since a base, the
    // method which takes seconds since a base time uses a different base, so an offset is added.
    setTime_t( seconds + EPICSQtEpocOffset );

    // Add the nanoseconds. Down to the millisecond goes in the Qt structure,
    // the remaining nanoseconds are saved in this class
    addMSecs( nanoseconds / 1000000 );
    nSec = nanoseconds % 1000000;
}

/*
  Copy a QCaDateTime from another and return value to allow t1 = t2 = t3 = etc.
 */
QCaDateTime& QCaDateTime::operator=( const QCaDateTime& other )
{
    // Do parent class stuff assignment.
    *(QDateTime*) this = (QDateTime) other;

    // and then copy class specific stuff.
    nSec = other.nSec;

    // return value as well.
    return *this;
}

/*
  Returns a string which represents the date and time
 */
QString QCaDateTime::text()
{
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

/*
  Returns a double which represents the date and time in seconds (to mS resolution) from the base time
 */
double QCaDateTime::floating( const QDateTime & base ) const
{
    qint64 msec = base.msecsTo (*this);
    return (double) (msec / 1000);
}

/*
  Returns original number of seconds from EPICS Epoch
 */
unsigned long QCaDateTime::getSeconds() const
{
   qint64 msec = epicsEpoch.msecsTo (*this);

   if( msec < 0 ) msec = 0;
   return (unsigned long) (msec / 1000);
}

/*
  Return soriginal number of nano-seconds.
 */
unsigned long QCaDateTime::getNanoSeconds() const
{
   qint64 msec = epicsEpoch.msecsTo (*this);

   if( msec < 0 ) msec = 0;
   return  (unsigned long) ((msec % 1000)*1000000) + nSec;
}

// end
