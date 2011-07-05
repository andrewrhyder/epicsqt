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

/*!
  This class is a label that uses the same macro substitution mechanisms as other CA aware label widgets.
  It is intended to enable unique titles and text in sub forms.
 */

#include <QSubstitutedLabel.h>

/*!
    Constructor with no initialisation
*/
QSubstitutedLabel::QSubstitutedLabel( QWidget *parent ) : QLabel( parent ) {
    setup();
}

/*!
    Setup common to all constructors
*/
void QSubstitutedLabel::setup() {

    // Set up the number of variables managed by the variable name manager
    // NOTE: there is no data associated with this widget, but it uses the same substitution mechanism as other data widgets.
    variableNameManagerInitialise( 1 );


    // Set the initial state
    setText( "" );

    // Use label signals
    // --Currently none--
}

//==============================================================================
// Property convenience functions


// Access functions for variableName and variableNameSubstitutions
// variable substitutions Example: SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
//
// NOTE: there is no data associated with this widget, but it uses the same substitution mechanism as other data widgets.
void QSubstitutedLabel::setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex ) {
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
    setVariableName( variableNameIn, variableIndex );
    establishConnection( variableIndex );
}

/*!
    Update the label text with the required substitutions.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    NOTE, In this usage, the variable name manager is used to manage substitutions in the text displayed in the label, so the
    'Variable Name' is not used by this widget as as a PV name (as for most other widgets), rather is is used as text to display.
*/
void QSubstitutedLabel::establishConnection( unsigned int variableIndex ) {

    setText( this->getSubstitutedVariableName( variableIndex ) );
}
