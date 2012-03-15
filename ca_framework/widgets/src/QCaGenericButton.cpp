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

#include <QCaGenericButton.h>
#include <QProcess>
#include <QMessageBox>
#include <QMainWindow>
#include <QIcon>
#include <QInputDialog>

QCaGenericButton::QCaGenericButton( QWidget *owner ) : QCaWidget( owner )
{
}

/*!
    Setup common to all button constructors
*/
void QCaGenericButton::setup() {
    dataSetup();
    commandSetup();
    guiSetup();

    // Use push button signals
    QObject::connect( getButtonQObject(), SIGNAL( pressed() ), getButtonQObject(), SLOT( userPressed() ) );
    QObject::connect( getButtonQObject(), SIGNAL( released() ), getButtonQObject(), SLOT( userReleased() ) );
    QObject::connect( getButtonQObject(), SIGNAL( clicked( bool ) ), getButtonQObject(), SLOT( userClicked( bool ) ) );
}

/*!
    Setup for reading and writing data
*/
void QCaGenericButton::dataSetup() {
    // Set up data
    // This control uses two data sources, the first is written to and (by default) read from. The second is the alternative read back
    setNumVariables(QCAGENERICBUTTON_NUM_VARIABLES);

    // Set up default properties
    writeOnPress = false;
    writeOnRelease = false;
    writeOnClick = true;
    localEnabled = true;
    setAllowDrop( false );

    // Set text alignment to the default for a push button
    // This will make no visual difference unless the style has been changed from the default
    textAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
    this->setTextAlignment( textAlignment );

    pressText = "1";
    releaseText = "0";
    clickText = "1";
    clickCheckedText = "0";

    // Override default QCaWidget properties
    subscribe = false;

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    updateOption = getDefaultUpdateOption();
}

/*!
    Setup for running commands
*/
void QCaGenericButton::commandSetup() {
}

/*!
    Setup for starting new GUIs
*/
void QCaGenericButton::guiSetup() {

    // Set default properties
    creationOption = ASguiForm::CREATION_OPTION_OPEN;

    // If a profile is define by whatever contains the button, use it
    if( profile.isProfileDefined() )
    {
        // Setup a signal to launch a new gui
        // The signal will be used by whatever the button is in
        QObject::connect( getButtonQObject(), SIGNAL( newGui(  QString, ASguiForm::creationOptions ) ),
                          profile.getGuiLaunchConsumer(), SLOT( launchGui( QString,ASguiForm::creationOptions ) ) );
    }

    // A profile is not already defined, create one. This is the case if this class is used by an application that does not set up a profile, such as 'designer'.
    else
    {
        // Set up the button's own message handler
        userMessage.setup( getButtonQObject() );

        // Set up the button's own gui form launcher
        QObject::connect( getButtonQObject(), SIGNAL( newGui(  QString, ASguiForm::creationOptions ) ),
                          getButtonQObject(), SLOT( launchGui( QString, ASguiForm::creationOptions ) ) );
    }
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a push button a QCaObject that streams strings is required.
*/
qcaobject::QCaObject* QCaGenericButton::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QCaString
    return new QCaString( getSubstitutedVariableName( variableIndex ), getButtonQObject(), &stringFormatting, variableIndex );
}

/*!
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QCaGenericButton::establishConnection( unsigned int variableIndex ) {

    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots
    if(  qca ) {
        // Get updates if subscribing and if this is the alternate read back, or if this is the primary readback and there is no alternate read back.
        if( subscribe &&
            (
              ( variableIndex == 1 /*1=Alternate readback variable*/ ) ||
              ( variableIndex == 0 /*0=Primary readback variable*/ && getSubstitutedVariableName(1/*1=Alternate readback variable*/ ).isEmpty() )
            )
          )
        {
            if( updateOption == UPDATE_TEXT || updateOption == UPDATE_TEXT_AND_ICON)
            {
                setButtonText( "" );
            }
            connectButtonDataChange( qca );
        }

        // Get conection status changes always (subscribing or not)
        QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          getButtonQObject(), SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QCaGenericButton::connectionChanged( QCaConnectionInfo& connectionInfo )
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
            setButtonEnabled( true );
    }

    /// If disconnected always disable the widget.
    else
    {
        isConnected = false;
        updateToolTipConnection( isConnected );

        setButtonEnabled( false );
    }
}

