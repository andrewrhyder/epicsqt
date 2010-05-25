/*! 
  \class QCaString
  \version $Revision: #3 $
  \date $DateTime: 2009/07/28 17:01:05 $
  \author andrew.rhyder
  \brief String wrapper for QCaObject variant data.
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

#ifndef QCASTRING_H
#define QCASTRING_H

#include <QtDebug>
#include <QVariant>
#include <QCaObject.h>
#include <QCaStringFormatting.h>

class QCaString : public qcaobject::QCaObject {
    Q_OBJECT

  public:
    QCaString( QString recordName, QObject *eventObject, QCaStringFormatting *stringFormattingIn, unsigned int variableIndexIn );
    QCaString( QString recordName, QObject *eventObject, QCaStringFormatting *stringFormattingIn, unsigned int variableIndexIn, UserMessage* userMessageIn );

  signals:
    void stringChanged( const QString& value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp, const unsigned int &variableIndex );

  public slots:
    void writeString( const QString &data );

  private:
    void initialise( QCaStringFormatting* newStringFormat, unsigned int variableIndexIn );
    QCaStringFormatting *stringFormat;
    unsigned int variableIndex;

  private slots:
    void convertVariant( const QVariant& value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp );
};

#endif /// QCASTRING_H
