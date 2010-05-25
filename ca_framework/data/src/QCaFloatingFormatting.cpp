/*! 
  \class QCaFloatingFormatting
  \version $Revision: #1 $
  \date $DateTime: 2009/11/23 08:44:03 $
  \author andrew.rhyder
  \brief Provides textual formatting for QCaFloating data.
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

#include <QCaFloatingFormatting.h>
#include <QtDebug>

#define MAXUINT (2^(sizeof(qulonglong)))

/*!
    ???
*/
QCaFloatingFormatting::QCaFloatingFormatting() {
    /// Default formatting properties.
    format = FORMAT_g;
    precision = 6;
}

/*!
    Generate a value given a floating point number, using formatting defined within this class.
    The formatting mainly applies if formatting as a string. For example, was is
    the number base? should a sign always be included? are leading zeros requried?
    The formatting could include properties related to other types. For example, generate
    an error if attempting to convert a negative floating point number to an unsigned integer.
*/
QVariant QCaFloatingFormatting::formatValue( const double &floatingValue, generic::generic_types valueType ) {
    switch( valueType ) {
        case generic::DOUBLE :
        {
            QVariant dValue( floatingValue );
            return dValue;
        }
        case generic::UNSIGNED_LONG :
        {
            qulonglong unsignedIntegerValue;
            if( floatingValue < 0 )
                unsignedIntegerValue = 0;
            else if( floatingValue > (double)MAXUINT )
                unsignedIntegerValue = MAXUINT;
            else
                unsignedIntegerValue = (qulonglong)floatingValue;

            QVariant ulValue( unsignedIntegerValue );
            return ulValue;
        }
        case generic::STRING :
        {
            QString string = QString::number( floatingValue, getFormatChar(), precision );
            QVariant sValue( string );
            return sValue;
        }
        case generic::UNKNOWN :
        {
        }
        default :
        {
            //qDebug() << "QCaFloatingFormatting::formatValue() Unknown value 'Generic' type: " << valueType;
            QVariant unknown;
            return unknown;
        }
    }
    //qDebug() << "QCaFloatingFormatting::formatValue() Unknown value 'Generic' type: " << valueType;
    QVariant unknown;
    return unknown;
}

/*!
    Generate an floating point number given a value, using formatting defined within this class.
*/
double QCaFloatingFormatting::formatFloating( const QVariant &value ) {
    // Determine the format from the variant type.
    // Only the types used to store ca data are used. any other type is considered a failure.
    switch( value.type() ) {
        case QVariant::Double :
        {
            return value.toDouble(); // No conversion requried. Stored in variant as required type
        }
        case QVariant::LongLong :
        {
            return formatFromInteger( value );
        }
        case QVariant::ULongLong :
        {
            return formatFromUnsignedInteger( value );
        }
        case QVariant::String :
        {
            return formatFromString( value );
        }
        default :
        {
            return formatFailure( QString( "Bug in QCaFloatingFormatting::formatFloating(). The QVariant type was not expected" ) );
        }
    }
}

/*!
    Format a variant value as a integer representation of a floating point number.
    Convert the variant value to a double. It may or may not be a double type variant. If it is - good,
    there will be no conversion problems.
*/
double QCaFloatingFormatting::formatFromInteger( const QVariant &value ) {
    // Extract the value as a double using whatever conversion the QVariant uses.
    //
    // Note, this will not pick up if the QVariant type is not one of the types used to represent CA data.
    // This is OK as it is not absolutely nessesary to do this sort of check at this point. Also the code is more robust as it will
    // work if the range of QVariant types used expands.
    // Note, this does not give us the freedom to specify what conversions should fail or succeed. For example, does QVariant::toDouble()
    // work if the value it holds is the string 1.234a, and should it?
    // If QVariant::toDouble() does not do exactly what is required, a switch statement for each of the types used to hold CA data
    // will need to be added and the conversions done  manually or using QVariant::toDouble() as required.
    bool convertOk;
    long iValue = value.toDouble( &convertOk );

    if( !convertOk )
        return formatFailure( QString( "Warning from QCaFloatingFormatting::formatFromFloating(). A variant could not be converted to a double." ) );

    return iValue;
}

