/*! 
  \class QCaLabel
  \version $Revision: #9 $
  \date $DateTime: 2010/02/18 15:15:02 $
  \author andrew.rhyder
  \brief Manage updating tool tip with variable name, alarm state and connected state
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
 *    andrew.rhyder@synchrotron.org
 */

#ifndef QCALABEL_H
#define QCALABEL_H

#include <QLabel>
#include <QCaWidget.h>
#include <QCaString.h>
#include <QCaStringFormatting.h>

class QCaLabel : public QLabel, public QCaWidget {
    Q_OBJECT

  public:
    QCaLabel( QWidget *parent = 0 );
    QCaLabel( const QString &variableName = "", QWidget *parent = 0 );

    bool isEnabled() const;
    void setEnabled( bool state );

  protected:
    QCaStringFormatting stringFormatting;
    bool enabledProperty;

    void establishConnection( unsigned int variableIndex );

    bool visibleProperty;               // Flag true if the widget should be visible outside 'Designer'
    void setVisibleProperty( bool visiblePropertyIn );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setLabelText( const QString& text, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );

  public slots:
    void requestEnabled( const bool& state );

  signals:
    void dbValueChanged( const QString& out );

  private:
    void setup();
    void createQcaItem( unsigned int variableIndex );
    void updateToolTip( const QString& tip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;
};

#endif /// QCALABEL_H