/*!
  Implement a slot to set the current text of the push button
  This is the slot used to recieve data updates from a QCaObject based class.
*/
void QCaGenericButton::setGenericButtonText( const QString& text, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex )
{
    // If not subscribing, or subscribing but update is not for the readback variable, then do nothing.
    //
    // Note, This will still be called even if not subscribing as there may be an initial sing shot read
    // to ensure we have valid information about the variable when it is time to do a write.
    //
    // Note, variableIndex = 0 = Primary readback variable, variableIndex = 1 = Alternate readback variable,
    // so an update for variable 1 is always OK, an update from variable 0 is OK as long as there is no variable 1
    if( !subscribe || ( variableIndex == 0 && !getSubstitutedVariableName( 1 ).isEmpty() ))
    {
        return;
    }

    /// Signal a database value change to any Link widgets
    emitDbValueChanged( text );

    /// Update the button state if required
    /// Display checked if text matches what is written when checked
    if( updateOption == UPDATE_STATE )
    {
        setButtonState( !text.compare( clickCheckedText ) );
    }

    /// Update the text if required
    if( updateOption == UPDATE_TEXT || updateOption == UPDATE_TEXT_AND_ICON )
    {
        setButtonText( text );
    }

    /// Update the icon if required
    if( updateOption == UPDATE_ICON || updateOption == UPDATE_TEXT_AND_ICON )
    {
        QIcon icon;
        icon.addPixmap( getDataPixmap( text ) );
        setButtonIcon( icon );
    }

    /// If in alarm, display as an alarm
    if( alarmInfo.getSeverity() != lastSeverity )
    {
            updateToolTipAlarm( alarmInfo.severityName() );
            setButtonStyleSheet( alarmInfo.style() );
            lastSeverity = alarmInfo.getSeverity();
    }
}

/*!
    Button press event.
*/
void QCaGenericButton::userPressed() {
    /// Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// and the object is set up to write when the user presses the button
    /// then write the value
    if( qca && writeOnPress && checkPassword() ) {
        qca->writeString( substituteThis( pressText ));
    }
}

/*!
    Button release event.
*/

void QCaGenericButton::userReleased() {
    /// Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// and the object is set up to write when the user releases the button
    /// then write the value
    if( qca && writeOnRelease && checkPassword() ) {
        qca->writeString( substituteThis( releaseText ));
    }
}

/*!
    Button click event.
*/
void QCaGenericButton::userClicked( bool checked ) {
    /// Get the variable to write to
    QCaString *qca = (QCaString*)getQcaItem(0);

    // Do nothing if some action is due to be taken, but user does not anter any required password correctly
    if( ( writeOnClick || !program.isEmpty() || !guiName.isEmpty() ) && !checkPassword() )
        return;

    /// If a QCa object is present (if there is a variable to write to)
    /// and the object is set up to write when the user clicks the button
    /// then write the value
    if( qca && writeOnClick ) {
        if( !checked )
        {
            qca->writeString( substituteThis( substituteThis( clickText )));
        }
        else
        {
            qca->writeString( substituteThis( substituteThis( clickCheckedText )));
        }
    }

    // If there is a command to run, run it, with substitutions applied to the command and arguments
    if( !program.isEmpty() )
    {
        // Create a new process to run the program
        QProcess *process = new QProcess();

        // Apply substitutions to the arguments
        QStringList substitutedArguments = arguments;
        for( int i = 0; i < substitutedArguments.size(); i++ )
        {
            substitutedArguments[i] = substituteThis( substitutedArguments[i] );
        }

        // Apply substitutions to the program name
        QString prog = substituteThis( program );

        // Build up a single string with the command and arguments and run the program
        for( int i = 0; i < substitutedArguments.size(); i++)
        {
            prog.append( " " );
            prog.append( substitutedArguments[i] );
        }
        process->start( prog );

        // Alternate (and cleaner) way to run the program without building a string containing the program and arguments.
        // (This didn't seem to work when starting EDM with the '-one' switch, perhaps due to the
        //  way EDM checks all arguments are identical when the '-one' switch is present?)
        //process->start( substituteThis( program ), substitutedArguments );
    }

    // If a new GUI is required, start it
    if( !guiName.isEmpty() )
    {

        // Publish the profile this button recieved
        profile.publishOwnProfile();

        // Extend any variable name substitutions with this button's substitutions
        profile.addMacroSubstitutions( getVariableNameSubstitutions() );

        emitNewGui( substituteThis( guiName ), creationOption );

        // Remove this form's macro substitutions now all it's children are created
        profile.removeMacroSubstitutions();

        // Release the profile now all QCa widgets have been created
        profile.releaseProfile();
    }


}

