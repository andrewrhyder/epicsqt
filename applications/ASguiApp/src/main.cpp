/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/applications/ASguiApp/src/main.cpp $
 * $Revision: #4 $ 
 * $DateTime: 2009/08/04 10:57:14 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \version $Revision: #4 $
  \date $DateTime: 2009/08/04 10:57:14 $
  \author andrew.rhyder
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

#include <QtGui/QApplication>
#include <MainWindow.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    // Initialise
    QApplication a(argc, argv);
    bool enableEdit = false;
    QString filename;
    QString substitutions;
    QStringList args = QCoreApplication::arguments();

    // Discard application name
    args.removeFirst();

    // Get switches
    while( args.size() && args[0].left(1) == QString( "-" ) )
    {
        QString arg = args[0];
        args.removeFirst();
        while( arg.remove(0,1).size() )
        {
            switch( arg[0].toAscii() )
            {
                case 'e':
                case 'E':
                    enableEdit = true;
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

    // get substitutions if any
    if( args.size() )
    {
        substitutions = args[0];
        args.removeFirst();
    }

    // Create the main window.
    MainWindow w( filename, substitutions, enableEdit );
    w.show();
    return a.exec();
}
