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
  This class is a CA aware push button widget based on the Qt push button widget.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details
 */

#include <QCaPushButton.h>
#include <QProcess>
#include <QMessageBox>
#include <QMainWindow>
#include <QIcon>

/*!
    Constructor with no initialisation
*/
QCaPushButton::QCaPushButton( QWidget *parent ) : QPushButton( parent ), QCaWidget() {
    setup();
}

/*!
    Constructor with known variable
*/
QCaPushButton::QCaPushButton( const QString &variableNameIn, QWidget *parent ) : QPushButton( parent ), QCaWidget() {
    setVariableName( variableNameIn, 0 );

    setup();

}

/*!
    Setup common to all constructors
*/
void QCaPushButton::setup() {
    dataSetup();
    commandSetup();
    guiSetup();

    setText( "QCaPushButton" );

    // Use push button signals
    QObject::connect( this, SIGNAL( pressed() ), this, SLOT( userPressed() ) );
    QObject::connect( this, SIGNAL( released() ), this, SLOT( userReleased() ) );
    QObject::connect( this, SIGNAL( clicked() ), this, SLOT( userClicked() ) );
}

/*!
    Setup for reading and writing data
*/
void QCaPushButton::dataSetup() {
    // Set up data
    // This control used a single data source
    setNumVariables(1);

    // Set up default properties
    writeOnPress = false;
    writeOnRelease = false;
    writeOnClick = true;
    localEnabled = true;

    pressText = "1";
    releaseText = "0";
    clickText = "1";

    // Override default QCaWidget properties
    subscribe = false;

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
}

/*!
    Setup for running commands
*/
void QCaPushButton::commandSetup() {
}

/*!
    Setup for starting new GUIs
*/
void QCaPushButton::guiSetup() {

    // Set default properties
    creationOption = ASguiForm::CREATION_OPTION_OPEN;

    // If a profile is define by whatever contains the button, use it
    if( profile.isProfileDefined() )
    {
        // A profile is already defined, either by the gui application or a ASGui form form.
        // Extend any variable name substitutions with this form's substitutions
        profile.addMacroSubstitutions( getVariableNameSubstitutions() );

        // Setup a signal to launch a new gui
        // The signal will be used by whatever the button is in
        QObject::connect( (QObject*)this, SIGNAL( newGui(  QString, QString, ASguiForm::creationOptions ) ),
                          profile.getGuiLaunchConsumer(), SLOT( launchGui( QString, QString, ASguiForm::creationOptions ) ) );

    }

    // A profile is not already defined, create one. This is the case if this class is used by an application that does not set up a profile, such as 'designer'.
    else
    {
        // Set up the button's own message handler
        userMessage.setup( this );

        // Set up the button's own gui form launcher
        QObject::connect( (QObject*)this, SIGNAL( newGui(  QString, QString, ASguiForm::creationOptions ) ),
                          this, SLOT( launchGui( QString, QString, ASguiForm::creationOptions ) ) );
    }
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a push button a QCaObject that streams strings is required.
*/
qcaobject::QCaObject* QCaPushButton::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QCaString
    return new QCaString( getSubstitutedVariableName( variableIndex ), this, &stringFormatting, variableIndex );
}

/*!
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QCaPushButton::establishConnection( unsigned int variableIndex ) {

    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots
    if(  qca ) {
        // Get updates if subscribing
        if( subscribe )
        {
            setText( "" );
            QObject::connect( qca,  SIGNAL( stringChanged( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                              this, SLOT( setButtonText( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        }

        // Get conection status changes always (subscribing or not)
        QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}

/*!
    Update the tool tip as requested by QCaToolTip.
*/
void QCaPushButton::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QCaPushButton::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    // Do nothing if no variable name, but there is a program to run or a new gui to open.
    // Most widgets will be dissabled at this point if there is no good connection to a PV,
    // but this widget may be doing other stuff (running a program of starting a GUI)
    if( getSubstitutedVariableName( 0 ).isEmpty() )
        return;

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
  Implement a slot to set the current text of the push button
  This is the slot used to recieve data updates from a QCaObject based class.
