/*! 
  \class QCaSlider
  \version $Revision: #8 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author andrew.rhyder
  \brief CA Slider Widget.
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

#ifndef QCASLIDER_H
#define QCASLIDER_H

#include <QSlider>
#include <QCaWidget.h>
#include <QCaInteger.h>
#include <QCaIntegerFormatting.h>

class QCaSlider : public QSlider, public QCaWidget {
    Q_OBJECT

  public:
    QCaSlider( QWidget *parent = 0 );
    QCaSlider( const QString& variableName = "", QWidget *parent = 0 );

    bool isEnabled() const;
    void setEnabled( bool state );

  protected:
    QCaIntegerFormatting integerFormatting; /// Integer formatting options.
    bool writeOnChangeProperty;             /// Write changed value to database when ever the position changes.
    bool enabledProperty;

    void establishConnection( unsigned int variableIndex );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setValueIfNoFocus( const long& value, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void userValueChanged( const int& newValue );

  public slots:
    void requestEnabled( const bool& state );

  signals:
    void dbValueChanged( const qlonglong& out );

  private:
    void setup();
    void createQcaItem( unsigned int variableIndex );
    void updateToolTip ( const QString & toolTip );

    bool updateInProgress;                  /// Ignore changes during updates, it isn't the user changing the slider.

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;
};

#endif /// QCASLIDER_H
