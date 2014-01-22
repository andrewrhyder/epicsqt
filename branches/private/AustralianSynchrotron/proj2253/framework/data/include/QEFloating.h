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

// Floating specific wrapper for QCaObject.

#ifndef QEFLOATING_H
#define QEFLOATING_H

#include <QtDebug>
#include <QVariant>
#include <QCaObject.h>
#include <QEFloatingFormatting.h>
#include <QEPluginLibrary_global.h>

class QEPLUGINLIBRARYSHARED_EXPORT QEFloating : public qcaobject::QCaObject {
    Q_OBJECT

  public:
    QEFloating( QString recordName, QObject *eventObject, QEFloatingFormatting *floatingFormattingIn, unsigned int variableIndexIn );
    QEFloating( QString recordName, QObject *eventObject, QEFloatingFormatting *floatingFormattingIn, unsigned int variableIndexIn, UserMessage* userMessageIn );

  signals:
    void floatingConnectionChanged( QCaConnectionInfo& connectionInfo, const unsigned int &variableIndex );
    void floatingChanged( const double& value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp, const unsigned int& variableIndex );
    void floatingArrayChanged( const QVector<double>& values, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp, const unsigned int& variableIndex );

  public slots:
    void writeFloating( const double &data );
    void writeFloating( const QVector<double> &data );

  private:
    void initialise( QEFloatingFormatting *floatingFormattingIn, unsigned int variableIndexIn );
    QEFloatingFormatting *floatingFormat;
    unsigned int variableIndex;

  private slots:
    void convertVariant( const QVariant &value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp );
    void forwardConnectionChanged( QCaConnectionInfo& connectionInfo);
};

#endif // QEFLOATING_H