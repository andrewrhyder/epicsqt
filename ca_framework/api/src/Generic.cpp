/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/api/src/Generic.cpp $
 * $Revision: #1 $ 
 * $DateTime: 2009/07/14 15:59:56 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class Generic
  \version $Revision: #1 $
  \date $DateTime: 2009/07/14 15:59:56 $
  \author anthony.owen
  \brief Provides a generic holder for different types.
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
 * anthony.owen@synchrotron.org.au
 * 800 Blackburn Road, Clayton, Victoria 3168, Australia.
 *
 */

#include <Generic.h>

using namespace generic;

/*!
    Creates unknown type
*/
Generic::Generic() { 
    value = NULL;
    type = UNKNOWN;
}

/*!
    Creates overloaded constructor of type string
*/
Generic::Generic( std::string newValue ) {
    value = NULL;
    setString( newValue );
}

/*!
    Creates overloaded constructor of type short
*/
Generic::Generic( short newValue ) {
    value = NULL;
    setShort( newValue );
}

/*!
    Creates overloaded constructor of type char
*/
Generic::Generic( char newValue ) {
    value = NULL;
    setChar( newValue );
}

/*!
    Creates overloaded constructor of type unsigned long
*/
Generic::Generic( unsigned long newValue ) {
    value = NULL;
    setUnsignedLong( newValue );
}

/*!
    Creates overloaded constructor of type double
*/
Generic::Generic( double newValue ) {
    value = NULL;
    setDouble( newValue );
}

/*!
    Actively delete the stored value and set flags
*/
Generic::Generic::~Generic() {
    deleteValue();
}

/*!
    Copy constructor for deep copy
*/
Generic::Generic( Generic &param ) {
    cloneValue( &param );
}

/*!
    = Operator for assignment
*/
Generic& Generic::operator= ( Generic &param ) {
    cloneValue( &param );
    return *this;
}

/*!
    Creates and records new type string
*/
void Generic::setString( std::string newValue ) {
    deleteValue();
    value = new std::string( newValue );
    type = STRING;
}

/*!
    Creates and records new type short
*/
void Generic::setShort( short newValue ) { 
    deleteValue();
    value = new short( newValue );
    type = SHORT;
}

/*!
    Creates and records new type char
*/
void Generic::setChar( char newValue ) { 
    deleteValue();
    value = new char( newValue );
    type = CHAR;
}

/*!
    Creates and records new type unsigned long
*/
void Generic::setUnsignedLong( unsigned long newValue ) { 
    deleteValue();
    value = new unsigned long( newValue );
    type = UNSIGNED_LONG;
}

/*!
    Creates and records new type double
*/
void Generic::setDouble( double newValue ) { 
    deleteValue();
    value = new double( newValue );
    type = DOUBLE;
}

/*!
    Returns type string or invalid
*/
std::string Generic::getString() {
    if( getType() == STRING ) {
        return *(std::string*)value;
    }
    return "";
}

/*!
    Returns type string or short
*/
short Generic::getShort() { 
    if( getType() == SHORT ) {
        return *(short*)value;
    }
    return 0;
}

/*!
    Returns type char or invalid
*/
char Generic::getChar() { 
    if( getType() == CHAR ) {
        return *(char*)value;
    }
    return 0;
}

/*!
    Returns type unsigned long or invalid
*/
unsigned long Generic::getUnsignedLong() { 
    if( getType() == UNSIGNED_LONG ) {
        return *(unsigned long*)value;
    }
    return 0;
}

/*!
    Returns type double or invalid
*/
double Generic::getDouble() { 
    if( getType() == DOUBLE ) {
        return *(double*)value;
    }
    return 0;
}

/*!
    Returns value type
*/
generic_types Generic::getType() {
    return type;
}

/*!
    Sets the value type
*/
void Generic::setType( generic_types newType ) {
    type = newType;
}

/*!
    Deletes stored value 
*/
void Generic::deleteValue() {
    if( value == NULL ) {
        type = UNKNOWN;
        return;
    }
    switch( getType() ) {
        case STRING :
            delete (std::string*)value;
        break;
        case SHORT :
            delete (short*)value;
        break;
        case CHAR :
            delete (char*)value;
        break;
        case UNSIGNED_LONG :
            delete (unsigned long*)value;
        break;
        case DOUBLE :
            delete (double*)value;
        break;
        case UNKNOWN :
            value = NULL;
            return;
        break;
    }
    value = NULL;
    type = UNKNOWN;
}

/*!
    Clone from given Generic
*/
void Generic::cloneValue( Generic *param ) {
    switch( param->getType() ) {
        case STRING :
            setString( param->getString() );
        break;
        case SHORT :
            setShort( param->getShort() );
        break;
        case CHAR :
            setChar( param->getChar() );
        break;
        case UNSIGNED_LONG :
            setUnsignedLong( param->getUnsignedLong() );
        break;
        case DOUBLE :
            setDouble( param->getDouble() );
        break;
        case UNKNOWN :
            deleteValue();
        break;
    }
}
