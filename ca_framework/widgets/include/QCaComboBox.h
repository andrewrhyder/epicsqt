/*! 
  \class QCaComboBox
  \version $Revision: #10 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author andrew.rhyder
  \brief CA Combobox Widget.
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

#ifndef QCAComboBOX_H
#define QCAComboBOX_H

#include <QComboBox>
#include <QCaWidget.h>
#include <QCaInteger.h>
#include <QCaIntegerFormatting.h>
#include <QCaConnectionInfo.h>

class QCaComboBox : public QComboBox, public QCaWidget {
    Q_OBJECT

  public:
    QCaComboBox( QWidget *parent = 0 );
    QCaComboBox( const QString& variableName = "", QWidget *parent = 0 );

    bool isEnabled() const;
    void setEnabled( bool state );

  protected:
    QCaIntegerFormatting integerFormatting;
    bool useDbEnumerationsProperty;
    bool enabledProperty;

    void establishConnection( unsigned int variableIndex );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setValueIfNoFocus( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& );
    void userValueChanged( int value );

  public slots:
    void requestEnabled( const bool& state );

signals:
    void dbValueChanged( const qlonglong& out );

  private:
    void setup();
    void createQcaItem( unsigned int variableIndex );
    void updateToolTip ( const QString & toolTip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;
};

#endif ///QCAComboBOX_H
