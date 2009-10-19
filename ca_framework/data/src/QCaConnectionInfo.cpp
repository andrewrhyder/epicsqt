/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/src/QCaConnectionInfo.cpp $
 * $Revision: #1 $
 * $DateTime: 2009/07/29 08:55:07 $
 * Last checked in by: $Author: rhydera $
 */

/*!
  \class QCaDatetime
  \version $Revision: #1 $
  \date $DateTime: 2009/07/29 08:55:07 $
  \author andrew.rhyder
  \brief CA connection info manager
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
 * Description:
 *
 * This class holds the connection state of a connected variable.
 * It provides methods for checking for channel and link states without
 * an awareness of EPICS channel and link enumerations.
 */

#include <CaConnection.h>
#include <QCaConnectionInfo.h>

/*!
  Construct an empty instance.
  By default the channel and link states are unknown
 */
QCaConnectionInfo::QCaConnectionInfo()
{
    channelState = caconnection::CHANNEL_UNKNOWN;
    linkState = caconnection::LINK_UNKNOWN;
}

/*!
  Construct an instance given a channel and link state
 */
QCaConnectionInfo::QCaConnectionInfo( unsigned short channelStateIn, unsigned short linkStateIn ) {
    channelState = channelStateIn;
    linkState = linkStateIn;
}

/*!
  Return true if the channel is connected
 */
bool QCaConnectionInfo::isChannelConnected() {
    return( channelState == caconnection::CONNECTED );
}

/*!
  Return true if the link is up
 */
bool QCaConnectionInfo::isLinkUp() {
    return( linkState == caconnection::LINK_UP );
}
