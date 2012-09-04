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
  This class is a CA aware spin box widget based on the Qt spin box widget.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details
 */

#include "QCaSpinBox.h"

/*!
    Create a CA aware spin box with no variable name yet
*/
QCaSpinBox::QCaSpinBox( QWidget *parent ) : QDoubleSpinBox( parent ), QCaWidget( this ) {
    setup();
}

/*!
    Create a CA aware spin box with a variable name already known
*/
QCaSpinBox::QCaSpinBox( const QString &variableNameIn, QWidget *parent ) : QDoubleSpinBox( parent ), QCaWidget( this ) {
    setVariableName( variableNameIn, 0 );

    setup();

}

/*!
    Common construction
*/
void QCaSpinBox::setup() {
    // Set up data
    // This control used a single data source
    setNumVariables(1);

    // Initialise the flag indicating the value is being changed programatically (not by the user)
    programaticValueChange = false;

    // Set up default properties
    writeOnChange = true;
    setAllowDrop( false );
    addUnitsAsSuffix = false;
    useDbPrecisionForDecimal = true;

    // Set the initial state
    lastValue = 0.0;
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    ignoreSingleShotRead = false;

    // Use spin box signals
    QObject::connect( this, SIGNAL( valueChanged( double ) ), this, SLOT( userValueChanged( double ) ) );
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a spin box a QCaObject that streams integers is required.
*/
qcaobject::QCaObject* QCaSpinBox::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QCaInteger
    return new QCaFloating( getSubstitutedVariableName( variableIndex ), this, &floatingFormatting, variableIndex );
}

/*!
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QCaSpinBox::establishConnection( unsigned int variableIndex ) {

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
void QCaSpinBox::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QCaSpinBox::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    /// If connected, enable the widget if the QCa enabled property is true
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

    //!!! ??? not sure if this is right. Added as the record type was comming back as GENERIC::UNKNOWN deep in the write
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
    Pass the update straight on to the SpinBox unless the user is changing it.
    Note, it would not be common to have a user editing a regularly updating
    value. However, this scenario should be allowed for. A reasonable reason
    for a user modified value to update on a gui is if is is written to by
    another user on another gui.
    This is the slot used to recieve data updates from a QCaObject based class.
    This is the slot used to recieve data updates from a QCaObject based class.
*/
void QCaSpinBox::setValueIfNoFocus( const double& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& ) {

    // Set the limits and step size
    QCaFloating* qca = (QCaFloating*)getQcaItem(0);
    double upper = qca->getControlLimitUpper();
    double lower = qca->getControlLimitLower();
    if( upper != lower)
    {
        setMaximum( qca->getControlLimitUpper() );
        setMinimum( qca->getControlLimitLower() );
    }
    setDecimalsFromPrecision( qca );
    setSuffixEgu( qca );

    // Do nothing more if doing a single shot read (done when not subscribing to get range values)
    if( ignoreSingleShotRead )
    {
        ignoreSingleShotRead = false;
        return;
    }

    /// Signal a database value change to any Link widgets
    emit dbValueChanged( value );

    // Save the last database value
    lastValue = value;

    /// Update the spin box only if the user is not interacting with the object.
    if( !hasFocus() ) {
        // Update the spin box
        programaticValueChange = true;
        setValue( value );
        programaticValueChange = false;

        // Note the last value seen by the user
        lastUserValue = text();
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
    The user has changed the spin box.
*/
void QCaSpinBox::userValueChanged( double value )
{
    // If the user is not changing the value, or not writing on change, do nothing
    if( programaticValueChange || !writeOnChange )
    {
        return;
    }

    /// Get the variable to write to
    QCaFloating* qca = (QCaFloating*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// then write the value
    if( qca ) {
        // Write the value
        qca->writeFloating( value );

        // Manage notifying user changes
        emit userChange( text(), lastUserValue, QString("%1").arg( lastValue ) );
    }
}

// Write a value immedietly.
// Used when writeOnChange is false
// (widget will never write due to the user pressing return or leaving the widget)
void QCaSpinBox::writeNow()
{
    // Get the variable to write to
    QCaFloating* qca = (QCaFloating*)getQcaItem(0);

    // If a QCa object is present (if there is a variable to write to)
    // then write the value
    if( qca )
    {
        // Write the value
        qca->writeFloating( value() );
    }
}

// Set the EGU as the suffix
void QCaSpinBox::setSuffixEgu( qcaobject::QCaObject* qca )
{
    // If using the EGU as the suffix, and the EGU is available, set the suffix to the EGU
    // otherwise clear the suffix
    if( qca && addUnitsAsSuffix )
    {
        setSuffix( QString( " " ).append( qca->getEgu() ) );
    }
    else
    {
        setSuffix( "" );
    }
}

// Set the spin box decimal places from the data precision if required
void QCaSpinBox::setDecimalsFromPrecision( qcaobject::QCaObject* qca )
{
    // If using the database precision to determine the number of decimal places, and it is available, then apply it
    if( qca && useDbPrecisionForDecimal )
    {
        setDecimals( qca->getPrecision() );
    }
}

//==============================================================================
// Drag drop
void QCaSpinBox::setDrop( QVariant drop )
{
    setVariableName( drop.toString(), 0 );
    establishConnection( 0 );
}

QVariant QCaSpinBox::getDrop()
{
    return QVariant( getSubstitutedVariableName(0) );
}

//==============================================================================
// Property convenience functions

// write on change
void QCaSpinBox::setWriteOnChange( bool writeOnChangeIn )
{
    writeOnChange = writeOnChangeIn;
}
bool QCaSpinBox::getWriteOnChange()
{
    return writeOnChange;
}

// subscribe
void QCaSpinBox::setSubscribe( bool subscribeIn )
{
    subscribe = subscribeIn;
}
bool QCaSpinBox::getSubscribe()
{
    return subscribe;
}

// Add units (as suffix).
// Note, for most widgets with an 'addUnits' property, the property is passed to a
//       QCaStringFormatting class where the units are added to the displayed string.
//       In this case, the units are added as the spin box suffix.
bool QCaSpinBox::getAddUnitsAsSuffix()
{
    return addUnitsAsSuffix;
}

void QCaSpinBox::setAddUnitsAsSuffix( bool addUnitsAsSuffixIn )
{
    addUnitsAsSuffix = addUnitsAsSuffixIn;
    qcaobject::QCaObject* qca = (QCaFloating*)getQcaItem(0);
    setSuffixEgu( qca );
}

// useDbPrecision
// Note, for most widgets with an 'useDbPrecision' property, the property is passed to a
//       QCaStringFormatting class where it is used to determine the precision when formatting numbers as a string.
//       In this case, it is used to determine the spin box number-of-decimals property.
void QCaSpinBox::setUseDbPrecisionForDecimals( bool useDbPrecisionForDecimalIn )
{
    useDbPrecisionForDecimal = useDbPrecisionForDecimalIn;
    qcaobject::QCaObject* qca = (QCaFloating*)getQcaItem(0);
    setDecimalsFromPrecision( qca );
}

bool QCaSpinBox::getUseDbPrecisionForDecimals()
{
    return useDbPrecisionForDecimal;
}


