/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/widgets/src/QCaLineEdit.cpp $
 * $Revision: #7 $
 * $DateTime: 2009/07/31 15:55:17 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaLineEdit
  \version $Revision: #7 $
  \date $DateTime: 2009/07/31 15:55:17 $
  \author andrew.rhyder
  \brief CA Line Edit Widget.
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
 * andrew.rhyder@synchrotron.org.au
 * 800 Blackburn Road, Clayton, Victoria 3168, Australia.
 *
 */

#include <QCaLineEdit.h>

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
    writeOnChangeProperty = false;
    enabledProperty = true;

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
    /// and the text has actually changed
    /// then write the value

    if( qca && writeOnEnterProperty && isModified() ) {
        qca->writeString( text() );
        setText( text() ); /// clear 'isModified' flag
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
    /// Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// and the object is set up to write when the user changes focus away from the object
    /// and the text has actually changed
    /// then write the value
    if( qca && writeOnLoseFocusProperty && isModified() ) {
        qca->writeString( text() );
        setText( text() ); /// clear 'isModified' flag
    }

}

/*!
    The user has changed the text. For example, typed a character or deleted a character.
*/
void QCaLineEdit::userTextEdited ( const QString &text ) {
    /// Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// and the object is set up to write when the user changes the text (each key stroke, paste, etc)
    /// and the text has actually changed
    /// then write the value
    if( qca && writeOnChangeProperty && isModified() ) {
        qca->writeString( text );
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
   Override the default widget setEnabled slot to allow alarm states to override current enabled state
 */
void QCaLineEdit::setEnabled( bool state )
{
    /// Note the new 'enabled' state
    enabledProperty = state;

    /// Set the enabled state of the widget only if connected
    if( isConnected )
        QWidget::setEnabled( enabledProperty );
}
