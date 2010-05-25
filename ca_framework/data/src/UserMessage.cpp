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

/** Description:
 *
 * A class to manage user messages.
 *
 * Prior to use the class instance is supplied with slot references to 
 * receive status, warning, and error messages.
 *
 * Once setup, this class can be used to present messages to the user.
 *
 * If the slots used change only the call to UserMessage::setup() needs
 * to change.  The methods used to present messages to the user are overloaded 
 * so the caller can supply either a message string, or a message string and
 * a source string to help identify where teh message originated.
 *
 * A reference to an instance of this class can be passed to an object 
 * that needs to present messages to the user, but has no user context. 
 * That is, it does not know what slots to connect to.
 *
 * FUTURE USES
 * Perhaps add additioinal setup methods to provide log file locations, 
 * connections to slots that take more than a single QString, etc.
 *
 */

#include <QDebug>

#include <UserMessage.h>

/// Construction
UserMessage::UserMessage()
{
}

/// Destruction
UserMessage::~UserMessage()
{
}


/*!
  Set up the signal / slot connections required for various types of messages.

  The caller can supply a slot that accepts a QString for each of status, warning, and error messages.
  Once set up, this class can be used to send messages to the user without knowledge of what signal needs to be emited.
  */
void UserMessage::setup( QObject* statusMessageConsumer,
                         QObject* warningMessageConsumer,
                         QObject* errorMessageConsumer )
{
    if( statusMessageConsumer )
        QObject::connect( (QObject*)this, SIGNAL( statusMessage( QString ) ), statusMessageConsumer, SLOT( onStatusMessage( QString ) ) );

    if( warningMessageConsumer )
        QObject::connect( (QObject*)this, SIGNAL( warningMessage( QString ) ), warningMessageConsumer, SLOT( onWarningMessage( QString ) ) );

    if( errorMessageConsumer )
        QObject::connect( (QObject*)this, SIGNAL( errorMessage( QString ) ), errorMessageConsumer, SLOT( onErrorMessage( QString ) ) );
}

/*!
  Set up the signal / slot connections required for user messages.

  The caller can supply a slot that accepts a QString for general messages.
  Once set up, this class can be used to send messages to the user without knowledge of what signal needs to be emited.
  */
void UserMessage::setup( QObject* generalMessageConsumer )
{
    if( generalMessageConsumer )
        QObject::connect( (QObject*)this, SIGNAL( generalMessage( QString ) ), generalMessageConsumer, SLOT( onGeneralMessage( QString ) ) );
}

/*!
  Send a status message to the user.
  A string containing the message and a string containing information as to the source of the message is required
  */
void UserMessage::sendStatusMessage( QString message, QString source  ) {
    // Combine message and source and call base send message function
    sendStatusMessage( QString("%1 (Source %2)").arg(message).arg(source) );
}

/*!
  Send a status message to the user.
  A string containing the message is required
  */
void UserMessage::sendStatusMessage( QString message ) {
    emit statusMessage( message );
    emit generalMessage( message );
}

/*!
  Send a warning message to the user.
  A string containing the message and a string containing information as to the source of the message is required
  */
void UserMessage::sendWarningMessage( QString message, QString source ) {
    // Combine message and source and call base send message function
    sendWarningMessage( QString("%1 (Source %2)").arg(message).arg(source) );
}

/*!
  Send a warning message to the user.
  A string containing the message is required
  */
void UserMessage::sendWarningMessage( QString message ) {
    emit errorMessage( message );
    emit generalMessage( message );
}

/*!
  Send an error message to the user.
  A string containing the message and a string containing information as to the source of the message is required
  */

void UserMessage::sendErrorMessage( QString message, QString source ) {
    // Combine message and source and call base send message function
    sendErrorMessage( QString("%1 (Source %2)").arg(message).arg(source) );
}

/*!
  Send an error message to the user.
  A string containing the message is required
  */

void UserMessage::sendErrorMessage( QString message ) {
    emit warningMessage( message );
    emit generalMessage( message );
}

