/*! 
  \class Generic
  \version $Revision: #4 $
  \date $DateTime: 2010/08/30 16:37:08 $
  \author anthony.owen
  \brief Provides a generic holder for different types.
 */
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
 *    Anthony Owen
 *  Contact details:
 *    anthony.owen@gmail.com
 */

#include <Generic.h>
#include <stdlib.h>
#include <string.h>

using namespace generic;

/*!
    Creates unknown type
*/
Generic::Generic() { 
    value = NULL;
    arrayCount = 0;
    type = UNKNOWN;
}

/*!
    Creates overloaded constructor of type string
*/
Generic::Generic( std::string newValue ) {
    value = NULL;
    arrayCount = 0;
    setString( newValue );
}

/*!
    Creates overloaded constructor of type short
*/
Generic::Generic( short newValue ) {
    value = NULL;
    arrayCount = 0;
    setShort( newValue );
}

Generic::Generic( short* newValue, unsigned long arrayCountIn ) {
    value = NULL;
    arrayCount = 0;
    setShort( newValue, arrayCountIn );
}

/*!
    Creates overloaded constructor of type unsigned short
*/
Generic::Generic( unsigned short newValue ) {
    value = NULL;
    arrayCount = 0;
    setUnsignedShort( newValue );
}

Generic::Generic( unsigned short* newValue, unsigned long arrayCountIn ) {
    value = NULL;
    arrayCount = 0;
    setUnsignedShort( newValue, arrayCountIn );
}

/*!
    Creates overloaded constructor of type char
*/
Generic::Generic( unsigned char newValue ) {
    value = NULL;
    arrayCount = 0;
    setUnsignedChar( newValue );
}

Generic::Generic( unsigned char* newValue, unsigned long arrayCountIn ) {
    value = NULL;
    arrayCount = 0;
    setUnsignedChar( newValue, arrayCountIn );
}

/*!
    Creates overloaded constructor of type long
*/
Generic::Generic( long newValue ) {
    value = NULL;
    arrayCount = 0;
    setLong( newValue );
}

Generic::Generic( long* newValue, unsigned long arrayCountIn ) {
    value = NULL;
    arrayCount = 0;
    setLong( newValue, arrayCountIn );
}

/*!
    Creates overloaded constructor of type unsigned long
*/
Generic::Generic( unsigned long newValue ) {
    value = NULL;
    arrayCount = 0;
    setUnsignedLong( newValue );
}

Generic::Generic( unsigned long* newValue, unsigned long arrayCountIn ) {
    value = NULL;
    arrayCount = 0;
    setUnsignedLong( newValue, arrayCountIn );
}

/*!
    Creates overloaded constructor of type float
*/
Generic::Generic( float newValue ) {
    value = NULL;
    arrayCount = 0;
    setFloat( newValue );
}

Generic::Generic( float* newValue, unsigned long arrayCountIn ) {
    value = NULL;
    arrayCount = 0;
    setFloat( newValue, arrayCountIn );
}

/*!
    Creates overloaded constructor of type double
*/
Generic::Generic( double newValue ) {
    value = NULL;
    arrayCount = 0;
    setDouble( newValue );
}

Generic::Generic( double* newValue, unsigned long arrayCountIn ) {
    value = NULL;
    arrayCount = 0;
    setDouble( newValue, arrayCountIn );
}

/*!
    Actively delete the stored value and set flags
*/
Generic::~Generic() {
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
    setShort( &newValue, 1 );
}

/*!
    Creates and records new type short (an array larger than 1)
*/
void Generic::setShort( short* newValueArray, unsigned long arrayCountIn ) {
    deleteValue();
    value = new short[arrayCountIn];
    memcpy( value, newValueArray, sizeof(short)*arrayCountIn );
    arrayCount = arrayCountIn;
    type = SHORT;
}

/*!
    Creates and records new type unsigned short
*/
void Generic::setUnsignedShort( unsigned short newValue ) {
    setUnsignedShort( &newValue, 1 );
}

/*!
    Creates and records new type unsigned short (an array larger than 1)
*/
void Generic::setUnsignedShort( unsigned short* newValueArray, unsigned long arrayCountIn ) {
    deleteValue();
    value = new unsigned short[arrayCountIn];
    memcpy( value, newValueArray, sizeof(unsigned short)*arrayCountIn );
    arrayCount = arrayCountIn;
    type = UNSIGNED_SHORT;
}

