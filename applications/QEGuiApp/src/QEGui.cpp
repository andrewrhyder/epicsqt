/*  QEGui.cpp
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

/*
 This class is used to manage the overall QEGui application.
 Note, each main window is managed by the QEMainWindow class. This class manages anything not common to all main windows.
 */

#include "QEGui.h"
#include <iostream>
#include <QEFrameworkVersion.h>
#include <InstanceManager.h>
#include <QDebug>
#include <saveRestoreManager.h>
#include <QSettings>
#include <QEScaling.h>
#include <QEForm.h>
#include <QMetaType>
#include <QVariant>
#include <QMetaType>

Q_DECLARE_METATYPE( QEForm* )

// Construction
QEGui::QEGui(int& argc, char **argv ) : QApplication( argc, argv )
{
    qRegisterMetaType<QEForm*>( "QEForm*" );   // must also register declared meta types.
    loginForm = NULL;
}

// Run the application
int QEGui::run()
{
    // Get the startup parameters from the command line arguments
    // Just print a message if required (bad parameters, or only or versionversion info required)
    QStringList args = QCoreApplication::arguments();
    bool argsAreOkay = params.getStartupParams( args );

    if (!argsAreOkay)
    {
        printUsage (std::cerr);
        return 1;
    }

    if (params.printHelp)
    {
       printHelp ();
       return 0;
    }

    if (params.printVersion)
    {
       printVersion ();
       return 0;
    }

    // Restore the user level passwords
    QSettings settings( "epicsqt", "QEGui");
    setUserLevelPassword( userLevelTypes::USERLEVEL_USER, settings.value( "userPassword" ).toString() );
    setUserLevelPassword( userLevelTypes::USERLEVEL_SCIENTIST, settings.value( "scientistPassword" ).toString() );
    setUserLevelPassword( userLevelTypes::USERLEVEL_ENGINEER, settings.value( "engineerPassword" ).toString() );

    // Restore recent files
    int i;
    bool ok;
    int recentFilesCount = settings.value( "recentFileCount" ).toInt( &ok );
    if( ok )
    {
        for( i = 0; i < recentFilesCount; i++ )
        {
            QString name = settings.value( QString( "recentFileName%1" ).arg( i )).toString();
            QString path = settings.value( QString( "recentFilePath%1" ).arg( i )).toString();
            QStringList pathList = settings.value( QString( "recentFilePathList%1" ).arg( i )).toStringList();
            QString macroSubstitutions = settings.value( QString( "recentFileMacroSubstitutions%1" ).arg( i )).toString();
            recentFiles.append( new recentFile( name, path, pathList, macroSubstitutions, this ));
        }
    }

    // Prepare to manage save and restore
    // Note, main windows look after them selves, this is for the overall application
    saveRestoreManager saveRestore( this );

    // If only a single instance has been requested,
    // and if there is already another instance of QEGui
    // and it takes the parameters, do no more
    instanceManager instance( this );
    if( params.singleApp && instance.handball( &params ) )
        return 0;

    // Start the main application window
    instance.newWindow( params );
    int ret = exec();

    // Save passwords
    settings.setValue( "userPassword", getUserLevelPassword( userLevelTypes::USERLEVEL_USER ));
    settings.setValue( "scientistPassword", getUserLevelPassword( userLevelTypes::USERLEVEL_SCIENTIST ));
    settings.setValue( "engineerPassword", getUserLevelPassword( userLevelTypes::USERLEVEL_ENGINEER ));

    // Save recent files
    settings.setValue( "recentFileCount", recentFiles.count() );
    for( i = 0; i < recentFiles.count(); i++ )
    {
        settings.setValue( QString( "recentFileName%1" ).arg( i ), recentFiles.at( i )->name );
        settings.setValue( QString( "recentFilePath%1" ).arg( i ), recentFiles.at( i )->path );
        settings.setValue( QString( "recentFilePathList%1" ).arg( i ), recentFiles.at( i )->pathList );
        settings.setValue( QString( "recentFileMacroSubstitutions%1" ).arg( i ), recentFiles.at( i )->macroSubstitutions );
    }

    return ret;
}

// Print version info
void QEGui::printVersion ()
{
   std::cout  << "QEGui version:     " << QE_VERSION_STRING << "  "<< QE_VERSION_DATE_TIME <<  "\n";
   std::cout  << "Framework version: "
              << QEFrameworkVersion::getString().toAscii().data()   << "  "
              << QEFrameworkVersion::getDateTime().toAscii().data() <<  "\n";
}

// Print command line usage
void QEGui::printUsage (std::ostream & stream)
{
    stream  << "usage: qegui [-v] [-h] [-a scale] [-s] [-e] [-b] [-m macros] [-r [configuration_name]] [-c configuration_file] [-p pathname] [file_name] [file_name] [file_name...]\n";
}

