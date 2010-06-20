/*! 
  \class QCaSlider
  \version $Revision: #9 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author andrew.rhyder
  \brief CA Slider Widget.
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

#include <QCaSlider.h>

/*!
    Constructor with no initialisation
*/
QCaSlider::QCaSlider( QWidget *parent ) : QSlider( parent ), QCaWidget() {
    setup();
}

/*!
    Constructor with known variable
*/
QCaSlider::QCaSlider( const QString &variableNameIn, QWidget *parent ) : QSlider( parent ), QCaWidget() {
    setVariableName( variableNameIn, 0 );

    setup();

}

/*!
    Setup common to all constructors
*/
void QCaSlider::setup() {
    // Set up data
    // This control used a single data source
    setNumVariables(1);

    // Set up default properties
    updateInProgress = false;
    writeOnChangeProperty = true;
    enabledProperty = true;

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    // Use slider signals
    QObject::connect( this, SIGNAL( valueChanged( const int &) ), this, SLOT( userValueChanged( const int & ) ) );
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a slider a QCaObject that streams integers is required.
*/
void QCaSlider::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QCaInteger
    setQcaItem( new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex ), variableIndex );
}

/*!
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
*/
void QCaSlider::establishConnection( unsigned int variableIndex ) {
    if( createConnection( variableIndex ) == true ) {
        setValue( 0 );
        QObject::connect( getQcaItem( variableIndex ),
                          SIGNAL( integerChanged( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setValueIfNoFocus( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( getQcaItem( variableIndex ), SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}

/*!
    Update the tool tip as requested by QCaWidget.
*/
void QCaSlider::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
 */
void QCaSlider::connectionChanged( QCaConnectionInfo& connectionInfo )
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

    /// ??? not sure if this is right. Added as the record type was comming back as GENERIC::UNKNOWN deep in the write
    /// Start a single shot read if the channel is up (ignore channel down),
    /// This will allow initialisation of the widget using info from the database.
    /// If subscribing, then an update will occur without having to initiated one here.
    /// Note, channel up implies link up
    /// Note, even though there is nothing to do to initialise the spin box if not subscribing, an
    /// initial sing shot read is still performed to ensure we have valid information about the
    /// variable when it is time to do a write.
    if( connectionInfo.isChannelConnected() && !subscribeProperty )
    {
        QCaInteger* qca = (QCaInteger*)getQcaItem(0);
        qca->singleShotRead();
    }
}

/*!
    Pass the update straight on to the QSlider unless the user is moving the slider.
    Note, it would not be common to have a user editing a regularly updating value. However, this
    scenario should be allowed for. A reasonable reason for a user modified value to update on a gui is
    if is is written to by another user on another gui.
*/
void QCaSlider::setValueIfNoFocus( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& ) {
    /// If not subscribing, then do nothing.
    /// Note, even though there is nothing to do here if not subscribing, an initial sing shot read is still
    /// performed to ensure we have valid information about the variable when it is time to do a write.
    if( !subscribeProperty )
        return;

    /// Signal a database value change to any Link widgets
    emit dbValueChanged( value );

    /// Update the slider only if the user is not interacting with the object.
    if( !hasFocus() ) {
        updateInProgress = true;
        setValue( value );
        updateInProgress = false;
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
    The user has modified the slider position.
    This will occur as the user slides the slider if tracking is enabled,
    or when the user completes sliding if tracking is not enabled.
*/
void QCaSlider::userValueChanged( const int &value) {
    // If the change is due to an update (and not the user) then ignore the change
    if( updateInProgress == true ) {
        return;
    }

    /// Get the variable to write to
    QCaInteger* qca = (QCaInteger*)getQcaItem(0);

    /** If a QCa object is present (if there is a variable to write to)
     * and the object is set up to write when the user completes moving the slider
     * then write the value
     */
    if( qca && writeOnChangeProperty ) {
        /// Attempt to write the data if the destination data type is known.
        /// It is not known until a connection is established.
        if( qca->dataTypeKnown() ) {
            qca->writeInteger( value );
        } else {
            /// Inform the user that the write could not be performed.
            /// It is normally not possible to get here. If the connection or link has not
            /// yet been established (and therefore the data type is unknown) then the user
            /// interface object should be unaccessable. This code is here in the event that
            /// the user can, by design or omision, still attempt a write.
            userMessage.sendWarningMessage( "Could not write value as type is not known yet. From QCaSlider::userValueChanged()"  );
        }
    }
}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaSlider::isEnabled() const
{
    /// Return what the state of widget would be if connected.
    return enabledProperty;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaSlider::setEnabled( bool state )
{
    /// Note the new 'enabled' state
    enabledProperty = state;

    /// Set the enabled state of the widget only if connected
    if( isConnected )
        QWidget::setEnabled( enabledProperty );
}

/*!
   Slot similar to default widget setEnabled slot, but will use our own setEnabled which will allow alarm states to override current enabled state
 */
void QCaSlider::requestEnabled( const bool& state )
{
    setEnabled(state);
}
