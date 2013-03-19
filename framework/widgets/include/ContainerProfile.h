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
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef CONTAINERPROFILE_H
#define CONTAINERPROFILE_H

#include <QObject>
#include <QMutex>
#include <QList>
#include <QStringList>
#include <QDebug>
#include <QEPluginLibrary_global.h>

class QEWidget;
class ContainerProfile;

// Define the user levels
// NOTE: order must remain least privileged to most privileged
/// \public
/// \enum userLevels
/// User levels set by widgets such as QELogin and used by many widgets to determine visibility, enabled state, and style.
enum userLevels { USERLEVEL_USER,       ///< User level - least privilaged
                  USERLEVEL_SCIENTIST,  ///< User level - more privilaged than user, less than engineer
                  USERLEVEL_ENGINEER    ///< User level - most privilaged
              };

// Class used to generate signals that the user level has changed.
// A single instance of this class is shared by all instances of
// the ContainerProfile class.
// The ContainerProfile class can't generate signals directly as it
// is not based on QObject and can't be as it is a base class for
// Widgets that may also be based on Qwidgets and only one base
// class can be based on a QObject.
class userLevelSignal : public QObject
{
    Q_OBJECT

public:

    // Constructor, destructor
    // Default to 'user'
    userLevelSignal();
    ~userLevelSignal();

    // Set the application wide user level
    // When level is set in the single instance of this class, all ContainerPRofile
    // classes are signaled
    void setLevel( userLevels levelIn );

    // Get the application wide user level
    // Each widget can reimplement ContainerProfile::userLevelChanged() to be
    // notified of user level changes, but this function can be used to
    // determine the user level when a widget is first created
    userLevels getLevel();

  signals:
    /// Internal use only. Send when the user level has changed
    void userChanged( userLevels level );   // User level change signal

  private:
    userLevels level;    // Current user level

};

// Class used to recieve signals that the user level has changed.
// An instance of this class is used by each instance of the
// ContainerProfile class.
// The ContainerProfile class can't recieve signals directly as it
// is not based on QObject and can't be as it is a base class for
// Widgets that may also be based on Qwidgets and only one base
// class can be based on a QObject.
class userLevelSlot : public QObject
{
    Q_OBJECT

public:
    // Constructor, destructor
    // Default to no owner
    userLevelSlot();
    ~userLevelSlot();

    // Set the ContainerProfile class that this instance is a part of
    void setOwner( ContainerProfile* ownerIn );

public slots:
    void userChanged( userLevels level );  // Receive user level change signals

private:
    ContainerProfile* owner;                                // ContainerProfile class that this instance is a part of
};

// Class to allow construction of a QE widgets list
// The class simply holds a reference to a class based on a QEWidget
// Usage QList<WidgetRef> myWidgetList
class WidgetRef
{
    public:
        WidgetRef( QEWidget* refIn ) { ref = refIn; }
        ~WidgetRef() {}
        QEWidget* getRef() { return ref; }

    private:
        QEWidget* ref;
};

// Class to allow a truly unique instance of the published profile.
// On Windows, static variables are defined twice when the QE plugin library is loaded: once by QEGui and once by the Qt .ui loader when creating forms.
class PublishedProfile
{
public:
    PublishedProfile()                  // Construction
    {
        guiLaunchConsumer = NULL;
        messageFormId = 0;
        profileDefined = false;
    }

    QObject* guiLaunchConsumer;         // Object to send GUI launch requests to
    QStringList pathList;               // Path list used for file operations (scope: application wide)
    QString parentPath;                 // Path used for file operations (scope: Parent object, if any. This is set up by the application, but is temporarily overwritten and then reset by each level of sub object (sub form)
    QList<QString> macroSubstitutions;  // list of variable name macro substitution strings. Extended by each sub form created
    unsigned int messageFormId;         // Current form ID. Used to group forms with their widgets for messaging

    QList<WidgetRef> containedWidgets;  // List of QE widgets created with this profile

    userLevelSignal userSignal;         // Current user level signal object. One instance to signal all QE Widgets

    QString userLevelPassword;          // User Level password for 'user'
    QString scientistLevelPassword;     // User Level password for 'scientist'
    QString engineerLevelPassword;      // User Level password for 'engineer'

    bool profileDefined;                // Flag true if a profile has been setup. Set between calling setupProfile() and releaseProfile()
};


// Class to provide a communication mechanism from the code creating QE widgets to the QE widgets.
// See ContainerProfile.cpp for details
class QEPLUGINLIBRARYSHARED_EXPORT ContainerProfile
{
public:
    ContainerProfile();    // Construction. Sets up local copies of any profile defined by setupProfile() in an earlier instance
    virtual ~ContainerProfile();   // Destruction
    void takeLocalCopy();

