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
    Setup common to all constructors
*/
void QCaLogin::setup()
{


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

















void QCaLogin::setDropText( QString text )
{
    setVariableName( text, 0 );
    establishConnection( 0 );
}



QString QCaLogin::getDropText()
{
    return getSubstitutedVariableName(0);
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


