/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/src/QCaStringFormatting.cpp $
 * $Revision: #8 $
 * $DateTime: 2009/07/27 16:34:58 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaStringFormatting
  \version $Revision: #8 $
  \date $DateTime: 2009/07/27 16:34:58 $
  \author andrew.rhyder
  \brief Format the string for QCaString data.
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

#include <QtDebug>
#include <QCaStringFormatting.h>

/*!
    Construction
*/
QCaStringFormatting::QCaStringFormatting() {
    // Set up the stream that will perform most conversions
    stream.setString( &outStr );

    // Set up default formatting behaviour
    useDbPrecision = true;
    precision = 4;
    leadingZero = true;
    trailingZeros = true;
    format = FORMAT_DEFAULT;
    stream.setIntegerBase( 10 );
    stream.setRealNumberNotation( QTextStream::FixedNotation );
    addUnits = true;

    // Initialise database information
    dbPrecision = 0;
    dbVariableIsStatField = false;
}

/*!
    Set up the precision specified by the database.
    This precision is used when formatting floating point numbers if 'useDbPrecision' is set
*/
void QCaStringFormatting::setDbPrecision( unsigned int dbPrecisionIn )
{
    dbPrecision = dbPrecisionIn;
}

/*!
    Set up the engineering units that will be added to or removed from strings if 'addUnits' flag is set
*/
void QCaStringFormatting::setDbEgu( QString egu )
{
    dbEgu = egu;
}

/*!
    Set up the enumeration values. Thses are used if avaiable if the formatting is FORMAT_DEFAULT
*/
void QCaStringFormatting::setDbEnumerations( QStringList enumerations )
{
    dbEnumerations = enumerations;
}

/*!
    Set up a flag indicating the variable represents a STAT field.
    This is required as formatting enumerated strings for the STAT field is a special case.
*/
void QCaStringFormatting::setDbVariableIsStatField( bool isStatField )
{
    dbVariableIsStatField = isStatField;
}


/*!
    Generate a value given a string, using formatting defined within this
    class.
*/
QVariant QCaStringFormatting::formatValue( const QString &text ) {
    //??? should know the record type (passed in to QCaStringFormatting::formatValue() )

    // Strip unit if present
    QString unitlessText = text.trimmed();
    if( addUnits )
    {
        if( dbEgu == unitlessText.right( dbEgu.length() ) )
          unitlessText.chop( dbEgu.length() );
    }

    //??? interpret the string according to the formatting. for example, if
    //??? formatting says unsigned integer, fail if it looks like a floating,
    //??? negative, text, etc.
    //??? then convert to the record type and return

    /// ??? Temporarily use the variants text convertion.
    QVariant value( unitlessText );

    return value;
}