/*!
  Check the password.
  Return true if there is no password, or if the user enters it correctly.
  Return false if the user cancels, or enteres an incorrect password.
  Give the user a warning message if a password is entered incorrectly.
*/
bool QCaGenericButton::checkPassword()
{
    // All OK if there is no password
    if( password.isEmpty() )
       return true;

    // Ask the user what the password is
    bool ok;
    QString text = QInputDialog::getText( (QWidget*)getButtonQObject(), "Password", "Password:", QLineEdit::Password, "", &ok );

    // If the user canceled, silently return password failure
    if( !ok )
        return false;

    // If the user entered the wrong password, show a warning, then return password failure
    if ( text.compare( password ) )
    {
        QMessageBox::warning( (QWidget*)getButtonQObject(), "Incorrect Password", "You entered the wrong password. No action will be taken" );
        return false;
    }

    // All OK, return password success
    return true;
}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaGenericButton::isEnabled() const
{
    // Return what the state of widget would be if connected.
    return localEnabled;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaGenericButton::setGenericEnabled( const bool& state )
{
    // Note the new 'enabled' state
    localEnabled = state;

    // Set the enabled state of the widget only if connected
    if( isConnected )
        setButtonEnabled( localEnabled );
}

//==============================================================================

// Update option Property convenience function
void QCaGenericButton::setUpdateOption( updateOptions updateOptionIn )
{
    updateOption = updateOptionIn;
}
QCaGenericButton::updateOptions QCaGenericButton::getUpdateOption()
{
    return updateOption;
}

// 'Data button' Property convenience functions


// Variable Name and substitution
void QCaGenericButton::setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
{
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
    setVariableName( variableNameIn, variableIndex );
    establishConnection( variableIndex );
}

// subscribe
void QCaGenericButton::setSubscribe( bool subscribeIn )
{
    subscribe = subscribeIn;
}
bool QCaGenericButton::getSubscribe()
{
    return subscribe;
}

// variable as tool tip
void QCaGenericButton::setVariableAsToolTip( bool variableAsToolTipIn )
{
    variableAsToolTip = variableAsToolTipIn;
}
bool QCaGenericButton::getVariableAsToolTip()
{
    return variableAsToolTip;
}

// text alignment
void QCaGenericButton::setTextAlignment( Qt::Alignment textAlignmentIn )
{
    // Keep a local copy of the alignment
    textAlignment = textAlignmentIn;

    // Update the style to match the property
    QString styleSheetString = "text-align: ";

    if( textAlignmentIn & Qt::AlignLeft )
        styleSheetString.append( " left");
    else if( textAlignment & Qt::AlignRight )
        styleSheetString.append( " right");
    else if( textAlignment & Qt::AlignHCenter )
        styleSheetString.append( " center");

    else if( textAlignment & Qt::AlignTop )
        styleSheetString.append( " top");
    else if( textAlignment & Qt::AlignBottom )
        styleSheetString.append( " bottom");

    setButtonStyleSheet( styleSheetString );

}
Qt::Alignment QCaGenericButton::getTextAlignment()
{
    return textAlignment;
}

// password
void QCaGenericButton::setPassword( QString password )
{
    QCaGenericButton::password = password;
}
QString QCaGenericButton::getPassword()
{
    return QCaGenericButton::password;
}

// write on press
void QCaGenericButton::setWriteOnPress( bool writeOnPress )
{
    QCaGenericButton::writeOnPress = writeOnPress;
}
bool QCaGenericButton::getWriteOnPress()
{
    return QCaGenericButton::writeOnPress;
}

// write on release
void QCaGenericButton::setWriteOnRelease( bool writeOnRelease )
{
    QCaGenericButton::writeOnRelease = writeOnRelease;
}
bool QCaGenericButton::getWriteOnRelease()
{
    return QCaGenericButton::writeOnRelease;
}

// write on click
void QCaGenericButton::setWriteOnClick( bool writeOnClick )
{
    QCaGenericButton::writeOnClick = writeOnClick;
}
bool QCaGenericButton::getWriteOnClick()
{
    return QCaGenericButton::writeOnClick;
}

// press value
void QCaGenericButton::setPressText( QString pressText )
{
    QCaGenericButton::pressText = pressText;
}
QString QCaGenericButton::getPressText()
{
    return QCaGenericButton::pressText;
}

// release value
void QCaGenericButton::setReleaseText( QString releaseTextIn )
{
    releaseText = releaseTextIn;
}
QString QCaGenericButton::getReleaseText(){ return releaseText; }

// click value
void QCaGenericButton::setClickText( QString clickTextIn )
{
    clickText = clickTextIn;
}
QString QCaGenericButton::getClickText()
{
    return clickText;
}

// click off value
void QCaGenericButton::setClickCheckedText( QString clickCheckedTextIn )
{
    clickCheckedText = clickCheckedTextIn;
}
QString QCaGenericButton::getClickCheckedText()
{
    return clickCheckedText;
}

//==============================================================================
// 'Command button' Property convenience functions

// Program String
void QCaGenericButton::setProgram( QString program ){ QCaGenericButton::program = program; }
QString QCaGenericButton::getProgram(){ return QCaGenericButton::program; }

// Arguments String
void QCaGenericButton::setArguments( QStringList arguments ){ QCaGenericButton::arguments = arguments; }
QStringList QCaGenericButton::getArguments(){ return QCaGenericButton::arguments; }




//==============================================================================
// 'Start new GUI' Property convenience functions

// GUI name
void QCaGenericButton::setGuiName( QString guiNameIn )
{
    guiName = guiNameIn;
}
QString QCaGenericButton::getGuiName()
{
    return guiName;
}

// Qt Designer Properties Creation options
void QCaGenericButton::setCreationOption( ASguiForm::creationOptions creationOptionIn )
{
    creationOption = creationOptionIn;
}
ASguiForm::creationOptions QCaGenericButton::getCreationOption()
{
    return creationOption;
}

// label text (prior to substitution)
void QCaGenericButton::setLabelTextProperty( QString labelTextIn )
{
    bool wasBlank = labelText.isEmpty();
    labelText = labelTextIn;

    // Update the button's text.
    // But don't do it if the label was already displaying something and the
    // text-to-be-substituted is just being re-set to blank). This behaviour will
    // mean the normal label 'text' property can be used if text substitution is
    // not required. Without this the user would always have to use the labelText property.
    if( !getButtonText().isEmpty() && !(wasBlank && labelText.isEmpty() ))
    {
        setButtonText( substituteThis( labelText ));
    }
}

QString QCaGenericButton::getLabelTextProperty()
{
    return labelText;
}

// allow drop (Enable/disable as a drop site for drag and drop)
void QCaGenericButton::setAllowDrop( bool allowDropIn )
{
    allowDrop = allowDropIn;
    QWidget* btn = (QWidget*)getButtonQObject();
    btn->setAcceptDrops( allowDrop );
}

bool QCaGenericButton::getAllowDrop()
{
    return allowDrop;
}

//==============================================================================
// 'Start new GUI' slots

// Slot for presenting messages to the user.
// Normally a gui will have provided it's own message and error handling.
// This is only used if no environment profile has been set up when a form is created. This is the case if created within 'designer'
// Use a general message
void QCaGenericButton::onGeneralMessage( QString message )
{
    QMessageBox msgBox;
    msgBox.setText( "Message" );
    msgBox.setInformativeText( message );
    msgBox.exec();
}

// Slot for launching a new gui.
// This is the button's default action for launching a gui.
// Normally the button would be within a container, such as a tab on a gui, that will provide a 'launch gui' mechanism.
void QCaGenericButton::launchGui( QString guiName, ASguiForm::creationOptions )
{
    // Build the gui
    // Build it in a new window.
    //??? This could use the create options as follows: (instead of always creating a new window)
    //       - Wind up through parents until the parent of the first scroll
    //       - Replace the scroll area's widget with the new gui
    QMainWindow* w = new QMainWindow;
    ASguiForm* gui = new ASguiForm( guiName );
    if( gui )
    {
        if( gui->readUiFile())
        {
            w->setCentralWidget( gui );
            w->show();
        }
        else
        {
            delete gui;
            gui = NULL;
        }
    }
    else
    {
        delete w;
    }
}
