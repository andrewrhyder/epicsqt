/*!
  \class MainWindow
  \version $Revision: #2 $
  \date $DateTime: 2010/01/25 16:09:07 $
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

#ifndef STARTUPPARAMS_H
#define STARTUPPARAMS_H

#include <QByteArray>
#include <QSharedMemory>

#define STARTUPPARAMSSERIALLEN 1000

// Class to manage startup parameters, and in particular sharing them with an other instance of this application
class startupParams
{

  public:
    startupParams();                                // Construction

    void setSharedParams( QByteArray& out );        // Serialise parameters as a serries of bytes
    void getSharedParams( const void* in );         // Extract parameters from a series of bytes

    void getStartupParams( QStringList args );      // Extract startup parameters from command line arguments

    // Startup parameters
    bool enableEdit;                                // Flag true if 'Edit' menu should be available
    QString filename;                               // Default gui file name
    QString path;                                   // Default gui file path
    QString substitutions;                          // Substitutions. For example, "SECTOR=01,PUMP=03"
};


#endif // STARTUPPARAMS_H
