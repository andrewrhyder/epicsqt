/*! 
  \class QCaInteger
  \version $Revision: #3 $
  \date $DateTime: 2009/07/28 17:01:05 $
  \author andrew.rhyder
  \brief Integer specific wrapper for QCaObject.
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

#ifndef QCAINTEGER_H
#define QCAINTEGER_H

#include <QtDebug>
#include <QVariant>
#include <QCaObject.h>
#include <QCaIntegerFormatting.h>

class QCaInteger : public qcaobject::QCaObject {
    Q_OBJECT

  public:
    QCaInteger( QString recordName, QObject *eventObject, QCaIntegerFormatting *integerFormattingIn, unsigned int variableIndexIn );
    QCaInteger( QString recordName, QObject *eventObject, QCaIntegerFormatting *integerFormattingIn, unsigned int variableIndexIn, UserMessage* userMessageIn );

  signals:
    void integerChanged( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp, const unsigned int& variableIndex );

  public slots:
    void writeInteger( const long &data );

  private:
    void initialise( QCaIntegerFormatting *integerFormattingIn, unsigned int variableIndexIn );
    QCaIntegerFormatting *integerFormat;
    unsigned int variableIndex;

  private slots:
    void convertVariant( const QVariant &value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp );
};

#endif /// QCAINTEGER_H
