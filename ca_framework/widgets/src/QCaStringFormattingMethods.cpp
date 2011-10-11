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

/*!
  This class is used by QCaWidgets when setting properties managed by the QCaStringformatting class.
  This class calls matching set and get methods from the QCaStringformatting class but also calls back
  the QCawidget when setting properties so the widget can update itself based on the changes.
 */

#include <QCaStringFormattingMethods.h>

// String formatting properties

// precision
void QCaStringFormattingMethods::setPrecision( unsigned int precision )
{
    stringFormatting.setPrecision( precision );
    stringFormattingChange();
}
unsigned int QCaStringFormattingMethods::getPrecision()
{
    return stringFormatting.getPrecision();
}

// useDbPrecision
void QCaStringFormattingMethods::setUseDbPrecision( bool useDbPrecision )
{
    stringFormatting.setUseDbPrecision( useDbPrecision);
    stringFormattingChange();
}
bool QCaStringFormattingMethods::getUseDbPrecision()
{
    return stringFormatting.getUseDbPrecision();
}

// leadingZero
void QCaStringFormattingMethods::setLeadingZero( bool leadingZero )
{
    stringFormatting.setLeadingZero( leadingZero );
    stringFormattingChange();
}
bool QCaStringFormattingMethods::getLeadingZero()
{
    return stringFormatting.getLeadingZero();
}

// trailingZeros
void QCaStringFormattingMethods::setTrailingZeros( bool trailingZeros )
{
    stringFormatting.setTrailingZeros( trailingZeros );
    stringFormattingChange();
}
bool QCaStringFormattingMethods::getTrailingZeros()
{
    return stringFormatting.getTrailingZeros();
}

// addUnits
void QCaStringFormattingMethods::setAddUnits( bool addUnits )
{
    stringFormatting.setAddUnits( addUnits );
    stringFormattingChange();
}
bool QCaStringFormattingMethods::getAddUnits()
{
    return stringFormatting.getAddUnits();
}

// localEnumeration
void QCaStringFormattingMethods::setLocalEnumeration( QString localEnumeration )
{
    stringFormatting.setLocalEnumeration( localEnumeration );
    stringFormattingChange();
}
QString QCaStringFormattingMethods::getLocalEnumeration()
{
    return stringFormatting.getLocalEnumeration();
}

// format
void QCaStringFormattingMethods::setFormat( QCaStringFormatting::formats format )
{
    stringFormatting.setFormat( format );
    stringFormattingChange();
}
QCaStringFormatting::formats QCaStringFormattingMethods::getFormat()
{
    return stringFormatting.getFormat();
}

// radix
void QCaStringFormattingMethods::setRadix( unsigned int radix )
{
    stringFormatting.setRadix( radix);
    stringFormattingChange();
}
unsigned int QCaStringFormattingMethods::getRadix()
{
    return stringFormatting.getRadix();
}

// notation
void QCaStringFormattingMethods::setNotation( QCaStringFormatting::notations notation )
{
    stringFormatting.setNotation( notation );
    stringFormattingChange();
}
QCaStringFormatting::notations QCaStringFormattingMethods::getNotation()
{
    return stringFormatting.getNotation();
}

// arrayAction
void QCaStringFormattingMethods::setArrayAction( QCaStringFormatting::arrayActions arrayAction )
{
    stringFormatting.setArrayAction( arrayAction );
    stringFormattingChange();
}
QCaStringFormatting::arrayActions QCaStringFormattingMethods::getArrayAction()
{
    return stringFormatting.getArrayAction();
}

// arrayIndex
void QCaStringFormattingMethods::setArrayIndex( unsigned int arrayIndex )
{
    stringFormatting.setArrayIndex( arrayIndex );
    stringFormattingChange();
}
unsigned int QCaStringFormattingMethods::getArrayIndex()
{
    return stringFormatting.getArrayIndex();
}

