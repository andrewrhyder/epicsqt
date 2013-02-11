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

#ifndef GENERIC_H_
#define GENERIC_H_

#include <string>
#include <db_access.h>

namespace generic {

  enum generic_types { STRING, SHORT, UNSIGNED_SHORT, UNSIGNED_CHAR, LONG, UNSIGNED_LONG, FLOAT, DOUBLE, UNKNOWN };

  class Generic {
    public:
      Generic();
      Generic( std::string newValue );
      Generic( short newValue );
      Generic( short* newValueArray, unsigned long countIn );
      Generic( unsigned short newValue );
      Generic( unsigned short* newValueArray, unsigned long countIn );
      Generic( unsigned char newValue );
      Generic( unsigned char* newValueArray, unsigned long countIn );
      Generic( long newValue );
      Generic( long* newValueArray, unsigned long countIn );
      Generic( unsigned long newValue );
      Generic( unsigned long* newValueArray, unsigned long countIn );
      Generic( float newValue );
      Generic( float* newValueArray, unsigned long countIn );
      Generic( double newValue );
      Generic( double* newValueArray, unsigned long countIn );
      virtual ~Generic();

      Generic( Generic &param );
      Generic& operator= ( Generic &param );

      void setString( std::string newValue );
      void setShort( short newValue );
      void setShort( short* newValueArray, unsigned long countIn );
      void updateShort( short newValue, unsigned long arrayIndex );
      void setUnsignedShort( unsigned short newValue );
      void setUnsignedShort( unsigned short* newValueArray, unsigned long countIn );
      void updateUnsignedShort( unsigned short newValue, unsigned long arrayIndex );
      void setUnsignedChar( unsigned char newValue );
      void setUnsignedChar( unsigned char* newValueArray, unsigned long countIn );
      void updateUnsignedChar( unsigned char newValue, unsigned long arrayIndex );
      void setLong( long newValue );
      void setLong( long* newValueArray, unsigned long countIn );
      // Most of the array set set functions can just memcpy the data.
      // processChannel (out of CaObject "long" type is infact a 32 bit type, so
      // we need to copy (and cast) element by element.
      //
      void setLong( dbr_long_t* newValueArray, unsigned long countIn );
      void updateLong( long newValue, unsigned long arrayIndex );
      void setUnsignedLong( unsigned long newValue );
      void setUnsignedLong( unsigned long* newValueArray, unsigned long countIn );
      void updateUnsignedLong( unsigned long newValue, unsigned long arrayIndex );
      void setFloat( float newValue );
      void setFloat( float* newValueArray, unsigned long countIn );
      void updateFloat( float newValue, unsigned long arrayIndex );
      void setDouble( double newValue );
      void setDouble( double* newValueArray, unsigned long countIn );
      void updateDouble( double newValue, unsigned long arrayIndex );

      std::string getString();
      void getString( char** valueArray, unsigned long* countOut = NULL );
      short getShort();
      void  getShort( short** valueArray, unsigned long* countOut = NULL );
      unsigned short getUnsignedShort();
      void           getUnsignedShort( unsigned short** valueArray, unsigned long* countOut = NULL );
      unsigned char getUnsignedChar();
      void          getUnsignedChar( unsigned char** valueArray, unsigned long* countOut = NULL );
      long getLong();
      void getLong( long** valueArray, unsigned long* countOut = NULL );
      unsigned long getUnsignedLong();
      void          getUnsignedLong( unsigned long** valueArray, unsigned long* countOut = NULL );
      float getFloat();
      void  getFloat( float** valueArray, unsigned long* countOut = NULL );
      double getDouble();
      void   getDouble( double** valueArray, unsigned long* countOut = NULL );

      unsigned long getArrayCount();

      generic_types getType();

    protected:
      void cloneValue( Generic *param );

    private:
      unsigned long arrayCount;
      generic_types type;
      void* value;

      void setType( generic_types newType );
      void deleteValue();
  };

}

#endif  // GENERIC_H_
