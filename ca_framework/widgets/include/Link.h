/*! 
  \class Link
  \version $Revision: #4 $
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
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QLINK_H
#define QLINK_H

#include <QLabel>
#include <QVariant>
#include <QString>

class Link : public QLabel {
   Q_OBJECT

  public:
    Link( QWidget *parent = 0 );

    enum conditions { CONDITION_EQ, CONDITION_NE, CONDITION_GT, CONDITION_GE, CONDITION_LT, CONDITION_LE };

  protected:
    conditions conditionProperty;       // Rule for comparing incoming signal
    QVariant comparisonValueProperty;    // Value to compare incoming signal against

    bool signalTrueProperty;            // Flag true if a signal should be emmited if the condition matches
    bool signalFalseProperty;           // Flag true if a signal should be emmited if the condition does not match
    QVariant outTrueValueProperty;      // Value to send in signal if condition matches
    QVariant outFalseValueProperty;     // Value to send in signal if condition does not match

    bool visibleProperty;               // Flag true if the widget should be visible outside 'Designer'
    void setVisibleProperty( bool visiblePropertyIn );

  public slots:
    void in( const bool& in );
    void in( const qlonglong& in );
    void in( const double& in );
    void in( const QString& in );

    void autoFillBackground( const bool& enable );

  signals:
    void out( const bool& out );
    void out( const qlonglong& out );
    void out( const double& out );
    void out( const QString& out );


  private:
    void sendValue( bool match );
    void emitValue( QVariant value );

};

#endif /// QLINK_H