/*!
    Creates and records new type unsigned char
*/
void Generic::setUnsignedChar( unsigned char newValue ) {
    setUnsignedChar( &newValue, 1 );
}

/*!
    Creates and records new type unsigned char (an array larger than 1)
*/
void Generic::setUnsignedChar( unsigned char* newValueArray, unsigned long arrayCountIn ) {
    deleteValue();
    value = new unsigned char[arrayCountIn];
    memcpy( value, newValueArray, sizeof(unsigned char)*arrayCountIn );
    arrayCount = arrayCountIn;
    type = UNSIGNED_CHAR;
}

/*!
    Creates and records new type  long
*/
void Generic::setLong( long newValue ) {
    setLong( &newValue, 1 );
}

/*!
    Creates and records new type long (an array larger than 1)
*/
void Generic::setLong( long* newValueArray, unsigned long arrayCountIn ) {
    deleteValue();
    value = new long[arrayCountIn];
    memcpy( value, newValueArray, sizeof(long)*arrayCountIn );
    arrayCount = arrayCountIn;
    type = LONG;
}

/*!
    Creates and records new type unsigned long
*/
void Generic::setUnsignedLong( unsigned long newValue ) {
    setUnsignedLong( &newValue, 1 );
}

/*!
    Creates and records new type unsigned long (an array larger than 1)
*/
void Generic::setUnsignedLong( unsigned long* newValueArray, unsigned long arrayCountIn ) {
    deleteValue();
    value = new unsigned long[arrayCountIn];
    memcpy( value, newValueArray, sizeof(unsigned long)*arrayCountIn );
    arrayCount = arrayCountIn;
    type = UNSIGNED_LONG;
}

/*!
    Creates and records new type float
*/
void Generic::setFloat( float newValue ) {
    setFloat( &newValue, 1 );
}

/*!
    Creates and records new type float (an array larger than 1)
*/
void Generic::setFloat( float* newValueArray, unsigned long arrayCountIn ) {
    deleteValue();
    value = new float[arrayCountIn];
    memcpy( value, newValueArray, sizeof(float)*arrayCountIn );
    arrayCount = arrayCountIn;
    type = FLOAT;
}

/*!
    Creates and records new type double
*/
void Generic::setDouble( double newValue ) {
    setDouble( &newValue, 1 );
}

