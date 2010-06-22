/*! 
  \class QCaWidget
  \version $Revision: #4 $
  \date $DateTime: 2010/06/21 11:33:51 $
  \author anthony.owen
  \brief Template for Qt-CA based widgets.
 */
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
 *    Anthony Owen
 *  Contact details:
 *    anthony.owen@gmail.com
 */

/*!
  This class is used as a base for all CA aware wigets, such as QCaLabel, QCaSpinBox, etc.
  It manages common issues including creating a source of CA data updates, handling error,
  warning and status messages, and setting tool tips based on variable names.

  Note, there is tight integration between the CA aware widget classes, this class, and its
  base classes, especially VariableNameManager and QCaToolTip.

  In particular, this class manages QCaObject classes that stream updates to the
  CA aware widget class. But this class, however, doesn't know how to format the data,
  or how the updates will be used.
  To resolve this, this class asks its parent class (such as QCaLabel) to create the
  QCaObject class in what ever flavour it wants, by calling the virtual function createQcaItem.
  A QCaLabel, for example, wants string updates so it creates a QCaString which is based on a
  QCaObject class and formats all updates as strings.
  The widget specific version of QCaObject is passed back to this class to manage by calling QCaWidget::setQcaItem().
  The CA aware parent class (such as QCaLabel) defines a variable by calling VariableNameManager::setVariableName().
  The VariableNameManager class calls the establishConnection function of the CA aware parent class, such as QCaLabel
  when it has a new variable name.
  This class uses its base QCaToolTip class to format tool tips. that class in turn calls the CA aware parent class
  (such as QCaLabel) directly to make use of a new tool tip.
 */

#include <QDebug>
#include <QCaWidget.h>

/*!
    Constructor
*/
QCaWidget::QCaWidget() {

    /// Initially flag no variables array is defined.
    /// This will be corrected when the first variable is declared
    numVariables = 0;
    qcaItem = 0;

    /// Default properties
    subscribeProperty = true;
    variableAsToolTipProperty = true;

    // Setup an object to emit message signals according to the profile defined by whatever is creating
    // this widget (or use a default mechanism if no profile has been defined)
    // If there is a profile defining the environment containing this widget, add this widget
    // to the list of contained widgets so whatever is managing the container can activate this widget.
    //
    // Although a widget is self contained, whatever is creating the widget has the option of providing
    // a list of services and other information through a containerProfile that QCaWidgets can use.
    // For example, an application creating QCaWidgets can provide a mechanism to display error
    // messages in a manner appropriate for the application.
    // In this case, the widget is taking the oppertunity to tell its creator it exists, and also to
    // get any variable name macro substitutions offered by its creator.
    userMessage.setup( getStatusMessageConsumer(),
                       getErrorMessageConsumer(),
                       getWarningMessageConsumer() );
    if( isProfileDefined() )
    {
        addContainedWidget( this );
        setVariableNameSubstitutionsOverride( getMacroSubstitutions() );
    }
}

/*!
    Destruction:
    Delete all variable sources for the widgeet
*/
QCaWidget::~QCaWidget() {

    // Delete all the QCaObject instances
    for( unsigned int i = 0; i < numVariables; i++ ) {
        deleteQcaItem( i );
    }

    // Release the list
    delete[] qcaItem;
    qcaItem = NULL;
}

/*!
    Set the number of variables that will be used for this widget.
    Create an array of QCaObject based objects to suit.
    This is called by the CA aware widgets based on this class, such as a QCaLabel.
*/
void QCaWidget::setNumVariables( unsigned int numVariablesIn ) {

    /// Get the number of variables that will be used by this widget
    /// Don't accept zero or the qca array will be invalid
    if( numVariablesIn ) {
        numVariables = numVariablesIn;
    } else {
        numVariables = 1;
    }

    /// Set up the number of variables managed by the variable name manager
    variableNameManagerInitialise( numVariables );

    /// Allocate the array of QCa objects
    qcaItem = new qcaobject::QCaObject* [numVariables];
    for( unsigned int i = 0; i < numVariables; i++ ) {
        qcaItem[i] = NULL;
    }
}

