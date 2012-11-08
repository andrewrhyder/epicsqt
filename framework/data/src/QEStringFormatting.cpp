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

#include <math.h>
#include <QtDebug>
#include <QEStringFormatting.h>

/*
    Construction
*/
QEStringFormatting::QEStringFormatting() {
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
    arrayAction = ASCII;//INDEX;
    arrayIndex = 0;

    // Initialise database information
    dbPrecision = 0;
    dbVariableIsStatField = false;
}

/*
    Set up the precision specified by the database.
    This precision is used when formatting floating point numbers if 'useDbPrecision' is set
*/
void QEStringFormatting::setDbPrecision( unsigned int dbPrecisionIn )
{
    dbPrecision = dbPrecisionIn;
}

/*
    Set up the engineering units that will be added to or removed from strings if 'addUnits' flag is set
*/
void QEStringFormatting::setDbEgu( QString egu )
{
    dbEgu = egu;
}

/*
    Set up the enumeration values. Thses are used if avaiable if the formatting is FORMAT_DEFAULT
*/
void QEStringFormatting::setDbEnumerations( QStringList enumerations )
{
    dbEnumerations = enumerations;
}

/*
    Set up a flag indicating the variable represents a STAT field.
    This is required as formatting enumerated strings for the STAT field is a special case.
*/
void QEStringFormatting::setDbVariableIsStatField( bool isStatField )
{
    dbVariableIsStatField = isStatField;
}


/*
    Generate a value given a string, using formatting defined within this
    class.
*/
QVariant QEStringFormatting::formatValue( const QString &text ) {
    //??? should know the record type (passed in to QEStringFormatting::formatValue() )

    // Strip unit if present
    QString unitlessText = text;
    if( addUnits )
    {
        if( dbEgu == unitlessText.right( dbEgu.length() ) )
          unitlessText.chop( dbEgu.length() );
    }

    //??? interpret the string according to the formatting. for example, if
    //??? formatting says unsigned integer, fail if it looks like a floating,
    //??? negative, text, etc.
    //??? then convert to the record type and return

    // ??? Temporarily use the variants text convertion.
    QVariant value( unitlessText );

    return value;
}

/*
    Generate a string given a value, using formatting defined within this class.
*/
QString QEStringFormatting::formatString( const QVariant &value ) {
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
                    case QVariant::Int:
                        formatFromInteger( value );
                        break;

                    case QVariant::ULongLong:
                    case QVariant::UInt:
                        formatFromUnsignedInteger( value );
                        break;

                    case QVariant::String:
                        stream << value.toString(); // No conversion requried. Stored in variant as required type
                        break;

                    case QVariant::List :
                        {
                            // Get the list
                            const QVariantList valueArray = value.toList();

                            // Add nothing to the stream if a value beyond the end of the list has been requested
                            if( arrayAction == INDEX && arrayIndex >= (unsigned int)(valueArray.count()) )
                            {
                                break;
                            }

                            switch( valueArray[0].type() )
                            {
                                case QVariant::Double:
                                    //???!!! ignores arrayAction and arrayIndex See uint and ulonglong below
                                    formatFromFloating( valueArray[0].toDouble() );
                                    break;

                                case QVariant::LongLong:
                                case QVariant::Int:
                                    //???!!! ignores arrayAction and arrayIndex See uint and ulonglong below
                                    formatFromInteger( valueArray[0] );
                                    break;

                                case QVariant::ULongLong:
                                case QVariant::UInt:
                                    switch( arrayAction )
                                    {
                                        case APPEND:
                                            for( int i = 0; i < valueArray.count(); i++ )
                                            {
                                                formatFromUnsignedInteger( valueArray[i] );
                                                stream << " ";
                                            }
                                            break;

                                        case ASCII:
                                            {
                                                // Translate all non printing characters to '?' except for trailing zeros (ignore them)
                                                int nonZeroLen = 0;  // Length before trailing zeros
                                                int len = valueArray.count();
                                                for( int i = 0; i < len; i++ )
                                                {
                                                    if( valueArray[i].toInt() != 0 )
                                                    {
                                                        nonZeroLen = i;
                                                    }
                                                }
                                                for( int i = 0; i <= nonZeroLen; i++ )
                                                {
                                                    if( valueArray[i].toInt() < ' ' || valueArray[i].toInt() > '~' )
                                                    {
                                                        stream << "?";
                                                    }
                                                    else
                                                    {
                                                        stream << valueArray[i].toChar();
                                                    }
                                                }
                                            }
                                            break;

                                        case INDEX:
                                            formatFromUnsignedInteger( valueArray[arrayIndex] );
                                            break;
                                    }
                                    break;

                                case QVariant::String:
                                    //???!!! ignores arrayAction and arrayIndex See uint and ulonglong above
                                    stream << valueArray[0].toString(); // No conversion requried. Stored in variant as required type
                                    break;

                                default:
                                    formatFailure( QString( "Bug in QEStringFormatting::formatString(). The QVariant type was not expected" ) );
                                    errorMessage = true;
                                    break;

                            }
                        }
                        break;

                    default:
                        formatFailure( QString( "Bug in QEStringFormatting::formatString(). The QVariant type was not expected" ) );
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

        case FORMAT_LOCAL_ENUMERATE:
            formatFromEnumeration( value );
            break;

        case FORMAT_TIME:
            formatFromTime( value );
            break;


        // Don't know how to format.
        // This is a code error. All cases in QEStringFormatting::formats should be catered for
        default:
            formatFailure( QString( "Bug in QEStringFormatting::format(). The format type was not expected" ) );
            errorMessage = true;
            break;
    }

    // Add units if required, if there are any present, and if the text is not an error message
    int eguLen = dbEgu.length(); // ??? Why cant this be in the 'if' statement? If it is it never adds an egu
    if( addUnits && !errorMessage && eguLen && (format != FORMAT_TIME))
        stream << " " << dbEgu;

    return outStr;
}

