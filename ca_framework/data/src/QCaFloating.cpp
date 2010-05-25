/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/src/QCaFloating.cpp $
 * $Revision: #1 $
 * $DateTime: 2009/11/23 08:44:03 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaFloating
  \version $Revision: #1 $
  \date $DateTime: 2009/11/23 08:44:03 $
  \author andrew.rhyder
  \brief Floating specific wrapper for QCaObject.
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

#include <QCaFloating.h>
#include <QtDebug>

/*!
    ???
*/
QCaFloating::QCaFloating( QString recordName, QObject *eventObject,
                        QCaFloatingFormatting *floatingFormattingIn,
                        unsigned int variableIndexIn ) : QCaObject( recordName, eventObject ) {
    initialise( floatingFormattingIn, variableIndexIn );
}
QCaFloating::QCaFloating( QString recordName, QObject *eventObject,
                        QCaFloatingFormatting *floatingFormattingIn,
                        unsigned int variableIndexIn, UserMessage* userMessageIn ) : QCaObject( recordName, eventObject, userMessageIn ) {
    initialise( floatingFormattingIn, variableIndexIn );
}

/*!
    Stream the QCaObject data through this class to generate floating data updates
*/
void QCaFloating::initialise( QCaFloatingFormatting* floatingFormattingIn,
                             unsigned int variableIndexIn ) {
    floatingFormat = floatingFormattingIn;
    variableIndex = variableIndexIn;

    QObject::connect( this, SIGNAL( dataChanged( const QVariant&, QCaAlarmInfo&, QCaDateTime& ) ),
                      this, SLOT( convertVariant( const QVariant&, QCaAlarmInfo&, QCaDateTime& ) ) );
}

/*!
    Take a new floating value and write it to the database.
    The type of data formatted (text, floating, integer, etc) will be determined by the record data type,
    How the floating is parsed will be determined by the floating formatting. For example, floating to string may require always including a sign.
*/
void QCaFloating::writeFloating( const long &data ) {
    writeData( floatingFormat->formatValue( data, getDataType() ) );
}

/*!
    Slot to recieve data updates from the base QCaObject and generate floating updates.
*/
void QCaFloating::convertVariant( const QVariant &value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp ) {
    emit floatingChanged( floatingFormat->formatFloating( value ), alarmInfo, timeStamp, variableIndex );
}
