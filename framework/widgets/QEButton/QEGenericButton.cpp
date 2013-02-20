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

/*
  This class is a CA aware push button widget based on the Qt push button widget.
  It is tighly integrated with the base class QEWidget. Refer to QEWidget.cpp for details
 */

#include <QEGenericButton.h>
#include <QProcess>
#include <QMessageBox>
#include <QMainWindow>
#include <QIcon>
#include <QInputDialog>

QEGenericButton::QEGenericButton( QWidget *owner ) : QEWidget( owner )
{
}

/*
    Setup common to all button constructors
*/
void QEGenericButton::setup() {
    dataSetup();
    commandSetup();
    guiSetup();

    // Use push button signals
    QObject::connect( getButtonQObject(), SIGNAL( pressed() ), getButtonQObject(), SLOT( userPressed() ) );
    QObject::connect( getButtonQObject(), SIGNAL( released() ), getButtonQObject(), SLOT( userReleased() ) );
    QObject::connect( getButtonQObject(), SIGNAL( clicked( bool ) ), getButtonQObject(), SLOT( userClicked( bool ) ) );
}

/*
    Setup for reading and writing data
*/
void QEGenericButton::dataSetup() {
    // Set up data
    // This control uses two data sources, the first is written to and (by default) read from. The second is the alternative read back
    setNumVariables(QEGENERICBUTTON_NUM_VARIABLES);

    // Set up default properties
    writeOnPress = false;
    writeOnRelease = false;
    writeOnClick = true;
    setAllowDrop( false );
    confirmRequired = false;

    // Set text alignment to the default for a push button
    // This will make no visual difference unless the style has been changed from the default
    textAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
    this->setTextAlignment( textAlignment );

    pressText = "1";
    releaseText = "0";
    clickText = "1";
    clickCheckedText = "0";

    // Override default QEWidget properties
    subscribe = false;

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    updateOption = getDefaultUpdateOption();
}

/*
    Setup for running commands
*/
void QEGenericButton::commandSetup() {
}

/*
    Setup for starting new GUIs
*/
void QEGenericButton::guiSetup() {

    // Set default properties
    creationOption = QEForm::CREATION_OPTION_OPEN;

    // If a profile is define by whatever contains the button, use it
    if( isProfileDefined() )
    {
        // Setup a signal to launch a new gui
        // The signal will be used by whatever the button is in
        QObject::connect( getButtonQObject(), SIGNAL( newGui(  QString, QEForm::creationOptions ) ),
                          getGuiLaunchConsumer(), SLOT( launchGui( QString,QEForm::creationOptions ) ) );
    }

    // A profile is not already defined, create one. This is the case if this class is used by an application that does not set up a profile, such as 'designer'.
    else
    {
        // Set up the button's own gui form launcher
        QObject::connect( getButtonQObject(), SIGNAL( newGui(  QString, QEForm::creationOptions ) ),
                          getButtonQObject(), SLOT( launchGui( QString, QEForm::creationOptions ) ) );
    }
}

/*
    Implementation of QEWidget's virtual funtion to create the specific type of QCaObject required.
    For a push button a QCaObject that streams strings is required.
*/
qcaobject::QCaObject* QEGenericButton::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QEString
    return new QEString( getSubstitutedVariableName( variableIndex ), getButtonQObject(), &stringFormatting, variableIndex );
}

/*
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QEGenericButton::establishConnection( unsigned int variableIndex ) {

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

/*
    Act on a connection change.
    Change how the label looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QEGenericButton::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    // Do nothing if no variable name, but there is a program to run or a new gui to open.
    // Most widgets will be dissabled at this point if there is no good connection to a PV,
    // but this widget may be doing other stuff (running a program of starting a GUI)
    if( getSubstitutedVariableName( 0 ).isEmpty() )
        return;

    // If connected enabled the widget if required.
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
}

/*
  Implement a slot to set the current text of the push button
  This is the slot used to recieve data updates from a QCaObject based class.
*/
void QEGenericButton::setGenericButtonText( const QString& text, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex )
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

    // Signal a database value change to any Link widgets
    emitDbValueChanged( text );

    // Update the button state if required
    // Display checked if text matches what is written when checked
    if( updateOption == UPDATE_STATE )
    {
        setButtonState( !text.compare( clickCheckedText ) );
    }

    // Update the text if required
    if( updateOption == UPDATE_TEXT || updateOption == UPDATE_TEXT_AND_ICON )
    {
        setButtonText( text );
    }

    // Update the icon if required
    if( updateOption == UPDATE_ICON || updateOption == UPDATE_TEXT_AND_ICON )
    {
        QIcon icon;
        icon.addPixmap( getDataPixmap( text ) );
        setButtonIcon( icon );
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
            setButtonStyleSheet( ai.style() );
            lastSeverity = ai.getSeverity();
    }
}

/*
 Returns true if no user confirmation required, or if user confirms a button press acion
 */
