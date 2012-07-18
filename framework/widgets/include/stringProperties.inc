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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef STRINGPROPERTIES_H
#define STRINGPROPERTIES_H

    // String formatting properties
    Q_PROPERTY(unsigned int precision READ getPrecision WRITE setPrecision)
    Q_PROPERTY(bool useDbPrecision READ getUseDbPrecision WRITE setUseDbPrecision)
    Q_PROPERTY(bool leadingZero READ getLeadingZero WRITE setLeadingZero)
    Q_PROPERTY(bool trailingZeros READ getTrailingZeros WRITE setTrailingZeros)
    Q_PROPERTY(bool addUnits READ getAddUnits WRITE setAddUnits)
    Q_PROPERTY(QString/*localEnumerationList*/ localEnumeration READ getLocalEnumeration WRITE setLocalEnumeration)

    Q_ENUMS(Formats)
    Q_PROPERTY(Formats format READ getFormatProperty WRITE setFormatProperty)
    enum Formats { Default          = QCaStringFormatting::FORMAT_DEFAULT,
                   Floating         = QCaStringFormatting::FORMAT_FLOATING,
                   Integer          = QCaStringFormatting::FORMAT_INTEGER,
                   UnsignedInteger  = QCaStringFormatting::FORMAT_UNSIGNEDINTEGER,
                   Time             = QCaStringFormatting::FORMAT_TIME,
                   LocalEnumeration = QCaStringFormatting::FORMAT_LOCAL_ENUMERATE };
    void setFormatProperty( Formats format ){ setFormat( (QCaStringFormatting::formats)format ); }
    Formats getFormatProperty(){ return (Formats)getFormat(); }

    Q_PROPERTY(unsigned int radix READ getRadix WRITE setRadix)

    Q_ENUMS(Notations)
    Q_PROPERTY(Notations notation READ getNotationProperty WRITE setNotationProperty)
    enum Notations { Fixed = QCaStringFormatting::NOTATION_FIXED,
                     Scientific   = QCaStringFormatting::NOTATION_SCIENTIFIC,
                     Automatic      = QCaStringFormatting::NOTATION_AUTOMATIC };
    void setNotationProperty( Notations notation ){ setNotation( (QCaStringFormatting::notations)notation ); }
    Notations getNotationProperty(){ return (Notations)getNotation(); }

    Q_ENUMS(ArrayActions)
    Q_PROPERTY(ArrayActions arrayAction READ getArrayActionProperty WRITE setArrayActionProperty)
    enum ArrayActions { Append = QCaStringFormatting::APPEND,
                        Ascii  = QCaStringFormatting::ASCII,
                        Index  = QCaStringFormatting::INDEX };
    void setArrayActionProperty( ArrayActions arrayAction ){ setArrayAction( (QCaStringFormatting::arrayActions)arrayAction ); }
    ArrayActions getArrayActionProperty(){ return (ArrayActions)getArrayAction(); }

    Q_PROPERTY(unsigned int arrayIndex READ getArrayIndex WRITE setArrayIndex)

#endif // STRINGPROPERTIES_H