/*!
   Initiate updates.
   This is only required when QCa widgets are loaded within a form and not directly by 'designer'.
   When loaded directly by 'designer' they are activated (a CA connection is established) as soon as either
   the variable name or variable name substitution properties are set
 */
void QCaWidget::activate()
{
    // For each variable, ask the CA aware widget based on this class to initiate updates and to set up
    // whatever signal/slot connections are required to make use of data updates.
    // Note, establish connection is a virtual function of the VariableNameManager class and is normally
    // called by that class when a variable name is defined or changed
    for( unsigned int i = 0; i < numVariables; i++ )
        establishConnection( i );
}


/*!
    Create a CA connection and initiates updates if required.
    This is called by the establishConnection function of CA aware widgets based on this class, such as a QCaLabel.
*/
bool QCaWidget::createConnection( unsigned int variableIndex ) {

    /// If the index is invalid do nothing
    /// This same test is also valid if qcaItem has never been set up yet as numVariables will be zero
    if( variableIndex >= numVariables ) {
        return false;
    }

    // Set the tool tip to the variable names if required
    if( variableAsToolTipProperty ) {
        QString tip;
        for( unsigned int i = 0; i < numVariables; i++ ) {
            /// If a variable name is present, add it to the tip
            QString variableName = getSubstitutedVariableName( i );
            if( variableName.size() ) {
                /// Add a space between variable names
                if( tip.size() > 0 ) {
                    tip.append( QString( " " ) );
                }
                /// Add the variable name to the tip
                tip.append( variableName );
            }
        }

        if( tip.size() == 0 ) {
            tip = "No variables defined";
        }
        updateToolTipVariable( tip );
    }

    /// Remove any existing QCa connection
    deleteQcaItem( variableIndex );

    /// Connect to new variable.
    /// If a new variable name is present, ask the CA aware widgets based on this class to create an
    /// appropriate object based on a QCaObject (by calling its createQcaItem function) and subscribe
    /// to the new variable
    if( getSubstitutedVariableName( variableIndex ).length() > 0 ) {
        createQcaItem( variableIndex );
        qcaItem[variableIndex]->setUserMessage( &userMessage );

        if( subscribeProperty )
            qcaItem[variableIndex]->subscribe();

        return true;
    }

    return false;
}

/*!
    Save a reference to a recently created QCaObject based object.
    A QCaObject based class is created by the CA aware widgets based on this class (such as QCaLabel)
    to supply CA data updates for each variable name used by the CA aware widget.
    This function is called by the CA aware widget to manage the newly created QCaObject based class.
*/
void QCaWidget::setQcaItem( qcaobject::QCaObject *newQcaItem, unsigned int variableIndex ) {
    /// If the index is invalid do nothing
    /// This same test is also valid if qcaItem has never been set up yet as numVariables will be zero
    if( variableIndex >= numVariables ) {
        return;
    }

    /// Delete any previous QCaObject used by the widget, and record a reference to the new object
    deleteQcaItem( variableIndex );
    qcaItem[variableIndex] = newQcaItem;
}

/*!
    Return a reference to one of the qCaObjects used to stream CA data updates to the widget
    This is called by CA aware widgets based on this class, such as a QCaLabel, mainly when they
    want to connect to its signals to recieve data updates.
*/
qcaobject::QCaObject* QCaWidget::getQcaItem( unsigned int variableIndex ) {
    /// If the index is invalid return NULL.
    /// This same test is also valid if qcaItem has never been set up yet as numVariables will be zero
    if( variableIndex >= numVariables )
        return NULL;

    // Return the QCaObject used for the specified variable name
    return qcaItem[variableIndex];
}

/*!
    Remove any previous QCaObject created to supply CA data updates for a variable name
*/
void QCaWidget::deleteQcaItem( unsigned int variableIndex ) {
    /// If the index is invalid do nothing.
    /// This same test is also valid if qcaItem has never been set up yet as numVariables will be zero
    if( variableIndex >= numVariables )
        return;

    // Delete the QCaObject used for the specified variable name
    if( qcaItem[variableIndex] )
        delete qcaItem[variableIndex];

    // Remove the reference to the deleted object to prevent accidental use
    qcaItem[variableIndex] = NULL;
}
