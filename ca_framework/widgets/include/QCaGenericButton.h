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

#ifndef QCAGENERICBUTTON_H
#define QCAGENERICBUTTON_H

#include <QCaWidget.h>
#include <QCaString.h>
#include <QCaStringFormatting.h>
#include <ASguiForm.h>
#include <UserMessage.h>
#include <ContainerProfile.h>
#include <managePixmaps.h>
#include <QCaStringFormattingMethods.h>

//! Maximum number of variables.
#define QCAGENERICBUTTON_NUM_VARIABLES 2

class QCaGenericButton : public QCaWidget, public managePixmaps, public QCaStringFormattingMethods {

  public:
    QCaGenericButton( QWidget *owner );
    virtual ~QCaGenericButton(){}

    enum updateOptions { UPDATE_TEXT, UPDATE_ICON, UPDATE_TEXT_AND_ICON, UPDATE_STATE };

    bool isEnabled() const;
    void setGenericEnabled( const bool& state );

    // 'Data button' Property convenience functions

    // Property convenience functions

    // Variable Name and substitution
    void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

    // subscribe
    void setSubscribe( bool subscribe );
    bool getSubscribe();

    // variable as tool tip
    void setVariableAsToolTip( bool variableAsToolTip );
    bool getVariableAsToolTip();

    // Allow user to drop new PVs into this widget
    void setAllowDrop( bool allowDropIn );
    bool getAllowDrop();

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

    // 'Start new GUI' Property convenience functions

    // GUI name
    void setGuiName( QString guiName );
    QString getGuiName();

    // Qt Designer Properties Creation options
    void setCreationOption( ASguiForm::creationOptions creationOption );
    ASguiForm::creationOptions getCreationOption();

    // label text (prior to substitution)
    void setLabelTextProperty( QString labelTextIn );
    QString getLabelTextProperty();

protected:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setGenericButtonText( const QString& text, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex );
    void userPressed();
    void userReleased();
    void userClicked( bool checked );

    void launchGui( QString guiName, ASguiForm::creationOptions creationOption );

    virtual updateOptions getDefaultUpdateOption() = 0;

public:
    void onGeneralMessage( QString message );

  protected:
    Qt::Alignment textAlignment;

    QString password; // Text to be entered in a dialog before any action

    bool writeOnPress;
    bool writeOnRelease;
    bool writeOnClick;
    QString releaseText;      /// Text to write on a button release
    QString pressText;        /// Text to write on a button press
    QString clickText;        /// Text to write on a button click
    QString clickCheckedText; /// Text to write on a button click when the button moves to the checked position

    QString program;        /// Program to run
    QStringList arguments;  /// Program arguments

    QString guiName;      /// GUI file name to launch
    ASguiForm::creationOptions creationOption;

    bool localEnabled;

    void establishConnection( unsigned int variableIndex );

    updateOptions updateOption;
    bool allowDrop;

    QString labelText;                                                 // Fixed label text to which substitutions will be applied

    bool getIsConnected(){ return isConnected; }

protected:
    void setup();
private:
    void dataSetup();
    void commandSetup();
    void guiSetup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex  );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    UserMessage userMessage;
    ContainerProfile profile;

    virtual void setButtonState( bool checked ) = 0;
    virtual void setButtonText( QString text ) = 0;
    virtual QString getButtonText() = 0;
    virtual void setButtonIcon( QIcon& icon ) = 0;

    virtual void setButtonEnabled( bool state ) = 0;
    virtual void setButtonStyleSheet( QString style ) = 0;

    virtual void emitDbValueChanged( QString text ) = 0;
    virtual void emitNewGui( QString guiName, ASguiForm::creationOptions creationOption ) = 0;

    virtual void connectButtonDataChange( qcaobject::QCaObject* qca ) = 0;

    virtual QObject* getButtonQObject() = 0;
    bool checkPassword();
};

#endif /// QCAGENERICBUTTON_H
