/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/include/QCaDateTime.h $
 * $Revision: #1 $
 * $DateTime: 2009/07/28 17:01:05 $
 * Last checked in by: $Author: rhydera $
 */

/*!
  \class QCaEventFilter
  \version $Revision: #1 $
  \date $DateTime: 2009/07/28 17:01:05 $
  \author andrew.rhyder
  \brief CA Date Time manager
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

#ifndef QCADATETIME_H
#define QCADATETIME_H

#include <QDateTime>

class QCaDateTime : public QDateTime
{
public:
    QCaDateTime();
    QCaDateTime( QDateTime dt );
    void operator=( QCaDateTime& other );
    QCaDateTime( unsigned long seconds, unsigned long nanoseconds );

    QString text();

    unsigned long nSec;
};

#endif // QCADATETIME_H
