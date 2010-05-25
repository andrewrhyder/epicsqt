/*! 
  \class QCaInteger
  \version $Revision: #3 $
  \date $DateTime: 2009/07/28 17:01:05 $
  \author andrew.rhyder
  \brief Integer specific wrapper for QCaObject.
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

#include <QCaInteger.h>
#include <QtDebug>

/*!
    ???
*/
QCaInteger::QCaInteger( QString recordName, QObject *eventObject,
                        QCaIntegerFormatting *integerFormattingIn,
                        unsigned int variableIndexIn ) : QCaObject( recordName, eventObject ) {
    initialise( integerFormattingIn, variableIndexIn );
}
QCaInteger::QCaInteger( QString recordName, QObject *eventObject,
                        QCaIntegerFormatting *integerFormattingIn,
                        unsigned int variableIndexIn, UserMessage* userMessageIn ) : QCaObject( recordName, eventObject, userMessageIn ) {
    initialise( integerFormattingIn, variableIndexIn );
}

/*!
    Stream the QCaObject data through this class to generate integer data updates
*/
void QCaInteger::initialise( QCaIntegerFormatting* integerFormattingIn,
                             unsigned int variableIndexIn ) {
    integerFormat = integerFormattingIn;
    variableIndex = variableIndexIn;

    QObject::connect( this, SIGNAL( dataChanged( const QVariant&, QCaAlarmInfo&, QCaDateTime& ) ),
                      this, SLOT( convertVariant( const QVariant&, QCaAlarmInfo&, QCaDateTime& ) ) );
}

/*!
    Take a new integer value and write it to the database.
    The type of data formatted (text, floating, integer, etc) will be determined by the record data type,
    How the integer is parsed will be determined by the integer formatting. For example, integer to string may require always including a sign.
*/
void QCaInteger::writeInteger( const long &data ) {
    writeData( integerFormat->formatValue( data, getDataType() ) );
}

/*!
    Slot to recieve data updates from the base QCaObject and generate integer updates.
*/
void QCaInteger::convertVariant( const QVariant &value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp ) {
    emit integerChanged( integerFormat->formatInteger( value ), alarmInfo, timeStamp, variableIndex );
}