bool QEGenericButton::confirmAction()
{
    // Return OK if no confirmation required
    if( !confirmRequired )
        return true;

    // Get confirmation from the user as to what to do
    int confirm = QMessageBox::Yes;
    confirm = QMessageBox::warning( (QWidget*)getButtonQObject(), "Confirm write", "Do you want to perform this action ?",
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );

    // Return yes/no
    switch( confirm )
    {
        case QMessageBox::Yes:  return true;
        case QMessageBox::No:   return false;
        default:                return false;   // Sanity check
    }

}

/*
    Button press event.
*/
void QEGenericButton::userPressed()
{
    // Do nothing if not acting on button press, or user confirmation required but not given, or password required but not given
    if( !writeOnPress ||  !confirmAction() || !checkPassword() )
        return;

    // Determine the string to write
    QString writeText = substituteThis( pressText );

    // Emit a 'released' signal
    emitPressed( writeText.toInt() );

    // Get the variable to write to
    QEString *qca = (QEString*)getQcaItem(0);

    // If a QCa object is present (if there is a variable to write to) then write the value
    if( qca )
    {
        QString error;
        if( !qca->writeString( writeText, error ) )
        {
            QMessageBox::warning( (QWidget*)getButtonQObject(), QString( "Write failed" ), error, QMessageBox::Cancel );
        }
    }
}

/*
    Button release event.
*/

void QEGenericButton::userReleased()
{
    // Do nothing if not acting on button release, or user confirmation required but not given, or password required but not given
    if( !writeOnRelease ||  !confirmAction() || !checkPassword() )
        return;

    // Determine the string to write
    QString writeText = substituteThis( releaseText );

    // Emit a 'released' signal
    emitReleased( writeText.toInt() );

    // Get the variable to write to
    QEString *qca = (QEString*)getQcaItem(0);

    // If a QCa object is present (if there is a variable to write to) then write the value
    if( qca )
    {
        QString error;
        if( !qca->writeString( writeText, error ) )
        {
            QMessageBox::warning( (QWidget*)getButtonQObject(), QString( "Write failed" ), error, QMessageBox::Cancel );
        }
    }
}

