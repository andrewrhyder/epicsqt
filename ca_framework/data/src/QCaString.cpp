/*! 
  \class QCaString
  \version $Revision: #5 $
  \date $DateTime: 2009/07/28 17:01:05 $
  \author andrew.rhyder
  \brief String wrapper for QCaObject variant data.
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

#include <QCaString.h>

/*!

*/
QCaString::QCaString( QString recordName, QObject* eventObject,
                      QCaStringFormatting* newStringFormat,
                      unsigned int variableIndexIn ) : QCaObject( recordName, eventObject ) {
    initialise( newStringFormat, variableIndexIn );
}

QCaString::QCaString( QString recordName, QObject* eventObject,
                      QCaStringFormatting* newStringFormat,
                      unsigned int variableIndexIn, UserMessage* userMessageIn ) : QCaObject( recordName, eventObject, userMessageIn ) {
    initialise( newStringFormat, variableIndexIn );
}
/*!
    Stream the QCaObject data through this class to generate textual data
    updates.
*/
void QCaString::initialise( QCaStringFormatting* newStringFormat, unsigned int variableIndexIn ) {
    stringFormat = newStringFormat;
    variableIndex = variableIndexIn;

    QObject::connect( this, SIGNAL( dataChanged( const QVariant&, QCaAlarmInfo&, QCaDateTime& ) ),
                      this, SLOT( convertVariant( const QVariant&, QCaAlarmInfo&, QCaDateTime& ) ) );
}

/*!
    Take a new text value and write it to the database.
    The type of data formatted (text, floating, integer, etc) will be determined by the record data type,
    How the text is parsed will be determined by the string formatting.
    For example, assume the record is a floating point record and the text is formatted as an integer.
    The string is parsed as in integer (123 is OK, 123.456 would fail), then converted to a floating point number.
    The above example is pedantic
    if the string formatting
*/
void QCaString::writeString( const QString &data ) {
    writeData( stringFormat->formatValue( data ) );
}

/*!
    Take a new value from the database and emit a string,formatted
    as directed by the set of formatting information held by this class
*/
void QCaString::convertVariant( const QVariant& value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp ) {

    // Set up variable details used by some formatting options
    stringFormat->setDbEgu( getEgu() );
    stringFormat->setDbEnumerations( getEnumerations() );
    stringFormat->setDbPrecision( getPrecision() );

    // Format the data and send it
    emit stringChanged( stringFormat->formatString( value ), alarmInfo, timeStamp, variableIndex );
}