/*!
    Generate a string given a value, using formatting defined within this class.
*/
QString QCaStringFormatting::formatString( const QVariant &value ) {
    // Initialise
    bool errorMessage = false;      // Set if an error message is the result
    outStr.clear();

    // Set the precision
    if( useDbPrecision )
        stream.setRealNumberPrecision( dbPrecision );
    else
        stream.setRealNumberPrecision( precision );

    // Format the value as requested
    switch( format )
    {
        // Determine the format from the variant type.
        // Only the types used to store ca data are used. any other type is
        // considered a failure.
        case FORMAT_DEFAULT :
        {
            bool haveEnumeratedString = false;  // Successfully converted the value to an enumerated string
            // If a list of enumerated strings is available, attempt to get an enumerated string
            if( dbEnumerations.size() )
            {
                // Ensure the input value can be used as an index into the list of enumerated strings
                bool convertOk;
                long lValue = value.toLongLong( &convertOk );
                if( convertOk && lValue >= 0 )
                {
                    // Get the appropriate enumerated string
                    if( lValue < dbEnumerations.size() )
                    {
                        outStr = dbEnumerations[lValue];
                        haveEnumeratedString = true;
                    }
                    // If the index was higher than the available enumerated strings, handle one special case:
                    // If the field is the STAT field, and the value is between 16 and 21, then use hard coded enumerated strings.
                    // This is due to a limit in EPICS. The STAT field which has 21 enumerated values, but only 16 enumerated strings can be included in the record.
                    else if( lValue >= 16 && lValue <= 21 && dbVariableIsStatField )
                    {
                        switch( lValue )
                        {
                            case 16:
                                outStr = "BAD_SUB";
                                haveEnumeratedString = true;
                                break;

                            case 17:
                                outStr = "UDF";
                                haveEnumeratedString = true;
                                break;

                            case 18:
                                outStr = "DISABLE";
                                haveEnumeratedString = true;
                                break;

                            case 19:
                                outStr = "SIMM";
                                haveEnumeratedString = true;
                                break;

                            case 20:
                                outStr = "READ_ACCESS";
                                haveEnumeratedString = true;
                                break;

                            case 21:
                                outStr = "WRITE_ACCESS";
                                haveEnumeratedString = true;
                                break;

                        }
                    }
                }
            }

            // If no list of enumerated strings was available, or a string could not be selected,
            // convert the value based on it's type.
            if( !haveEnumeratedString )
            {
                switch( value.type() ) {
                    case QVariant::Double:
                        formatFromFloating( value );
                        break;

                    case QVariant::LongLong:
                        formatFromInteger( value );
                        break;

                    case QVariant::ULongLong:
                        formatFromUnsignedInteger( value );
                        break;

                    case QVariant::String:
                        stream << value.toString(); // No conversion requried. Stored in variant as required type
                        break;

                    default:
                        formatFailure( QString( "Bug in QCaStringFormatting::formatString(). The QVariant type was not expected" ) );
                        errorMessage = true;
                        break;
                }
            }
            break;
        }

        // Format as requested, ignoring the database type
        case FORMAT_FLOATING:
            formatFromFloating( value );
            break;

         case FORMAT_INTEGER:
            formatFromInteger( value );
            break;

        case FORMAT_UNSIGNEDINTEGER:
            formatFromUnsignedInteger( value );
            break;

        case FORMAT_TIME:
            formatFromTime( value );
            break;


        // Don't know how to format.
        // This is a code error. All cases in QCaStringFormatting::formats should be catered for
        default:
            formatFailure( QString( "Bug in QCaStringFormatting::format(). The format type was not expected" ) );
            errorMessage = true;
            break;
    }

    // Add units if required, if there are any present, and if the text is not an error message
    int eguLen = dbEgu.length(); // ??? Why cant this be in the 'if' statement? If it is it never adds an egu
    if( addUnits && !errorMessage && eguLen )
        stream << " " << dbEgu;

    return outStr;
}

/*!
    Format a variant value as a string representation of a floating point
    number.
    First convert the variant value to a double. It may or may not be a
    floating point type variant. If it is - good,
    there will be no conversion problems.
    Then format it as a string using the formatting information stored in this
    class.
*/
void QCaStringFormatting::formatFromFloating( const QVariant &value ) {
    // Extract the value as a double using whatever conversion the QVariant uses.
    //
    // Note, this will not pick up if the QVariant type is not one of the types used to represent CA data.
    // This is OK as it is not absolutely nessesary to do this sort of check at this point. Also the code is more robust as it will
    // work if the range of QVariant types used expands.
    // Note, this does not give us the freedom to specify what conversions should fail or succeed. For example, does QVariant::toDouble()
    // work if the value it holds is the string 1.234 10^6, or does it work for both - 1.234 and -1.234, and should it?
    // If QVariant::toDouble() does not do exactly what is required, a switch statement for each of the types used to hold CA data
    // will need to be added and the conversions done  manually or using QVariant::toDouble() as required.
    bool convertOk;
    double dValue = value.toDouble( &convertOk );

    if( !convertOk ) {
        formatFailure( QString( "Warning from QCaStringFormatting::formatFromFloating(). A variant could not be converted to a long." ) );
        return;
    }

    // Generate the text
    stream << dValue;

    // Remove leading zero if required
    if( !leadingZero ) {
        if( outStr.left(2) == "0." )
            outStr = outStr.right( outStr.length()-1);
    }

    // Remove trailing zeros if required
    if( !trailingZeros ) {
        int numZeros = 0;
        for( int i = outStr.length()-1; i >= 0; i-- ) {
            if( outStr[i] != '0' )
                break;
            numZeros++;
        }
        outStr.chop( numZeros );
    }
}

