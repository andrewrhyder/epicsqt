/*!
  \class
  \version $Revision: #7 $
  \date $DateTime: 2009/12/14 16:17:28 $
  \author andrew.rhyder
  \brief
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


