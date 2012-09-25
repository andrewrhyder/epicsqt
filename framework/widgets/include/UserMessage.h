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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef UserMessage_H
#define UserMessage_H

#include <QObject>
#include <QCaPluginLibrary_global.h>
#include <QtDebug>

enum message_types {MESSAGE_TYPE_INFO, MESSAGE_TYPE_WARNING, MESSAGE_TYPE_ERROR };

class UserMessage;

// Class used to send message signals.
// Used only within UserMessage.cpp
// A single instance of this class is shared by all instances of
// the UserMessage class. This allows every UserMessage class instance to
// connect to a single source of messages
class UserMessageSignal : public QObject
{
    Q_OBJECT

public:
    UserMessageSignal(){}
    ~UserMessageSignal(){}

    void sendMessage( QString msg,
                      message_types type,
                      unsigned int formId,
                      unsigned int sourceId,
                      UserMessage* originator );    // Send a message to all widgets

signals:
    void message( QString msg,
                  message_types type,
                  unsigned int formId,
                  unsigned int sourceId,
                  UserMessage* originator );        // Emit a message signal
};

// Class used to receive message signals.
// Used only within UserMessage.cpp
// An instance of this class is created by all instances of
// the UserMessage class. The UserMessage class uses an instance of this class
// to receive messages so it does not have to be based on QObject itself. This is
// required as derived classes generally need to be also based on another object
// derived from QObject (and QObject can only be the base of a single base class)
class UserMessageSlot : public QObject
{
    Q_OBJECT

public:
    UserMessageSlot(){}
    ~UserMessageSlot(){}

    void setOwner( UserMessage* ownerIn ){ owner = ownerIn; }   // Set the UserMessage class this is a part of

public slots:
    void message( QString msg,
                  message_types type,
                  unsigned int formId,
                  unsigned int sourceId,
                  UserMessage* originator );                    // A message has been received

private:
    UserMessage* owner;                                         // UserMessage class that this instance is a part of
};

//!!! No need to export this once it is within the QCaWidget core class???
class QCAPLUGINLIBRARYSHARED_EXPORT UserMessage
{
public:
    friend class UserMessageSlot;
    friend class UserMessageSignal;

    enum message_filter_options {MESSAGE_FILTER_ANY, MESSAGE_FILTER_MATCH, MESSAGE_FILTER_NONE };
    UserMessage();
    virtual ~UserMessage();

    void setSourceId( unsigned int sourceId );                    // Set the source ID (the ID set up by the GUI designer, usually matched to the source ID of logging widgets)
    void setFormId( unsigned int formId );                        // Set the form ID (the the same ID for all sibling widgets within an QEForm widget)
    void setFormFilter( message_filter_options formFilterIn );    // Set the message filtering applied to the form ID
    void setSourceFilter( message_filter_options sourceFilterIn );// Set the message filtering applied to the source ID

    unsigned int getSourceId();                                 // Get the source ID (the ID set up by the GUI designer, usually matched to the source ID of logging widgets
    unsigned int getFormId();                                   // Get the form ID (the the same ID for all sibling widgets within an QEForm widget)
    message_filter_options getFormFilter();                     // Get the message filtering applied to the form ID
    message_filter_options getSourceFilter();                   // Get the message filtering applied to the source ID

    void setChildFormId( unsigned int );                        // Set the for ID of all widgets that are children of this widget
    unsigned int getChildFormId();                              // Get the for ID of all widgets that are children of this widget

    unsigned int getNextMessageFormId();                        // Generate a new form ID for all widgets in a new form

    void sendMessage( QString message,
                      message_types type = MESSAGE_TYPE_INFO ); // Send a message to the user
    void sendMessage( QString message,
                      QString source,
                      message_types type = MESSAGE_TYPE_INFO ); // Send a message to the user with a source reference

    QString getMessageTypeName( message_types type );           // Convenience function to provide string names for each message type

    virtual void newMessage( QString, message_types );          // Virtual function to pass messages to derived classes (typicaly logging widgets or application windows)


private:
    static UserMessageSignal userMessageSignal;                 // Single object to send all message signals
    static unsigned int nextMessageFormId;                      // The next message form ID
    unsigned int formId;                                        // The form ID passed with each message. Shared by all widgets within an QEForm widget
    unsigned int sourceId;                                      // The source ID passed with each message. Set to any value the GUI designer requires.
    UserMessageSlot userMessageSlot;                            // QObject based object to receive all messages. It calls newMessage() with each message.

    unsigned int childFormId;                                   // Only relevent for form (QEForm) widgets. Form ID of all child widgets
    message_filter_options formFilter;                          // Message filtering to apply to form ID
    message_filter_options sourceFilter;                        // Message filtering to apply to source ID
};

#endif // UserMessage_H