/*!
    Format a variant value as a string representation of an integer.
    First convert the variant value to a long. It may or may not be a longlong
    type variant. If it is - good,
    there will be no conversion problems.
    Then format it as a string using the formatting information stored in this
    class.
*/
void QCaStringFormatting::formatFromInteger( const QVariant &value ) {
    // Extract the value as a long using whatever conversion the QVariant uses.
    //
    // Note, this will not pick up if the QVariant type is not one of the types used to represent CA data.
    // This is OK as it is not absolutely nessesary to do this sort of check at this point. Also the code is more robust as it will
    // work if the range of QVariant types used expands.
    // Note, this does not give us the freedom to specify what conversions should fail or succeed. For example, does QVariant::toLongLong()
    // work if the value it holds is the string 1.000 and should it?
    // If QVariant::toLongLong() does not do exactly what is required, a switch statement for each of the types used to hold CA data
    // will need to be added and the conversions done  manually or using QVariant::toLongLong() as required.
    bool convertOk;
    long lValue = value.toLongLong( &convertOk );

    if( !convertOk )
    {
        formatFailure( QString( "Warning from QCaStringFormatting::formatFromInteger(). A variant could not be converted to a long." ) );
        return;
    }

    // Generate the text
    stream << lValue;
}

/*!
    Format a variant value as a string representation of an unsigned integer.
    First convert the variant value to an unsigned long. It may or may not be a
    ulonglong type variant. If it is - good,
    there will be no conversion problems.
    Then format it as a string using the formatting information stored in this
    class.
*/
void QCaStringFormatting::formatFromUnsignedInteger( const QVariant &value ) {
    // Extract the value as an unsigned long using whatever conversion the QVariant uses.
    //
    // Note, this will not pick up if the QVariant type is not one of the types used to represent CA data.
    // This is OK as it is not absolutely nessesary to do this sort of check at this point. Also the code is more robust as it will
    // work if the range of QVariant types used expands.
    // Note, this does not give us the freedom to specify what conversions should fail or succeed. For example, does QVariant::toULongLong()
    // work if the value it holds is the string 1.000 and should it?
    // If QVariant::toULongLong() does not do exactly what is required, a switch statement for each of the types used to hold CA data
    // will need to be added and the conversions done  manually or using QVariant::toULongLong() as required.
    bool convertOk;
    unsigned long ulValue = value.toULongLong( &convertOk );

    if( !convertOk )
    {
        formatFailure( QString( "Warning from QCaStringFormatting::formatFromUnsignedInteger(). A variant could not be converted to an unsigned long." ) );
        return;
    }

    // Generate the text
    stream << ulValue;
}

/*!
    Format a variant value as a string representation of a time.
    Times are represented in CA data as an integer???
    First convert the variant value to an ??? (the type used to represent CA
    time). It may or may not be a ??? type variant. If it is - good,
    there will be no conversion problems.
    Then format it as a string using the formatting information stored in this
    class.
*/
void QCaStringFormatting::formatFromTime( const QVariant &value ) {
    if( value.type() == QVariant::String )
        stream << value.toString();
    else
        stream << QString( "notImplemented" ); //??? to do
}

/*!
    Do something with the fact that the value could not be formatted as
    requested.
*/
void QCaStringFormatting::formatFailure( QString message ) {
    // Log the format failure if required.
    //???
    qDebug() << message;

    // Return whatever is required for a formatting falure.
    stream << "---";
}

