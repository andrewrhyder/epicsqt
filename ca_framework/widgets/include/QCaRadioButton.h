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

#ifndef QCARADIOBUTTON_H
#define QCARADIOBUTTON_H

#include <QRadioButton>
#include <QCaWidget.h>
#include <QCaString.h>
#include <QCaStringFormatting.h>
#include <QCaPluginLibrary_global.h>
#include <ASguiForm.h>
#include <UserMessage.h>
#include <ContainerProfile.h>
#include <managePixmaps.h>
#include <QCaGenericButton.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QCaRadioButton : public QRadioButton, public QCaGenericButton {
    Q_OBJECT

  public:
    QCaRadioButton( QWidget *parent = 0 );
    QCaRadioButton( const QString& variableName, QWidget *parent = 0 );

    bool isEnabled() const;
    void setEnabled( const bool& state ){ setGenericEnabled( state ); }

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo ) { QCaGenericButton::connectionChanged( connectionInfo ); }
    void setButtonText( const QString& text, QCaAlarmInfo& alarmInfo, QCaDateTime& timestamp, const unsigned int& variableIndex ) { setGenericButtonText( text, alarmInfo, timestamp, variableIndex); }
    void userPressed() { QCaGenericButton::userPressed(); }
    void userReleased() { QCaGenericButton::userReleased(); }
    void userClicked( bool checked ) { QCaGenericButton::userClicked( checked ); }


public slots:
    void launchGui( QString guiName, ASguiForm::creationOptions creationOption ){ QCaGenericButton::launchGui( guiName, creationOption); }

    void requestEnabled( const bool& state );

    void onGeneralMessage( QString message ){ QCaGenericButton::onGeneralMessage( message ); }


  signals:
    void dbValueChanged( const QString& out );

    void newGui( QString guiName, ASguiForm::creationOptions creationOption );

    void requestResend();

  protected:

private:
    void setup();
    void updateToolTip ( const QString & toolTip );

    void setButtonState( bool checked ){ QRadioButton::setChecked( checked ); }
    void setButtonText( QString text ){ QRadioButton::setText( text ); }
    QString getButtonText(){ return text(); }
    void setButtonIcon( QIcon& icon ) {setIcon( icon ); }

    void setButtonEnabled( bool state ){ QWidget::setEnabled( state ); }

    void setButtonStyleSheet( QString style ){ setStyleSheet( style ); }

    void emitDbValueChanged( QString text ){ emit dbValueChanged( text ); }

    void emitNewGui( QString guiName, ASguiForm::creationOptions creationOption  ){ emit newGui( guiName, creationOption); }

    void connectButtonDataChange( qcaobject::QCaObject* qca )
    {
                QObject::connect( qca,  SIGNAL( stringChanged( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                                  this, SLOT( setButtonText( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
                QObject::connect( this, SIGNAL( requestResend() ),
                                  qca, SLOT( resendLastData() ) );
    }

    QObject* getButtonQObject(){ return this; }

    void stringFormattingChange(){ requestResend(); }

    QCaGenericButton::updateOptions getDefaultUpdateOption() { return QCaGenericButton::UPDATE_STATE; }

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDropText( QString text );
    QString getDropText();
};

#endif /// QCARADIOBUTTON_H
