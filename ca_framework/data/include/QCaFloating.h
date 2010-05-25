/*! 
  \class QCaFloating
  \version $Revision: #1 $
  \date $DateTime: 2009/11/23 08:44:03 $
  \author andrew.rhyder
  \brief Floating specific wrapper for QCaObject.
 */
/*
 *  This file is part of the EPICS QT Framework.
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
 *    andrew@rhyder.org
 */

#ifndef QCAFLOATING_H
#define QCAFLOATING_H

#include <QtDebug>
#include <QVariant>
#include <QCaObject.h>
#include <QCaFloatingFormatting.h>

class QCaFloating : public qcaobject::QCaObject {
    Q_OBJECT

  public:
    QCaFloating( QString recordName, QObject *eventObject, QCaFloatingFormatting *floatingFormattingIn, unsigned int variableIndexIn );
    QCaFloating( QString recordName, QObject *eventObject, QCaFloatingFormatting *floatingFormattingIn, unsigned int variableIndexIn, UserMessage* userMessageIn );

  signals:
    void floatingChanged( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp, const unsigned int& variableIndex );

  public slots:
    void writeFloating( const long &data );

  private:
    void initialise( QCaFloatingFormatting *floatingFormattingIn, unsigned int variableIndexIn );
    QCaFloatingFormatting *floatingFormat;
    unsigned int variableIndex;

  private slots:
    void convertVariant( const QVariant &value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp );
};

#endif /// QCAFLOATING_H
