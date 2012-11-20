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

// Manage startup parameters. Parse the startup parameters in a command line, serialize and unserialize parameters when passing them to another application instance.

#include <QStringList>
#include "StartupParams.h"


// Construction
startupParams::startupParams()
{
    enableEdit = false;
    disableMenu = false;
    singleApp = false;
}

// Unserialize application startup parameters
// This must match startupParams::setSharedParams()
void startupParams::getSharedParams( const QByteArray& in )
{
    // Initialise parameters
    filename.clear();
    pathList.clear();
    substitutions.clear();

    // Extract parameters from a stream of bytes.
    int len = 0;
    const char* d = in.constData();

    enableEdit    = (bool)(d[len]);    len += 1;
    disableMenu   = (bool)(d[len]);    len += 1;
    singleApp     = (bool)(d[len]);    len += 1;
    filename.append( &(d[len]) );      len += filename.size()+1;
    int pathCount = d[len];            len += 1;
    for( int i = 0; i < pathCount; i++ )
    {
        pathList.append( &(d[len]) );          len += pathList[i].size()+1;
    }
    substitutions.append( &(d[len]) ); len += substitutions.size()+1;
}

// Serialize application startup parameters
// This must match startupParams::getSharedParams()
void startupParams::setSharedParams( QByteArray& out )
{
    // Convert parameters into a stream of bytes.
    int len = 0;

    out[len++] = enableEdit;
    out[len++] = disableMenu;
    out[len++] = singleApp;
    out.insert( len, filename.toAscii() );       len += filename.size();        out[len++] = '\0';
    out[len++] = pathList.count();
    for( int i = 0; i < pathList.count(); i++ )
    {
        out.insert( len, pathList[i].toAscii() );len += pathList[i].size();   out[len++] = '\0';
    }
    out.insert( len, substitutions.toAscii() );  len += substitutions.size(); out[len++] = '\0';
}


// Extract required parameters from argv and argc
void startupParams::getStartupParams( QStringList args )
{
    // Discard application name
    args.removeFirst();

    // Get switches
    // Switches may be seperate or grouped with the exception that a switch parameter will extend to the end of the argument
    // Examples:
    // -e -p/home
    // -ep/home
    while( args.size() && args[0].left(1) == QString( "-" ) )
    {
        // Get the next argument
        QString arg = args[0];
        args.removeFirst();

        // Remove the leading '-' and process the argument if there is anything left of it
        while( arg.remove(0,1).size() )
        {
            // Identify the argument by the next letter
            switch( arg[0].toAscii() )
            {
                // 'Editable' flag
                case 'e':
                case 'E':
                    enableEdit = true;
                    break;

                // 'Single App' flag
                case 's':
                case 'S':
                    singleApp = true;
                    break;

                        // 'Menu disabled' flag
                case 'm':
                case 'M':
                    disableMenu = true;
                    break;

                // 'path' flag (Remainder of argument is the path)
                case 'p':
                case 'P':
                    // Get the path (everthing after the 'p')
                    {
                        QString pathParam = arg.remove(0,1);
                        pathList = pathParam.split(QRegExp("\\s+"));
                        arg.clear();
                    }
                    break;

                default:
                    // Unrecognised switch
                    break;
            }
        }
    }

    // Get file name if any
    if( args.size() )
    {
        filename = args[0];
        args.removeFirst();
    }

    // Get substitutions if any
    if( args.size() )
    {
        substitutions = args[0];
        args.removeFirst();
    }
}
