/*  MainWindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <ui_MainWindow.h>
#include <QEForm.h>
#include <UserMessage.h>
#include <ContainerProfile.h>
#include <QProcess>
#include <QTimer>
#include <StartupParams.h>

// Save / Restore configuration name
#define QE_CONFIG_NAME "QEGuiConfig"

class QEGui;

class MainWindow : public QMainWindow, public UserMessage
{
    Q_OBJECT

public:
    MainWindow( QEGui* appIn, QString fileName, bool openDialog, QWidget *parent = 0 );

    ~MainWindow();

    void closeAll();                                        // Static function to close all main windows

private:
    Ui::MainWindowClass ui;                                 // Main window layout
    bool usingTabs;                                         // True if using tabs to display multiple GUIs, false if displaying a single GUI

    void setSingleMode();                                   // Set up to use only a single gui
    void setTabMode();                                      // Set up to use multiple guis in tabs
    QEForm* createGui( QString filename );                  // Create a gui
    void loadGuiIntoCurrentWindow( QEForm* newGui, bool resize );     // Load a new gui into the current window (either single window, or tab)
    void loadGuiIntoNewTab( QEForm* gui );                  // Load a new gui into a new tab
    void launchLocalGui( QString filename );                // Launch a new gui from the 'File' menu

    void setTitle( QString title );                         // Set the main window title

    QTabWidget* getCentralTabs();                           // Return the central widget if it is the tab widget
    QEForm* getCentralGui();                                // Return the central widget if it is a single gui, else return NULL
    QEForm* getCurrentGui();                                // Return the current gui if any (central, or tab)
    void refresh();                                         // Reload the current gui

    void addGuiToWindowsMenu( QEForm* gui );                // Add a gui to the 'windows' menus
    void buildWindowsMenu();                                // Build a new 'windows' menu
    void addWindowMenuAction( QMenu* menu, QEForm* gui );   // Add a gui to a 'window' menu
    void removeGuiFromWindowsMenu( QEForm* gui );           // Remove a gui from the 'windows' menus
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

    void getUniqueId();                                     // Generate a unique main window ID
    int uniqueId;                                           // An ID unique to this window. Used when saving configuration.
    static int nextUniqueId;                                // Next unique main window ID. (warning. may need to skip IDs in restored windows)

    QScrollArea* guiScrollArea( QEForm* gui );              // Return the scroll area a gui is in if it is in one.
    QRect setGeomRect;                                      // Parameter to setGeom() slot (This slot is called from the timer and can't take parameters)

    void raiseGui( QEForm* gui );

    bool beingDeleted;                                      // This main window is being deleted (deleteLater() has been called on it)
    int scrollToCount;                                      // Number of times scrollTo() has been called waiting for geometry has been rinalised

    QEGui* app;

private:
    void newMessage( QString msg, message_types type );     // Slot to receive a message to present to the user (typically from the QE framework)

private slots:
    void on_actionManage_Configurations_triggered();
    void on_actionExit_triggered();                             // Slot to perform 'Exit' action
    void on_actionMessage_Log_triggered();                      // Slot to perform 'Open Message Log window' action
    void on_actionUser_Level_triggered();                       // Slot to perform 'Refresh Current Form' action
    void on_actionRefresh_Current_Form_triggered();             // Slot to perform 'Refresh Current Form' action
    void on_actionOpen_Current_Form_In_Designer_triggered();    // Slot to perform 'Open Current Form In Designer' action
    void on_actionDesigner_triggered();                         // Slot to perform 'Open Designer' action
    void on_actionNew_Window_triggered();                       // Slot to perform 'New Window' action
    void on_actionNew_Tab_triggered();                          // Slot to perform 'New Tab' action
    void on_actionOpen_triggered();                             // Slot to perform 'Open' action
    void on_actionClose_triggered();                            // Slot to perform 'Close' action
    void on_actionAbout_triggered();                            // Slot to perform 'About' action
    void onWindowMenuSelection( QAction* action );              // Slot to recieve requests to change focus to a specific gui

    void launchGui( QString guiName, QEForm::creationOptions creationOption );  // Slot to recieve requests to launch a new GUI (typically from a QE Button)

    void tabCurrentChanged( int index );               // Slot to act on user changing tabs
    void tabCloseRequest( int index );                 // Slot to act on user closing a tab
    void resizeToFitGui();                             // Resize the form to fit a GUI without scroll bars

    void processError( QProcess::ProcessError error );  // An error occured starting designer process
    void startDesignerAlternate();                      // Timer signal used to attempt restarting designer from outside a QProcess error signal

    void on_actionPVProperties_triggered();             // Slot to perform 'Create PV Properties window' action
    void on_actionStrip_Chart_triggered();              // Slot to perform 'Create strip chart window' action

    void on_actionSave_Configuration_triggered();       // Slot to perform 'Save Configuration' action
    void on_actionRestore_Configuration_triggered();    // Slot to perform 'Save Configuration' action

    void saveRestore( SaveRestoreSignal::saveRestoreOptions option );  // A save or restore has been requested (Probably by QEGui itself)

    void setGeom();                                 // Timer slot to set the window geometry on a restore
    void scrollTo();                                // Timer slot to set the gui scroll positions on a restore
    void on_actionSet_Passwords_triggered();

    void deleteConfigs( const QStringList names );        // Delete configurations

};

#endif // MAINWINDOW_H
