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

/*
  This class is used by QEWidgets when setting properties managed by the QEStringformatting class.
  This class calls matching set and get methods from the QEStringformatting class but also calls back
  the QEWidget when setting properties so the widget can update itself based on the changes.
 */

#include <QEStringFormattingMethods.h>

// String formatting properties

// precision
void QEStringFormattingMethods::setPrecision( int precision )
{
    stringFormatting.setPrecision( precision );
    stringFormattingChange();
}
int QEStringFormattingMethods::getPrecision()
{
    return stringFormatting.getPrecision();
}

// useDbPrecision
void QEStringFormattingMethods::setUseDbPrecision( bool useDbPrecision )
{
    stringFormatting.setUseDbPrecision( useDbPrecision);
    stringFormattingChange();
}
bool QEStringFormattingMethods::getUseDbPrecision()
{
    return stringFormatting.getUseDbPrecision();
}

// leadingZero
void QEStringFormattingMethods::setLeadingZero( bool leadingZero )
{
    stringFormatting.setLeadingZero( leadingZero );
    stringFormattingChange();
}
bool QEStringFormattingMethods::getLeadingZero()
{
    return stringFormatting.getLeadingZero();
}

// trailingZeros
void QEStringFormattingMethods::setTrailingZeros( bool trailingZeros )
{
    stringFormatting.setTrailingZeros( trailingZeros );
    stringFormattingChange();
}
bool QEStringFormattingMethods::getTrailingZeros()
{
    return stringFormatting.getTrailingZeros();
}

// addUnits
void QEStringFormattingMethods::setAddUnits( bool addUnits )
{
    stringFormatting.setAddUnits( addUnits );
    stringFormattingChange();
}
bool QEStringFormattingMethods::getAddUnits()
{
    return stringFormatting.getAddUnits();
}

// localEnumeration
void QEStringFormattingMethods::setLocalEnumeration( QString localEnumeration )
{
    stringFormatting.setLocalEnumeration( localEnumeration );
    stringFormattingChange();
}
QString QEStringFormattingMethods::getLocalEnumeration()
{
    return stringFormatting.getLocalEnumeration();
}

// format
void QEStringFormattingMethods::setFormat( QEStringFormatting::formats format )
{
    stringFormatting.setFormat( format );
    stringFormattingChange();
}
QEStringFormatting::formats QEStringFormattingMethods::getFormat()
{
    return stringFormatting.getFormat();
}

// radix
void QEStringFormattingMethods::setRadix( unsigned int radix )
{
    stringFormatting.setRadix( radix);
    stringFormattingChange();
}
unsigned int QEStringFormattingMethods::getRadix()
{
    return stringFormatting.getRadix();
}

// notation
void QEStringFormattingMethods::setNotation( QEStringFormatting::notations notation )
{
    stringFormatting.setNotation( notation );
    stringFormattingChange();
}
QEStringFormatting::notations QEStringFormattingMethods::getNotation()
{
    return stringFormatting.getNotation();
}

// arrayAction
void QEStringFormattingMethods::setArrayAction( QEStringFormatting::arrayActions arrayAction )
{
    stringFormatting.setArrayAction( arrayAction );
    stringFormattingChange();
}
QEStringFormatting::arrayActions QEStringFormattingMethods::getArrayAction()
{
    return stringFormatting.getArrayAction();
}

// arrayIndex
void QEStringFormattingMethods::setArrayIndex( unsigned int arrayIndex )
{
    stringFormatting.setArrayIndex( arrayIndex );
    stringFormattingChange();
}
unsigned int QEStringFormattingMethods::getArrayIndex()
{
    return stringFormatting.getArrayIndex();
}

