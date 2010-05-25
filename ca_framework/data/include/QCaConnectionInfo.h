/*!
  \class QCaEventFilter
  \version $Revision: #1 $
  \date $DateTime: 2009/07/29 08:55:07 $
  \author andrew.rhyder
  \brief Manage CA connection information
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

#ifndef QCACONNECTIONINFO_H
#define QCACONNECTIONINFO_H

class QCaConnectionInfo
{
public:
    QCaConnectionInfo();
    QCaConnectionInfo( unsigned short channelStateIn, unsigned short linkStateIn );

    bool isChannelConnected();   // Return true if the channel is connected
    bool isLinkUp();             // Return true if the link is up

private:
    unsigned short channelState;      // Channel status
    unsigned short linkState;    // Link status


};

#endif // QCACONNECTIONINFO_H
