/*  MainWindow.cpp
 *
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2014 Australian Synchrotron
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/*
 * Simple GUI application to demonstrate programatically adding QE widgets to a user interface
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QELabel.h"
#include "QEComboBox.h"
#include <QPalette>
#include "QELineEdit.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Read the UI
    ui->setupUi(this);

    // Create a QELabel
    QELabel* qel = new QELabel( centralWidget() );
//    QELabel* qel = new QELabel( "OOE:ai", centralWidget() );  // See below for all the alternatives for setting a variable name

    // Set its position and size
    qel->setGeometry( 100, 30, 200, 30 );

    // Display alarm state option 1.
    // Don't display the alarm state. This is done by setting the background colour and
    // we will be setting the background ourselves and don't want it overwritten
//    qel->setDisplayAlarmStateOption( standardProperties::DISPLAY_ALARM_STATE_NEVER );

    // Display alarm state option 2.
    // Don't display the alarm state unless it is actually in alarm.
    // This is done by setting the background colour and
    // we will be setting the background ourselves and don't want it overwritten unless nessesary
    qel->setDisplayAlarmStateOption( standardProperties::DISPLAY_ALARM_STATE_WHEN_IN_ALARM );

    // Display alarm state option 3.
    // Always display display the alarm state even when the current alarm state is 'No Alarm'.
    // This is done by setting the background colour which will make setting the background ourselves
    // (below) redundant.
//    qel->setDisplayAlarmStateOption( standardProperties::DISPLAY_ALARM_STATE_ALWAYS );

    // Set the background to blue
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::blue );
    qel->setAutoFillBackground(true);
    qel->setPalette(Pal);

    //=============================================================================================
    // Set the variable name and establish a CA connection using one of the following alternatives:
    // Only one of the following alternatives should be uncommented.
    //=============================================================================================

    // Alternative 1.
    // Set the variable name using the access function for the variable name properties.
    // For a widget with only a single variable this is generally setVariableNameProperty().
    // This will activate the widget using the variable name and the current macro substitutions.
    // Note, there is no variable index. QE widgets that take more than one variable name have a different
    // set and get property function for each variable. For example, QEImage has setVariableName0Property(),
    // setVariableName1Property(), etc.
    // CA connection is established for the resultant variable immedietly (unless called from within
    // designer when the connection is only established once the user has stopped typing for a short time).
    // Using setVariableNameProperty() and similar 'set variable name property' functions has the advantage
    // that only a single call is required to set the variable name and to establish the connection.
    // Macro substitutions should be set beforehand if required using setVariableNameSubstitutionsProperty().
    // Calling setVariableNameSubstitutionsProperty() later works but an inapropriate connection
    // without macro substitutions will be attempted first.

/*
    qel->setVariableNameSubstitutionsProperty( "P=OOE" );
    qel->setVariableNameProperty( "$(P):ai" );
*/


    //=============================================================================================
    // Alterntive 2.
    // Set the variable name and substitutions.
    // CA connection is established for the resultant variable immedietly.
    // Using this alternative has the advantage of setting the variable name and macro substitutions,
    // and establishing a CA connection, in a single call.

    qel->setVariableNameAndSubstitutions( "$(P):ai", "P=OOE", 0 );



    //=============================================================================================
    // Alternative 3.
    // Set the variable name and substitutions seperately, then once variable names are resolved
    // establish a CA connection using activate().
    // Note, the calls to setVariableNameSubstitutions() and setVariableName() can occur in any order and
    // setVariableNameSubstitutions() is optional.
    // Using this alternative has the advantage of seperating the tasks of dealing with macro substitutions, variable names, and establisghing CA connections.
    // Note, calling activate() does more than just establish a CA connection, it also asks the widget
    // to perform any tasks which should only be done once all other widgets have been created. For example,
    // calling activate() may notify other widgets about itself so activate() should only be called once all other widgets this widget may relate to are constructed.

/*
    qel->setVariableNameSubstitutions( "P=OOE" );
    qel->setVariableName( "$(P):ai", 0 );
    qel->activate();
*/

    //=============================================================================================
    // Alternative 4.
    // !!!!! CURRENTLY this alternative is unavailable as establishConnection() is not public. Should this change?
    // Set the variable name and substitutions seperately, then once variable names are resolved
    // establish a CA connection using establishConnection().
    // Note, the calls to setVariableNameSubstitutions() and setVariableName() can occur in any order and
    // setVariableNameSubstitutions() is optional.
    // Using this alternative has the advantage of seperating the tasks of dealing with macro substitutions, variable names, and establisghing CA connections.
    // Note, unlike activate() in alternative 3 calling establishConnection() only updates the
    // CA connection.

/*
    qel->setVariableNameSubstitutions( "P=OOE" );
    qel->setVariableName( "$(P):ai", 0 );
    qel->establishConnection( 0 );
*/

    //=============================================================================================
    // Alternative 4.
    // Don't uncomment this here! If you want tp try it out, uncomment it above.
    // It is a different way to construct the label and is duplicated here for the sake of the narative.
    // When this constructor is used, the variable name is set and the widget is activated immedietly.

//    QELabel* qel = new QELabel( "OOE:ai", centralWidget() );


    //=============================================================================================
    // Alternative 5.
    // In this simplistic example this alternative appears cumbersome.
    // This alternative is used when setting the properties is out of the control of the programmer.
    // For example, when loading a .ui file using Qt's UI loader.
    // When using the UI loader, the caller has no clue what widgets are being created,
    // what properties are being set, in what order, and when all properties have been set.
    // This alternative allows the establishment of a CA connection to be held off until it is
    // cirtain that all property set functions that are going to be called have been called.
    // QEForm uses this alternative when loading .ui files.

    // Flag we don't want immediate activation...
/*
    ContainerProfile profile;
    bool oldDontActivateYet = profile.setDontActivateYet( true );

    // Set the properties. Here we know exactly what we are setting, but when loading a .ui
    // file using Qt's UI loader, we don't know what properties are being set, or in what order...
    qel->setVariableNameProperty( "$(P):ai" );
    qel->setVariableNameSubstitutionsProperty( "P=OOE" );

    // Remove the flag and establish a CA connection...
    profile.setDontActivateYet( oldDontActivateYet );
    qel->activate();
*/

    //=============================================================================================



    // Create a QEComboBox and set its position and size
    QEComboBox* qeqb = new QEComboBox( centralWidget() );
    qeqb->setGeometry( 100, 90, 200, 30 );

    // Don't display the alarm state. This is done by setting the background colour and
    // we will be setting the background ourselves and don't want it overwritten
    // USE OF setDisplayAlarmState() is deprecated. USE setDisplayAlarmStateOption() instead
    qeqb->setDisplayAlarmState( false );

    // Set the variable name using the access function for the variable name property.
    // This will activate the widget using the variable name and the current macro substitutions.
    // (See the creation of the QELabel above to see alternative ways of setting the
    // variable name and macro substitutions).
    qeqb->setVariableNameProperty( "OOE:mbbi" );

    // Create a QELineEdit with an active variable
    // and set its position and size
    QELineEdit* qew = new QELineEdit( "OOE:ai", centralWidget() );
    qew->setGeometry( 100, 150, 200, 30 );

}

MainWindow::~MainWindow()
{
    delete ui;
}
