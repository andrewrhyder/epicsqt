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
  This class is a CA aware slider widget based on the Qt slider widget.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details
 */

#include <QCaSlider.h>

/*!
    Constructor with no initialisation
*/
QCaSlider::QCaSlider( QWidget *parent ) : QSlider( parent ), QCaWidget( this ) {
    setup();
}

/*!
    Constructor with known variable
*/
QCaSlider::QCaSlider( const QString &variableNameIn, QWidget *parent ) : QSlider( parent ), QCaWidget( this ) {

    setup();
    setVariableName( variableNameIn, 0 );

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
    writeOnChange = true;
    setAllowDrop( false );

    scale = 1.0;
    offset = 0.0;

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    ignoreSingleShotRead = false;

    // Use slider signals
    QObject::connect( this, SIGNAL( valueChanged( const int &) ), this, SLOT( userValueChanged( const int & ) ) );
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a slider a QCaObject that streams integers is required.
*/
qcaobject::QCaObject* QCaSlider::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QCaFloating
    return new QCaFloating( getSubstitutedVariableName( variableIndex ), this, &floatingFormatting, variableIndex );
}

/*!
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QCaSlider::establishConnection( unsigned int variableIndex ) {

    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots
    if(  qca ) {
        setValue( 0 );
        QObject::connect( qca,  SIGNAL( floatingChanged( const double&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setValueIfNoFocus( const double&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}

/*!
    Update the tool tip as requested by QCaToolTip.
*/
void QCaSlider::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QCaSlider::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    /// If connected enabled the widget if required.
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );

        setDataDisabled( false );
    }

    /// If disconnected always disable the widget.
    else
    {
        isConnected = false;
        updateToolTipConnection( isConnected );

        setDataDisabled( true );
    }

    // !!! ??? not sure if this is right. Added as the record type was comming back as GENERIC::UNKNOWN deep in the write
    /// Start a single shot read if the channel is up (ignore channel down),
    /// This will allow initialisation of the widget using info from the database.
    /// If subscribing, then an update will occur without having to initiated one here.
    /// Note, channel up implies link up
    /// Note, even though there is nothing to do to initialise the spin box if not subscribing, an
    /// initial sing shot read is still performed to ensure we have valid information about the
    /// variable when it is time to do a write.
    if( connectionInfo.isChannelConnected() && !subscribe )
    {
        QCaFloating* qca = (QCaFloating*)getQcaItem(0);
        qca->singleShotRead();
        ignoreSingleShotRead = true;
    }
}

/*!
    Pass the update straight on to the QSlider unless the user is moving the slider.
    Note, it would not be common to have a user editing a regularly updating value. However, this
    scenario should be allowed for. A reasonable reason for a user modified value to update on a gui is
    if is is written to by another user on another gui.
    This is the slot used to recieve data updates from a QCaObject based class.
*/
void QCaSlider::setValueIfNoFocus( const double& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& ) {

    // Do nothing if doing a single shot read (done when not subscribing to get enumeration values)
    if( ignoreSingleShotRead )
    {
        ignoreSingleShotRead = false;
        return;
    }

    /// Signal a database value change to any Link widgets
    emit dbValueChanged( qlonglong( value ) );

    /// Update the slider only if the user is not interacting with the object.
    if( !hasFocus() ) {
        updateInProgress = true;
        int intValue = int( (value - offset) * scale );
        setValue( intValue );
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

    // If the change is due to an update (and not the user)
    // or not writing on change, then ignore the change
    if( updateInProgress == true || !writeOnChange )
    {
        return;
    }

    /// Get the variable to write to
    QCaFloating* qca = (QCaFloating*)getQcaItem(0);

    /** If a QCa object is present (if there is a variable to write to)
     * then write the value
     */
    if( qca )
    {
        /// Attempt to write the data if the destination data type is known.
        /// It is not known until a connection is established.
        if( qca->dataTypeKnown() )
        {
            qca->writeFloating( (value/scale)+offset );
        }
        else
        {
            /// Inform the user that the write could not be performed.
            /// It is normally not possible to get here. If the connection or link has not
            /// yet been established (and therefore the data type is unknown) then the user
            /// interface object should be unaccessable. This code is here in the event that
            /// the user can, by design or omision, still attempt a write.
            sendMessage( "Could not write value as type is not known yet.", "QCaSlider::userValueChanged()", MESSAGE_TYPE_WARNING );
        }
    }
}

// Write a value immedietly.
// Used when writeOnChange is false
// (widget will never write due to the user pressing return or leaving the widget)
void QCaSlider::writeNow()
{
    // Get the variable to write to
    QCaFloating* qca = (QCaFloating*)getQcaItem(0);

    // If a QCa object is present (if there is a variable to write to)
    // then write the value
    if( qca )
    {
        // Attempt to write the data if the destination data type is known.
        // It is not known until a connection is established.
        if( qca->dataTypeKnown() )
        {
            qca->writeFloating( (value()/scale)+offset );
        }
    }
}

//==============================================================================
// Drag drop
void QCaSlider::setDrop( QVariant drop )
{
    setVariableName( drop.toString(), 0 );
    establishConnection( 0 );
}

QVariant QCaSlider::getDrop()
{
    return QVariant( getSubstitutedVariableName(0) );
}

//==============================================================================
// Property convenience functions

// write on change
void QCaSlider::setWriteOnChange( bool writeOnChangeIn )
{
    writeOnChange = writeOnChangeIn;
}
bool QCaSlider::getWriteOnChange()
{
    return writeOnChange;
}

// subscribe
void QCaSlider::setSubscribe( bool subscribeIn )
{
    subscribe = subscribeIn;
}
bool QCaSlider::getSubscribe()
{
    return subscribe;
}

// Set scale and offset (used to scale data when inteter scale bar min and max are not suitable)
void QCaSlider::setScale( double scaleIn )
{
    scale = scaleIn;
}

double QCaSlider::getScale()
{
    return scale;
}

void QCaSlider::setOffset( double offsetIn )
{
    offset = offsetIn;
}

double QCaSlider::getOffset()
{
    return offset;
}
