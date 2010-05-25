/*!
  \class CmdPushButton
  \version $Revision: #2 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author Last checked in by: $Author: rhydera $
  \brief Command Push Button Widget.
 */
/*
 *  This file is part of the EPICS QT Framework.
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
 *    andrew@rhyder.org
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

