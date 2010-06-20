/*! 
  \class QCaPushButton
  \version $Revision: #9 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author andrew.rhyder
  \brief CA Push Button Widget.
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
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org
 */

#include <QCaPushButton.h>

/*!
    Constructor with no initialisation
*/
QCaPushButton::QCaPushButton( QWidget *parent ) : QPushButton( parent ), QCaWidget() {
    setup();
}

/*!
    Constructor with known variable
*/
QCaPushButton::QCaPushButton( const QString &variableNameIn, QWidget *parent ) : QPushButton( parent ), QCaWidget() {
    setVariableName( variableNameIn, 0 );

    setup();

}

/*!
    Setup common to all constructors
*/
void QCaPushButton::setup() {
    // Set up data
    // This control used a single data source
    setNumVariables(1);

    // Set up default properties
    writeOnPress = false;
    writeOnRelease = false;
    writeOnClick = true;
    enabledProperty = true;

    pressText = "1";
    releaseText = "0";
    clickText = "1";

    // Override default QCaWidget properties
    subscribeProperty = false;

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    // Use push button signals
    QObject::connect( this, SIGNAL( pressed() ), this, SLOT( userPressed() ) );
    QObject::connect( this, SIGNAL( released() ), this, SLOT( userReleased() ) );
    QObject::connect( this, SIGNAL( clicked() ), this, SLOT( userClicked() ) );
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a push button a QCaObject that streams strings is required.
*/
void QCaPushButton::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QCaString
    setQcaItem( new QCaString( getSubstitutedVariableName( variableIndex ), this, &stringFormatting, variableIndex ), variableIndex );
}

/*!
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
*/
void QCaPushButton::establishConnection( unsigned int variableIndex ) {
    if( createConnection( variableIndex ) == true ) {

        // Get updates if subscribing
        if( subscribeProperty )
        {
            setText( "" );
            QObject::connect(getQcaItem( variableIndex ),
                             SIGNAL( stringChanged( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                             this, SLOT( setButtonText( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        }

        // Get conection status changes always (subscribing or not)
        QObject::connect( getQcaItem( variableIndex ), SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}

/*!
    Update the tool tip as requested by QCaWidget.
*/
void QCaPushButton::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
 */
void QCaPushButton::connectionChanged( QCaConnectionInfo& connectionInfo )
{

    /// If connected enabled the widget if required.
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );

        if( enabledProperty )
            QWidget::setEnabled( true );
    }

    /// If disconnected always disable the widget.
    else
    {
        isConnected = false;
        updateToolTipConnection( isConnected );

        QWidget::setEnabled( false );
    }
}

/*!
  Implement a slot to set the current text of the push button
*/
void QCaPushButton::setButtonText( const QString& text, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& )
{
    /// If not subscribing, then do nothing.
    /// Note, This will still be called even if not subscribing as there may be an initial sing shot read
    /// to ensure we have valid information about the variable when it is time to do a write.
    if( !subscribeProperty )
        return;

    /// Signal a database value change to any Link widgets
    emit dbValueChanged( text );

    /// Update the text
    setText( text );

    /// If in alarm, display as an alarm
    if( alarmInfo.getSeverity() != lastSeverity )
    {
            updateToolTipAlarm( alarmInfo.severityName() );
            setStyleSheet( alarmInfo.style() );
            lastSeverity = alarmInfo.getSeverity();
    }
}

/*!
    Button press event.
*/
void QCaPushButton::userPressed() {
    /// Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// and the object is set up to write when the user presses the button
    /// then write the value
    if( qca && writeOnPress ) {
        qca->writeString( pressText );
    }
}

/*!
    Button release event.
*/

void QCaPushButton::userReleased() {
    /// Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// and the object is set up to write when the user releases the button
    /// then write the value
    if( qca && writeOnRelease ) {
        qca->writeString( releaseText );
    }
}

/*!
    Button click event.
*/
void QCaPushButton::userClicked() {
    /// Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// and the object is set up to write when the user clicks the button
    /// then write the value
    if( qca && writeOnClick ) {
        qca->writeString( clickText );
    }
}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaPushButton::isEnabled() const
{
    // Return what the state of widget would be if connected.
    return enabledProperty;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaPushButton::setEnabled( const bool& state )
{
    // Note the new 'enabled' state
    enabledProperty = state;

    // Set the enabled state of the widget only if connected
    if( isConnected )
        QWidget::setEnabled( enabledProperty );
}

/*!
   Slot similar to default widget setEnabled slot, but will use our own setEnabled which will allow alarm states to override current enabled state
 */
void QCaPushButton::requestEnabled( const bool& state )
{
    QCaPushButton::setEnabled(state);
}