/*!
    Set the precision - the number of significant digits displayed when
    formatting a floating point number.
    Relevent when formatting the string as a floating point number.
    Note, this will only be used if 'useDbPrecision' is false.
*/
void QCaStringFormatting::setPrecision( unsigned int precisionIn ) {
    precision = precisionIn;
}

/*!
    Set or clear a flag to ignore the precision held by this class and use the
    precision read from the database record.
    The precision is the number of significant digits displayed when formatting
    a floating point number.
    Relevent when formatting the string as a floating point number.
*/
void QCaStringFormatting::setUseDbPrecision( bool useDbPrecisionIn ) {
    useDbPrecision = useDbPrecisionIn;
}

/*!
    Set or clear a flag to display a leading zero before a decimal point for
    floating point numbers between 1 and -1
    Relevent when formatting the string as a floating point number.
*/
void QCaStringFormatting::setLeadingZero( bool leadingZeroIn ) {
    leadingZero = leadingZeroIn;
}

/*!
    Set or clear a flag to display trailing zeros at the end up the precision for
    floating point
    Relevent when formatting the string as a floating point number.
*/
void QCaStringFormatting::setTrailingZeros( bool trailingZerosIn ) {
    trailingZeros = trailingZerosIn;
}

/*!
    Set the type of information being displayed (floating point number,
    date/time, etc).
*/
void QCaStringFormatting::setFormat( formats formatIn ) {
    format = formatIn;
}

/*!
    Set the numer system base.
    Relevent when formatting the string as an interger of floating point
    number.
    Any radix of 2 or more is accepted. Check the conversion code that uses
    this number to see what values are expected.
    At the time of writing (16/2/9) it is anticipated that floating point
    numbers will always be base 10 and integer numbers will
    be base 2, 8, 10, or 16.
*/
void QCaStringFormatting::setRadix( unsigned int radix ) {
    if( radix >= 2 )
        stream.setIntegerBase( radix );
}

/*!
    Set the notation (floating, scientific, or automatic)
    Note, the notation enumerations match the QTextStream RealNumberNotation values
*/
void QCaStringFormatting::setNotation( notations notation ) {
    stream.setRealNumberNotation( (QTextStream::RealNumberNotation)notation );
}

/*!
    Set or clear a flag to include the engineering units in a string
*/
void QCaStringFormatting::setAddUnits( bool AddUnitsIn ) {
    addUnits = AddUnitsIn;
}

/*!
    Get the precision. See setPrecision() for the use of 'precision'.
*/
unsigned int QCaStringFormatting::getPrecision() {
    return precision;
}

/*!
    Get the 'use DB precision' flag. See setUseDbPrecision() for the use of the
    'use DB precision' flag.
*/
bool QCaStringFormatting::getUseDbPrecision() {
    return useDbPrecision;
}

/*!
    Get the 'leading zero' flag. See setLeadingZero() for the use of the
    'leading zero' flag.
*/
bool QCaStringFormatting::getLeadingZero() {
    return leadingZero;
}

/*!
    Get the 'trailing zeros' flag. See setTrailingZeros() for the use of the
    'trailing zeros' flag.
*/
bool QCaStringFormatting::getTrailingZeros() {
    return trailingZeros;
}

/*!
    Get the type of information being formatted. See setFormat() for the use of
    the format type.
*/
QCaStringFormatting::formats QCaStringFormatting::getFormat() {
    return format;
}

/*!
    Get the numerical base. See setRadix() for the use of 'radix'.
*/
unsigned int QCaStringFormatting::getRadix() {
    return stream.integerBase();
}

/*!
    Return the floating point notation
*/
QCaStringFormatting::notations QCaStringFormatting::getNotation() {
    return (QCaStringFormatting::notations)stream.realNumberNotation();
}

/*!
    Get the 'include engineering units' flag.
*/
bool QCaStringFormatting::getAddUnits() {
    return addUnits;
}