/*
    Format a variant value as a string representation of a floating point
    number.
    First convert the variant value to a double. It may or may not be a
    floating point type variant. If it is - good,
    there will be no conversion problems.
    Then format it as a string using the formatting information stored in this
    class.
*/
void QEStringFormatting::formatFromFloating( const QVariant &value ) {
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
        formatFailure( QString( "Warning from QEStringFormatting::formatFromFloating(). A variant could not be converted to a long." ) );
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

/*
    Format a variant value as a string representation of a signed integer.
    This method was written to convert a QVariant of type LongLong, but should cope with a variant of any type.
    First convert the variant value to a long. It may or may not be a longlong type variant. If it is - good,
    there will be no conversion problems.
    Then format it as a string using the formatting information stored in this class.
*/
void QEStringFormatting::formatFromInteger( const QVariant &value ) {
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
        formatFailure( QString( "Warning from QEStringFormatting::formatFromInteger(). A variant could not be converted to a long." ) );
        return;
    }

    // Generate the text
    stream << lValue;
}

/*
    Format a variant value as a string representation of an unsigned integer.
    This method was written to convert a QVariant of type ULongLong, but should cope with a variant of any type.
    First convert the variant value to an unsigned long. It may or may not be a ulonglong type variant. If it is - good,
    there will be no conversion problems.
    Then format it as a string using the formatting information stored in this class.
*/
void QEStringFormatting::formatFromUnsignedInteger( const QVariant &value ) {
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
        formatFailure( QString( "Warning from QEStringFormatting::formatFromUnsignedInteger(). A variant could not be converted to an unsigned long." ) );
        return;
    }

    // Generate the text
    stream << ulValue;
}

