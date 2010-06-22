/*! 
  \class QCaPushButton
  \version $Revision: #8 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author andrew.rhyder
  \brief CA Push Button Widget.
 */
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

#ifndef QCAPUSHBUTTON_H
#define QCAPUSHBUTTON_H

#include <QPushButton>
#include <QCaWidget.h>
#include <QCaString.h>
#include <QCaStringFormatting.h>

class QCaPushButton : public QPushButton, public QCaWidget {
    Q_OBJECT

  public:
    QCaPushButton( QWidget *parent = 0 );
    QCaPushButton( const QString& variableName = "", QWidget *parent = 0 );

    bool isEnabled() const;
    void setEnabled( const bool& state );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setButtonText( const QString& text, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& );
    void userPressed();
    void userReleased();
    void userClicked();

  public slots:
    void requestEnabled( const bool& state );

  signals:
    void dbValueChanged( const QString& out );

  protected:
    QCaStringFormatting stringFormatting;
    bool writeOnPress;
    bool writeOnRelease;
    bool writeOnClick;
    QString releaseText;    /// Text to write on a button release
    QString pressText;      /// Text to write on a button press
    QString clickText;      /// Text to write on a button click
    bool enabledProperty;

    void establishConnection( unsigned int variableIndex );

  private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex  );
    void updateToolTip ( const QString & toolTip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

};

#endif /// QCAPUSHBUTTON_H
