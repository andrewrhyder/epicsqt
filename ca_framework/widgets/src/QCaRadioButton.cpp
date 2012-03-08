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
  This class is a CA aware push button widget based on the Qt push button widget.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details
 */

#include <QCaRadioButton.h>
#include <QProcess>
#include <QMessageBox>
#include <QMainWindow>
#include <QIcon>

/*!
    Constructor with no initialisation
*/
QCaRadioButton::QCaRadioButton( QWidget *parent ) : QRadioButton( parent ), QCaGenericButton( this ) {
    QCaGenericButton::setup();
    setup();
}

/*!
    Constructor with known variable
*/
QCaRadioButton::QCaRadioButton( const QString &variableNameIn, QWidget *parent ) : QRadioButton( parent ), QCaGenericButton( this ) {
    setVariableName( variableNameIn, 0 );

    QCaGenericButton::setup();
    setup();

}

/*!
    Setup common to all constructors
*/
void QCaRadioButton::setup() {
    setText( "QCaRadioButton" );
}

/*!
    Update the tool tip as requested by QCaToolTip.
*/
void QCaRadioButton::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaRadioButton::isEnabled() const
{
    // Return what the state of widget would be if connected.
    return localEnabled;
}

/*!
   Slot similar to default widget setEnabled slot, but will use our own setEnabled which will allow alarm states to override current enabled state
 */
void QCaRadioButton::requestEnabled( const bool& state )
{
    setGenericEnabled( state );
}

//==============================================================================
// Drag drop
void QCaRadioButton::setDropText( QString text )
{
    setVariableName( text, 0 );
    establishConnection( 0 );
}

QString QCaRadioButton::getDropText()
{
    return getSubstitutedVariableName(0);
}