/*
    Format a variant value using local enumeration list.
    If the value is numeric, then the value is compared to the numeric interpretation of the enumeration values,
    if the value is textual, then the value is compared to the textual enumeration values.
*/
void QEStringFormatting::formatFromEnumeration( const QVariant &value ) {

    bool isDouble;
    double dValue;
    QString sValue;

    dValue = 0;

    // If it is a double, use it as a double.
    // If it is a string, use it as a string.
    // If it is anything else, try to convert it to a double, else a string.
    switch( value.type() )
    {
        case QVariant::Double:
            dValue = value.toDouble();
            isDouble = true;
            break;

        case QVariant::String:
            sValue = value.toString();
            isDouble = false;
            break;

        default:
            bool convertOk;
            dValue = value.toDouble( &convertOk );
            if( convertOk )
            {
                isDouble = true;
            }
            else
            {
                sValue = value.toString();
                isDouble = false;
            }
            break;
    }

    // Search for a matching value in the list of local enumerated strings
    int i;
    for( i = 0; i < localEnumeration.size(); i++ )
    {
        // Determine if the value matches an enumeration
        bool match = false;

#define LOCAL_ENUM_SEARCH( VALUE )             \
        switch( localEnumeration[i].op )   \
        {                                  \
            case localEnumerationItem::LESS:          if( VALUE <  localEnumeration[i].VALUE ) match = true;  break; \
            case localEnumerationItem::LESS_EQUAL:    if( VALUE <= localEnumeration[i].VALUE ) match = true;  break; \
            case localEnumerationItem::EQUAL:         if( VALUE == localEnumeration[i].VALUE ) match = true;  break; \
            case localEnumerationItem::NOT_EQUAL:     if( VALUE != localEnumeration[i].VALUE ) match = true;  break; \
            case localEnumerationItem::GREATER_EQUAL: if( VALUE >= localEnumeration[i].VALUE ) match = true;  break; \
            case localEnumerationItem::GREATER:       if( VALUE >  localEnumeration[i].VALUE ) match = true;  break; \
            case localEnumerationItem::ALWAYS:                                                 match = true;  break; \
            default:                                                                           match = false; break; \
        }

        if( isDouble )
        {
            LOCAL_ENUM_SEARCH( dValue );
        }
        else
        {
            LOCAL_ENUM_SEARCH( sValue );
        }

        // If the value does match, use the enumeration value
        if( match )
        {
            stream << localEnumeration[i].text;
            break;
        }
    }

    // If no match was found, generate the text directly from the value
    if( i >= localEnumeration.size() )
    {
        if( sValue.isEmpty() )
            stream << dValue;
        else
            stream << sValue;
    }
}

/*
    Format a variant value as a string representation of time.
    This method was written to convert a QVariant of type ??? (the type used to represent times in CA),
    but should cope with a variant of any type.
*/
void QEStringFormatting::formatFromTime( const QVariant &value ) {
    bool okay;
    double seconds;
    double time;
    QString sign;
    int days;
    int hours;
    int mins;
    int secs;
    int nanoSecs;
    QString image;
    int effectivePrecision;
    QString fraction;


    if( value.type() == QVariant::String )
        stream << value.toString();
    else {
        seconds = value.toDouble(&okay);
        if (okay) {
           if (seconds >= 0.0) {
              time = seconds;
              sign= "";
           } else {
              time = -seconds;
              sign= "-";
           }

           #define EXTRACT(item, spi) { item = int (floor (time / spi)); time = time - (spi * item); }

           EXTRACT (days, 86400.0);
           EXTRACT (hours, 3600.0);
           EXTRACT (mins, 60.0);
           EXTRACT (secs, 1.0);
           EXTRACT (nanoSecs, 1.0E-9);

           #undef EXTRACT

           image.sprintf ("%d %02d:%02d:%02d", days, hours, mins, secs);

           // Select data base or user precision as appropriate.
           //
           effectivePrecision = useDbPrecision ? dbPrecision : precision;
           if (effectivePrecision > 9) effectivePrecision = 9;

           if (effectivePrecision > 0) {
              fraction.sprintf (".%09d", nanoSecs);
              fraction.truncate( effectivePrecision + 1 );
           } else {
              fraction = "";
           }

           stream << sign << image << fraction;

        } else {
            stream << QString( "not a valid numeric" );
        }
    }
}

/*
    Do something with the fact that the value could not be formatted as
    requested.
*/
void QEStringFormatting::formatFailure( QString message ) {
    // Log the format failure if required.
    //???
    qDebug() << message;

    // Return whatever is required for a formatting falure.
    stream << "---";
}

//========================================================================================
// 'Set' formatting configuration methods

/*
    Set the precision - the number of significant digits displayed when
    formatting a floating point number.
    Relevent when formatting the string as a floating point number.
    Note, this will only be used if 'useDbPrecision' is false.
*/
void QEStringFormatting::setPrecision( int precisionIn ) {
    precision = precisionIn;
    // Ensure rangeis sensible.
    //
    if (precision < 0) precision = 0;
    if (precision > 18) precision = 18;
}

