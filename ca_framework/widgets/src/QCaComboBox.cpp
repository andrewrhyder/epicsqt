/*! 
  \class QCaComboBox
  \version $Revision: #12 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author andrew.rhyder
  \brief CA Combobox Widget.
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

#include <QCaComboBox.h>

/*!
    Construct a combo box with no variable specified yet
*/
QCaComboBox::QCaComboBox( QWidget *parent ) : QComboBox( parent ), QCaWidget() {
    setup();
}

/*!
    Construct a combo box with a variable specified
*/
QCaComboBox::QCaComboBox( const QString &variableNameIn, QWidget *parent ) : QComboBox( parent ), QCaWidget() {
    setVariableName( variableNameIn, 0 );

    setup();

}

/*!
    Common construction
*/
void QCaComboBox::setup() {
    // Set up data
    // This control used a single data source
    setNumVariables(1);

    // Set up default properties
    useDbEnumerationsProperty = true;
    subscribeProperty = false;
    enabledProperty = true;

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    // Use line edit signals
    // Set up to write data when the user changes the value
    QObject::connect( this, SIGNAL( activated ( int ) ), this, SLOT( userValueChanged( int ) ) );
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a Combo box a QCaObject that streams integers is required.
*/
void QCaComboBox::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QCaInteger
    setQcaItem( new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex ), variableIndex );
}

/*!
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
*/
void QCaComboBox::establishConnection( unsigned int variableIndex ) {
    if( createConnection( variableIndex ) == true ) {
        setCurrentIndex( 0 );
        QObject::connect( getQcaItem( variableIndex ),
                          SIGNAL( integerChanged( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setValueIfNoFocus( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( getQcaItem( variableIndex ), SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}

/*!
   Act on a connection change.
   Change how the label looks and change the tool tip.

   Perform initialisation that can only be completed once data from the variable has been read.
   Specifically, set up the combo box entries to match the enumerated types if required.
   This function is called when the channel is first established to the data. It will also be called if the channel fails
   and recovers. Subsequent calls will do nothing as the combo box is already populated.
*/
void QCaComboBox::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    /// If connected, enable the widget if the QCa enabled property is true
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

    /// Start a single shot read if the channel is up (ignore channel down),
    /// This will allow initialisation of the widget using info from the database.
    /// If the combo box is already populated, then it has been set up at design time, or this is a subsequent 'channel up'
    /// If subscribing, then an update will occur without having to initiated one here.
    /// Note, channel up implies link up
    if( connectionInfo.isChannelConnected() && count() == 0 && !subscribeProperty )
    {
        QCaInteger* qca = (QCaInteger*)getQcaItem(0);
        qca->singleShotRead();
    }
}

/*!
    Update the tool tip as requested by QCaWidget.
*/
void QCaComboBox::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Pass the update straight on to the ComboBox unless the user is changing it.
    Note, it would not be common to have a user editing a regularly updating
    value. However, this scenario should be allowed for. A reasonable reason
    for a user modified value to update on a gui is if is is written to by
    another user on another gui.

    Note, this will still be called once if not subscribing to set up enumeration values.
    See  QCaComboBox::dynamicSetup() for details.
*/

void QCaComboBox::setValueIfNoFocus( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& ) {

    // If the combo box is not populated, setup the enumerations if any.
    // If not subscribing, there will still be an initial update to get enumeration values.
    if( count() == 0 )
    {
        QCaInteger* qca = (QCaInteger*)getQcaItem(0);
        QStringList enumerations = qca->getEnumerations();
        if( useDbEnumerationsProperty && enumerations.size() )
        {
            insertItems( 0,enumerations );
        }
    }

    /// If not subscribing, then do nothing.
    /// Note, This will still be called even if not subscribing as there may be initial sing shot read
    /// to ensure we have valid information about the variable when it is time to do a write.
    if( !subscribeProperty )
        return;

    // Signal a database value change to any Link widgets
    emit dbValueChanged( value );

    /// Update the text if appropriate
    /// If the user is editing the object then updates will be inapropriate
    if( hasFocus() == false )
    {
        setCurrentIndex( value );
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
    The user has changed the Combo box.
*/
void QCaComboBox::userValueChanged( int value ) {

    /// Get the variable to write to
    QCaInteger* qca = (QCaInteger*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// then write the value
    if( qca ) {
        qca->writeInteger( (long)value );
    }
}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaComboBox::isEnabled() const
{
    /// Return what the state of widget would be if connected.
    return enabledProperty;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaComboBox::setEnabled( bool state )
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
void QCaComboBox::requestEnabled( const bool& state )
{
    setEnabled(state);
}
