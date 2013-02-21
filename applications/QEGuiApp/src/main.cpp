/*  main.cpp
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

#include <QtGui/QApplication>
#include <MainWindow.h>
#include <StartupParams.h>
#include <InstanceManager.h>
#include <QDebug>
#include <iostream>
#include <QEFrameworkVersion.h>

//------------------------------------------------------------------------------
//
static void printVersion ()
{
   std::cout  << "QEGui version  " << QE_VERSION_STRING << " ("<< QE_VERSION_DATE_TIME <<  ")\n";
   std::cout  << "Plugin version "
              << QEFrameworkVersion::getString().toAscii().data()  << " ("
              << QEFrameworkVersion::getDateTime().toAscii().data() <<  ")\n";
}

//------------------------------------------------------------------------------
//
static void printUsage ()
{
    std::cout  << "usage: QEGui [-v] [-h] [-s] [-e] [-b] [-m macros] [-p pathname] [file_name]\n";
}

//------------------------------------------------------------------------------
//
static void printHelp ()
{
   static const char * help_text =
      "Options\n"
      "\n"
      "-s      Single application.\n"
      "        QEGui will attempt to pass all parameters to an existing instance of QEGui. When\n"
      "        one instance of QEGui managing all QEGui windows, all windows will appear in the\n"
      "        window menu. A typical use is when a QEGui window is started by a button in EDM.\n"
      "\n"
      "-e      Enable edit menu option.\n"
      "        When the edit menu is enabled Designer can be launched from QEGui, typically to\n"
      "        edit the current GUI.\n"
      "\n"
      "-b      Disable menu bar.\n"
      "\n"
      "-p      Search paths\n"
      "        When opening a file, this list of space seperated paths may be used when searching\n"
      "        for the file.  Refer to File location rules (page 3) for the rules QEGui uses\n"
      "        when searching for a file\n"
      "\n"
      "-m      Macro substitutions applied to GUIs\n"
      "\n"
      "        macros format: keyword=substitution, keyword=substitution,...\n"
      "        Enclose in quotes if this parameter contains any spaces.\n"
      "\n"
      "        Typically substitutions are used to specify specific variable names when loading\n"
      "        generic template forms. Substitutions are not limited to template forms, and some\n"
      "        QEWidgets use macro substitutions for purposes other than variable names.\n"
      "\n"
      "-h      Display help text explaining these options and exit.\n"
      "\n"
      "-v      Display version info and exit.\n"
      "\n"
      "\n"
      "Parameter\n"
      "\n"
      "filename  GUI filename to open.\n"
      "\n"
      "\n"
      "Environment variables\n"
      "\n"
      "As well as the standard Channel Access (client) environment variables, the QE framework\n"
      "and hence the QEGui program's behaviour may be modified by setting a number of QE\n"
      "environment variables.\n"
      "\n"
      "\n"
      "QE_ARCHIVE_LIST - specifies a list of channel archive servers. The format of this\n"
      "variable is a space separated set of one or more archive severs. Each server is specified\n"
      "as a hostname:port/endpoint triplet, e.g.:\n"
      "\n"
      "  CR01ARC01:80/cgi-bin/ArchiveDataServer.cgi CR01ARC02:80/cgi-bin/ArchiveDataServer.cgi\n"
      "\n"
      "A leading http:// for each triplet is neither required nor permitted.\n"
      "\n"
      "\n"
      "QE_ARCHIVE_PATTERN - This variable specifies the pattern match applied when the framework\n"
      "retrieves sets of channel names from the archiver. The pattern is a regular expression.\n"
      "If undefined/empty then all names are retrived.\n"
      "\n"
      "\n"
      "QE_RECORD_FIELD_LIST - This variable specifies a file that defines the set of field\n"
      "names associated with each record type, as used by the PV Propeties widget. If specified,\n"
      "the data will be merged with the internal resoure file built into the framework.\n"
      "The format of the file is a simple ASCII file consisting of:\n"
      "\n"
      "   # example             -- comment lines - ignored\n"
      "                         -- blank lines - ignored\n"
      "   <<recordtype>>        -- introduce record type, e.g. <<ai>>\n"
      "   field_name            -- field name, e.g. DESC\n"
      "   field_name            -- field name, e.g. SCAN\n"
      "\n"
      "All field names are associated with the preceeding record type.\n"
      "\n"
      "The internal resoure file defines fields for the following record types:\n"
      "   ai          ao          aSub        asyn        bi          \n"
      "   bo          busy        calc        calcout     camac       \n"
      "   compress    concat      dfanout     dxp         epid        \n"
      "   er          erevent     event       fanout      filter      \n"
      "   genSub      histogram   longin      longout     mbbiDirect  \n"
      "   mbbi        mbboDirect  mbbo        mca         motor       \n"
      "   permissive  sCalcout    scaler      scanparm    sel         \n"
      "   seq         sscan       sseq        state       status      \n"
      "   stringin    stringout   subArray    sub         swait       \n"
      "   table       timestamp   transform   vme         waveform    \n"
      "\n"
      "The file specified by the QE_RECORD_FIELD_LIST variable can be used to define additional\n"
      "record types and/or completely replace exiting record types. It cannot be used to define\n"
      "extra fields for an existing record type.\n"
      "\n";
  
   printVersion ();
   std::cout << "\n";
   printUsage ();
   std::cout << "\n";
   std::cout << help_text;
}


//------------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Get the startup parameters from the command line arguments
    startupParams params;
    QStringList args = QCoreApplication::arguments();
    bool argsAreOkay = params.getStartupParams( args );

    if (!argsAreOkay) {
        printUsage ();
        return 1;
    }

    if (params.printHelp) {
       printHelp ();
       return 0;
    }

    if (params.printVersion) {
       printVersion ();
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