/*!
    Creates and records new type double (an array larger than 1)
*/
void Generic::setDouble( double* newValueArray, unsigned long arrayCountIn ) {
    deleteValue();
    value = new double[arrayCountIn];
    memcpy( value, newValueArray, sizeof(double)*arrayCountIn );
    arrayCount = arrayCountIn;
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
    Returns type short or invalid
*/
short Generic::getShort() { 
    if( getType() == SHORT ) {
        return *(short*)value;
    }
    return 0;
}

/*!
    Returns type short array or invalid
*/
void Generic::getShort( short** valueArray, unsigned long* arrayCountOut ) {
    if( getType() == SHORT ) {
        *valueArray = (short*)value;
        if( arrayCountOut )
            *arrayCountOut = arrayCount;
        return;
    }
    *valueArray = NULL;
    if( arrayCountOut )
        *arrayCountOut = 0;
}

/*!
    Returns type unsigned short or invalid
*/
unsigned short Generic::getUnsignedShort() {
    if( getType() == UNSIGNED_SHORT ) {
        return *(unsigned short*)value;
    }
    return 0;
}

/*!
    Returns type short array or invalid
*/
void Generic::getUnsignedShort( unsigned short** valueArray, unsigned long* arrayCountOut ) {
    if( getType() == UNSIGNED_SHORT ) {
        *valueArray = (unsigned short*)value;
        if( arrayCountOut )
            *arrayCountOut = arrayCount;
        return;
    }
    *valueArray = NULL;
    if( arrayCountOut )
        *arrayCountOut = 0;
}

/*!
    Returns type char or invalid
*/
unsigned char Generic::getUnsignedChar() {
    if( getType() == UNSIGNED_CHAR ) {
        return *(char*)value;
    }
    return 0;
}

/*!
    Returns type char array or invalid
*/
void Generic::getUnsignedChar( unsigned char** valueArray, unsigned long* arrayCountOut ) {
    if( getType() == UNSIGNED_CHAR ) {
        *valueArray = (unsigned char*)value;
        if( arrayCountOut )
            *arrayCountOut = arrayCount;
        return;
    }
    *valueArray = NULL;
    if( arrayCountOut )
        *arrayCountOut = 0;
}

/*!
    Returns type long or invalid
*/
long Generic::getLong() {
    if( getType() == LONG ) {
        return *(long*)value;
    }
    return 0;
}

/*!
    Returns type long array or invalid
*/
void Generic::getLong( long** valueArray, unsigned long* arrayCountOut ) {
    if( getType() == LONG ) {
        *valueArray = (long*)value;
        if( arrayCountOut )
            *arrayCountOut = arrayCount;
        return;
    }
    *valueArray = NULL;
    if( arrayCountOut )
        *arrayCountOut = 0;
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
    Returns type unsigned long array or invalid
*/
void Generic::getUnsignedLong( unsigned long** valueArray, unsigned long* arrayCountOut ) {
    if( getType() == UNSIGNED_LONG ) {
        *valueArray = (unsigned long*)value;
        if( arrayCountOut )
            *arrayCountOut = arrayCount;
        return;
    }
    *valueArray = NULL;
    if( arrayCountOut )
        *arrayCountOut = 0;
}

/*!
    Returns type float or invalid
*/
float Generic::getFloat() {
    if( getType() == FLOAT ) {
        return *(float*)value;
    }
    return 0;
}

/*!
    Returns type double array or invalid
*/
void Generic::getFloat( float** valueArray, unsigned long* arrayCountOut ) {
    if( getType() == FLOAT ) {
        *valueArray = (float*)value;
        if( arrayCountOut )
            *arrayCountOut = arrayCount;
        return;
    }
    *valueArray = NULL;
    if( arrayCountOut )
        *arrayCountOut = 0;
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
    Returns type double array or invalid
*/
void Generic::getDouble( double** valueArray, unsigned long* arrayCountOut ) {
    if( getType() == DOUBLE ) {
        *valueArray = (double*)value;
        if( arrayCountOut )
            *arrayCountOut = arrayCount;
        return;
    }
    *valueArray = NULL;
    if( arrayCountOut )
        *arrayCountOut = 0;
}

/*!
    Returns value type
*/
generic_types Generic::getType() {
    return type;
}

/*!
    Returns array size
*/
unsigned long Generic::getArrayCount() {
    return arrayCount;
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
        case UNSIGNED_SHORT :
            delete (unsigned short*)value;
        break;
        case UNSIGNED_CHAR :
            delete (char*)value;
        break;
        case LONG :
            delete (long*)value;
        break;
        case UNSIGNED_LONG :
            delete (unsigned long*)value;
        break;
        case FLOAT :
            delete (float*)value;
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
        case UNSIGNED_SHORT :
            {
                unsigned short* paramValue;
                unsigned long paramCount;
                param->getUnsignedShort( &paramValue, &paramCount );
                setUnsignedShort( paramValue, paramCount );
            }
        break;
        case SHORT :
            {
                short* paramValue;
                unsigned long paramCount;
                param->getShort( &paramValue, &paramCount );
                setShort( paramValue, paramCount );
            }
        break;
        case UNSIGNED_CHAR :
            {
                unsigned char* paramValue;
                unsigned long paramCount;
                param->getUnsignedChar( &paramValue, &paramCount );
                setUnsignedChar( paramValue, paramCount );
            }
        break;
        case LONG :
            {
                long* paramValue;
                unsigned long paramCount;
                param->getLong( &paramValue, &paramCount );
                setLong( paramValue, paramCount );
            }
        break;
         case UNSIGNED_LONG :
            {
                unsigned long* paramValue;
                unsigned long paramCount;
                param->getUnsignedLong( &paramValue, &paramCount );
                setUnsignedLong( paramValue, paramCount );
            }
        break;
        case FLOAT :
            {
                float* paramValue;
                unsigned long paramCount;
                param->getFloat( &paramValue, &paramCount );
                setFloat( paramValue, paramCount );
            }
        break;
        case DOUBLE :
            {
                double* paramValue;
                unsigned long paramCount;
                param->getDouble( &paramValue, &paramCount );
                setDouble( paramValue, paramCount );
            }
        break;
        case UNKNOWN :
            deleteValue();
        break;
    }
}
