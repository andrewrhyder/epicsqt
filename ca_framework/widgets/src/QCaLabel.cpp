/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/widgets/src/QCaLabel.cpp $
 * $Revision: #11 $
 * $DateTime: 2010/02/18 15:15:02 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaLabel
  \version $Revision: #11 $
  \date $DateTime: 2010/02/18 15:15:02 $
  \author andrew.rhyder
  \brief CA Label Widget.
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

#include <QCaLabel.h>

/*!
    Constructor with no initialisation
*/
QCaLabel::QCaLabel( QWidget *parent ) : QLabel( parent ), QCaWidget() {
    setup();
}

/*!
    Constructor with known variable
*/
QCaLabel::QCaLabel( const QString &variableNameIn, QWidget *parent ) : QLabel( parent ), QCaWidget() {
    setup();
    setVariableName( variableNameIn, 0 );
}

/*!
    Setup common to all constructors
*/
void QCaLabel::setup() {
    // Set up data
    // This control used a single data source
    setNumVariables(1);

    // Set up default properties
    enabledProperty = true;
    visibleProperty = true;

    // Set the initial state
    setText( "" );
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    // Use label signals
    // --Currently none--
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a label a QCaObject that streams strings is required.
*/
void QCaLabel::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QCaString
    setQcaItem( new QCaString( getSubstitutedVariableName( variableIndex ), this, &stringFormatting, variableIndex ), variableIndex );
}

/*!
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
*/
void QCaLabel::establishConnection( unsigned int variableIndex ) {

    if( createConnection( variableIndex ) == true ) {
        QObject::connect(getQcaItem( variableIndex ), SIGNAL( stringChanged( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                         this, SLOT( setLabelText( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( getQcaItem( variableIndex ), SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}


/*!
    Update the tool tip as requested by QCaWidget.
*/
void QCaLabel::updateToolTip( const QString& tip )
{
    setToolTip( tip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
 */
void QCaLabel::connectionChanged( QCaConnectionInfo& connectionInfo )
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
}

/*!
    Update the label text
 */
void QCaLabel::setLabelText( const QString& text, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& ) {

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
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaLabel::isEnabled() const
{
    /// Return what the state of widget would be if connected.
    return enabledProperty;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaLabel::setEnabled( bool state )
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
void QCaLabel::requestEnabled( const bool& state )
{
    setEnabled(state);
}


/*!
  Manage property to set widget visible or not
 */
void QCaLabel::setVisibleProperty( bool visiblePropertyIn )
{
    // Update the property
    visibleProperty = visiblePropertyIn;

    // If a container profile has been defined, then this widget is being used in a real GUI and
    // should be visible or not according to the visible property. (While in Designer it can always be displayed)
    ContainerProfile profile;
    if( profile.isProfileDefined() )
    {
        setVisible( visibleProperty );
    }

}
