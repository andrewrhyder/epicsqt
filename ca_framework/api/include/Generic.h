/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/api/include/Generic.h $
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

#ifndef GENERIC_H_
#define GENERIC_H_

#include <string>


namespace generic {

  enum generic_types { STRING, SHORT, CHAR, UNSIGNED_LONG, DOUBLE, UNKNOWN };

  class Generic {
    public:
      Generic();
      Generic( std::string newValue );
      Generic( short newValue );
      Generic( char newValue );
      Generic( unsigned long newValue );
      Generic( double newValue );
      ~Generic();

      Generic( Generic &param );
      Generic& operator= ( Generic &param );

      void setString( std::string newValue );
      void setShort( short newValue );
      void setChar( char newValue );
      void setUnsignedLong( unsigned long newValue );
      void setDouble( double newValue );

      std::string getString();
      short getShort();
      char getChar();
      unsigned long getUnsignedLong();
      double getDouble();

      generic_types getType();

    protected:
      void cloneValue( Generic *param );

    private:
      generic_types type;
      void* value;

      void setType( generic_types newType );
      void deleteValue();
  };

}

#endif  // GENERIC_H_
