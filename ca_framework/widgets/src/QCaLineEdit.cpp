/*! 
  \class QCaLineEdit
  \version $Revision: #11 $
  \date $DateTime: 2010/03/23 11:07:38 $
  \author andrew.rhyder
  \brief CA Line Edit Widget.
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

#include <QCaLineEdit.h>
#include <QMessageBox>

/*!
    Constructor with no initialisation
*/
QCaLineEdit::QCaLineEdit( QWidget *parent ) : QLineEdit( parent ), QCaWidget() {
    setup();
}

/*!
    Constructor with known variable
*/
QCaLineEdit::QCaLineEdit( const QString& variableNameIn, QWidget *parent ) : QLineEdit( parent ), QCaWidget() {
    setVariableName( variableNameIn, 0 );

    setup();
}

/*!
    Setup common to all constructors
*/
void QCaLineEdit::setup() {
    // Set up data
    // This control used a single data source
    setNumVariables(1);

    // Set up default properties
    writeOnLoseFocusProperty = false;
    writeOnEnterProperty = true;
    enabledProperty = true;
    confirmWriteProperty = false;

    // Set the initial state
    setText( "" );
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    // Use line edit signals
    QObject::connect( this, SIGNAL( returnPressed() ), this, SLOT( userReturnPressed() ) );
    QObject::connect( this, SIGNAL( editingFinished() ), this, SLOT( userEditingFinished() ) );
    QObject::connect( this, SIGNAL( textEdited( const QString & ) ), this, SLOT( userTextEdited( const QString & ) ) );
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a line edit a QCaObject that streams strings is required.
*/
void QCaLineEdit::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QCaString
    setQcaItem( new QCaString( getSubstitutedVariableName( variableIndex ), this, &stringFormatting, variableIndex ), variableIndex );
}

/*!
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.

*/
void QCaLineEdit::establishConnection( unsigned int variableIndex ) {
    if( createConnection( variableIndex ) == true ) {
        QObject::connect( getQcaItem( variableIndex ),
                          SIGNAL( stringChanged( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setTextIfNoFocus( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( getQcaItem( variableIndex ), SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}

/*!
    Update the tool tip as requested by QCaWidget.
*/
void QCaLineEdit::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
 */
void QCaLineEdit::connectionChanged( QCaConnectionInfo& connectionInfo )
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
    Pass the text update straight on to the QLineEdit unless the user is
    editing the text.
    Note, it would not be common to have a user editing a regularly updating
    value. However, this scenario should be allowed for. A reasonable reason
    for a user updated value to update on a gui is if is is written to by
    another user on another gui.
*/
void QCaLineEdit::setTextIfNoFocus( const QString& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& ) {

    /// If not subscribing, then do nothing.
    /// Note, This will still be called even if not subscribing as there is an initial sing shot read
    /// to ensure we have valid information about the variable when it is time to do a write.
    if( !subscribeProperty )
        return;

    /// Save the most recent value.
    /// If the user is editing the value updates are not applied. If the user cancels the write, the value the widget
    /// should revert to the latest value.
    lastValue = value;

    /// Signal a database value change to any Link widgets
    emit dbValueChanged( value );

    /// Update the text if appropriate
    /// If the user is editing the object then updates will be inapropriate
    if( hasFocus() == false )
        setText( value );

    /// If in alarm, display as an alarm
    if( alarmInfo.getSeverity() != lastSeverity )
    {
            updateToolTipAlarm( alarmInfo.severityName() );
            setStyleSheet( alarmInfo.style() );
            lastSeverity = alarmInfo.getSeverity();
    }
}

/*!
    The user has pressed return.
    Note, it doesn't matter if the user presses return and both this function
    AND userReturnPressed() is called since setText is called in each to clear
    the 'isModified' flag. So, the first called will perform the write, the
    second (if any) will do nothing.
*/
void QCaLineEdit::userReturnPressed() {

    /// Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// and the object is set up to write when the user presses return
    /// then write the value.
    /// Note, write even if the value has not changed (isModified() is not checked)

    if( qca && writeOnEnterProperty )
    {
        writeValue( qca, text() );
    }
}

/*!
    The user has 'finished editing' such as pressed return/enter or moved
    focus from the object.
    Note, it doesn't matter if the user presses return and both this function
    AND userReturnPressed() is called since setText is called in each to clear
    the 'isModified' flag. So, the first called will perform the write, the
    second (if any) will do nothing.
*/
void QCaLineEdit::userEditingFinished() {

    /// If no changes were made by the user, do nothing
    if( !isModified() )
        return;

    /// Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// and the object is set up to write when the user changes focus away from the object
    /// and the text has actually changed
    /// then write the value
    if( qca && writeOnLoseFocusProperty )
    {
        writeValue( qca, text() );
    }

    /// If, for what ever reason, the value has been changed by the user but not but not written
    /// check with the user what to do about it.
    else
    {
        int confirm = QMessageBox::warning( this, "Value changed", "You altered a value but didn't write it.\nDo you want to write this value?",
                                            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::No );

        switch( confirm )
        {
            /// Write the value
            case QMessageBox::Yes:
                if( qca )
                    writeValue( qca, text() );
                break;

            /// Abort the write, revert to latest value
            case QMessageBox::No:
                setText( lastValue );       /// Note, also clears 'isModified' flag
                break;

            /// Don't write the value, move back to the field being edited
            case QMessageBox::Cancel:
                setFocus();
                break;
        }
    }
}

/// Write a value in response to user editing the widget
/// Request confirmation if required
void QCaLineEdit::writeValue( QCaString *qca, QString newValue )
{
    /// If required, get confirmation from the user as to what to do
    int confirm = QMessageBox::Yes;
    if( confirmWriteProperty )
    {
        confirm = QMessageBox::warning( this, "Confirm write", "Do you want to write this value?",
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes );
    }

    /// Perform the required action. Either write the value (the default) or what ever the user requested
    switch( confirm )
    {
        /// Write the value
        case QMessageBox::Yes:
            qca->writeString( newValue );
            setText( text() );          /// Clear 'isModified' flag
            break;

        /// Abort the write, revert to latest value
        case QMessageBox::No:
            setText( lastValue );       /// Note, also clears 'isModified' flag
            break;

        /// Don't write the value, keep editing the field
        case QMessageBox::Cancel:
            /// Do nothing
            break;
    }
}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaLineEdit::isEnabled() const
{
    /// Return what the state of widget would be if connected.
    return enabledProperty;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaLineEdit::setEnabled( bool state )
{
    /// Note the new 'enabled' state
    enabledProperty = state;

    /// Set the enabled state of the widget only if connected
    if( isConnected )
        QWidget::setEnabled( enabledProperty );
}

/*!
   Slot similar to default widget setEnabled, but will use our own setEnabled which will allow alarm states to override current enabled state
 */
void QCaLineEdit::requestEnabled( const bool& state )
{
    setEnabled(state);
}
