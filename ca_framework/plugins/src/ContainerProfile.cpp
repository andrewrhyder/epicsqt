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

/*
 * Description:
 *
 * This class provides a communication mechanism from the code creating QCa widgets to the QCa widgets.
 *
 * When QCa widgets, such as QCaLabel, are created, they need to know environmental
 * information such as what macro substitutions to apply, or where to signal error messages.
 * Also, the code creating the QCa widgets may require a reference to all the created QCa widgets.
 * In some cases this information cannot be passed during construction or set up post construction
 * via a method. For example, when the object is being created from a UI file by Qt. In this case
 * the application code asks Qt to generate objects from a UI file and has no idea what
 * QCa widgets if any have been created.
 *
 * To use this class, an instance of this class is instantiated prior to creating the QCa widgets.
 * Information to be communicated such as message handlers and macro substitutions is set up within this class.
 * Then the QCa widgets are created using a mechanism such as the QUiLoader class.
 *
 * As each QCa widget is created it also instantiates an instance of the ContainerProfile class.
 * If any information has been provided, it can then be used.
 *
 * Note, a local copy of the environment profile is saved per instance, so an application
 * creating QCa widgets (the container) can define a profile, create QCa widgets, then release the profile.
 *
 * To use this class
 *         - Instantiate a ContainerProfile class
 *         - Call setupProfile()
 *         - Create QCa widgets
 *         - Call releaseProfile()
 *
 * Notes:
 * - If an application creates the ContainerProfile class early, before the widgets that are published in the
 *   profile, or if the published widgets change the widgets in the profile can be updated by calling updateConsumers().
 *   Alternatively, just the widget that launches new GUIs can be updated with replaceGuiLaunchConsumer().
 *
 * - An application may need to temprarily extend the the macro substitutions. For example, when creating an ASguiForm
 *   widget as a sub form within another ASguiform widget. Macro substitutions can be extended by calling addMacroSubstitutions()
 *   then restored using removeMacroSubstitutions().
 *
 */

#include <ContainerProfile.h>
#include <QCaWidget.h>
#include <QtDebug>


/// Static variables used to pass information from the creator of QCa widgets to the QCa widgets themselves.
QObject* ContainerProfile::publishedStatusMessageConsumer = NULL;
QObject* ContainerProfile::publishedErrorMessageConsumer = NULL;
QObject* ContainerProfile::publishedWarningMessageConsumer = NULL;

QObject* ContainerProfile::publishedGuiLaunchConsumer = NULL;

QList<QString>   ContainerProfile::publishedMacroSubstitutions;
QList<WidgetRef> ContainerProfile::containedWidgets;
QString          ContainerProfile::publishedPath;
QString          ContainerProfile::publishedParentPath;

//bool ContainerProfile::publishedInteractive = false;

bool ContainerProfile::profileDefined = false;

// Constructor.
// A local copy of the defined profile (if any) is made.
// Note, this does not define a profile. A profile is defined only when ContainerProfile::setupProfile() is called
ContainerProfile::ContainerProfile()
{
    takeLocalCopy();
}

// Destructor
// Note, if the profile has been defined (ContainerProfile::setupProfile() has been
// called) this does not release the profile. A profile is released only when
// ContainerProfile::releaseProfile() is called.
ContainerProfile::~ContainerProfile()
{
}

/**
  Setup the environmental profile prior to creating some QCa widgets.
  The new widgets will use this profile to determine their external environment.

  This method locks access to the envionmental profile. ReleaseProfile() must be
  called to release the lock once all QCa widgets have been created.
  */
void ContainerProfile::setupProfile( QObject* statusMessageConsumerIn,
                                     QObject* errorMessageConsumerIn,
                                     QObject* warningMessageConsumerIn,
                                     QObject* guiLaunchConsumerIn,
                                     QString pathIn,
                                     QString parentPathIn,
                                     QString macroSubstitutionsIn )
{
    // Publish the profile supplied
    publishProfile(statusMessageConsumerIn,
                   errorMessageConsumerIn,
                   warningMessageConsumerIn,
                   guiLaunchConsumerIn,
                   pathIn,
                   parentPathIn,
                   macroSubstitutionsIn );

    // Save a local copy of what has been published
    takeLocalCopy();
}

/**
  Update published signal consumer objects.
  This is used if the signal consumer objects were not available when the profile was
  first set up, or if the objects are changing
  */
