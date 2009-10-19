/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/src/QCaIntegerFormatting.cpp $
 * $Revision: #1 $
 * $DateTime: 2009/07/14 15:59:56 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaIntegerFormatting
  \version $Revision: #1 $
  \date $DateTime: 2009/07/14 15:59:56 $
  \author andrew.rhyder
  \brief Provides textual formatting for QCaInteger data.
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

#include <QCaIntegerFormatting.h>
#include <QtDebug>

/*!
    ???
*/
QCaIntegerFormatting::QCaIntegerFormatting() {
    /// Default formatting properties.
    radix = 10;
}

/*!
    Generate a value given an integer, using formatting defined within this class.
    The formatting mainly applies if formatting as a string. For example, was is
    the number base? should a sign always be included? are leading zeros requried?
    The formatting could include properties related to other types. For example, generate
    an error if attempting to convert a negative integer to an unsigned integer.
*/
QVariant QCaIntegerFormatting::formatValue( const long &integerValue, generic::generic_types valueType ) {
    switch( valueType ) {
        case generic::DOUBLE :
        {
            QVariant dValue( (double)integerValue );
            return dValue;
        }
        case generic::UNSIGNED_LONG :
        {
            qulonglong unsignedIntegerValue;
            ( integerValue < 0 ) ? unsignedIntegerValue=0 : unsignedIntegerValue=integerValue;
            QVariant ulValue( unsignedIntegerValue );
            return ulValue;
        }
        case generic::STRING :
        {
            QString string = QString::number( integerValue, radix);
            QVariant sValue( string );
            return sValue;
        }
        case generic::UNKNOWN :
        {
        }
        default :
        {
            //qDebug() << "QCaIntegerFormatting::formatValue() Unknown value 'Generic' type: " << valueType;
            QVariant unknown;
            return unknown;
        }
    }
    //qDebug() << "QCaIntegerFormatting::formatValue() Unknown value 'Generic' type: " << valueType;
    QVariant unknown;
    return unknown;
}

/*!
    Generate an integer given a value, using formatting defined within this class.
*/
long QCaIntegerFormatting::formatInteger( const QVariant &value ) {
    // Determine the format from the variant type.
    // Only the types used to store ca data are used. any other type is considered a failure.
    switch( value.type() ) {
        case QVariant::Double :
        {
            return formatFromFloating( value );
        }
        case QVariant::LongLong :
        {
            return value.toLongLong(); // No conversion requried. Stored in variant as required type
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
            return formatFailure( QString( "Bug in QCaIntegerFormatting::formatInteger(). The QVariant type was not expected" ) );
        }
    }
}

/*!
    Format a variant value as an integer representation of a floating point number.
    Convert the variant value to a long. It may or may not be a longlong type variant. If it is - good,
    there will be no conversion problems.
*/
long QCaIntegerFormatting::formatFromFloating( const QVariant &value ) {
    // Extract the value as an integer using whatever conversion the QVariant uses.
    //
    // Note, this will not pick up if the QVariant type is not one of the types used to represent CA data.
    // This is OK as it is not absolutely nessesary to do this sort of check at this point. Also the code is more robust as it will
    // work if the range of QVariant types used expands.
    // Note, this does not give us the freedom to specify what conversions should fail or succeed. For example, does QVariant::toLongLong()
    // work if the value it holds is the string 1.234a, and should it?
    // If QVariant::toLongLong() does not do exactly what is required, a switch statement for each of the types used to hold CA data
    // will need to be added and the conversions done  manually or using QVariant::toLongLong() as required.
    bool convertOk;
    long iValue = value.toLongLong( &convertOk );

    if( !convertOk )
        return formatFailure( QString( "Warning from QCaIntegerFormatting::formatFromFloating(). A variant could not be converted to a long." ) );

    return iValue;
}

/*!
    Format a variant value as a (signed) integer representation of an unsigned integer.
    Convert the variant value to an unsigned long. It may or may not be a ulonglong type variant. If it is - good,
    there will be no conversion problems.
*/
long QCaIntegerFormatting::formatFromUnsignedInteger( const QVariant &value ) {
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
    long lValue = value.toLongLong( &convertOk );

    if( !convertOk )
        return formatFailure( QString( "Warning from QCaIntegerFormatting::formatFromUnsignedInteger(). A variant could not be converted to an unsigned long." ) );

    return lValue;
}

/*!
    Format a variant value as an integer representation of a string.
    Convert the variant value to an unsigned long. It may or may not be a ulonglong type variant. If it is - good,
    there will be no conversion problems.
*/
long QCaIntegerFormatting::formatFromString( const QVariant &value ) {
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
    long lValue = value.toLongLong( &convertOk );

    if( !convertOk )
        return formatFailure( QString( "Warning from QCaIntegerFormatting::formatFromUnsignedInteger(). A variant could not be converted to an unsigned long." ) );

    return lValue;
}

/*!
    Format a variant value as an integer representation of a time.
    Times are represented in CA data as an integer???
    Convert the variant value to an ??? (the type used to represent CA time). It may or may not be a ??? type variant. If it is - good,
    there will be no conversion problems.
*/
long QCaIntegerFormatting::formatFromTime( const QVariant &value ) {
    //??? what is the ca time format and how do you convert it to an integer? Should there be conversion properties such as 'convert to minutes', 'convert to hours'.
    return value.toLongLong();
}

/*!
    Do something with the fact that the value could not be formatted as requested.
*/
long QCaIntegerFormatting::formatFailure( QString message ) {
    // Log the format failure if required.
    qDebug() << message;

    // Return whatever is required for a formatting falure.
    return 0;
}

/*!
    Set the numer system base.
    Relevent when formatting the string as an interger.
    Any radix of 2 or more is accepted. Check the conversion code that uses this number to see what values are expected.
    At the time of writing (16/2/9) it is anticipated that floating point numbers will always be base 10 and integer numbers will
    be base 2, 8, 10, or 16.
    ??? if radix processing beocomes significant, create a radix class that can be used for both QCaIntegerFormatting and QCaStringFormatting.
*/
void QCaIntegerFormatting::setRadix( unsigned int radixIn ) {
    if( radixIn >= 2 )
        radix = radixIn;
}

/*!
    Get the numerical base. See setRadix() for the use of 'radix'.
*/
unsigned int QCaIntegerFormatting::getRadix() {
    return radix;
}
