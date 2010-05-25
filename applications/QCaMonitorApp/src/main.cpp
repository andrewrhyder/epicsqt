/*!
  \class 
  \version $Revision: #2 $
  \date $DateTime: 2009/07/21 17:58:00 $
  \author andrew.rhyder
  \brief 
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
 *
 * Description:
 *
 * Console application to monitor a set of PVs
 * PV names are supplied as command line arguments.
 *
 */

#include <QtCore/QCoreApplication>
#include <monitor.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Start a monitor for each PV in the argument list
    for( int i = 1; i < argc; i++ )
    {
        new monitor(  QString( argv[i] ) );
    }

    return a.exec();
}
