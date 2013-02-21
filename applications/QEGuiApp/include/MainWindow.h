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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <ui_MainWindow.h>
#include <QEForm.h>
#include <UserMessage.h>
#include <ContainerProfile.h>
#include <QProcess>
#include <QTimer>

class MainWindow : public QMainWindow, public UserMessage
{
    Q_OBJECT

public:
    MainWindow( QString fileName, bool enableEditIn, bool disableMenuIn, QWidget *parent = 0 );

    ~MainWindow();

private:
    bool enableEdit;                                        // Enable edit menu
    bool disableMenu;                                       // Disable menu bar
    Ui::MainWindowClass ui;                                 // Main window layout
    static QList<QEForm*> guiList;                       // Shared list of all forms being displayed in all main windows
    static QList<MainWindow*> mainWindowList;               // Shared list of all main windows
    bool usingTabs;                                         // True if using tabs to display multiple GUIs, false if displaying a single GUI

    void setSingleMode();                                   // Set up to use only a single gui
    void setTabMode();                                      // Set up to use multiple guis in tabs
    QEForm* createGui( QString filename );               // Create a gui
    void loadGuiIntoCurrentWindow( QEForm* newGui );     // Load a new gui into the current window (either single window, or tab)
    void loadGuiIntoNewTab( QEForm* gui );               // Load a new gui into a new tab
    void launchLocalGui( QString filename );             // Launch a new gui from the 'File' menu

    void setTitle( QString title );                      // Set the main window title

    QTabWidget* getCentralTabs();                           // Return the central widget if it is the tab widget
    QEForm* getCentralGui();                             // Return the central widget if it is a single gui, else return NULL
    QEForm* getCurrentGui();                             // Return the current gui if any (central, or tab)
    void refresh();                                         // Reload the current gui

    void addGuiToWindowsMenu( QEForm* gui );             // Add a gui to the 'windows' menus
    void buildWindowsMenu();                                // Build a new 'windows' menu
    void addWindowMenuAction( QMenu* menu, QEForm* gui );// Add a gui to a 'window' menu
    void removeGuiFromWindowsMenu( QEForm* gui );        // Remove a gui from the 'windows' menus
    void removeAllGuisFromWindowsMenu();                    // Remove all guis on a main window from the 'windows' menus

    QString GuiFileNameDialog( QString caption );           // Get a gui filename from the user
    ContainerProfile profile;                               // Environment profile for new QE widgets

    QProcess process;                                       // Process used to start designer
    QTimer processTimer;                                    // Timer used to attempt restarting designer from outside a QProcess error signal
    void startDesigner();                                   // Start designer (attempt with first command)
    void startDesignerCore( QString command );              // Start designer core called for both start attempts

    bool processSecondAttempt;                              // Flag indicating this is the second attempt to start designer with an alternate command
    bool processOpenGui;                                    // Flag indicating designer should be opened with the current GUI

    QWidget* resizeableGui( QEForm* gui );                  // Given a QEForm, return a widget that will manage being resized.
    QEForm* extractGui( QWidget* rGui );                    // Return a QEForm from a widget that may be a QEForm, or a QScrollArea containg a QEForm

    QSize nativeSize;                                       // Size of gui as defined in .ui file (prior to any resizing)

    QString UILoaderFrameworkVersion;                       // QE framework version used by QUILoader when creating widgets in a form

private:
    void newMessage( QString msg, message_types type );

private slots:
    void on_actionMessage_Log_triggered();
    void on_actionUser_Level_triggered();
    void on_actionRefresh_Current_Form_triggered();             // Slot to perform 'Refresh Current Form' action
    void on_actionOpen_Current_Form_In_Designer_triggered();    // Slot to perform 'Open Current Form In Designer' action
    void on_actionDesigner_triggered();                         // Slot to perform Open Designer' action
    void on_actionNew_Window_triggered();           // Slot to perform 'New Window' action
    void on_actionNew_Tab_triggered();              // Slot to perform 'New Tab' action
    void on_actionOpen_triggered();                 // Slot to perform 'Open' action
    void on_actionClose_triggered();                // Slot to perform 'Close' action
    void onWindowMenuSelection( QAction* action );  // Slot to recieve requests to change focus to a specific gui
    void on_actionAbout_triggered();                // Slot to perform 'About' action

    void launchGui( QString guiName, QEForm::creationOptions creationOption );

    void tabCurrentChanged( int index );            // Slot to act on user changing tabs
    void tabCloseRequest( int index );              // Slot to act on user closing a tab
    void resizeToFitGui();                          // Resize the form to fit a GUI without scroll bars

    void processError( QProcess::ProcessError error ); // An error occured starting designer process
    void startDesignerAlternate();                  // Timer signal used to attempt restarting designer from outside a QProcess error signal
    void on_actionPVProperties_triggered();
    void on_actionStrip_Chart_triggered();
};

#endif // MAINWINDOW_H
