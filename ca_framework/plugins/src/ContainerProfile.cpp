/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/plugins/src/ContainerProfile.cpp $
 * $Revision: #1 $ 
 * $DateTime: 2009/07/14 15:59:56 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class ContainerProfile
  \version $Revision: #1 $
  \date $DateTime: 2009/07/14 15:59:56 $
  \author andrew.rhyder
  \brief Defines attributes of the containing window (form, dialog, etc) within which QCa widgets are being created.
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
 *
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
 * Information to be communicated such as message handlers and macro substitutions is set up then
 * the QCa widgets are created using a mechanism such as the QUiLoader class.
 *
 * As each QCa widget is created it also instantiates an instance of the ContainerProfile class.
 * If any information has been provided, it is used or saved for future reference.
 */

#include <ContainerProfile.h>
#include <QCaWidget.h>
#include <QtDebug>


/// Static variables used to pass information from the creator of QCa widgets to the QCa widgets themselves.
QObject* ContainerProfile::publishedStatusMessageConsumer = NULL;
QObject* ContainerProfile::publishedErrorMessageConsumer = NULL;
QObject* ContainerProfile::publishedWarningMessageConsumer = NULL;

QObject* ContainerProfile::publishedGuiLaunchConsumer = NULL;

QList<QString> ContainerProfile::publishedMacroSubstitutions;
QList<WidgetRef> ContainerProfile::containedWidgets;

bool ContainerProfile::publishedInteractive = false;

bool ContainerProfile::profileDefined = false;

/**
  Constructor.
  */
ContainerProfile::ContainerProfile()
{

    // Save the current environment profile.
    // If this is an instance created by a QCa widget then this will storing the
    // profile, set up by the code creating the QCa widgets.
    // If this is the instance created by the code creating the QCa widgets then this
    // will simply clear the saved profile which is never used by this instance anyway.

    statusMessageConsumer = publishedStatusMessageConsumer;
    errorMessageConsumer = publishedErrorMessageConsumer;
    warningMessageConsumer = publishedWarningMessageConsumer;

    guiLaunchConsumer = publishedWarningMessageConsumer;

    interactive = publishedInteractive;

    for( int i = 0; i < publishedMacroSubstitutions.size(); i++ ) {
        macroSubstitutions.append( " " );
        macroSubstitutions.append( publishedMacroSubstitutions[i] );
    }

}

ContainerProfile::~ContainerProfile()
{
}

/**
  Setup the environmental profile prior to creating some QCa widgets.
  The new widgets will use this profile to determine their external environment.

  This method locks access to the envionmental profile. ReleaseProfile() must be
  called to release the lock once all QCa widgets have been created.
  */
void ContainerProfile::setupProfile( QObject* publishedStatusMessageConsumerIn,
                                     QObject* publishedErrorMessageConsumerIn,
                                     QObject* publishedWarningMessageConsumerIn,
                                     QObject* guiLaunchConsumerIn,
                                     QString publishedMacroSubstitutionsIn,
                                     bool interactiveIn )
{
    publishedStatusMessageConsumer = publishedStatusMessageConsumerIn;
    publishedErrorMessageConsumer = publishedErrorMessageConsumerIn;
    publishedWarningMessageConsumer = publishedWarningMessageConsumerIn;

    publishedGuiLaunchConsumer = guiLaunchConsumerIn;

    publishedMacroSubstitutions.clear();
    publishedMacroSubstitutions.append( publishedMacroSubstitutionsIn );

    publishedInteractive = interactiveIn;

    profileDefined = true;
}

/**
  Extend the macro substitutions currently being used by all new QCaWidgets.
  This is used when a form is created. This allow a form to pass on macro substitutions to the QCa widgets it contains.
  Since it adds to the end of the existing macro substitutions, any substitutions already added by the originating
  container or higher forms take precedence.
  */

void ContainerProfile::addMacroSubstitutions( QString macroSubstitutionsIn )
{
    if( profileDefined )
        publishedMacroSubstitutions.append( macroSubstitutionsIn );
}

/**
  Reduce the macro substitutions currently being used by all new QCaWidgets.
  This is used after a form is created. Any macro substitutions passed on by the form being created are no longer relevent.
  */

void ContainerProfile::removeMacroSubstitutions()
{
    if( profileDefined )
        publishedMacroSubstitutions.removeLast();
}

/**
  Clears any profile context. Must be called by any code that calls setupProfile() once the profile should no longer be used
  */
void ContainerProfile::releaseProfile()
{
    publishedStatusMessageConsumer = NULL;
    publishedErrorMessageConsumer = NULL;
    publishedWarningMessageConsumer = NULL;

    publishedGuiLaunchConsumer = NULL;

    publishedMacroSubstitutions.clear();

    containedWidgets.clear();

    publishedInteractive = false;

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
  Return the object to emit warning message signals to.
  If NULL, there is no object available.
  */
QString ContainerProfile::getMacroSubstitutions()
{
    return macroSubstitutions;
}

bool ContainerProfile::isProfileDefined()
{
    return profileDefined;
}

void ContainerProfile::addContainedWidget( QCaWidget* containedWidget )
{
    containedWidgets.append( WidgetRef( containedWidget ) );
}

QCaWidget* ContainerProfile::getNextContainedWidget()
{
    if( !containedWidgets.isEmpty() )
        return containedWidgets.takeFirst().getRef();
    else
        return NULL;
}

bool ContainerProfile::isInteractive()
{
    return interactive;
}

QObject* ContainerProfile::replaceGuiLaunchConsumer( QObject* newGuiLaunchConsumerIn )
{
    QObject* savedGuiLaunchConsumer = guiLaunchConsumer;
    publishedGuiLaunchConsumer = newGuiLaunchConsumerIn;
    guiLaunchConsumer = publishedGuiLaunchConsumer;

    return savedGuiLaunchConsumer;
}