/*
    Button click event.
*/
void QEGenericButton::userClicked( bool checked )
{
    // Do nothing if nothing to do. (no point asking for confirmation or password)
    // Then keep doing nothing if user confirmation required but not given, or password required but not given
    if(( !writeOnClick && program.isEmpty() && guiName.isEmpty() ) ||  !confirmAction() || !checkPassword() )
        return;

    // Get the variable to write to
    QEString *qca = (QEString*)getQcaItem(0);

    // If the object is set up to write when the user clicks the button
    // emit a signal
    // Also, if a QCa object is present (if there is a variable to write to)
    // then write the value
    if( writeOnClick )
    {
        // Determine the string to write
        QString writeText;
        if( !checked )
        {
            writeText = clickText;
        }
        else
        {
            writeText = clickCheckedText;
        }

        writeText = substituteThis( writeText );

        // Emit a 'released' signal
        emitClicked( writeText.toInt() );

        // Write to the variable if present
        if( qca )
        {
            QString error;
            if( !qca->writeString( writeText, error ) )
            {
                QMessageBox::warning( (QWidget*)getButtonQObject(), QString( "Write failed" ), error, QMessageBox::Cancel );
            }
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
        publishOwnProfile();

        // Extend any variable name substitutions with this button's substitutions
        // Like most other macro substitutions, the substitutions already present take precedence.
        addMacroSubstitutions( getVariableNameSubstitutions() );

        // Extend any variable name substitutions with this button's priority substitutions
        // Unlike most other macro substitutions, these macro substitutions take precedence over
        // substitutions already present.
        addPriorityMacroSubstitutions( prioritySubstitutions );

        qDebug()<<"QEGenericButton::userClicked() before launch signal";

        emitNewGui( substituteThis( guiName ), creationOption );

        qDebug()<<"QEGenericButton::userClicked() after launch signal";

        // Remove this button's priority macro substitutions now all its children are created
        removeMacroSubstitutions();

        // Remove this button's normal macro substitutions now all its children are created
        removeMacroSubstitutions();

        // Release the profile now all QE widgets have been created
        releaseProfile();
    }


}

/*
  Check the password.
  Return true if there is no password, or if the user enters it correctly.
  Return false if the user cancels, or enteres an incorrect password.
  Give the user a warning message if a password is entered incorrectly.
*/
bool QEGenericButton::checkPassword()
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

//==============================================================================

// Update option Property convenience function
void QEGenericButton::setUpdateOption( updateOptions updateOptionIn )
{
    updateOption = updateOptionIn;
}
QEGenericButton::updateOptions QEGenericButton::getUpdateOption()
{
    return updateOption;
}

// 'Data button' Property convenience functions

// subscribe
void QEGenericButton::setSubscribe( bool subscribeIn )
{
    subscribe = subscribeIn;
}
bool QEGenericButton::getSubscribe()
{
    return subscribe;
}

// text alignment
void QEGenericButton::setTextAlignment( Qt::Alignment textAlignmentIn )
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
Qt::Alignment QEGenericButton::getTextAlignment()
{
    return textAlignment;
}

// password
void QEGenericButton::setPassword( QString password )
{
    QEGenericButton::password = password;
}
QString QEGenericButton::getPassword()
{
    return QEGenericButton::password;
}

// confirm action
void QEGenericButton::setConfirmAction( bool confirmRequiredIn )
{
    confirmRequired = confirmRequiredIn;
}
bool QEGenericButton::getConfirmAction()
{
    return confirmRequired;
}

// write on press
void QEGenericButton::setWriteOnPress( bool writeOnPress )
{
    QEGenericButton::writeOnPress = writeOnPress;
}
bool QEGenericButton::getWriteOnPress()
{
    return QEGenericButton::writeOnPress;
}

// write on release
void QEGenericButton::setWriteOnRelease( bool writeOnRelease )
{
    QEGenericButton::writeOnRelease = writeOnRelease;
}
bool QEGenericButton::getWriteOnRelease()
{
    return QEGenericButton::writeOnRelease;
}

// write on click
void QEGenericButton::setWriteOnClick( bool writeOnClick )
{
    QEGenericButton::writeOnClick = writeOnClick;
}
bool QEGenericButton::getWriteOnClick()
{
    return QEGenericButton::writeOnClick;
}

// press value
void QEGenericButton::setPressText( QString pressText )
{
    QEGenericButton::pressText = pressText;
}
QString QEGenericButton::getPressText()
{
    return QEGenericButton::pressText;
}

// release value
void QEGenericButton::setReleaseText( QString releaseTextIn )
{
    releaseText = releaseTextIn;
}
QString QEGenericButton::getReleaseText(){ return releaseText; }

// click value
void QEGenericButton::setClickText( QString clickTextIn )
{
    clickText = clickTextIn;
}
QString QEGenericButton::getClickText()
{
    return clickText;
}

// click off value
void QEGenericButton::setClickCheckedText( QString clickCheckedTextIn )
{
    clickCheckedText = clickCheckedTextIn;
}
QString QEGenericButton::getClickCheckedText()
{
    return clickCheckedText;
}

//==============================================================================
// 'Command button' Property convenience functions

// Program String
void QEGenericButton::setProgram( QString program ){ QEGenericButton::program = program; }
QString QEGenericButton::getProgram(){ return QEGenericButton::program; }

// Arguments String
void QEGenericButton::setArguments( QStringList arguments ){ QEGenericButton::arguments = arguments; }
QStringList QEGenericButton::getArguments(){ return QEGenericButton::arguments; }




//==============================================================================
// 'Start new GUI' Property convenience functions

// GUI name
void QEGenericButton::setGuiName( QString guiNameIn )
{
    guiName = guiNameIn;
}
QString QEGenericButton::getGuiName()
{
    return guiName;
}

// Qt Designer Properties Creation options
void QEGenericButton::setCreationOption( QEForm::creationOptions creationOptionIn )
{
    creationOption = creationOptionIn;
}
QEForm::creationOptions QEGenericButton::getCreationOption()
{
    return creationOption;
}


// GUI name
void QEGenericButton::setPrioritySubstitutions( QString prioritySubstitutionsIn )
{
    prioritySubstitutions = prioritySubstitutionsIn;
}
QString QEGenericButton::getPrioritySubstitutions()
{
    return prioritySubstitutions;
}

//==============================================================================


// label text (prior to substitution)
void QEGenericButton::setLabelTextProperty( QString labelTextIn )
{
    bool wasBlank = labelText.isEmpty();
    labelText = labelTextIn;

    // Update the button's text.
    // But don't do it if the labelText property is just changing from blank to blank. This behaviour will
    // mean the normal label 'text' property can be used if text substitution is
    // not required. Without this the user would always have to use the labelText property.
    if( !(wasBlank && labelText.isEmpty() ))
    {
        setButtonText( substituteThis( labelText ));
    }
}

QString QEGenericButton::getLabelTextProperty()
{
    return labelText;
}

//==============================================================================
// 'Start new GUI' slots

// Slot for launching a new gui.
// This is the button's default action for launching a gui.
// Normally the button would be within a container, such as a tab on a gui, that will provide a 'launch gui' mechanism.
void QEGenericButton::launchGui( QString guiName, QEForm::creationOptions )
{
    // Build the gui
    // Build it in a new window.
    //??? This could use the create options as follows: (instead of always creating a new window)
    //       - Wind up through parents until the parent of the first scroll
    //       - Replace the scroll area's widget with the new gui
    QMainWindow* w = new QMainWindow;
    QEForm* gui = new QEForm( guiName );
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