// Prinf command line help
void QEGui::printHelp ()
{
   static const char * help_text =
      "Options\n"
      "\n"
      "-a      Adjust the GUIs scaling. This option takes a single value with is the percentage\n"
      "        scaling to be applies to each GUI. The value may be either an integer or a floating\n"
      "        point number. If specified its value will be constrained to the range 40 to 400.\n"
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
      "-r      Restore from saved configuration.\n"
      "        If a configuration name is not provided the saved default configuration is used\n"
      "        if available.\n"
      "        Note, a single configuration file may contain multiple named configurations as well\n"
      "        as a default configuration."
      "\n"
      "-c      Configuration file.\n"
      "        Named configurations will be saved to and read from this file. If not provided\n"
      "        the default is QEGuiConfig.xml in the current working directory.\n"
      "\n"
      "-p      Search paths\n"
      "        When opening a file, this list of space seperated paths may be used when searching\n"
      "        for the file. Paths may end with ... to search all sub directories of the path.\n"
      "        Refer to 'File location rules' for the rules QEGui uses when searching for a file.\n"
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
      "Parameters\n"
      "\n"
      "filenames  GUI filenames to open. Each filename is a separate parameter.\n"
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
      "The internal resource file defines fields for all the records from base-3-14-11, most of\n"
      "the records from the synApps distribution, together with the AS developed concat record,\n"
      "i.e. the following record types:\n"
      "   ai          ao          aSub        asyn        bi          \n"
      "   bo          busy        calc        calcout     camac       \n"
      "   compress    concat      dfanout     dxp         epid        \n"
      "   er          erevent     event       fanout      genSub      \n"
      "   histogram   longin      longout     mbbi        mbbiDirect  \n"
      "   mbbo        mbboDirect  mca         motor       permissive  \n"
      "   sCalcout    scaler      scanparm    sel         seq         \n"
      "   sscan       sseq        state       status      stringin    \n"
      "   stringout   subArray    sub         swait       table       \n"
      "   timestamp   transform   vme         waveform    \n"
      "\n"
      "The file specified by the QE_RECORD_FIELD_LIST variable can be used to define additional\n"
      "record types and/or completely replace the specification of a specified record type. It \n"
      "cannot be used to define extra fields for an existing record type.\n"
      "\n";

   printVersion ();
   std::cout << "\n";
   printUsage (std::cout);
   std::cout << "\n";
   std::cout << help_text;
}

// Get the application's startup parameters
startupParams* QEGui::getParams()
{
    return &params;
}

// Get the number of main windows
int QEGui::getMainWindowCount()
{
    return mainWindowList.count();
}

// Get a main window from the application's list of main windows
MainWindow* QEGui::getMainWindow( int i )
{
    return mainWindowList.at( i );
}

// Locate a main window in the application's list of main windows
int QEGui::getMainWindowPosition( MainWindow* mw )
{
    for( int i = 0; i < mainWindowList.count(); i++ )
    {
        if( mainWindowList[i] == mw )
            return i;
    }

    // Should never get here
    return 0;
}

// Add a main window to the application's list of main windows
void QEGui::addMainWindow( MainWindow* window )
{
    mainWindowList.append( window );
}

// Remove a main window from the application's list of main windows given a reference to the main window
void QEGui::removeMainWindow( MainWindow* window )
{
    // Remove this main window from the global list of main windows
    // Note, this may have already been done to hide the the main window if deleting using deleteLater()
    for( int i = 0; i < mainWindowList.size(); ++i )
    {
        if( mainWindowList[i] == window )
        {
            mainWindowList.removeAt( i );
            break;
        }
    }
}

// Remove a main window from the application's list of main windows given an index into the application's list of main windows
void QEGui::removeMainWindow( int i )
{
    mainWindowList.removeAt( i );
}

// Return list of recently added files
const QList<recentFile*>&  QEGui::getRecentFiles()
{
    return recentFiles;
}

// Get the total number of GUIs in the application's list of GUIs
// (This includes all GUIs in all main windows)
int QEGui::getGuiCount()
{
    return guiList.count();
}

// Get the action used to raise a gui in the 'Windows' menus
QAction* QEGui::getGuiAction( int i )
{
    return guiList[i].getAction();
}

// Get a GUI given an index into the application's list of GUIs
QEForm* QEGui::getGuiForm( int i )
{
    return guiList[i].getForm();
}

// Get the main window for a GUI given an index into the application's list of GUIs
MainWindow* QEGui::getGuiMainWindow( int i )
{
    return guiList[i].getMainWindow();
}

// Get the scroll information for a GUI given an index into the application's list of GUIs
QPoint QEGui::getGuiScroll( int i )
{
    return guiList[i].getScroll();
}

