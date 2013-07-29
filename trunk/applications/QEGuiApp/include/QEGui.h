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

#ifndef QEGUI_H
#define QEGUI_H

#include <QApplication>
#include <StartupParams.h>
#include <ContainerProfile.h>
#include <MainWindow.h>
#include <loginDialog.h>
#include <recentFile.h>

// Class used to hold information about a GUI in the application's list of GUIs
class guiListItem
{
public:
    guiListItem( QEForm* formIn, MainWindow* mainWindowIn, QAction* actionIn ){ form = formIn; mainWindow = mainWindowIn; action = actionIn; }
    QEForm*     getForm(){ return form; }                               // Return the QEForm implementing the GUI
    MainWindow* getMainWindow(){ return mainWindow; }                   // Return the main window containing the GUI
    void        setScroll( QPoint scrollIn ){ scroll = scrollIn; }      // Set the scroll position of the GUI (saved during configuration restore)
    QPoint      getScroll(){ return scroll; }                           // Get the scroll position of the GUI (used immedietly after a restore has completed)
    QAction*    getAction(){ return action; }

private:
    QEForm* form;               // QEForm implementing the GUI
    MainWindow* mainWindow;     // Main window the GUI is in
    QPoint scroll;              // Scroll position of the GUI (used to hold the scroll position during a configuration restore)
    QAction*    action;                                                 // Action to add to window menus
};

// Class representing the QEGui application
class QEGui : public QApplication, ContainerProfile
{
public:
    QEGui( int& argc, char **argv );            // Construction

    int run();                                  // Main application code including call to exec()

    startupParams* getParams();                 // Get the parsed application startup parameters

    int         getMainWindowCount();                       // Get the number of main windows
    MainWindow* getMainWindow( int i );                     // Get a main window from the application's list of main windows
    int         getMainWindowPosition( MainWindow* mw );    // Locate a main window in the application's list of main windows
    void        addMainWindow( MainWindow* window );        // Add a main window to the application's list of main windows
    void        removeMainWindow( MainWindow* window );     // Remove a main window from the application's list of main windows given a reference to the main window
    void        removeMainWindow( int i );                  // Remove a main window from the application's list of main windows given an index into the application's list of main windows

    int         getGuiCount();                              // Get the total number of GUIs in the application's list of GUIs
    QEForm*     getGuiForm( int i );                        // Get a GUI given an index into the application's list of GUIs
    QAction*    getGuiAction( int i );

    MainWindow* getGuiMainWindow( int i );                  // Get the main window for a GUI given an index into the application's list of GUIs
    QPoint      getGuiScroll( int i );                      // Get the scroll information for a GUI given an index into the application's list of GUIs
    void        setGuiScroll( int i, QPoint scroll );       // Set the scroll information for a GUI given an index into the application's list of GUIs
    void        addGui( QEForm* gui, MainWindow* window );  // Add a GUI to the application's list of GUIs

    void        identifyWindowsAndForms();                  // Ensure all main windows and QEForms managed by this application (top level forms) have a unique identifier

    void        login();                                    // Change user level

    const QList<recentFile*>&  getRecentFiles();            // Return list of recently added files

    void        removeGuiFromWindowsMenus( QEForm* gui );

    void        launchRecentGui( QString path, QStringList pathList, QString macroSubstitutions );

private:
    void printVersion ();                           // Print the version info
    void printUsage (std::ostream & stream);        // Print brief usage statement
    void printHelp ();                              // Print help info

    startupParams params;                           // Parsed startup prarameters
    QList<guiListItem> guiList;                     // List of all forms being displayed in all main windows
    QList<MainWindow*> mainWindowList;              // List of all main windows
    void addGuiToWindowsMenu( QEForm* gui );

    QList<recentFile*> recentFiles;                 // List of recently opened files

    loginDialog* loginForm;                         // Dialog to use when changing user level. Keep one instance to maintain logout history
};

#endif // QEGUI_H
