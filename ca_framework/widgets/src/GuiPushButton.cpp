/*!

  \class GuiPushButton

  \version $Revision: #5 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author Last checked in by: $Author: rhydera $

  \brief Launch GUI Push Button Widget.

  Copyright (c) 2009 Australian Synchrotron

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  Licence as published by the Free Software Foundation; either
  version 2.1 of the Licence, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public Licence for more details.

  You should have received a copy of the GNU Lesser General Public
  Licence along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.

  Contact details:
  andrew.rhyder@synchrotron.org.au
  800 Blackburn Road, Clayton, Victoria 3168, Australia.

*/

#include <QMainWindow>
#include <GuiPushButton.h>
#include <ASguiForm.h>
#include <QMessageBox>

#include <QtDebug>//Temp???


/*!
    ???
*/
GuiPushButton::GuiPushButton( QWidget *parent ) : QPushButton( parent ) {
    // Set default button text
    setText( "Launch GUI" );

    // Set default properties
    creationOption = ASguiForm::CREATION_OPTION_OPEN;

    // Use click signal to launch new GUI
    QObject::connect( this, SIGNAL( clicked() ), this, SLOT( userClicked() ) );

    // If a profile is define by whatever contains the button, use it
    if( profile.isProfileDefined() )
    {
        // A profile is already defined, either by the gui application or a ASGui form form.
        // Extend any variable name substitutions with this form's substitutions
        profile.addMacroSubstitutions( variableNameSubstitutions );

        // Setup a signal to launch a new gui
        // The signal will be used by whatever the button is in
        QObject::connect( (QObject*)this, SIGNAL( newGui(  QString, QString, ASguiForm::creationOptions ) ),
                          profile.getGuiLaunchConsumer(), SLOT( launchGui( QString, QString, ASguiForm::creationOptions ) ) );

    }

    // A profile is not already defined, create one. This is the case if this class is used by an application that does not set up a profile, such as 'designer'.
    else
    {
        // Set up the button's own message handler
        userMessage.setup( this );

        // Set up the button's own gui form launcher
        QObject::connect( (QObject*)this, SIGNAL( newGui(  QString, QString, ASguiForm::creationOptions ) ),
                          this, SLOT( launchGui( QString, QString, ASguiForm::creationOptions ) ) );
    }
}

/*!
    Update the tool tip as requested by QCaWidget.
*/
void GuiPushButton::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Button click event.
*/
void GuiPushButton::userClicked() {

    emit newGui( guiName, variableNameSubstitutions, creationOption );
}

// Slot for presenting messages to the user.
// Normally a gui will have provided it's own message and error handling.
// This is only used if no environment profile has been set up when a form is created. This is the case if created within 'designer'
// Use a general message
void GuiPushButton::onGeneralMessage( QString message )
{
    QMessageBox msgBox;
    msgBox.setText( "Message" );
    msgBox.setInformativeText( message );
    msgBox.exec();
}

// Slot for launching a new gui.
// This is the button's default action for launching a gui.
// Normally the button would be within a container, such as a tab on a gui, that will provide a 'launch gui' mechanism.
void GuiPushButton::launchGui( QString guiName, QString /*substitutions*/, ASguiForm::creationOptions )
{
    // Extend substitutions???


    // Build the gui
    // Build it in a new window.
    //??? This could use the create options as follows: (instead of always creating a new window)
    //       - Wind up through parents until the parent of the first scroll
    //       - Replace the scroll area's widget with the new gui
    QMainWindow* w = new QMainWindow;
    ASguiForm* gui = new ASguiForm( guiName );
    if( gui )
    {
        gui->readUiFile();
        w->setCentralWidget( gui );
        w->show();
    }
    else
    {
        delete w;
    }
}


// Slot same as default widget setEnabled slot, but renamed to match other QCa widgets where requestEnabled() will use our own setEnabled
// which will allow alarm states to override current enabled state
void GuiPushButton::requestEnabled( const bool& state )
{
    setEnabled(state);
}
