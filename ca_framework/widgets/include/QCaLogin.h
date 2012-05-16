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

#ifndef QCALOGIN_H
#define QCALOGIN_H

#include <QLabel>
#include <QCaWidget.h>
#include <QCaString.h>
#include <QCaStringFormatting.h>
#include <QCaPluginLibrary_global.h>
#include <managePixmaps.h>
#include <QCaStringFormattingMethods.h>


class QCAPLUGINLIBRARYSHARED_EXPORT QCaLogin : public QLabel, public QCaWidget, public managePixmaps, public QCaStringFormattingMethods {
    Q_OBJECT

  public:
    enum updateOptions { UPDATE_TEXT, UPDATE_PIXMAP };


    enum userTypes
    {
        USER_TYPE,
        SCIENTIST_TYPE,
        ENGINEER_TYPE
    };




    QCaLogin( QWidget *parent = 0 );


    bool isEnabled() const;
    void setEnabled( bool state );



  protected:
    bool caEnabled;

    void establishConnection( unsigned int variableIndex );

    bool caVisible;               // Flag true if the widget should be visible outside 'Designer'

    updateOptions updateOption;

    bool allowDrop;

private slots:

  public slots:
    void requestEnabled( const bool& state );

  signals:
    void dbValueChanged( const QString& out );
    void requestResend();

  private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip( const QString& tip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    QString lastTextStyle;

    QString defaultStyleSheet;
    QString alarmStyleSheet;
    QString textStyleSheet;

    void stringFormattingChange(){ requestResend(); }

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }




    protected:
        void setDropText(QString text);
        QString getDropText();




    public:

        void setShowButtonLogin(bool pValue);
        bool getShowButtonLogin();

        void setUserPassword(QString pValue);
        QString getUserPassword();

        void setScientistPassword(QString pValue);
        QString getScientistPassword();

        void setEngineerPassword(QString pValue);
        QString getEngineerPassword();

        void setCurrentUserType(int pValue);
        int getCurrentUserType();



};

#endif /// QCALOGIN_H



