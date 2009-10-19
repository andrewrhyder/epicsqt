/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/applications/QCaMonitorApp/include/monitor.h $
 * $Revision: #2 $ 
 * $DateTime: 2009/07/28 17:01:05 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class Monitor
  \version $Revision: #2 $
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

#ifndef MONITOR_H
#define MONITOR_H

#include <QObject>
#include <QCaString.h>

/*!
  Class to monitor a PV. All data updats and messages are timestamped and written to std out
  */
class monitor : public QObject
{
      Q_OBJECT

public:
    monitor( QString pvIn );

private:
    QCaString* source;
    UserMessage messages;
    QCaStringFormatting formatting;
    QString pv;
    QTextStream* stream;

private slots:
    void log( const QString& data, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp, const unsigned int & );

};

#endif // MONITOR_H