/*
    Set or clear a flag to ignore the precision held by this class and use the
    precision read from the database record.
    The precision is the number of significant digits displayed when formatting
    a floating point number.
    Relevent when formatting the string as a floating point number.
*/
void QEStringFormatting::setUseDbPrecision( bool useDbPrecisionIn ) {
    useDbPrecision = useDbPrecisionIn;
}

/*
    Set or clear a flag to display a leading zero before a decimal point for
    floating point numbers between 1 and -1
    Relevent when formatting the string as a floating point number.
*/
void QEStringFormatting::setLeadingZero( bool leadingZeroIn ) {
    leadingZero = leadingZeroIn;
}

/*
    Set or clear a flag to display trailing zeros at the end up the precision for
    floating point
    Relevent when formatting the string as a floating point number.
*/
void QEStringFormatting::setTrailingZeros( bool trailingZerosIn ) {
    trailingZeros = trailingZerosIn;
}

/*
    Set the type of information being displayed (floating point number,
    date/time, etc).
*/
void QEStringFormatting::setFormat( formats formatIn ) {
    format = formatIn;
}

/*
    Set the numer system base.
    Relevent when formatting the string as an interger of floating point
    number.
    Any radix of 2 or more is accepted. Check the conversion code that uses
    this number to see what values are expected.
    At the time of writing (16/2/9) it is anticipated that floating point
    numbers will always be base 10 and integer numbers will
    be base 2, 8, 10, or 16.
*/
void QEStringFormatting::setRadix( unsigned int radix ) {
    if( radix >= 2 )
        stream.setIntegerBase( radix );
}

/*
    Set the notation (floating, scientific, or automatic)
    Note, the notation enumerations match the QTextStream RealNumberNotation values
*/
void QEStringFormatting::setNotation( notations notation ) {
    stream.setRealNumberNotation( (QTextStream::RealNumberNotation)notation );
}

/*
    Set how arrays are converted to text (Treates as an array of ascii characters, an array of values, etc)
*/
void QEStringFormatting::setArrayAction( arrayActions arrayActionIn ) {
    arrayAction = arrayActionIn;
}

/*
    Set which value from an array is formatted (not relevent when the array is processed as ascii)
*/
void QEStringFormatting::setArrayIndex( unsigned int arrayIndexIn ) {
    arrayIndex = arrayIndexIn;
}

/*
    Set or clear a flag to include the engineering units in a string
*/
void QEStringFormatting::setAddUnits( bool AddUnitsIn ) {
    addUnits = AddUnitsIn;
}

