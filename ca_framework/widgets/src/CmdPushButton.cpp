/*!
  \class CmdPushButton
  \version $Revision: #2 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author Last checked in by: $Author: rhydera $
  \brief Command Push Button Widget.
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

#include <CmdPushButton.h>
#include <QMessageBox>
#include <QProcess>

#include <QtDebug>//Temp???


/*!
    Construct a push button to execute commands
*/
CmdPushButton::CmdPushButton( QWidget *parent ) : QPushButton( parent ) {
    // Set default button text
    setText( "Run Command" );

    // Use click signal to run command
    QObject::connect( this, SIGNAL( clicked() ), this, SLOT( userClicked() ) );

    // If a profile is define, extend the substitutions
    if( profile.isProfileDefined() )
        profile.addMacroSubstitutions( variableNameSubstitutions );
}

/*!
    Update the tool tip as requested by QCaWidget.
*/
void CmdPushButton::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Button click event.
*/
void CmdPushButton::userClicked() {

    //??? use substitutions (from the profile) in the command and arguments (change name from variableNameSubstitutions to commandSubstitutions)
    QProcess *process = new QProcess();
    process->start( program, arguments );

}


/*!
    Slot same as default widget setEnabled slot, but renamed to match other QCa widgets where requestEnabled() will use our own setEnabled
    which will allow alarm states to override current enabled state
*/
void CmdPushButton::requestEnabled( const bool& state )
{
    setEnabled(state);
}

