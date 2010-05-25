/*! 
  \class QCaIntegerFormatting
  \version $Revision: #2 $
  \date $DateTime: 2009/11/23 08:44:03 $
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

#ifndef QCAINTEGERFORMATTING_H
#define QCAINTEGERFORMATTING_H

#include <QString>
#include <QVariant>
#include <Generic.h>

class QCaIntegerFormatting {

public:

    // Construction
    QCaIntegerFormatting();

    //===============================================
    // Main functions of this class:
    //   - Format a double based on a value
    //   - Translate a double and generate a value
    //===============================================
    long formatInteger( const QVariant &value );
    QVariant formatValue( const long &integerValue, generic::generic_types valueType );

    // Functions to configure the formatting
    void setRadix( unsigned int radix );

    // Functions to read the formatting configuration
    unsigned int getPrecision();
    unsigned int getRadix();

  private:
    // Type specific conversion functions
    long formatFromFloating( const QVariant &value );
    long formatFromInteger( const QVariant &value );
    long formatFromUnsignedInteger( const QVariant &value );
    long formatFromString( const QVariant &value );
    long formatFromTime( const QVariant &value );

    // Error reporting
    long formatFailure( QString message );

    // Formatting configuration
    int radix; /// Positional base system to display data.
};

#endif // QCAINTEGERFORMATTING_H