*/
void QCaPushButton::setButtonText( const QString& text, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& )
{
    /// If not subscribing, then do nothing.
    /// Note, This will still be called even if not subscribing as there may be an initial sing shot read
    /// to ensure we have valid information about the variable when it is time to do a write.
    if( !subscribe )
        return;

    /// Signal a database value change to any Link widgets
    emit dbValueChanged( text );

    /// Update the text if required
    if( updateOption == UPDATE_TEXT || updateOption == UPDATE_TEXT_AND_ICON )
    {
        setText( text );
    }

    /// Update the icon if required
    if( updateOption == UPDATE_ICON || updateOption == UPDATE_TEXT_AND_ICON )
    {
        QIcon icon;
        icon.addPixmap( getDataPixmap( text ) );
        setIcon( icon );
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
    Button press event.
*/
void QCaPushButton::userPressed() {
    /// Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// and the object is set up to write when the user presses the button
    /// then write the value
    if( qca && writeOnPress ) {
        qca->writeString( pressText );
    }
}

/*!
    Button release event.
*/

void QCaPushButton::userReleased() {
    /// Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// and the object is set up to write when the user releases the button
    /// then write the value
    if( qca && writeOnRelease ) {
        qca->writeString( releaseText );
    }
}

/*!
    Button click event.
*/
void QCaPushButton::userClicked() {
    qDebug() << "userClicked()";
    /// Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// and the object is set up to write when the user clicks the button
    /// then write the value
    if( qca && writeOnClick ) {
        qca->writeString( clickText );
    }

    // If there is a command to run, run it
    //??? use substitutions (from the profile) in the command and arguments (change name from variableNameSubstitutions to commandSubstitutions)
    if( !program.isEmpty() )
    {
        QProcess *process = new QProcess();
        process->start( program, arguments );
    }

    // If a new GUI is required, start it
    if( !guiName.isEmpty() )
    {
        emit newGui( guiName, getVariableNameSubstitutions(), creationOption );
    }


}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaPushButton::isEnabled() const
{
    // Return what the state of widget would be if connected.
    return localEnabled;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaPushButton::setEnabled( const bool& state )
{
    // Note the new 'enabled' state
    localEnabled = state;

    // Set the enabled state of the widget only if connected
    if( isConnected )
        QWidget::setEnabled( localEnabled );
}

/*!
   Slot similar to default widget setEnabled slot, but will use our own setEnabled which will allow alarm states to override current enabled state
 */
void QCaPushButton::requestEnabled( const bool& state )
{
    QCaPushButton::setEnabled(state);
}

//==============================================================================

// Update option Property convenience function
void QCaPushButton::setUpdateOption( updateOptions updateOptionIn )
{
    updateOption = updateOptionIn;
}
QCaPushButton::updateOptions QCaPushButton::getUpdateOption()
{
    return updateOption;
}

// 'Data button' Property convenience functions


// Variable Name and substitution
void QCaPushButton::setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
{
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
    setVariableName( variableNameIn, variableIndex );
    establishConnection( variableIndex );
}

// subscribe
void QCaPushButton::setSubscribe( bool subscribeIn )
{
    subscribe = subscribeIn;
}
bool QCaPushButton::getSubscribe()
{
    return subscribe;
}

// variable as tool tip
void QCaPushButton::setVariableAsToolTip( bool variableAsToolTipIn )
{
    variableAsToolTip = variableAsToolTipIn;
}
bool QCaPushButton::getVariableAsToolTip()
{
    return variableAsToolTip;
}


// String formatting properties

// precision
void QCaPushButton::setPrecision( unsigned int precision )
{
    stringFormatting.setPrecision( precision );
}
unsigned int QCaPushButton::getPrecision()
{
    return stringFormatting.getPrecision();
}

// useDbPrecision
void QCaPushButton::setUseDbPrecision( bool useDbPrecision )
{
    stringFormatting.setUseDbPrecision( useDbPrecision);
}
bool QCaPushButton::getUseDbPrecision()
{
    return stringFormatting.getUseDbPrecision();
}

// leadingZero
void QCaPushButton::setLeadingZero( bool leadingZero )
{
    stringFormatting.setLeadingZero( leadingZero );
}
bool QCaPushButton::getLeadingZero()
{
    return stringFormatting.getLeadingZero();
}

// trailingZeros
void QCaPushButton::setTrailingZeros( bool trailingZeros )
{
    stringFormatting.setTrailingZeros( trailingZeros );
}
bool QCaPushButton::getTrailingZeros()
{
    return stringFormatting.getTrailingZeros();
}

// addUnits
void QCaPushButton::setAddUnits( bool addUnits )
{
    stringFormatting.setAddUnits( addUnits );
}
bool QCaPushButton::getAddUnits()
{
    return stringFormatting.getAddUnits();
}

// format
void QCaPushButton::setFormat( QCaStringFormatting::formats format )
{
    stringFormatting.setFormat( format );
}
QCaStringFormatting::formats QCaPushButton::getFormat()
{
    return stringFormatting.getFormat();
}

// radix
void QCaPushButton::setRadix( unsigned int radix )
{
    stringFormatting.setRadix( radix);
}
unsigned int QCaPushButton::getRadix()
{
    return stringFormatting.getRadix();
}

// notation
void QCaPushButton::setNotation( QCaStringFormatting::notations notation )
{
    stringFormatting.setNotation( notation );
}
QCaStringFormatting::notations QCaPushButton::getNotation()
{
    return stringFormatting.getNotation();
}

// write on press
void QCaPushButton::setWriteOnPress( bool writeOnPress )
{
    QCaPushButton::writeOnPress = writeOnPress;
}
bool QCaPushButton::getWriteOnPress()
{
    return QCaPushButton::writeOnPress;
}

// write on release
void QCaPushButton::setWriteOnRelease( bool writeOnRelease )
{
    QCaPushButton::writeOnRelease = writeOnRelease;
}
bool QCaPushButton::getWriteOnRelease()
{
    return QCaPushButton::writeOnRelease;
}

// write on click
void QCaPushButton::setWriteOnClick( bool writeOnClick )
{
    QCaPushButton::writeOnClick = writeOnClick;
}
bool QCaPushButton::getWriteOnClick()
{
    return QCaPushButton::writeOnClick;
}


// press value
void QCaPushButton::setPressText( QString pressText )
{
    QCaPushButton::pressText = pressText;
}
QString QCaPushButton::getPressText()
{
    return QCaPushButton::pressText;
}

// release value
void QCaPushButton::setReleaseText( QString releaseTextIn )
{
    releaseText = releaseTextIn;
}
QString QCaPushButton::getReleaseText(){ return releaseText; }

// click value
void QCaPushButton::setClickText( QString clickTextIn )
{
    clickText = clickTextIn;
}
QString QCaPushButton::getClickText()
{
    return clickText;
}

//==============================================================================
// 'Command button' Property convenience functions

// Program String
void QCaPushButton::setProgram( QString program ){ QCaPushButton::program = program; }
QString QCaPushButton::getProgram(){ return QCaPushButton::program; }

// Arguments String
void QCaPushButton::setArguments( QStringList arguments ){ QCaPushButton::arguments = arguments; }
QStringList QCaPushButton::getArguments(){ return QCaPushButton::arguments; }




//==============================================================================
// 'Start new GUI' Property convenience functions

// GUI name
void QCaPushButton::setGuiName( QString guiNameIn )
{
    guiName = guiNameIn;
}
QString QCaPushButton::getGuiName()
{
    return guiName;
}

// Qt Designer Properties Creation options
void QCaPushButton::setCreationOption( ASguiForm::creationOptions creationOptionIn )
{
    creationOption = creationOptionIn;
}
ASguiForm::creationOptions QCaPushButton::getCreationOption()
{
    return creationOption;
}

//==============================================================================
// 'Start new GUI' slots

// Slot for presenting messages to the user.
// Normally a gui will have provided it's own message and error handling.
// This is only used if no environment profile has been set up when a form is created. This is the case if created within 'designer'
// Use a general message
void QCaPushButton::onGeneralMessage( QString message )
{
    QMessageBox msgBox;
    msgBox.setText( "Message" );
    msgBox.setInformativeText( message );
    msgBox.exec();
}

// Slot for launching a new gui.
// This is the button's default action for launching a gui.
// Normally the button would be within a container, such as a tab on a gui, that will provide a 'launch gui' mechanism.
void QCaPushButton::launchGui( QString guiName, QString /*substitutions*/, ASguiForm::creationOptions )
{
    // Extend substitutions???


    // Build the gui
    // Build it in a new window.
    //??? This could use the create options as follows: (instead of always creating a new window)
    //       - Wind up through parents until the parent of the first scroll
    //       - Replace the scroll area's widget with the new gui
    QMainWindow* w = new QMainWindow;
    ASguiForm* gui = new ASguiForm( guiName );
    if( gui )
    {
        gui->readUiFile();
        w->setCentralWidget( gui );
        w->show();
    }
    else
    {
        delete w;
    }
}
