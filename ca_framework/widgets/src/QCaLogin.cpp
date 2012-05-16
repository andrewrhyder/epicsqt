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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Ricardo Fernandes
 *  Contact details:
 *    ricardo.fernandes@synchrotron.org.au
 */

/*!
  This class is a CA aware label widget based on the Qt label widget.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details
 */

#include <QCaLogin.h>

/*!
    Constructor with no initialisation
*/
QCaLogin::QCaLogin( QWidget *parent ) : QLabel( parent ), QCaWidget( this )
{
    setup();
}

/*!
    Constructor with known variable
*/
//QCaLogin::QCaLogin( const QString &variableNameIn, QWidget *parent ) : QLabel( parent ), QCaWidget( this )  {
//    setup();
//    setVariableName( variableNameIn, 0 );
//}

/*!
    Setup common to all constructors
*/
void QCaLogin::setup() {

    // Set up data
    // This control used a single data source
    setNumVariables(1);

    // Set up default properties
    caEnabled = true;
    caVisible = true;
    setAllowDrop( false );

    // Set the initial state
    setText( "" );
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state
    updateOption = UPDATE_TEXT;

    defaultStyleSheet = styleSheet();
    // Use label signals
    // --Currently none--
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a label a QCaObject that streams strings is required.
*/
qcaobject::QCaObject* QCaLogin::createQcaItem( unsigned int variableIndex ) {
    // Create the item as a QCaString
   return new QCaString( getSubstitutedVariableName( variableIndex ), this, &stringFormatting, variableIndex );
}

/*!
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QCaLogin::establishConnection( unsigned int variableIndex ) {

    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots
    if(  qca ) {
        QObject::connect( qca,  SIGNAL( stringChanged( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setLabelText( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
        QObject::connect( this, SIGNAL( requestResend() ),
                          qca, SLOT( resendLastData() ) );
    }
}


/*!
    Update the tool tip as requested by QCaToolTip.
*/
void QCaLogin::updateToolTip( const QString& tip )
{
    setToolTip( tip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QCaLogin::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    /// If connected, enable the widget if the QCa enabled property is true
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );

        if( caEnabled )
            setEnabled( true );
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
    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QCaLogin::setLabelText( const QString& textIn, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& ) {

    // Extract any formatting info from the text
    // For example "<background-color: red>Engineering Mode" or "<color: red>not selected"
    QString text = textIn;
    QString textStyle;
    int textStyleStart = text.indexOf( '<' );
    if( textStyleStart >= 0 )
    {
        int textStyleEnd = text.indexOf( '>', textStyleStart );
        if( textStyleEnd >= 1 )
        {
            textStyle = text.mid( textStyleStart+1, textStyleEnd-textStyleStart-1 );
            text = text.left( textStyleStart ).append( text.right( text.length()-textStyleEnd-1 ));
        }
    }

    // Update the color
    if( textStyle.compare( lastTextStyle ) )
    {
        if( !textStyle.isEmpty() )
        {
            textStyleSheet = QString( "QWidget { " ).append( textStyle ).append( "; }");
        }
        else
        {
            textStyleSheet = "";
        }
        updateStyleSheet();
        lastTextStyle = textStyle;
    }

    /// Signal a database value change to any Link widgets
    emit dbValueChanged( text );

    switch( updateOption )
    {
        /// Update the text if required
        case UPDATE_TEXT:
            setText( text );
            break;

        /// Update the pixmap if required
        case UPDATE_PIXMAP:
            setPixmap( getDataPixmap( text ).scaled( size() ) );
            break;
    }

    /// If in alarm, display as an alarm
    if( alarmInfo.getSeverity() != lastSeverity )
    {
        updateToolTipAlarm( alarmInfo.severityName() );
        alarmStyleSheet = alarmInfo.style();
        lastSeverity = alarmInfo.getSeverity();

        updateStyleSheet();
    }
}

/*!
   Update the style sheet with the various style sheet components used to modify the label style (alarm info, enumeration color)
 */
void QCaLogin::updateStyleSheet()
{
    QString newStyleSheet;
    newStyleSheet.append( defaultStyleSheet ).append( alarmStyleSheet ).append( textStyleSheet );
    setStyleSheet( newStyleSheet );
}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaLogin::isEnabled() const
{
    /// Return what the state of widget would be if connected.
    return caEnabled;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaLogin::setEnabled( bool state )
{
    /// Note the new 'enabled' state
    caEnabled = state;

    /// Set the enabled state of the widget only if connected
    if( isConnected )
        QWidget::setEnabled( caEnabled );
}
/*!
   Slot similar to default widget setEnabled, but will use our own setEnabled which will allow alarm states to override current enabled state
 */
void QCaLogin::requestEnabled( const bool& state )
{
    setEnabled(state);
}

//==============================================================================
// Drag drop
void QCaLogin::setDropText( QString text )
{
    setVariableName( text, 0 );
    establishConnection( 0 );
}

QString QCaLogin::getDropText()
{
    return getSubstitutedVariableName(0);
}

//==============================================================================
// Property convenience functions


// Access functions for variableName and variableNameSubstitutions
// variable substitutions Example: SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
void QCaLogin::setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex ) {
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
    setVariableName( variableNameIn, variableIndex );
    establishConnection( variableIndex );
}

// variable as tool tip
void QCaLogin::setVariableAsToolTip( bool variableAsToolTipIn )
{
    variableAsToolTip = variableAsToolTipIn;
}
bool QCaLogin::getVariableAsToolTip()
{
    return variableAsToolTip;
}

// Update option Property convenience function
void QCaLogin::setUpdateOption( updateOptions updateOptionIn )
{
    updateOption = updateOptionIn;
}
QCaLogin::updateOptions QCaLogin::getUpdateOption()
{
    return updateOption;
}

// visible (widget is visible outside 'Designer')
void QCaLogin::setRunVisible( bool visibleIn )
{
    // Update the property
    caVisible = visibleIn;

    // If a container profile has been defined, then this widget is being used in a real GUI and
    // should be visible or not according to the visible property. (While in Designer it can always be displayed)
    ContainerProfile profile;
    if( profile.isProfileDefined() )
    {
        QWidget::setVisible( caVisible );
    }

}
bool QCaLogin::getRunVisible()
{
    return caVisible;
}

// allow drop (Enable/disable as a drop site for drag and drop)
void QCaLogin::setAllowDrop( bool allowDropIn )
{
    allowDrop = allowDropIn;
    setAcceptDrops( allowDrop );
}

bool QCaLogin::getAllowDrop()
{
    return allowDrop;
}
























void QCaLogin::setShowButtonLogin(bool pValue)
{

    //qPushButtonLogin->setVisible(pValue);

}



bool QCaLogin::getShowButtonLogin()
{

    //return qPushButtonLogin->isVisible();

}



void QCaLogin::setUserPassword(QString pValue)
{

    //userPassword = pValue;

}



QString QCaLogin::getUserPassword()
{

    //return userPassword;
    return "abc";

}


void QCaLogin::setScientistPassword(QString pValue)
{

    //scientistPassword = pValue;

}


QString QCaLogin::getScientistPassword()
{

    //return scientistPassword;
    return "def";

}


void QCaLogin::setEngineerPassword(QString pValue)
{

    //engineerPassword = pValue;

}



QString QCaLogin::getEngineerPassword()
{

    //return engineerPassword;
    return "ghi";

}



void QCaLogin::setCurrentUserType(int pValue)
{

//    if (pValue == USER_TYPE || pValue == SCIENTIST_TYPE || pValue == ENGINEER_TYPE)
//    {
//        currentUserType = pValue;
//        setCurrentUserType(currentUserType);
//        // should update slot to notify other widgets that the user type has changed
//    }

}



int QCaLogin::getCurrentUserType()
{

    //return currentUserType;
    return 0;

}


