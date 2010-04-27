/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/include/QCaFloating.h $
 * $Revision: #1 $
 * $DateTime: 2009/11/23 08:44:03 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaFloating
  \version $Revision: #1 $
  \date $DateTime: 2009/11/23 08:44:03 $
  \author andrew.rhyder
  \brief Floating specific wrapper for QCaObject.
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
