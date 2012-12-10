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

/*
  This class is a CA aware push button widget based on the Qt push button widget.
  It is tighly integrated with the base class QEWidget. Refer to QEWidget.cpp for details
 */

#include <QERadioButton.h>
#include <QProcess>
#include <QMessageBox>
#include <QMainWindow>
#include <QIcon>

/*
    Constructor with no initialisation
*/
QERadioButton::QERadioButton( QWidget *parent ) : QRadioButton( parent ), QEGenericButton( this ) {
    QEGenericButton::setup();
    setup();
}

/*
    Constructor with known variable
*/
QERadioButton::QERadioButton( const QString &variableNameIn, QWidget *parent ) : QRadioButton( parent ), QEGenericButton( this ) {
    setVariableName( variableNameIn, 0 );

    QEGenericButton::setup();
    setup();

}

/*
    Setup common to all constructors
*/
void QERadioButton::setup() {
    setText( "QERadioButton" );

    // Set up a connection to recieve variable name property changes
    // The variable name property manager class only delivers an updated variable name after the user has stopped typing
    QObject::connect( &variableNamePropertyManager, SIGNAL( newVariableNameProperty( QString, QString, unsigned int ) ), this, SLOT( useNewVariableNameProperty( QString, QString, unsigned int) ) );
}

//==============================================================================
// Drag drop
void QERadioButton::setDrop( QVariant drop )
{
    setVariableName( drop.toString(), 0 );
    establishConnection( 0 );
}

QVariant QERadioButton::getDrop()
{
    return QVariant( getSubstitutedVariableName(0) );
}
