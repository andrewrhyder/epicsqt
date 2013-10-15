/*
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

// Floating specific wrapper for QCaObject.

#include <QEFloating.h>
#include <QtDebug>

/*
    ???
*/
QEFloating::QEFloating( QString recordName, QObject *eventObject,
                        QEFloatingFormatting *floatingFormattingIn,
                        unsigned int variableIndexIn ) : QCaObject( recordName, eventObject ) {
    initialise( floatingFormattingIn, variableIndexIn );
}
QEFloating::QEFloating( QString recordName, QObject *eventObject,
                        QEFloatingFormatting *floatingFormattingIn,
                        unsigned int variableIndexIn, UserMessage* userMessageIn ) : QCaObject( recordName, eventObject, userMessageIn ) {
    initialise( floatingFormattingIn, variableIndexIn );
}

/*
    Stream the QCaObject data through this class to generate floating data updates
*/
void QEFloating::initialise( QEFloatingFormatting* floatingFormattingIn,
                             unsigned int variableIndexIn ) {
    floatingFormat = floatingFormattingIn;
    variableIndex = variableIndexIn;

    QObject::connect( this, SIGNAL( connectionChanged(  QCaConnectionInfo& ) ),
                      this, SLOT( forwardConnectionChanged( QCaConnectionInfo& ) ) );

    QObject::connect( this, SIGNAL( dataChanged( const QVariant&, QCaAlarmInfo&, QCaDateTime& ) ),
                      this, SLOT( convertVariant( const QVariant&, QCaAlarmInfo&, QCaDateTime& ) ) );
}

/*
    Take a new floating value and write it to the database.
    The type of data formatted (text, floating, integer, etc) will be determined by the record data type,
    How the floating is parsed will be determined by the floating formatting. For example, floating to string may require always including a sign.
*/
void QEFloating::writeFloating( const double &data ) {
    writeData( floatingFormat->formatValue( data, getDataType() ) );
}

/*
    Slot to recieve data updates from the base QCaObject and generate floating updates.
*/
void QEFloating::convertVariant( const QVariant &value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp ) {

    if( value.type() == QVariant::List )
    {
        emit floatingArrayChanged( floatingFormat->formatFloatingArray( value ), alarmInfo, timeStamp, variableIndex );
    }
    else
    {
        emit floatingChanged( floatingFormat->formatFloating( value ), alarmInfo, timeStamp, variableIndex );
    }
}

/*
    Take a basic connection change and append variableIndex
*/
void QEFloating::forwardConnectionChanged( QCaConnectionInfo& connectionInfo) {
    emit floatingConnectionChanged( connectionInfo, variableIndex );
}

// end