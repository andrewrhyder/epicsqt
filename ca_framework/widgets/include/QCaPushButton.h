/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/widgets/include/QCaPushButton.h $
 * $Revision: #5 $
 * $DateTime: 2009/07/31 15:55:17 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaPushButton
  \version $Revision: #5 $
  \date $DateTime: 2009/07/31 15:55:17 $
  \author andrew.rhyder
  \brief CA Push Button Widget.
 */

/* Copyright (c) 2009 Australian Synchrotron
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * Licence as published by the Free Software Foundation; either
 * version 2.1 of the Licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public Licence for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * Licence along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Contact details:
 * andrew.rhyder@synchrotron.org.au
 * 800 Blackburn Road, Clayton, Victoria 3168, Australia.
 *
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

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setButtonText( const QString& text, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& );
    void userPressed();
    void userReleased();
    void userClicked();

  public slots:
    void setEnabled( bool state );

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
    void createQcaItem( unsigned int variableIndex  );
    void updateToolTip ( const QString & toolTip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

};

#endif /// QCAPUSHBUTTON_H
