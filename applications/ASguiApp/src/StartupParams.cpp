/*!
  \class startupParams
  \version $Revision: #3 $
  \date $DateTime: 2010/03/01 16:41:59 $
  \author andrew.rhyder
  \brief Manage startp parameters. Parse the startup parameters in a command line, serialize and unserialize parameters when passing them to another application instance.
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
 * Manage startp parameters.
 * Parse the startup parameters in a command line.
 * Serialize and unserialize parameters when passing them to another application instance.
 *
 */

#include <QStringList>
#include "StartupParams.h"


// Construction
startupParams::startupParams()
{
    enableEdit = false;
}

// Unserialize application startup parameters
// They must match startupParams::setSharedParams()
void startupParams::getSharedParams( const void* in )
{
    // Initialise parameters
    filename.clear();
    path.clear();
    substitutions.clear();

    // Extract parameters from a stream of bytes.
    int len = 0;
    const char* d = (char*)in;

    enableEdit    = (bool)(d[len]);    len += 1;
    filename.append( &(d[len]) );      len += filename.size()+1;
    path.append( &(d[len]) );          len += path.size()+1;
    substitutions.append( &(d[len]) ); len += substitutions.size()+1;
}

// Serialize application startup parameters
// The must match startupParams::getSharedParams()
void startupParams::setSharedParams( QByteArray& out )
{
    // Convert parameters into a stream of bytes.
    int len = 0;

    out[len++] = enableEdit;
    out.insert( len, filename.toAscii() );       len += filename.size();        out[len++] = '\0';
    out.insert( len, path.toAscii() );           len += path.size()+1;          out[len++] = '\0';
    out.insert( len, substitutions.toAscii() );  len += substitutions.size()+1; out[len++] = '\0';
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

                // 'path' flag (Remainder of argument is the path)
                case 'p':
                case 'P':
                    // Get the path (everthing after the 'p')
                    path = arg.remove(0,1);
                    arg.clear();
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
