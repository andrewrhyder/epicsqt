/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/include/UserMessage.h $
 * $Revision: #2 $
 * $DateTime: 2009/07/21 17:58:00 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class UserMessage
  \version $Revision: #2 $
  \date $DateTime: 2009/07/21 17:58:00 $
  \author andrew.rhyder
  \brief User message manager.
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

#ifndef UserMessage_H
#define UserMessage_H

#include <QObject>

class UserMessage : public QObject
{
    Q_OBJECT

    public:
        UserMessage();
        ~UserMessage();

    void setup( QObject* statusMessageConsumer,                 // Setup the signal / slot connections required to present messages to the user
                QObject* warningMessageConsumer,
                QObject* errorMessageConsumer );

    void setup( QObject* generalMessageConsumer );              // Setup the signal / slot connections required to present messages to the user

    void sendStatusMessage( QString message );                  // Send a status message to the user
    void sendStatusMessage( QString message, QString source );  // Send a status message to the user including a string identifying the source of the message

    void sendWarningMessage( QString message );                 // Send a warning message to the user
    void sendWarningMessage( QString message, QString source ); // Send a warning message to the user including a string identifying the source of the message

    void sendErrorMessage( QString message );                   // Send an error message to the user
    void sendErrorMessage( QString message, QString source );   // Send an error message to the user including a string identifying the source of the message

signals:
    void statusMessage( QString message );                      // Signal a status message
    void warningMessage( QString message );                     // Signal a warning message
    void errorMessage( QString message );                       // Signal an error message

    void generalMessage( QString message );                     // Signal a message (emitted for status, warning, and error
  };

#endif // UserMessage_H