    void setupProfile( QObject* guiLaunchConsumerIn,
                       QStringList pathListIn,
                       QString parentPathIn,
                       QString macroSubstitutionsIn );     // Setup a local and published environmental profile for all QEWidgets to use on creation (no passwords)
    void setupProfile( QObject* guiLaunchConsumerIn,
                       QStringList pathListIn,
                       QString parentPathIn,
                       QString macroSubstitutionsIn,
                       QString userLevelPasswordIn,
                       QString scientistLevelPasswordIn,
                       QString engineerLevelPasswordIn );     // Setup a local and published environmental profile for all QEWidgets to use on creation
    void setupLocalProfile( QObject* guiLaunchConsumerIn,
                            QStringList pathListIn,
                            QString parentPathIn,
                            QString macroSubstitutionsIn,
                            QString userLevelPasswordIn,
                            QString scientistLevelPasswordIn,
                            QString engineerLevelPasswordIn );// Setup the local environmental profile for this instance only
    void updateConsumers( QObject* guiLaunchConsumerIn );  // Update the local and published signal consumer objects
    QObject* replaceGuiLaunchConsumer( QObject* newGuiLaunchConsumerIn );  // Override the current GUI launch consumer

    void addMacroSubstitutions( QString macroSubstitutionsIn ); // Add another set of macro substitutions to those setup by setupProfile(). Used as sub forms are created
    void removeMacroSubstitutions();                            // Remove the last set of macro substitutions added by addMacroSubstitutions(). Used after sub forms are created

    void addPriorityMacroSubstitutions( QString macroSubstitutionsIn ); // Add another set of macro substitutions to those setup by setupProfile(). Used as sub forms are created. These macros take priority
    void removePriorityMacroSubstitutions();                            // Remove the last set of macro substitutions added by addPriorityMacroSubstitutions(). Used after sub forms are created

    QObject* getGuiLaunchConsumer();          // Get the local copy of the object that will recieve GUI launch requests
    QString getPath();                        // Get the local copy of the first entry in the application path list used for file operations
    QStringList getPathList();                // Get the local copy of the application path list used for file operations
    QString getParentPath();                  // Get the local copy of the current object path used for file operations
    void setPublishedParentPath( QString publishedParentPathIn ); // Set the published current object path used for file operations
    QString getMacroSubstitutions();          // Get the local copy of the variable name macro substitutions
    bool isProfileDefined();                  // Returns true if a profile has been setup by setupProfile()

    QString getUserLevelPassword( userLevels level );  // Get the local copy of the user level password for the specified user level
    void setUserLevelPassword( userLevels level, QString passwordIn );  // Set the local copy of the user level password for the specified user level

    void addContainedWidget( QEWidget* containedWidget );    // Adds a reference to the list of QE widgets created with this profile
    QEWidget* getNextContainedWidget();                      // Returns a reference to the next QE widgets in the list of QE widgets created with this profile
    void removeContainedWidget( QEWidget* containedWidget ); // Remove a reference from the list of QE widgets created with this profile

    unsigned int getMessageFormId();                    // Get the local copy of the message form ID
    unsigned int getPublishedMessageFormId();           // Get the currently published message form ID
    void setPublishedMessageFormId( unsigned int publishedMessageFormIdIn );  // Set the currently published message form ID

    void releaseProfile();                              // Clears the context setup by setupProfile(). Local data in all instances is still valid

    void publishOwnProfile();                           // Set the published profile to whatever is saved in our local copy

    void setUserLevel( userLevels level );              // Set the current user level
    userLevels getUserLevel();                          // Return the current user level

    virtual void userLevelChanged( userLevels ){}       // Virtual function implemented by widgets based on QEWidget to allow them to be notified of changes in user level

private:
    void publishProfile( QObject* guiLaunchConsumerIn,
                         QStringList pathListIn,
                         QString publishedParentPathIn,
                         QString macroSubstitutionsIn,
                         QString userLevelPasswordIn,
                         QString scientistLevelPasswordIn,
                         QString engineerLevelPasswordIn );// Publish an environmental profile for all QEWidgets to use on creation

    PublishedProfile* getPublishedProfile();            // Get the single instance of the published profile

    userLevelSlot userSlot;                             // Current user level slot object. An instance per ContainerProfile to recieve level changes

    QObject* guiLaunchConsumer;      // Local copy of GUI launch consumer. Still valid after the profile has been released by releaseProfile()
    QStringList pathList;            // Local copy of application path list used for file operations
    QString parentPath;              // Local copy of parent object path used for file operations
    QString macroSubstitutions;      // Local copy of macro substitutions (converted to a single string) Still valid after the profile has been released by releaseProfile()

    unsigned int messageFormId;      // Local copy of current form ID. Used to group forms with their widgets for messaging

    QString userLevelPassword;                      // Password for 'user' user level
    QString scientistLevelPassword;                 // Password for 'scientist' user level
    QString engineerLevelPassword;                  // Password for 'engineer' user level
};

#endif // CONTAINERPROFILE_H