void ContainerProfile::updateConsumers( QObject* statusMessageConsumerIn,
                                     QObject* errorMessageConsumerIn,
                                     QObject* warningMessageConsumerIn,
                                     QObject* guiLaunchConsumerIn )
{
    // If no profile has been defined, then can't update it
    if( !isProfileDefined() )
    {
        qDebug() << "Can't update consumers as a published profile has not yet been defined";
        return;
    }

    // Update the published profile
    publishedStatusMessageConsumer = statusMessageConsumerIn;
    publishedErrorMessageConsumer = errorMessageConsumerIn;
    publishedWarningMessageConsumer = warningMessageConsumerIn;

    publishedGuiLaunchConsumer = guiLaunchConsumerIn;

    // Keep the local copy matching what has been published
    takeLocalCopy();
}

/**
  Update just the published signal consumer object that is used to launch new GUIs.
  The previous object is returned so it can be reinstated later.
  */
QObject* ContainerProfile::replaceGuiLaunchConsumer( QObject* newGuiLaunchConsumerIn )
{
    QObject* savedGuiLaunchConsumer = guiLaunchConsumer;
    publishedGuiLaunchConsumer = newGuiLaunchConsumerIn;
    guiLaunchConsumer = publishedGuiLaunchConsumer;

    return savedGuiLaunchConsumer;
}

/*
  Set up the published profile.
  All instances of ContainerProfile will be able to see the published profile.
  */
void ContainerProfile::publishProfile( QObject* statusMessageConsumerIn,
                                     QObject* errorMessageConsumerIn,
                                     QObject* warningMessageConsumerIn,
                                     QObject* guiLaunchConsumerIn,
                                     QString pathIn,
                                     QString parentPathIn,
                                     QString macroSubstitutionsIn )
{
    // Do nothing if a profile has already been published
    if( profileDefined )
    {
        qDebug() << "Can't publish a profile as one is already published";
        return;
    }

    // Publish the profile
    publishedStatusMessageConsumer = statusMessageConsumerIn;
    publishedErrorMessageConsumer = errorMessageConsumerIn;
    publishedWarningMessageConsumer = warningMessageConsumerIn;

    publishedGuiLaunchConsumer = guiLaunchConsumerIn;

    publishedPath = pathIn;
    publishedParentPath = parentPathIn;

    publishedMacroSubstitutions.clear();
    if( !macroSubstitutionsIn.isEmpty() )
    {
        publishedMacroSubstitutions.append( macroSubstitutionsIn );
    }

    // flag a published profile now exists
    profileDefined = true;
}

/*
 Take a local copy of the profile visable to all instances of ContainerProfile
 */
void ContainerProfile::takeLocalCopy()
{
    QString subs;
    for( int i = 0; i < publishedMacroSubstitutions.size(); i++ )
    {
        subs.append( " " );
        subs.append( publishedMacroSubstitutions[i] );
    }

    setupLocalProfile( publishedStatusMessageConsumer,
                       publishedErrorMessageConsumer,
                       publishedWarningMessageConsumer,
                       publishedGuiLaunchConsumer,
                       publishedPath,
                       publishedParentPath,
                       subs );
}

/**
  Set up the local profile only (don't refer to any published profile)
  This is used when a QCa widget needs a profile, but none has been published.
  A default local profile can be set up using this method.
  The local profile can then be made public if required by calling publishOwnProfile()
  */
void ContainerProfile::setupLocalProfile( QObject* statusMessageConsumerIn,
                                     QObject* errorMessageConsumerIn,
                                     QObject* warningMessageConsumerIn,
                                     QObject* guiLaunchConsumerIn,
                                     QString pathIn,
                                     QString parentPathIn,
                                     QString macroSubstitutionsIn )
{
    // Set up the local profile as specified
    statusMessageConsumer = statusMessageConsumerIn;
    errorMessageConsumer = errorMessageConsumerIn;
    warningMessageConsumer = warningMessageConsumerIn;

    guiLaunchConsumer = guiLaunchConsumerIn;

    macroSubstitutions = macroSubstitutionsIn;

    macroSubstitutions = macroSubstitutionsIn;

    path = pathIn;
    parentPath = parentPathIn;

}

