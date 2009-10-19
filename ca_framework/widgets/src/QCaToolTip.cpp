/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/widgets/src/QCaToolTip.cpp $
 * $Revision: #1 $
 * $DateTime: 2009/07/30 14:33:44 $
 * Last checked in by: $Author: rhydera $
 */

/*!
  \class QCaLabel
  \version $Revision: #1 $
  \date $DateTime: 2009/07/30 14:33:44 $
  \author andrew.rhyder
  \brief CA Label Widget.
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

#include "QCaToolTip.h"

QCaToolTip::QCaToolTip()
{
    isConnected = false;
}

/*!
    Update the tool tip as requested by QCaWidget.
*/
void QCaToolTip::updateToolTipVariable ( const QString& variable ) {
    toolTipVariable = variable;
    displayToolTip();
}

void QCaToolTip::updateToolTipAlarm ( const QString& alarm )
{
    toolTipAlarm = alarm;
    displayToolTip();
}

void QCaToolTip::updateToolTipConnection ( bool isConnectedIn )
{
    isConnected = isConnectedIn;
    displayToolTip();
}

/*!
   Build and display the tool tip from the name and state
*/
void QCaToolTip::displayToolTip() {

    QString toolTip( toolTipVariable );

    if( toolTipAlarm.size() )
        toolTip.append( " - " ).append( toolTipAlarm );

    if( !isConnected )
        toolTip.append( " - Disconnected" );

    updateToolTip( toolTip );
}