/*
    Set the string used to specify local enumeration.
    This is used when a value is to be enumerated and the value is either not the VAL field,
    or the database does not provide any enumeration, or the database enumeration is not appropriate
*/
void QEStringFormatting::setLocalEnumeration( QString/*localEnumerationList*/ localEnumerationIn ) {

    // Save the original local enumeration string.
    // This is returned when the enumeration is requested as a property.
    localEnumerationString = localEnumerationIn;

    // Parse the local enumeration string.
    //
    // Format is:
    //
    //  [[<|<=|=|!=|>=|>]value1|*] : string1 , [[<|<=|=|!=|>=|>]value2|*] : string2 , [[<|<=|=|!=|>=|>]value3|*] : string3 , ...
    //
    // Where:
    //   <  Less than
    //   <= Less than or equal
    //   =  Equal (default if no operator specified)
    //   >= Greather than or equal
    //   >  Greater than
    //   *  Always match (used to specify default text)
    //
    // Values may be numeric or textual
    // Values do not have to be in any order, but first match wins
    // Values may be quoted
    // Strings may be quoted
    // Consecutive values do not have to be present.
    // Operator is assumed to be equality if not present.
    // White space is ignored except within quoted strings.
    // \n may be included in a string to indicate a line break
    //
    // Examples are:
    //
    // 0:Off,1:On
    // 0 : "Pump Running", 1 : "Pump not running"
    // 0:"", 1:"Warning!\nAlarm"
    // <2:"Value is less than two", =2:"Value is equal to two", >2:"Value is grater than 2"
    // 3:"Beamline Available", *:""
    // "Pump Off":"OH NO!, the pump is OFF!","Pump On":"It's OK, the pump is on"
    //
    // The data value is converted to a string if no enumeration for that value is available.
    // For example, if the local enumeration is '0:off,1:on', and a value of 10 is processed, the text generated is '10'.
    // If a blank string is required, this should be explicit. for example, '0:off,1:on,10:""'

    // A range of numbers can be covered by a pair of values as in the following example: >=4:"Between 4 and 8",<=8:"Between 4 and 8"

    localEnumerationItem item;

    enum states { STATE_START,

                  STATE_OPERATOR,

                  STATE_START_VALUE_QUOTE,
                  STATE_UNQUOTED_VALUE,
                  STATE_QUOTED_VALUE,
                  STATE_END_VALUE_QUOTE,

                  STATE_DELIMITER,

                  STATE_START_TEXT_QUOTE,
                  STATE_UNQUOTED_TEXT,
                  STATE_QUOTED_TEXT,
                  STATE_END_TEXT_QUOTE,

                  STATE_COMMA,

                  STATE_END };

    int start = 0;                          // Index into enumeration text of current item of interest.
    int len = 0;                            // Length of current item of interest
    int state = STATE_OPERATOR;             // Current state of finite state table
    int size = localEnumerationIn.size();   // Length of local enumeration string to be processed

    // Start with no enumerations
    localEnumeration.clear();

    // Process the enumeration text using a finite state table.
    while( start+len <= size )
    {
        switch( state )
        {
            // Reading the optional operator before the value and it's enumeration string.  For example, the '=' in '0=on,1=off'
            case STATE_OPERATOR:
                // If nothing left, finish
                if( start >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If haven't started yet, skip white space
                if( len == 0 && localEnumerationIn[start] == ' ' )
                {
                    start++;
                    break;
                }

                // If more operator characters, continue
                if( localEnumerationIn[start+len] == '>' ||
                    localEnumerationIn[start+len] == '<' ||
                    localEnumerationIn[start+len] == '=' ||
                    localEnumerationIn[start+len] == '!' ||
                    localEnumerationIn[start+len] == '*' )
                {
                    len++;
                    break;
                }

                // If an operator is found, use it
                if( len )
                {
                    // Assume operation will not be valid
                    item.op = localEnumerationItem::UNKNOWN;

                    // Interpret operator
                    switch( len )
                    {
                        case 1: // single character operator <, >, =
                            switch( localEnumerationIn[start].toAscii() )
                            {
                                case '<': item.op = localEnumerationItem::LESS;    break;
                                case '>': item.op = localEnumerationItem::GREATER; break;
                                case '=': item.op = localEnumerationItem::EQUAL;   break;
                                case '*': item.op = localEnumerationItem::ALWAYS;   break;
                            }
                            break;

                        case 2: // double character operator (always ending in '=') <=, >=, !=
                            if( localEnumerationIn[start+1] == '=' )
                            {
                                switch( localEnumerationIn[start].toAscii() )
                                {
                                    case '<': item.op = localEnumerationItem::LESS_EQUAL;    break;
                                    case '>': item.op = localEnumerationItem::GREATER_EQUAL; break;
                                    case '!': item.op = localEnumerationItem::NOT_EQUAL;     break;
                                }
                            }
                            break;

                        default:
                            // Error do no more
                            state = STATE_END;
                            break;
                    }

                    // Step over operator onto the value if a comparison is to be made, or the text is there is no value expected
                    start += len;
                    len = 0;
                    if( item.op == localEnumerationItem::ALWAYS )
                        state = STATE_DELIMITER;
                    else
                        state = STATE_START_VALUE_QUOTE;
                    break;
                }

                // No operator - assume equality
                item.op = localEnumerationItem::EQUAL;
                state = STATE_START_VALUE_QUOTE;
                break;

            // Where an enumerations value is quoted, handle the opening quotation mark.
            // For example, the first quote in "Pump Off"=off,"Pump On"="on"
            case STATE_START_VALUE_QUOTE:
                // If nothing left, finish
                if( start >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If haven't started yet, skip white space
                if( localEnumerationIn[start] == ' ' )
                {
                    start++;
                    break;
                }

                // If found '"' use it
                if( localEnumerationIn[start] == '"' )
                {
                    start++;
                    len = 0;
                    state = STATE_QUOTED_VALUE;
                    break;
                }

                // No quote found, assume unquoted text instead
                state = STATE_UNQUOTED_VALUE;
                break;

                // Reading a value. For example, the '0' in '0:on,1:off'
            case STATE_QUOTED_VALUE:
                // If nothing left, finish
                if( start+len >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If have all value, note it
                if( localEnumerationIn[start+len] == '"' )
                {
                    item.sValue = localEnumerationIn.mid( start, len );
                    start += len;
                    len = 0;
                    item.dValue = item.sValue.toDouble();
                    state = STATE_END_VALUE_QUOTE;
                    break;
                }

                // Extend the text
                len++;
                break;

                // Reading a value. For example, the '0' in '0:on,1:off'
            case STATE_UNQUOTED_VALUE:
                // If nothing left, finish
                if( start+len >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If haven't started yet, skip white space
                if( len == 0 && localEnumerationIn[start] == ' ' )
                {
                    start++;
                    break;
                }

                // If more characters, continue
                if( localEnumerationIn[start+len] != ' ' && localEnumerationIn[start+len] != ':' )
                {
                    len++;
                    break;
                }

                // If have a value, save it
                if( len )
                {
                    item.sValue = localEnumerationIn.mid( start, len );
                    item.dValue = item.sValue.toDouble();
                    start += len;
                    len = 0;
                    state = STATE_DELIMITER;
                    break;
                }

                // Error do no more
                state = STATE_END;
                break;

            // Where an enumerations value is quoted, handle the closing quotation mark.
            // For example, the second quote in "Pump Off"=off,"Pump On"="on"
            case STATE_END_VALUE_QUOTE:
                // If nothing left, finish
                if( start >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If found '"' use it
                if( localEnumerationIn[start] == '"' )
                {
                    start++;
                    len = 0;
                    state = STATE_DELIMITER;
                    break;
                }

                // Error do no more
                state = STATE_END;
                break;

            // Reading the delimiter between value and text.
            case STATE_DELIMITER:
                // If nothing left, finish
                if( start+len >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If haven't started yet, skip white space
                if( len == 0 && localEnumerationIn[start] == ' ' )
                {
                    start++;
                    break;
                }

                // If delimiter found, use it
                if( localEnumerationIn[start] == ':' )
                {
                    start++;
                    len = 0;
                    state = STATE_START_TEXT_QUOTE;
                    break;
                }

                // Error do no more
                state = STATE_END;
                break;

            // Where an enumerations string is quoted, handle the opening quotation mark.
            // For example, the first quote in 0=off,1="pump on"
            case STATE_START_TEXT_QUOTE:
                // If nothing left, finish
                if( start >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If haven't started yet, skip white space
                if( localEnumerationIn[start] == ' ' )
                {
                    start++;
                    break;
                }

                // If found '"' use it
                if( localEnumerationIn[start] == '"' )
                {
                    start++;
                    len = 0;
                    state = STATE_QUOTED_TEXT;
                    break;
                }

                // No quote found, assume unquoted text instead
                state = STATE_UNQUOTED_TEXT;
                break;

            // Where an enumerations string is quoted, extract the string within quotation marks.
            // For example, the string 'pump on' in in 0:off,1:"pump on"
            case STATE_QUOTED_TEXT:
                // If nothing left, finish
                if( start+len >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If have all text, save it
                if( localEnumerationIn[start+len] == '"' )
                {
                    item.text = localEnumerationIn.mid( start, len );
                    start += len;
                    len = 0;
                    localEnumeration.append( item );
                    item.dValue = 0.0;
                    item.sValue.clear();
                    item.op = localEnumerationItem::UNKNOWN;
                    item.text.clear();
                    state = STATE_END_TEXT_QUOTE;
                    break;
                }

                // Extend the text
                len++;
                break;

            // Where an enumerations string is not quoted, extract the string.
            // For example, the string 'off' in in 0=off,1="pump on"
            case STATE_UNQUOTED_TEXT:
                // If nothing left, finish
                if( start+len >= size )
                {
                    // if reached the end, use what ever we have
                    if( len )
                    {
                        item.text = localEnumerationIn.mid( start, len );
                        localEnumeration.append( item );
                    }
                    state = STATE_END;
                    break;
                }

                // If haven't started yet, skip white space
                if( len == 0 && localEnumerationIn[start] == ' ' )
                {
                    start++;
                    break;
                }

                // If have started, finish text when white space, comma
                if( len != 0 && ( localEnumerationIn[start+len] == ' ' ||  localEnumerationIn[start+len] == ',' ) )
                {
                    item.text = localEnumerationIn.mid( start, len );
                    start += len;
                    len = 0;
                    localEnumeration.append( item );
                    item.dValue = 0.0;
                    item.sValue.clear();
                    item.text.clear();
                    state = STATE_COMMA;
                    break;
                }

                // Extend the text
                len++;
                break;

            // Where an enumerations string is quoted, handle the closing quotation mark.
            // For example, the second quote in 0=off,1="pump on"
            case STATE_END_TEXT_QUOTE:
                // If nothing left, finish
                if( start >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If found '"' use it
                if( localEnumerationIn[start] == '"' )
                {
                    start++;
                    len = 0;
                    state = STATE_COMMA;
                    break;
                }

                // Error do no more
                state = STATE_END;
                break;

            // Reading the ',' between each value and string pair.  For example, the ',' in '0:on,1:off'
            case STATE_COMMA:
                // If nothing left, finish
                if( start >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If haven't started yet, skip white space
                if( localEnumerationIn[start] == ' ' )
                {
                    start++;
                    break;
                }

                // If found ',' use it
                if( localEnumerationIn[start] == ',' )
                {
                    start++;
                    len = 0;
                    state = STATE_OPERATOR;
                    break;
                }

                // Error do no more
                state = STATE_END;
                break;

            // finish. Re-initialise for safety
            case STATE_END:
                start = size+1;
                len = 0;
                break;
        }
    }

    // Replace any \n strings with a real new line character
    for( int i = 0; i < localEnumeration.size(); i++ )
    {
        localEnumeration[i].text.replace( "\\n", "\n" );

    }
}


//========================================================================================
// 'Get' formatting configuration methods

/*
    Get the precision. See setPrecision() for the use of 'precision'.
*/
int QEStringFormatting::getPrecision() {
    return precision;
}

/*
    Get the 'use DB precision' flag. See setUseDbPrecision() for the use of the
    'use DB precision' flag.
*/
bool QEStringFormatting::getUseDbPrecision() {
    return useDbPrecision;
}

/*
    Get the 'leading zero' flag. See setLeadingZero() for the use of the
    'leading zero' flag.
*/
bool QEStringFormatting::getLeadingZero() {
    return leadingZero;
}

/*
    Get the 'trailing zeros' flag. See setTrailingZeros() for the use of the
    'trailing zeros' flag.
*/
bool QEStringFormatting::getTrailingZeros() {
    return trailingZeros;
}

/*
    Get the type of information being formatted. See setFormat() for the use of
    the format type.
*/
QEStringFormatting::formats QEStringFormatting::getFormat() {
    return format;
}

/*
    Get the numerical base. See setRadix() for the use of 'radix'.
*/
unsigned int QEStringFormatting::getRadix() {
    return stream.integerBase();
}

/*
    Return the floating point notation
*/
QEStringFormatting::notations QEStringFormatting::getNotation() {
    return (QEStringFormatting::notations)stream.realNumberNotation();
}

/*
    Return the action to take when formatting an array (treat as ascii characters, a series of numbers, etc)
*/
QEStringFormatting::arrayActions QEStringFormatting::getArrayAction() {
    return arrayAction;
}

/*
    Return the index to select a value from array of values (not relevent when the array is treated as ascii)
*/
unsigned int QEStringFormatting::getArrayIndex() {
    return arrayIndex;
}

/*
    Get the 'include engineering units' flag.
*/
bool QEStringFormatting::getAddUnits() {
    return addUnits;
}

/*
    Get the local enumeration strings. See setLocalEnumeration() for the use of 'localEnumeration'.
*/
QString/*localEnumerationList*/ QEStringFormatting::getLocalEnumeration() {

    return localEnumerationString;

/* was returning regenerated localEumeration string
    QString s;
    int i;
    for( i = 0; i < localEnumeration.size(); i++ )
    {
        s.append( localEnumeration[i].value ).append( "=" );
        if( localEnumeration[i].text.contains( ' ' ) )
        {
            s.append( "\"" ).append( localEnumeration[i].text ).append( "\"" );
        }
        else
        {
            s.append( localEnumeration[i].text );
        }
        if( i != localEnumeration.size()+1 )
        {
            s.append( "," );
        }
    }
    return s;
*/
}