/**
  Extend the macro substitutions currently being used by all new QCaWidgets.
  This is used when a form is created. This allow a form to pass on macro substitutions to the QCa widgets it contains.
  Since it adds to the end of the existing macro substitutions, any substitutions already added by the originating
  container or higher forms take precedence.
  */
void ContainerProfile::addMacroSubstitutions( QString macroSubstitutionsIn )
{
    if( profileDefined  )
        publishedMacroSubstitutions.append( macroSubstitutionsIn );
}

/**
  Reduce the macro substitutions currently being used by all new QCaWidgets.
  This is used after a form is created. Any macro substitutions passed on by the form being created are no longer relevent.
  */
void ContainerProfile::removeMacroSubstitutions()
{
    if( profileDefined && !publishedMacroSubstitutions.isEmpty() )
        publishedMacroSubstitutions.removeLast();
}

/**
  Set the published profile to whatever is saved in our local copy
  */
void ContainerProfile::publishOwnProfile()
{
    publishProfile( statusMessageConsumer,
                          errorMessageConsumer,
                          warningMessageConsumer,
                          guiLaunchConsumer,
                          path,
                          parentPath,
                          macroSubstitutions );
}

/**
  Clears any profile context. Must be called by any code that calls setupProfile() once the profile should no longer be used
  */
void ContainerProfile::releaseProfile()
{
    // Clear the profile
    publishedStatusMessageConsumer = NULL;
    publishedErrorMessageConsumer = NULL;
    publishedWarningMessageConsumer = NULL;

    publishedGuiLaunchConsumer = NULL;

    publishedPath.clear();
    publishedParentPath.clear();

    publishedMacroSubstitutions.clear();

    containedWidgets.clear();

    // Indicate no profile is defined
    profileDefined = false;
}

/**
  Return the object to emit status message signals to.
  If NULL, there is no object available.
  */
QObject* ContainerProfile::getStatusMessageConsumer()
{
    return statusMessageConsumer;
}

/**
  Return the object to emit error message signals to.
  If NULL, there is no object available.
  */
QObject* ContainerProfile::getErrorMessageConsumer()
{
    return errorMessageConsumer;
}

/**
  Return the object to emit warning message signals to.
  If NULL, there is no object available.
  */
QObject* ContainerProfile::getWarningMessageConsumer()
{
    return warningMessageConsumer;
}

/**
  Return the object to emit GUI launch request signals to.
  If NULL, there is no object available.
  */
QObject* ContainerProfile::getGuiLaunchConsumer()
{
    return guiLaunchConsumer;
}

/**
  Return the application path to use for file operations.
  */
QString ContainerProfile::getPath()
{
    return path;
}

/**
  Return the current object path to use for file operations.
  */
QString ContainerProfile::getParentPath()
{
    return parentPath;
}

/**
  Set the current object path to use for file operations.
  */
void ContainerProfile::setPublishedParentPath( QString publishedParentPathIn )
{
    publishedParentPath = publishedParentPathIn;
}

/**
  Return the object to emit warning message signals to.
  If NULL, there is no object available.
  */
QString ContainerProfile::getMacroSubstitutions()
{
    return macroSubstitutions;
}

/**
  Return the flag indicating true if a profile is currently being published.
  */
bool ContainerProfile::isProfileDefined()
{
    return profileDefined;
}

/**
  Add a QCa widget to the list of QCa widgets created under the currently published profile.
  This provides the application with a list of its QCa widgets without having to trawl through
  the widget hierarchy looking for them. Note, in some applications the application may know
  exactly what QCa widgets have been created, but if the application has loaded a .ui file
  unrelated to the application development (for example, a user created control GUI), then the
  application will not know how many, if any, QCa widgets it owns.
  */
void ContainerProfile::addContainedWidget( QCaWidget* containedWidget )
{
    containedWidgets.append( WidgetRef( containedWidget ) );
}

/**
  Return the next QCa widget from the list of QCa widgets built using addContainedWidget().
  Note, this is destructive to the list. It is fine if the application only needs to get the
  widgets from the list once, such as when activating QCa widgets after creating a form.
  */
QCaWidget* ContainerProfile::getNextContainedWidget()
{
    // Remove and return the first widget in the list, or return NULL if no more
    if( !containedWidgets.isEmpty() )
        return containedWidgets.takeFirst().getRef();
    else
        return NULL;
}
