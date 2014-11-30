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
#include <QPalette>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Read the UI
    ui->setupUi(this);

    // Create a QELabel and set its position and size
    QELabel* qel = new QELabel( centralWidget() );
    qel->setGeometry( 100, 100, 200, 50 );

    // Don't display the alarm state. This is done by setting the background colour and
    // we will be setting the background ourselves and don't want it overwritten
    qel->setDisplayAlarmState( false );

    // Set the background to blue
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::blue );
    qel->setAutoFillBackground(true);
    qel->setPalette(Pal);

    // Alternative 1
    // Set the variable name using the access function for the variable name property.
    // This will activate the widget using the variable name and the current macro substitutions.
    qel->setVariableNameProperty( "OOE:ai" );

    // Alterntive 2
    // Set the variable name and substitutions, then once data access is defined, activate the widget
    qel->setVariableNameAndSubstitutions( "OOE:ai", "", 0 );
    qel->activate();
}

MainWindow::~MainWindow()
{
    delete ui;
}
