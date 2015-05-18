/*
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2009, 2010 Australian Synchrotron
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QEGENERICBUTTON_H
#define QEGENERICBUTTON_H

#include <QEWidget.h>
#include <QEForm.h>
#include <QEString.h>
#include <QEStringFormatting.h>
#include <managePixmaps.h>
#include <QEStringFormattingMethods.h>
//#include <QProcess>
#include <applicationLauncher.h>


// Maximum number of variables.
#define QEGENERICBUTTON_NUM_VARIABLES 2


//// Class to manage a process started by a QE button
//class processManager : public QProcess
//{
//    Q_OBJECT

//public:
//    processManager( bool logOutput )
//    {
//        // Catch when the process can be deleted
//        QObject::connect( this, SIGNAL( finished(int, QProcess::ExitStatus) ), this, SLOT( doFinished(int, QProcess::ExitStatus) ) );

//        // Catch output if required
//        if( logOutput )
//        {
//            QObject::connect( this, SIGNAL( readyReadStandardOutput() ), this, SLOT( doRead() ) );
//            QObject::connect( this, SIGNAL( readyReadStandardError() ), this, SLOT( doRead() ) );
//        }
//    }

//public slots:
//    void doRead()
//    {
//        message.sendMessage( readAll() );
//    }
//    void doFinished( int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/ )
//    {
//        deleteLater();
//    }

//private:
//    UserMessage message;
//};

// Class common to all QE buttons
class QEPLUGINLIBRARYSHARED_EXPORT QEGenericButton : public QEWidget, public managePixmaps, public QEStringFormattingMethods {

  public:
    QEGenericButton( QWidget *owner );
    virtual ~QEGenericButton(){}

    enum updateOptions { UPDATE_TEXT, UPDATE_ICON, UPDATE_TEXT_AND_ICON, UPDATE_STATE };

//    enum programStartupOptions { PSO_NONE,         // Just run the program
//                                 PSO_TERMINAL,     // Run the program in a termainal
//                                 PSO_LOGOUTPUT };  // Run the program, and log the output in the QE message system

    // subscribe
    void setSubscribe( bool subscribe );
    bool getSubscribe();

    // Properties

    // Update option (icon, text, or both)
    void setUpdateOption( updateOptions updateOptionIn );
    updateOptions getUpdateOption();

    // text alignment
    void setTextAlignment( Qt::Alignment alignment );
    Qt::Alignment getTextAlignment();

    // password
    void setPassword( QString password );
    QString getPassword();

    // confirm
    void setConfirmAction( bool confirmRequiredIn );
    bool getConfirmAction();

    // confirm text
    void setConfirmText( QString confirmTextIn );
    QString getConfirmText();

    // write on press
    void setWriteOnPress( bool writeOnPress );
    bool getWriteOnPress();

    // write on release
    void setWriteOnRelease( bool writeOnRelease );
    bool getWriteOnRelease();

    // write on click
    void setWriteOnClick( bool writeOnClick );
    bool getWriteOnClick();


    // press value
    void setPressText( QString pressText );
    QString getPressText();

    // release value
    void setReleaseText( QString releaseTextIn );
    QString getReleaseText();

    // click value
    void setClickText( QString clickTextIn );
    QString getClickText();

    // click checked value
    void setClickCheckedText( QString clickCheckedTextIn );
    QString getClickCheckedText();

    // 'Command button' Property convenience functions

    // Program String
    void setProgram( QString program );
    QString getProgram();

    // Arguments String
    void setArguments( QStringList arguments );
    QStringList getArguments();

    // Qt Designer Properties program startup options
    void setProgramStartupOption( applicationLauncher::programStartupOptions programStartupOptionIn );
    applicationLauncher::programStartupOptions getProgramStartupOption();

    // Property convenience functions

    // GUI name
    void setGuiName( QString guiName );
    QString getGuiName();

    // Priority macro substitutions
    void setPrioritySubstitutions( QString prioritySubstitutionsIn );
    QString getPrioritySubstitutions();

    // Window customisation name
    void setCustomisationName( QString customisationNameIn );
    QString getCustomisationName();

    // Qt Designer Properties Creation options
    void setCreationOption( QEActionRequests::Options creationOption );
    QEActionRequests::Options getCreationOption();

    // Label text (prior to substitution)
    void setLabelTextProperty( QString labelTextIn );
    QString getLabelTextProperty();


protected:
    void connectionChanged( QCaConnectionInfo& connectionInfo, const unsigned int& variableIndex );
    void setGenericButtonText( const QString& text, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex );
    void userPressed();
    void userReleased();
    void userClicked( bool checked );

    // write the click/clockChecked/press/release text value (of the associated button object) into the PV immediately.
    // performs no other action such as run command, open ui file.
    void processWriteNow( const bool checked );

    virtual updateOptions getDefaultUpdateOption() = 0;

    applicationLauncher programLauncher;

private:
    Qt::Alignment textAlignment;

    QString password; // Text to be entered in a dialog before any action

    bool writeOnPress;
    bool writeOnRelease;
    bool writeOnClick;
    bool confirmRequired;     // Request confirmation before acting on a button event
    QString confirmText;      // Text presented when confirming action
    QString releaseText;      // Text to write on a button release
    QString pressText;        // Text to write on a button press
    QString clickText;        // Text to write on a button click
    QString clickCheckedText; // Text to write on a button click when the button moves to the checked position

//    QString program;        // Program to run
//    QStringList arguments;  // Program arguments
//    programStartupOptions programStartupOption; // Startup option (in a terminal, log output, or just start it and forget it)

    QString guiName;      // GUI file name to launch
    QEActionRequests::Options creationOption;  // Indicate how the new gui should be created ( examples: in a new window, or a top dock)
    QString prioritySubstitutions;  // Macro substitutions that take precedence over existing substitutions when creating new guis
    QString customisationName;      // Name of set of Window customisations such as additional menu items or tool bar push buttons

    bool localEnabled;


    updateOptions updateOption;

    QString labelText;                                                 // Fixed label text to which substitutions will be applied

    bool getIsConnected(){ return isConnected; }
    bool confirmAction();

    // Drag and Drop (See specific QE button widgets for button type specific drag and drop)
private:
    void setDrop( QVariant drop );
    QVariant getDrop();

    // Copy paste (See specific QE button widgets for button type specific copy and paste)
    QString copyVariable();
    virtual QVariant copyData() = 0;
    void paste( QVariant s );

protected:
    void setup();
    void establishConnection( unsigned int variableIndex );
    void calcStyleOption();                                             // Calculate style based on the widget usage and set a dynamic propert for style options.

private:
    void dataSetup();
//    void commandSetup();
    void guiSetup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex  );

    bool isConnected;

    // !! Any of these that are accessing the QWidget don't have to call back up to the specific push button
    virtual void setButtonState( bool checked ) = 0;
    virtual void setButtonText( QString text ) = 0;
    virtual QString getButtonText() = 0;
    virtual void setButtonIcon( QIcon& icon ) = 0;

    virtual void emitDbValueChanged( QString text ) = 0;
    virtual void emitNewGui( const QEActionRequests& request ) = 0;

    virtual void connectButtonDataChange( qcaobject::QCaObject* qca ) = 0;

    virtual QObject* getButtonQObject() = 0;
    bool checkPassword();

    virtual void emitPressed( int pressValue ) = 0;
    virtual void emitReleased( int releaseValue ) = 0;
    virtual void emitClicked( int clickValue ) = 0;
};

#endif // QEGENERICBUTTON_H
