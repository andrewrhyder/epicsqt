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

#include <QtGui/QApplication>
#include <MainWindow.h>
#include <StartupParams.h>
#include <InstanceManager.h>
#include <QDebug>
#include <iostream>
#include <QEFrameworkVersion.h>

static void printHelp ()
{
   std::cout
         << "QEGui version " << QE_VERSION_STRING << "\n"
         << "\n"
         << "usage: QEGui [-h] [-s] [-e] [-b] [-m macros] [-p pathname] [file_name]\n"
         << "\n"
         << "Options\n"
         << "\n"
         << "-s      Single application.\n"
         << "        QEGui will attempt to pass all parameters to an existing instance of QEGui. When\n"
         << "        one instance of QEGui managing all QEGui windows, all windows will appear in the\n"
         << "        window menu. A typical use is when a QEGui window is started by a button in EDM.\n"
         << "\n"
         << "-e      Enable edit menu option.\n"
         << "        When the edit menu is enabled Designer can be launched from QEGui, typically to\n"
         << "        edit the current GUI.\n"
         << "\n"
         << "-b      Disable menu bar.\n"
         << "\n"
         << "-p      Search paths\n"
         << "        When opening a file, this list of space seperated paths may be used when searching\n"
         << "        for the file.  Refer to File location rules (page 3) for the rules QEGui uses\n"
         << "        when searching for a file\n"
         << "\n"
         << "-m      Macro substitutions applied to GUIs\n"
         << "\n"
         << "        macros format: keyword=substitution, keyword=substitution,...\n"
         << "        Enclose in quotes if this parameter contains any spaces.\n"
         << "\n"
         << "        Typically substitutions are used to specify specific variable names when loading\n"
         << "        generic template forms. Substitutions are not limited to template forms, and some\n"
         << "        QEWidgets use macro substitutions for purposes other than variable names.\n"
         << "\n"
         << "-h      Display help text explaining these options.\n"
         << "\n"
         << "\n"
         << "Parameter\n"
         << "\n"
         << "filename  GUI filename to open.\n"
         << "\n";
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Get the startup parameters from the command line arguments
    startupParams params;
    QStringList args = QCoreApplication::arguments();
    params.getStartupParams( args );

    if (params.printHelp) {
       printHelp ();
       return 0;
    }

    // If only a single instance has been requested,
    // and if there is already another instance of QEGui
    // and it takes the parameters, do no more
    instanceManager instance( &app );
    if( params.singleApp && instance.handball( &params ) )
        return 0;

    // Start the main application window
    instance.newWindow( params );
    return app.exec();
}
