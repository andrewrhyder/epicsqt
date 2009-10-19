/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/widgets/include/QCaSlider.h $
 * $Revision: #6 $
 * $DateTime: 2009/07/31 15:55:17 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaSlider
  \version $Revision: #6 $
  \date $DateTime: 2009/07/31 15:55:17 $
  \author andrew.rhyder
  \brief CA Slider Widget.
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
    void setEnabled( bool state );

  private:
    void setup();
    void createQcaItem( unsigned int variableIndex );
    void updateToolTip ( const QString & toolTip );

    bool updateInProgress;                  /// Ignore changes during updates, it isn't the user changing the slider.

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;
};

#endif /// QCASLIDER_H
