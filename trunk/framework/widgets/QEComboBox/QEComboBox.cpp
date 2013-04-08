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

/*
  This class is a CA aware combo box widget based on the Qt combo box widget.
  It is tighly integrated with the base class QEWidget. Refer to QEWidget.cpp for details
 */

#include <QEComboBox.h>

/*
    Construct a combo box with no variable specified yet
*/
QEComboBox::QEComboBox( QWidget *parent ) : QComboBox( parent ), QEWidget( this ) {
    setup();
}

/*
    Construct a combo box with a variable specified
*/
QEComboBox::QEComboBox( const QString &variableNameIn, QWidget *parent ) : QComboBox( parent ), QEWidget( this ) {
    setVariableName( variableNameIn, 0 );

    setup();

}

/*
    Common construction
*/
void QEComboBox::setup() {
    // Set up data
    // This control used a single data source
    setNumVariables(1);

    // Set up default properties
    useDbEnumerations = true;
    writeOnChange = true;
    subscribe = false;
    setAllowDrop( false );

    // Set the initial state
    lastValue = 0;
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    ignoreSingleShotRead = false;

    // Use line edit signals
    // Set up to write data when the user changes the value
    QObject::connect( this, SIGNAL( activated ( int ) ), this, SLOT( userValueChanged( int ) ) );

    // Set up a connection to recieve variable name property changes
    // The variable name property manager class only delivers an updated variable name after the user has stopped typing
    QObject::connect( &variableNamePropertyManager, SIGNAL( newVariableNameProperty( QString, QString, unsigned int ) ), this, SLOT( useNewVariableNameProperty( QString, QString, unsigned int) ) );
}

/*
    Implementation of QEWidget's virtual funtion to create the specific type of QCaObject required.
    For a Combo box a QCaObject that streams integers is required.
*/
qcaobject::QCaObject* QEComboBox::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QEInteger
    return new QEInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );
}

/*
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QEComboBox::establishConnection( unsigned int variableIndex ) {

    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots
    if(  qca ) {
        setCurrentIndex( 0 );
        QObject::connect( qca,  SIGNAL( integerChanged( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setValueIfNoFocus( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}

/*
   Act on a connection change.
   Change how the label looks and change the tool tip.
   This is the slot used to recieve connection updates from a QCaObject based class.

   Perform initialisation that can only be completed once data from the variable has been read.
   Specifically, set up the combo box entries to match the enumerated types if required.
   This function is called when the channel is first established to the data. It will also be called if the channel fails
   and recovers. Subsequent calls will do nothing as the combo box is already populated.
*/
void QEComboBox::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    // If connected, enable the widget if the QE enabled property is true
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );

        setDataDisabled( false );
    }

    // If disconnected always disable the widget.
    else
    {
        isConnected = false;
        updateToolTipConnection( isConnected );

        setDataDisabled( true );
    }

    // Start a single shot read if the channel is up (ignore channel down),
    // This will allow initialisation of the widget using info from the database.
    // If the combo box is already populated, then it has been set up at design time, or this is a subsequent 'channel up'
    // If subscribing, then an update will occur without having to initiated one here.
    // Note, channel up implies link up
    if( connectionInfo.isChannelConnected() && count() == 0 && !subscribe )
    {
        QEInteger* qca = (QEInteger*)getQcaItem(0);
        qca->singleShotRead();
        ignoreSingleShotRead = true;
    }
}

/*
    Pass the update straight on to the ComboBox unless the user is changing it.
    Note, it would not be common to have a user editing a regularly updating
    value. However, this scenario should be allowed for. A reasonable reason
    for a user modified value to update on a gui is if is is written to by
    another user on another gui.

    Note, this will still be called once if not subscribing to set up enumeration values.
    See  QEComboBox::dynamicSetup() for details.
*/

void QEComboBox::setValueIfNoFocus( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& ) {

    // If the combo box is not populated, setup the enumerations if any.
    // If not subscribing, there will still be an initial update to get enumeration values.
    if( count() == 0 )
    {
        QEInteger* qca = (QEInteger*)getQcaItem(0);
        QStringList enumerations = qca->getEnumerations();
        if( useDbEnumerations && enumerations.size() )
        {
            insertItems( 0,enumerations );
        }
    }

    // Do nothing more if doing a single shot read (done when not subscribing to get enumeration values)
    if( ignoreSingleShotRead )
    {
        ignoreSingleShotRead = false;
        return;
    }

    // Signal a database value change to any Link widgets
    emit dbValueChanged( value );

    // Save the last database value
    lastValue = value;

    // Update the text if appropriate
    // If the user is editing the object then updates will be inapropriate
    if( hasFocus() == false )
    {
        // Update the combo box
        setCurrentIndex( value );

        // Note the last value presented to the user
        lastUserValue = currentText();
    }

    // Choose the alarm state to display.
    // If not displaying the alarm state, use a default 'no alarm' structure. This is
    // required so the any display of an alarm state is reverted if the displayAlarmState
    // property changes while displaying an alarm.
    QCaAlarmInfo ai;
    if( getDisplayAlarmState() )
    {
        ai = alarmInfo;
    }

    // If in alarm, display as an alarm
    if( ai.getSeverity() != lastSeverity )
    {
            updateToolTipAlarm( ai.severityName() );
            setStyleSheet( ai.style() );
            lastSeverity = ai.getSeverity();
    }
}

/*
    The user has changed the Combo box.
*/
void QEComboBox::userValueChanged( int value ) {

    // Do nothing unless writing on change
    if( !writeOnChange )
        return;

    // Get the variable to write to
    QEInteger* qca = (QEInteger*)getQcaItem(0);

    // If a QCa object is present (if there is a variable to write to)
    // then write the value
    if( qca )
    {
        // Write the value
        qca->writeInteger( (long)value );

        // Notify user changes
        QEInteger* qca = (QEInteger*)getQcaItem(0);
        QStringList enumerations = qca->getEnumerations();
        QString lastValueString;
        if( lastValue >= 0 && lastValue < enumerations.size() )
        {
            lastValueString = enumerations[lastValue];
        }
        emit userChange( currentText(), lastUserValue, lastValueString );

        // Note the last value presented to the user
        lastUserValue = currentText();
    }
}

// Write a value immedietly.
// Used when writeOnChange are false
// (widget will never write due to the user pressing return or leaving the widget)
void QEComboBox::writeNow()
{
    // Get the variable to write to
    QEInteger* qca = (QEInteger*)getQcaItem(0);

    // If a QCa object is present (if there is a variable to write to)
    // then write the value
    if( qca )
    {
        // Write the value
        qca->writeInteger( currentIndex() );
    }
}

//==============================================================================
// Drag drop
void QEComboBox::setDrop( QVariant drop )
{
    setVariableName( drop.toString(), 0 );
    establishConnection( 0 );
}

QVariant QEComboBox::getDrop()
{
    return QVariant( getSubstitutedVariableName(0) );
}

//==============================================================================
// Property convenience functions

// write on change
void QEComboBox::setWriteOnChange( bool writeOnChangeIn )
{
    writeOnChange = writeOnChangeIn;
}
bool QEComboBox::getWriteOnChange()
{
    return writeOnChange;
}

// subscribe
void QEComboBox::setSubscribe( bool subscribeIn )
{
    subscribe = subscribeIn;
}
bool QEComboBox::getSubscribe()
{
    return subscribe;
}

// use database enumerations
void QEComboBox::setUseDbEnumerations( bool useDbEnumerationsIn )
{
    useDbEnumerations = useDbEnumerationsIn;
}

bool QEComboBox::getUseDbEnumerations()
{
    return useDbEnumerations;
}
