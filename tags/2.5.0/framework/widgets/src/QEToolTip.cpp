/*  QEToolTip.cpp
 *
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

#include "QEToolTip.h"

QEToolTip::QEToolTip(  QWidget* ownerIn )
{
    // Sanity check.
    if( ownerIn == NULL )
    {
        qWarning( "QEToolTip constructor called with a null 'owner'" );
        exit( EXIT_FAILURE );
    }

    // Keep a handle on the underlying QWidget of the QE widgets
    owner = ownerIn;

    isConnected = false;

    variableAsToolTip = true;
}

// Property set: variable as tool tip
void QEToolTip::setVariableAsToolTip( bool variableAsToolTipIn )
{

    // Set the new tool tip type
    variableAsToolTip = variableAsToolTipIn;

    // Update the tool tip to match the new state
    displayToolTip();
}

// Property get: variable as tool tip
bool QEToolTip::getVariableAsToolTip()
{
    return variableAsToolTip;
}

// Update the variable used in the tool tip
// (Used when displaying a dynamic tool tip only)
void QEToolTip::updateToolTipVariable ( const QString& variable ) {
    toolTipVariable = variable;
    displayToolTip();
}

// Update the variable alarm status used in the tool tip
// (Used when displaying a dynamic tool tip only)
void QEToolTip::updateToolTipAlarm ( const QString& alarm )
{
    toolTipAlarm = alarm;
    displayToolTip();
}

// Update the variable custom information used in the tool tip
// (Used when displaying a dynamic tool tip only)
void QEToolTip::updateToolTipCustom ( const QString& custom )
{
    toolTipCustom = custom;
    displayToolTip();
}

// Update the variable connection status used in the tool tip
// (Used when displaying a dynamic tool tip only)
void QEToolTip::updateToolTipConnection ( bool isConnectedIn )
{
    isConnected = isConnectedIn;
    displayToolTip();
}

// Build and display the tool tip from the name and state if dynamic
void QEToolTip::displayToolTip()
{
    // If using the variable name as the tool tip, build the tool tip
    if( variableAsToolTip )
    {
        QString toolTip( toolTipVariable );

        if( toolTipAlarm.size() )
            toolTip.append( " - " ).append( toolTipAlarm );

        if( !isConnected )
            toolTip.append( " - Disconnected" );

        if( toolTipCustom.size() )
           toolTip.append( " - " ).append( toolTipCustom );

        owner->setToolTip( toolTip );
    }
}

