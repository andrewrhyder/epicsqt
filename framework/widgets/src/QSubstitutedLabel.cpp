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
QSubstitutedLabel::QSubstitutedLabel( QWidget *parent ) : QLabel( parent ), QCaWidget( this ) {
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

// label text (prior to substitution)
void QSubstitutedLabel::setLabelTextProperty( QString labelTextIn )
{
    bool wasBlank = labelText.isEmpty();
    labelText = labelTextIn;

    // Update the button's text.
    // But don't do it if the label was already displaying something and the
    // text-to-be-substituted is just being re-set to blank). This behaviour will
    // mean the normal label 'text' property can be used if text substitution is
    // not required. Without this the user would always have to use the labelText property.
    if( !text().isEmpty() && !(wasBlank && labelText.isEmpty() ))
    {
        setText( substituteThis( labelText ));
    }
}

QString QSubstitutedLabel::getLabelTextProperty()
{
    return labelText;
}

// Get the label text with line feeds replaced with C style line feed characters.
// This is to allow line feed to be entered in the property in designer, like the QLabel text property
QString QSubstitutedLabel::getLabelTextPropertyFormat()
{
    return getLabelTextProperty().replace( "\n", "\\n" );

}

// Set the label text with C style line feed characters replaced with line feeds.
// This is to allow line feed to be entered in the property in designer, like the QLabel text property
void QSubstitutedLabel::setLabelTextPropertyFormat( QString labelTextIn )
{
    setLabelTextProperty( labelTextIn.replace( "\\n", "\n" ));
}


/*!
    Update the label text with the required substitutions.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    NOTE, In this usage, the variable name manager is used to manage substitutions in the text displayed in the label, so the
    'Variable Name' is not used by this widget (as for most other widgets), rather the substitutions are applied to fixed text.
*/
void QSubstitutedLabel::establishConnection( unsigned int ) {

    setText( substituteThis( labelText ));
}