// Set the scroll information for a GUI given an index into the application's list of GUIs
void QEGui::setGuiScroll( int i, QPoint scroll )
{
    guiList[i].setScroll( scroll );
}

// Add a GUI to the application's list of GUIs, and to the recent menu
void QEGui::addGui( QEForm* gui, MainWindow* window )
{
    // Create an action for the 'Window' menus
    QAction* windowMenuAction = new QAction( gui->getQEGuiTitle(), this );
    windowMenuAction->setData( qVariantFromValue( gui ) );

    // Add this gui to the application wide list of guis
    guiList.append( guiListItem( gui, window, windowMenuAction ) );

    // For each main window, add a new action to the window menu
    for( int i = 0; i < mainWindowList.count(); i++ )
    {
        mainWindowList.at(i)->addWindowMenuAction( windowMenuAction );
    }

    // Note the GUI title and full file path
    QString name = gui->getQEGuiTitle();
    QString path = gui->getFullFileName();

    // Assume there is no 'Recent' action
    QAction* recentMenuAction = NULL;

    // Look for the gui in the recent files list
    for( int i = 0; i < recentFiles.count(); i++ )
    {
        // If already in the list, promote the found entry to the top of the list
        if( name == recentFiles[i]->name && path == recentFiles[i]->path )
        {
            // Note the found action
            recentMenuAction = recentFiles[i];

            // Promote the action to the top of all the menus it is in
            QList<QWidget*> assocWidgets = recentMenuAction->associatedWidgets();
            for( int j = 0; j < assocWidgets.count(); j++ )
            {
                QWidget* menu = assocWidgets[j];
                menu->removeAction( recentMenuAction );
                QAction* beforeAction = 0;
                if( menu->actions().count() )
                {
                    beforeAction = menu->actions().at(0);
                }
                menu->insertAction( beforeAction, recentMenuAction );
            }

            // Promote the recent file info in the recent file list
            recentFiles.prepend( recentFiles.takeAt( i ) );

            break;
        }
    }

    // If the current gui was not found in the recent file list, add it
    if( !recentMenuAction )
    {
        // Add a new recent gui
        recentFile* rf = new recentFile( name, path, gui->getPathList(), gui->getMacroSubstitutions(), this );
        recentFiles.prepend( rf );

        // Keep the list down to a limited size
        if( recentFiles.count() > 10 )
        {
            // Deleting the action will remove it from all the menus it is in
            delete( recentFiles.takeLast() );
        }

        // For each main window, add the recent file
        for( int i = 0; i < mainWindowList.count(); i++ )
        {
            mainWindowList[i]->addRecentMenuAction( rf );
        }
    }
}

// Remove a GUI from the application's list of GUIs
// Note, deleting the action will remove it from all the menus it was associated with
void QEGui::removeGuiFromWindowsMenus( QEForm* gui )
{
    for( int i = 0; i < guiList.count(); i++ )
    {
        if( guiList[i].getForm() == gui )
        {
            // Delete the action. This will remove it from all the menus it was associated with
            delete guiList[i].getAction();//!! dangerous do this in guiListItem????
            guiList.removeAt( i );
            break;
        }
    }
}

// Ensure all main windows and QEForms managed by this application (top level forms) have a unique identifier
void QEGui::identifyWindowsAndForms()
{
    // Give all main windows a unique ID for restoration purposes
    for( int i = 0; i < mainWindowList.count(); i++ )
    {
        mainWindowList[i]->setUniqueId( i );
    }

    // Give all top level QEForms (managed by this application - not sub forms) a unique ID for restoration purposes
    for( int i = 0; i < guiList.count(); i++ )
    {
        // Get form and main window details
        guiListItem* guiItem = &guiList[i];
        MainWindow* mw = guiItem->getMainWindow();

        QString name = QString("QEGui_window_%1_form_%2" ).arg( mw->getUniqueId() ).arg( i );
        guiItem->getForm()->setUniqueIdentifier( name );
    }
}

// Change user level
void QEGui::login( QWidget* fromForm )
{
    if( !loginForm )
    {
        loginForm = new loginDialog;
        // Ensure scaling is consistent with the rest of the application's forms.
        QEScaling::applyToWidget( loginForm );
    }

    loginForm->exec( fromForm );
}

// Launch a gui for the 'Recent...' menu
void QEGui::launchRecentGui( QString path, QStringList pathList, QString macroSubstitutions )
{
    // Set up the profile for the new window
    ContainerProfile profile;

    profile.setupProfile( NULL, pathList, "", macroSubstitutions );

    MainWindow* mw = new MainWindow( this, path, false );
    mw->show();
    profile.releaseProfile();
}

// end
