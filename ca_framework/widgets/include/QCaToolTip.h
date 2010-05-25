/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/widgets/include/QCaToolTip.h $
 * $Revision: #3 $
 * $DateTime: 2010/05/03 16:39:12 $
 * Last checked in by: $Author: rhydera $
 */

/*!
  \class QCaToolTip
  \version $Revision: #3 $
  \date $DateTime: 2010/05/03 16:39:12 $
  \author andrew.rhyder
  \brief Manage updating tool tip with variable name, alarm state and connected state
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
#ifndef QCATOOLTIP_H
#define QCATOOLTIP_H

#include <QCaString.h>

class QCaToolTip
{
    public:
        QCaToolTip();
        virtual ~QCaToolTip(){};
        void updateToolTipVariable ( const QString& variable );
        void updateToolTipAlarm ( const QString& alarm );
        void updateToolTipConnection ( bool connection );
        virtual void updateToolTip( const QString& tip ) = 0;

    private:
        void displayToolTip();
        QString toolTipVariable;
        QString toolTipAlarm;
        bool isConnected;
};

#endif // QCATOOLTIP_H