/*!
    Format a variant value as a (signed) integer representation of a floating point number.
    Convert the variant value to a double. It may or may not be a double type variant. If it is - good,
    there will be no conversion problems.
*/
double QCaFloatingFormatting::formatFromUnsignedInteger( const QVariant &value ) {
    // Extract the value as a double using whatever conversion the QVariant uses.
    //
    // Note, this will not pick up if the QVariant type is not one of the types used to represent CA data.
    // This is OK as it is not absolutely nessesary to do this sort of check at this point. Also the code is more robust as it will
    // work if the range of QVariant types used expands.
    // Note, this does not give us the freedom to specify what conversions should fail or succeed. For example, does QVariant::toDouble()
    // work if the value it holds is the string 'twenty two' and should it?
    // If QVariant::toDouble() does not do exactly what is required, a switch statement for each of the types used to hold CA data
    // will need to be added and the conversions done  manually or using QVariant::toDouble() as required.
    bool convertOk;
    long lValue = value.toDouble( &convertOk );

    if( !convertOk )
        return formatFailure( QString( "Warning from QCaFloatingFormatting::formatFromUnsignedInteger(). A variant could not be converted to a double." ) );

    return lValue;
}

/*!
    Format a variant value as an integer representation of a string.
    Convert the variant value to an unsigned long. It may or may not be a ulonglong type variant. If it is - good,
    there will be no conversion problems.
*/
double QCaFloatingFormatting::formatFromString( const QVariant &value ) {
    // Extract the value as a long using whatever conversion the QVariant uses.
    //
    // Note, this will not pick up if the QVariant type is not one of the types used to represent CA data.
    // This is OK as it is not absolutely nessesary to do this sort of check at this point. Also the code is more robust as it will
    // work if the range of QVariant types used expands.
    // Note, this does not give us the freedom to specify what conversions should fail or succeed. For example, does QVariant::toLongLong()
    // work if the value it holds is the string 1.0001 and should it?
    // If QVariant::toLongLong() does not do exactly what is required, a switch statement for each of the types used to hold CA data
    // will need to be added and the conversions done  manually or using QVariant::toLongLong() as required.
    bool convertOk;
    long lValue = value.toDouble( &convertOk );

    if( !convertOk )
        return formatFailure( QString( "Warning from QCaFloatingFormatting::formatFromString(). A variant could not be converted to an double." ) );

    return lValue;
}

/*!
    Format a variant value as an integer representation of a time.
    Times are represented in CA data as an integer???
    Convert the variant value to an ??? (the type used to represent CA time). It may or may not be a ??? type variant. If it is - good,
    there will be no conversion problems.
*/
double QCaFloatingFormatting::formatFromTime( const QVariant &value ) {
    //??? what is the ca time format and how do you convert it to an double? Should there be conversion properties such as 'convert to minutes', 'convert to hours'.
    return value.toDouble();
}

/*!
    Do something with the fact that the value could not be formatted as requested.
*/
double QCaFloatingFormatting::formatFailure( QString message ) {
    // Log the format failure if required.
    qDebug() << message;

    // Return whatever is required for a formatting falure.
    return 0.0;
}

/*!
    Set the precision.
    Relevent when formatting the floating point number as a string.
*/
void QCaFloatingFormatting::setPrecision( unsigned int precisionIn ) {
        precision = precisionIn;
}

/*!
    Get the precision.
    Relevent when formatting the floating point number as a string.
*/
unsigned int QCaFloatingFormatting::getPrecision() {
    return precision;
}

/*!
    Set the format.
    Relevent when formatting the floating point number as a string.
*/
void QCaFloatingFormatting::setFormat( formats formatIn ) {
        format = formatIn;
}

/*!
    Get the format.
    Relevent when formatting the floating point number as a string.
*/
int QCaFloatingFormatting::getFormat() {
    return format;
}

/*!
    Get the format character required for the QString::number function.
    Relevent when formatting the floating point number as a string.
*/
char QCaFloatingFormatting::getFormatChar() {
    switch( format )
    {
        case FORMAT_e: return 'e'; break;
        case FORMAT_E: return 'E'; break;
        case FORMAT_f: return 'f'; break;
        case FORMAT_g: return 'g'; break;
        case FORMAT_G: return 'G'; break;
        default:       return 'g'; break;
    }
}
