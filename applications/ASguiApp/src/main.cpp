/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/applications/ASguiApp/src/main.cpp $
 * $Revision: #7 $ 
 * $DateTime: 2009/12/14 16:17:28 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \version $Revision: #7 $
  \date $DateTime: 2009/12/14 16:17:28 $
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
#include <StartupParams.h>
#include <MainContext.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    // Initialise
    mainContext ctx( argc, argv );

    // If this application is already running, let that instance do the work.
    if( ctx.handball() )
        return 0;

    // The application is not running, start a new window
    ctx.newWindow();

    // Start the application main event processing loop
    return ctx.exec();
}


