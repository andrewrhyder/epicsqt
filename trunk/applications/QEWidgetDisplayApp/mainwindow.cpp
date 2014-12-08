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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Read the UI
    ui->setupUi(this);

    // Create a QELabel and set its position and size
    QELabel* qel = new QELabel( centralWidget() );
    qel->setGeometry( 100, 50, 200, 50 );

    // Don't display the alarm state. This is done by setting the background colour and
    // we will be setting the background ourselves and don't want it overwritten
    qel->setDisplayAlarmState( false );

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

//    qel->setVariableNameSubstitutionsProperty( "P=OOE" );
//    qel->setVariableNameProperty( "$(P):ai" );



    //=============================================================================================
    // Alterntive 2.
    // Set the variable name and substitutions.
    // CA connection is established for the resultant variable immedietly.
    // Using this alternative has the advantage of setting the variable name and macro substitutions,
    // and establishing a CA connection, in a single call.

//    qel->setVariableNameAndSubstitutions( "$(P):ai", "P=OOE", 0 );



    //=============================================================================================
    // Alternative 3.
    // Set the variable name and substitutions seperately, then once variable names are resolved
    // establish a CA connection.
    // Note, the calls to setVariableNameSubstitutions() and setVariableName() can occur in any order and
    // setVariableNameSubstitutions() is optional.
    // Using this alternative has the advantage of seperating the tasks of dealing with macro substitutions, variable names, and establisghing CA connections.

//    qel->setVariableNameSubstitutions( "P=OOE" );
//    qel->setVariableName( "$(P):ai", 0 );
//    qel->activate();


    //=============================================================================================
    // Alternative 4.
    // In this simplistic example this alternative appears cumbersome.
    // This alternative is used when setting the properties is out of the control of the programmer.
    // For example, when loading a .ui file using Qt's UI loader.
    // When using the UI loader, the caller has no clue what widgets are being created,
    // what properties are being set, in what order, and when all properties have been set.
    // This alternative allows the establishment of a CA connection to be held off until it is
    // cirtain that all property set functions that are going to be called have been called.
    // QEForm uses this alternative when loading .ui files.

    // Flag we don't want immediate activation...
    ContainerProfile profile;
    bool oldDontActivateYet = profile.setDontActivateYet( true );

    // Set the properties. Here we know exactly what we are setting, but when loading a .ui
    // file using Qt's UI loader, we don't know what properties are being set, or in what order...
    qel->setVariableNameProperty( "$(P):ai" );
    qel->setVariableNameSubstitutionsProperty( "P=OOE" );

    // Remove the flag and establish a CA connection...
    profile.setDontActivateYet( oldDontActivateYet );
    qel->activate();


    //=============================================================================================



    // Create a QEComboBox and set its position and size
    QEComboBox* qeqb = new QEComboBox( centralWidget() );
    qeqb->setGeometry( 100, 150, 200, 50 );

    // Don't display the alarm state. This is done by setting the background colour and
    // we will be setting the background ourselves and don't want it overwritten
    qeqb->setDisplayAlarmState( false );

    // Set the variable name using the access function for the variable name property.
    // This will activate the widget using the variable name and the current macro substitutions.
    // (See the creation of the QELabel above to see alternative ways of setting the
    // variable name and macro substitutions).
    qeqb->setVariableNameProperty( "OOE:mbbi" );
}

MainWindow::~MainWindow()
{
    delete ui;
}
