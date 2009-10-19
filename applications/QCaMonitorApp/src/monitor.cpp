/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/applications/QCaMonitorApp/src/monitor.cpp $
 * $Revision: #3 $ 
 * $DateTime: 2009/07/28 17:01:05 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class Monitor
  \version $Revision: #3 $
  \date $DateTime: 2009/07/28 17:01:05 $
  \author andrew.rhyder
  \brief Qt-CA based version of CA Monitor.
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

#include <QTime>
#include <QString>
#include <monitor.h>

monitor::monitor( QString pvIn )
{
    stream = new QTextStream( stdout );

    // Save the PV for logging udpates
    pv = pvIn;

    // Create the data source, connect to data update and message signals, then subscribe to updates.
    source = new QCaString( pv, this, &formatting, 1, &messages );
    QObject::connect( source, SIGNAL( stringChanged( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int & ) ),
                      this, SLOT( log( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int & ) ) );
    QObject::connect( &messages, SIGNAL( generalMessage( const QString& ) ), this, SLOT( log( const QString & ) ) );
    source->subscribe();
}

// Log data updates and messages
void monitor::log( const QString& data, QCaAlarmInfo&, QCaDateTime& timeStamp, const unsigned int & )
{
    *stream << QString( "%1: %2   %3\n").arg( timeStamp.text() ).arg( pv ).arg( data );
    stream->flush();
}
