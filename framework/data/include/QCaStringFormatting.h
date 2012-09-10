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

// Formats the string for QCaString data.

#ifndef QCASTRINGFORMATTING_H
#define QCASTRINGFORMATTING_H

#include <QVariant>
#include <QString>
#include <QStringList>
#include <QDataStream>
#include <QTextStream>
#include <QCaPluginLibrary_global.h>


// Support class used to build the localEnumeration list in the QCaStringFormatting class
class localEnumerationItem {
    public:
    enum operations { LESS, LESS_EQUAL, EQUAL, NOT_EQUAL, GREATER_EQUAL, GREATER, ALWAYS, UNKNOWN };
    double dValue;              // Numeric value that numeric data is compared to (derived from sValue if possible)
    QString sValue;             // Text value that textual data is compared with
    operations op;              // Operator used for comparison used between data and value (=,<,>)
    QString text;               // Text used if the data value matches
};

class QCAPLUGINLIBRARYSHARED_EXPORT QCaStringFormatting {
  public:

    // Formatting enumerations
    enum formats { FORMAT_DEFAULT, FORMAT_FLOATING, FORMAT_INTEGER, FORMAT_UNSIGNEDINTEGER, FORMAT_TIME, FORMAT_LOCAL_ENUMERATE };
    enum notations { NOTATION_FIXED = QTextStream::FixedNotation,
                     NOTATION_SCIENTIFIC = QTextStream::ScientificNotation,
                     NOTATION_AUTOMATIC = QTextStream::SmartNotation };    // WARNING keep these enumerations the same as QTextStream
    enum arrayActions { APPEND, ASCII, INDEX };

    // Construction
    QCaStringFormatting();

    //===============================================
    // Main functions of this class:
    //   - Format a string based on a value
    //   - Translate a string and generate a value
    //===============================================
    QString formatString( const QVariant& value );
    QVariant formatValue( const QString& text );


    // Functions to set up formatting information from the database
    void setDbEgu( QString egu );      /// Units to be added (or removed) from the formatted string if 'addUnits' flag is set
    void setDbEnumerations( QStringList enumerations );
    void setDbPrecision( unsigned int dbPrecisionIn );
    void setDbVariableIsStatField( bool isStatField );

    // Functions to configure the formatting
    void setPrecision( int precision );
    void setUseDbPrecision( bool useDbPrecision );
    void setLeadingZero( bool leadingZero );
    void setTrailingZeros( bool trailingZeros );
    void setFormat( formats format );
    void setRadix( unsigned int radix );
    void setNotation( notations notation );
    void setArrayAction( arrayActions arrayActionIn );
    void setArrayIndex( unsigned int arrayIndexIn );
    void setAddUnits( bool addUnits );
    void setLocalEnumeration( QString/*localEnumerationList*/ localEnumerationIn );

    // Functions to read the formatting configuration
    int          getPrecision();
    bool         getUseDbPrecision();
    bool         getLeadingZero();
    bool         getTrailingZeros();
    formats      getFormat();
    unsigned int getRadix();
    notations    getNotation();
    arrayActions getArrayAction();
    unsigned int getArrayIndex();
    bool         getAddUnits();
    QString      getLocalEnumeration();

  private:
    // Type specific conversion functions
    void formatFromFloating( const QVariant& value );
    void formatFromInteger( const QVariant& value );
    void formatFromUnsignedInteger( const QVariant& value );
    void formatFromTime( const QVariant& value );
    void formatFromEnumeration( const QVariant& value );

    // Error reporting
    void formatFailure( QString message );

    // Formatted output string
    QTextStream stream;
    QString outStr;

    // Database information
    QString dbEgu;
    QStringList dbEnumerations;
    unsigned int dbPrecision;
    bool dbVariableIsStatField;

    // Formatting configuration
    bool useDbPrecision;             /// Use the number of decimal places displayed as defined in the database.
    bool leadingZero;                /// Add a leading zero when required.
    bool trailingZeros;              /// Add trailing zeros when required (up to the precision).
    formats format;                  /// Presentation required (Floating, integer, etc).
    bool addUnits;                   /// Flag use engineering units from database
    int precision;                   /// Floating point precision. Used if 'useDbPrecision' is false.
    QList<localEnumerationItem> localEnumeration; /// Local enumerations (example: 0="Not referencing",1=Referencing)
    QString localEnumerationString; /// Original local enumerations string
    arrayActions arrayAction;       /// Action to take when processing array or waveform data
    unsigned int arrayIndex;        /// Index into array or waveform to use when into arrayAction is 'INDEX'
};

#endif /// QCASTRINGFORMATTING_H
