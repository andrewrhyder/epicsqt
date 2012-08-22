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
  This class is a CA aware line edit widget based on the Qt line edit widget.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details
 */

#include <QCaLineEdit.h>
#include <QMessageBox>

/*!
    Constructor with no initialisation
*/
QCaLineEdit::QCaLineEdit( QWidget *parent ) : QLineEdit( parent ), QCaWidget( this ) {
    setup();
}

/*!
    Constructor with known variable
*/
QCaLineEdit::QCaLineEdit( const QString& variableNameIn, QWidget *parent ) : QLineEdit( parent ), QCaWidget( this ) {
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
    writeOnLoseFocus = false;
    writeOnEnter = true;
    writeOnFinish = true;
    localEnabled = true;
    confirmWrite = false;
    setAllowDrop( false );

    // Set the initial state
    setText( "" );
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state
    writeConfirmDialogPresent = false;

    // Use line edit signals
    QObject::connect( this, SIGNAL( returnPressed() ), this, SLOT( userReturnPressed() ) );
    QObject::connect( this, SIGNAL( editingFinished() ), this, SLOT( userEditingFinished() ) );
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a line edit a QCaObject that streams strings is required.
*/
qcaobject::QCaObject* QCaLineEdit::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QCaString
    return new QCaString( getSubstitutedVariableName( variableIndex ), this, &stringFormatting, variableIndex );
}

/*!
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QCaLineEdit::establishConnection( unsigned int variableIndex ) {

    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots
    if(  qca ) {
        QObject::connect( qca,  SIGNAL( stringChanged( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setTextIfNoFocus( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
        QObject::connect( this, SIGNAL( requestResend() ),
                          qca, SLOT( resendLastData() ) );
    }
}

/*!
    Update the tool tip as requested by QCaToolTip.
*/
void QCaLineEdit::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QCaLineEdit::connectionChanged( QCaConnectionInfo& connectionInfo )
{

    /// If connected enabled the widget if required.
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );

        if( localEnabled )
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
    This is the slot used to recieve data updates from a QCaObject based class.
*/
void QCaLineEdit::setTextIfNoFocus( const QString& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& ) {

    // Save the most recent value.
    // If the user is editing the value updates are not applied. If the user cancels the write, the value the widget
    // should revert to the latest value.
    // This last value is also used to manage notifying user changes (save what the user will be changing from)
    lastValue = value;

    /// Signal a database value change to any Link widgets
    emit dbValueChanged( value );

    /// Update the text if appropriate
    /// If the user is editing the object then updates will be inapropriate
    if( hasFocus() == false && !writeConfirmDialogPresent )
    {
        setText( value );
        lastUserValue = value;
    }

    /// If in alarm, display as an alarm
    if( alarmInfo.getSeverity() != lastSeverity )
    {
            updateToolTipAlarm( alarmInfo.severityName() );
            setStyleSheet( alarmInfo.style() );
            lastSeverity = alarmInfo.getSeverity();
    }
}

/*!
    The user has pressed return/enter. (Not write when user enters the widget)
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

    if( qca && writeOnEnter )
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
    if( !isModified() || !writeOnFinish )
        return;

    /// Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// and the object is set up to write when the user changes focus away from the object
    /// and the text has actually changed
    /// then write the value
    if( qca && writeOnLoseFocus )
    {
        writeValue( qca, text() );
    }

    /// If, for what ever reason, the value has been changed by the user but not but not written
    /// check with the user what to do about it.
    else
    {
        writeConfirmDialogPresent = true;
        int confirm = QMessageBox::warning( this, "Value changed", "You altered a value but didn't write it.\nDo you want to write this value?",
                                            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::No );
        writeConfirmDialogPresent = false;

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

// Write a value immedietly.
// Used when writeOnLoseFocus, writeOnEnter, writeOnFinish are all false
// (widget will never write due to the user pressing return or leaving the widget)
void QCaLineEdit::writeNow()
{
    // Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    // If a QCa object is present (if there is a variable to write to)
    // then write the value.
    if( qca )
    {
        writeValue( qca, text() );
    }
}

/// Write a value in response to user editing the widget
/// Request confirmation if required
void QCaLineEdit::writeValue( QCaString *qca, QString newValue )
{
    /// If required, get confirmation from the user as to what to do
    int confirm = QMessageBox::Yes;
    if( confirmWrite )
    {
        confirm = QMessageBox::warning( this, "Confirm write", "Do you want to write this value?",
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes );
    }

    /// Perform the required action. Either write the value (the default) or what ever the user requested
    switch( confirm )
    {
        /// Write the value and inform any derived class
        case QMessageBox::Yes:
            // Write the value
            qca->writeString( newValue );

            // Manage notifying user changes
            emit userChange( newValue, lastUserValue, lastValue );

            // Clear 'isModified' flag
            setText( text() );
            break;

        /// Abort the write, revert to latest value
        case QMessageBox::No:
            // Revert the text. Note, also clears 'isModified' flag
            setText( lastValue );
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
    return localEnabled;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaLineEdit::setEnabled( bool state )
{
    /// Note the new 'enabled' state
    localEnabled = state;

    /// Set the enabled state of the widget only if connected
    if( isConnected )
        QWidget::setEnabled( localEnabled );
}

/*!
   Slot similar to default widget setEnabled, but will use our own setEnabled which will allow alarm states to override current enabled state
 */
void QCaLineEdit::requestEnabled( const bool& state )
{
    setEnabled(state);
}

//==============================================================================
// Drag drop
void QCaLineEdit::setDrop( QVariant drop )
{
    setVariableName( drop.toString(), 0 );
    establishConnection( 0 );
}

QVariant QCaLineEdit::getDrop()
{
    return QVariant( getSubstitutedVariableName(0) );
}

//==============================================================================
// Property convenience functions

// write on lose focus
void QCaLineEdit::setWriteOnLoseFocus( bool writeOnLoseFocusIn )
{
    writeOnLoseFocus = writeOnLoseFocusIn;
}
bool QCaLineEdit::getWriteOnLoseFocus()
{
    return writeOnLoseFocus;
}

// write on enter
void QCaLineEdit::setWriteOnEnter( bool writeOnEnterIn )
{
    writeOnEnter = writeOnEnterIn;
}
bool QCaLineEdit::getWriteOnEnter()
{
    return writeOnEnter;
}

// write on finish
void QCaLineEdit::setWriteOnFinish( bool writeOnFinishIn )
{
    writeOnFinish = writeOnFinishIn;
}
bool QCaLineEdit::getWriteOnFinish()
{
    return writeOnFinish;
}

// subscribe
void QCaLineEdit::setSubscribe( bool subscribeIn )
{
    subscribe = subscribeIn;
}
bool QCaLineEdit::getSubscribe()
{
    return subscribe;
}

// confirm write
void QCaLineEdit::setConfirmWrite( bool confirmWriteIn )
{
    confirmWrite = confirmWriteIn;
}
bool QCaLineEdit::getConfirmWrite()
{
    return confirmWrite;
}
