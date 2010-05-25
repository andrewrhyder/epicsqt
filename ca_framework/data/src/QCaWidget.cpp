/*! 
  \class QCaWidget
  \version $Revision: #3 $
  \date $DateTime: 2009/07/30 14:33:44 $
  \author anthony.owen
  \brief Template for Qt-CA based widgets.
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
 * anthony.owen@synchrotron.org.au
 * 800 Blackburn Road, Clayton, Victoria 3168, Australia.
 *
 */

/*!
  This class is used as a base for all CA aware wigets, such as QCaLabel, QCaSpinBox, etc.
  It manages common issues including creating a source of CA data updates, handling error,
  warning and status messages, and setting tool tips based on variable names.
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

    // Setup an object to emit message signals as required
    userMessage.setup( getStatusMessageConsumer(),
                       getErrorMessageConsumer(),
                       getWarningMessageConsumer() );

    // If there is a profile defining the environment containing this widget, add this widget
    // to the list of contained widgets so whatever is managing the container can activate this widget.
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
    for( unsigned int i = 0; i < numVariables; i++ ) {
        deleteQcaItem( i );
    }
    delete[] qcaItem;
}

/*!
    Set the number of variables that will be used for this widget.
    Create an array of QCaObject based objects to suit.
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

// Initiate updates.
void QCaWidget::activate()
{

    for( unsigned int i = 0; i < numVariables; i++ )
        establishConnection( i );
}


/*!
    Create a CA connection and initiates updates if required.
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

    /// Connect to new variable
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
    A QCaObject is created to supply CA data updates for each variable name used by the widget.
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
