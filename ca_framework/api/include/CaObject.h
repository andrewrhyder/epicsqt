/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/api/include/CaObject.h $
 * $Revision: #3 $ 
 * $DateTime: 2009/07/29 14:20:04 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class CaObject
  \version $Revision: #3 $
  \date $DateTime: 2009/07/29 14:20:04 $
  \author anthony.owen
  \brief Provides CA to an EPICS channel.
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

#ifndef CAOBJECT_H_
#define CAOBJECT_H_

#include <string>
#include <Generic.h>
using namespace generic;

namespace caobject {

  enum callback_reasons { SUBSCRIPTION_SUCCESS, SUBSCRIPTION_FAIL,
                          READ_SUCCESS, READ_FAIL,
                          WRITE_SUCCESS, WRITE_FAIL,
                          EXCEPTION,
                          CONNECTION_UP, CONNECTION_DOWN, CONNECTION_UNKNOWN };

    class CaObject {

            // Initialisation and deletion
        public:
            CaObject();
            virtual ~CaObject();
        private:
            void initialise();
            void shutdown();

        protected:
            // Data record interrogation
            bool          isFirstUpdate();
            void*         getRecordCopyPtr(); // Return a copy of the record as a void*. Caller is responsible for deleting.
            int           getEnumStateCount();
            std::string   getEnumState( int position );
            int           getPrecision();
            std::string   getUnits();
            generic_types getType();
            unsigned long getTimeStampSeconds();
            unsigned long getTimeStampNanoseconds();
            short         getAlarmStatus();
            short         getAlarmSeverity();
        public:
            virtual void  signalCallback( callback_reasons ) = 0;

            // CA specific data private to the CaObject code
            // To allow application to include the QCaFramework without requiring
            // EPICS include files, this is obscured by casting the CA specific data as a void*
        public:
            void*        priPtr;     // Actually of type CaObjectPrivate*
        protected:
            static int CA_UNIQUE_OBJECT_ID;            //! Unique ID assigned to each instance.

    };

}

#endif  // CAOBJECT_H_
