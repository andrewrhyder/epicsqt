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

#ifndef QCAFLOATINGFORMATTING_H
#define QCAFLOATINGFORMATTING_H

#include <QString>
#include <QVariant>
#include <Generic.h>

class QCaFloatingFormatting {
  public:

    // Formatting enumerations
    enum formats { FORMAT_e = 'e',   // format as [-]9.9e[+|-]999
                   FORMAT_E = 'E',   // format as [-]9.9E[+|-]999
                   FORMAT_f = 'f',   // format as [-]9.9
                   FORMAT_g = 'g',   // use e or f format, whichever is the most concise
                   FORMAT_G = 'G' }; // use E or f format, whichever is the most concise


    // Construction
    QCaFloatingFormatting();

    //===============================================
    // Main functions of this class:
    //   - Format a double based on a value
    //   - Translate a double and generate a value
    //===============================================
    double formatFloating( const QVariant &value );
    QVariant formatValue( const double &floatingValue, generic::generic_types valueType );

    // Functions to configure the formatting
    void setPrecision( unsigned int precision );
    void setFormat( formats format );

    // Functions to read the formatting configuration
    unsigned int getPrecision();
    int getFormat();

  private:
    // Private functions to read the formatting configuration
    char getFormatChar();

    // Type specific conversion functions
    double formatFromFloating( const QVariant &value );
    double formatFromInteger( const QVariant &value );
    double formatFromUnsignedInteger( const QVariant &value );
    double formatFromString( const QVariant &value );
    double formatFromTime( const QVariant &value );

    // Error reporting
    double formatFailure( QString message );

    // Formatting configuration
    unsigned int precision;
    formats format;
};

#endif // QCAFLOATINGFORMATTING_H
