/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/include/QCaStringFormatting.h $
 * $Revision: #7 $
 * $DateTime: 2009/11/23 08:44:03 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaStringFormatting
  \version $Revision: #7 $
  \date $DateTime: 2009/11/23 08:44:03 $
  \author andrew.rhyder
  \brief Formats the string for QCaString data.
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

#ifndef QCASTRINGFORMATTING_H
#define QCASTRINGFORMATTING_H

#include <QVariant>
#include <QString>
#include <QStringList>
#include <QDataStream>



// Support class used to build the localEnumeration list in the QCaStringFormatting class
class localEnumerationItem {
    public:
    int value;                  // This data value...
    QString text;               // ... is translated to this text
};

class QCaStringFormatting {
  public:

    // Formatting enumerations
    enum formats { FORMAT_DEFAULT, FORMAT_FLOATING, FORMAT_INTEGER, FORMAT_UNSIGNEDINTEGER, FORMAT_TIME, FORMAT_LOCAL_ENUMERATE };
    enum notations { NOTATION_FIXED = QTextStream::FixedNotation,
                     NOTATION_SCIENTIFIC = QTextStream::ScientificNotation,
                     NOTATION_AUTOMATIC = QTextStream::SmartNotation };    // WARNING keep these enumerations the same as QTextStream

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
    void setPrecision( unsigned int precision );
    void setUseDbPrecision( bool useDbPrecision );
    void setLeadingZero( bool leadingZero );
    void setTrailingZeros( bool trailingZeros );
    void setFormat( formats format );
    void setRadix( unsigned int radix );
    void setNotation( notations notation );
    void setAddUnits( bool addUnits );
    void setLocalEnumeration( QString/*localEnumerationList*/ localEnumerationIn );

    // Functions to read the formatting configuration
    unsigned int getPrecision();
    bool         getUseDbPrecision();
    bool         getLeadingZero();
    bool         getTrailingZeros();
    formats      getFormat();
    unsigned int getRadix();
    notations    getNotation();
    bool         getAddUnits();
    QString      getLocalEnumeration();

  private:
    // Type specific conversion functions
    void formatFromFloating( const QVariant& value );
    void formatFromInteger( const QVariant& value, const bool doLocalEnumeration );
    void formatFromUnsignedInteger( const QVariant& value );
    void formatFromTime( const QVariant& value );

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
    unsigned int precision;          /// Floating point precision. Used if 'useDbPrecision' is false.
    QList<localEnumerationItem> localEnumeration; /// Local enumerations (example: 0="Not referencing",1=Referencing)
    QString localEnumerationString; /// Original local enumerations string
};

#endif /// QCASTRINGFORMATTING_H
