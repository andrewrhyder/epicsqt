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
#include <QEActionRequests.h>
#include <QEForm.h>
#include <UserMessage.h>
#include <ContainerProfile.h>
#include <QProcess>
#include <QTimer>
#include <StartupParams.h>
#include <manageConfigDialog.h>
#include <windowCustomisation.h>
#include <QCloseEvent>


// Save / Restore configuration name
#define QE_CONFIG_NAME "QEGuiConfig"

class QEGui;
class MainWindow;

// Class used to hold information about each GUI in a main window
class guiListItem
{
public:
    guiListItem( QEForm* formIn, MainWindow* mainWindowIn, QAction* actionIn, QString customisationNameIn, bool isDockIn )
                                { form = formIn;
                                  mainWindow = mainWindowIn;
                                  action = actionIn;
                                  customisationName = customisationNameIn;
                                  isDock = isDockIn; }
    QEForm*     getForm(){ return form; }                               // Return the QEForm implementing the GUI
    MainWindow* getMainWindow(){ return mainWindow; }                   // Return the main window containing the GUI
    void        setScroll( QPoint scrollIn ){ scroll = scrollIn; }      // Set the scroll position of the GUI (saved during configuration restore)
    QPoint      getScroll(){ return scroll; }                           // Get the scroll position of the GUI (used immedietly after a restore has completed)
    QAction*    getAction(){ return action; }                           // Get the action to place in windows menus.
    QString     getCustomisationName(){ return customisationName; }     // Get the window customisations name
    bool        getIsDock(){ return isDock; }                           // Get the 'is a dock' flag
    void        deleteAction(){ if( action ){ delete action; } action = NULL; }  // Delete the action

private:
    QEForm*     form;                  // QEForm implementing the GUI
    MainWindow* mainWindow;            // Main window the GUI is in
    QPoint      scroll;                // Scroll position of the GUI (used to hold the scroll position during a configuration restore)
    QAction*    action;                // Action to add to window menus. (Owned by this class)
    QString     customisationName;     // Name of window customisations (menus, tool bar buttons)
    bool        isDock;                // Form has been added as a dock (not as the central widget, or in a tabwidget in the central widget)
};


class MainWindow : public QMainWindow, public UserMessage
{
    Q_OBJECT

public:
    MainWindow( QEGui* appIn, QString fileName, QString customisationName, bool openDialog, QWidget *parent = 0 );

    ~MainWindow();

    void closeAll();                                        // Static function to close all main windows
    void setUniqueId( int restoreId ){ uniqueId = restoreId; } // Set up an ID that will be used during a restore
    int getUniqueId(){ return uniqueId; }

    void buildRecentMenu();
    void addWindowMenuAction( QAction* action );            // Add a gui to a 'window' menu
    void addRecentMenuAction( QAction* action );

    bool showGui( QString guiFileName, QString macroSubstitutions );

private:
    Ui::MainWindowClass ui;                                 // Main window layout
    bool usingTabs;                                         // True if using tabs to display multiple GUIs, false if displaying a single GUI

    void setSingleMode();                                   // Set up to use only a single gui
    void setTabMode();                                      // Set up to use multiple guis in tabs
    QEForm* createGui( QString filename, QString customisationName, bool isDock = false, bool clearExistingCustomisations = false );                  // Create a gui
    QEForm* createGui( QString fileName, QString customisationName, QString restoreId, bool isDock = false, bool clearExistingCustomisations = false ); // Create a gui with an ID (required for a restore)
    void loadGuiIntoCurrentWindow( QEForm* newGui, bool resize );     // Load a new gui into the current window (either single window, or tab)
    void loadGuiIntoNewTab( QEForm* gui );                  // Load a new gui into a new tab
    void loadGuiIntoNewDock( QEForm* gui,
                             bool hidden = false,
                             bool tabbed = false,
                             QEActionRequests::Options createOption = QEActionRequests::OptionFloatingDockWindow,
                             Qt::DockWidgetArea allowedAreas = Qt::AllDockWidgetAreas,
                             QDockWidget::DockWidgetFeature features = QDockWidget::AllDockWidgetFeatures,
                             QRect geom = QRect( 0, 0, 0, 0 ) ); // Load a new gui into a new dock

    MainWindow* launchLocalGui( const QString& filename );  // Launch a new gui from the 'File' menu and gui launch requests.
    MainWindow* launchLocalGui( const QString& filename,    // Launch a new gui from the requestAction slot.
                                const QString& className,
                                const QString& pvName );

    void setTitle( QString title );                         // Set the main window title

    QTabWidget* getCentralTabs();                           // Return the central widget if it is the tab widget
    QEForm* getCentralGui();                                // Return the central widget if it is a single gui, else return NULL
    QEForm* getCurrentGui();                                // Return the current gui if any (central, or tab)
    void refresh();                                         // Reload the current gui

    void buildWindowsMenu();                                // Build a new 'windows' menu

    void removeAllGuisFromGuiList();                    // Remove all guis on a main window from the 'windows' menus


    QString GuiFileNameDialog( QString caption );           // Get a gui filename from the user
    ContainerProfile profile;                               // Environment profile for new QE widgets

    QProcess process;                                       // Process used to start designer
    QTimer processTimer;                                    // Timer used to attempt restarting designer from outside a QProcess error signal
    void startDesigner();                                   // Start designer (attempt with first command)
    void startDesignerCore( QString command );              // Start designer core called for both start attempts

    bool processSecondAttempt;                              // Flag indicating this is the second attempt to start designer with an alternate command
    bool processOpenGui;                                    // Flag indicating designer should be opened with the current GUI

    QWidget* resizeableGui( QEForm* gui, QSize* preferedSize = 0 ); // Given a QEForm, return a widget that will manage being resized, and optinoally the prefered size
    QEForm* extractGui( QWidget* rGui );                    // Return a QEForm from a widget that may be a QEForm, or a QScrollArea containg a QEForm

    QString UILoaderFrameworkVersion;                       // QE framework version used by QUILoader when creating widgets in a form

    int uniqueId;                                           // An ID unique to this window. Used when saving configuration.

    QScrollArea* guiScrollArea( QEForm* gui );              // Return the scroll area a gui is in if it is in one.
    QRect setGeomRect;                                      // Parameter to setGeom() slot (This slot is called from the timer and can't take parameters)

    void raiseGui( QEForm* gui );                           // Raise a gui and select the right tab so the user can see it.

    bool beingDeleted;                                      // This main window is being deleted (deleteLater() has been called on it)
    int waitForX11WindowManagerCount;                       // Number of times setGeom() or scrollTo() has been called waiting for geometry has been finalised

    QEGui* app;                                             // Application reference

    void closeEvent(QCloseEvent *event);                    // Close this window event

    void removeGuiFromGuiList( QEForm* gui );               // Remove a GUI from all window menus (by reference)
    void removeGuiFromGuiList( int i );                     // Remove a GUI from all window menus (by index)
    QString getCustomisationName( QEForm* gui );            // Get the customisation name used with a GUI


private:
    QMenu* tabMenu;                                         // ???We want to keep a reference to certain widget objects. Declaring these directly in the

    void newMessage( QString msg, message_types type );     // Slot to receive a message to present to the user (typically from the QE framework)
    MainWindow* launchGui( QString guiName, QString customisationName, QEActionRequests::Options creationOption, bool hidden );  // Launch a new GUI given a .ui file name
    QMenu* windowMenu;
    QMenu* recentMenu;
    QMenu* editMenu;

    windowCustomisationInfo customisationInfo;  // Current customisation of this window
    void setDefaultCustomisation();             // Set up the initial default customisation
    void setupPlaceholderMenus();               // Get whatever placeholder menus are available from the current customisation and use them (for example, populate a 'Recent' menu if present)
    void identifyWindowAndForms( int mwIndex );


    QDockWidget* getGuiDock( QWidget* gui );    // Determine the dock widget containing a docked GUI

    QList<guiListItem> guiList;

    Qt::DockWidgetArea creationOptionToDockLocation( QEActionRequests::Options createOption ); // Translate a creation option to a dock location.
    QEActionRequests::Options dockLocationToCreationOption( Qt::DockWidgetArea dockLocation ); // Translate a dock location to a creation option.

private slots:
    void on_actionManage_Configurations_triggered();
    void on_actionExit_triggered();                             // Slot to perform 'Exit' action
    void on_actionUser_Level_triggered();                       // Slot to perform 'Refresh Current Form' action
    void on_actionRefresh_Current_Form_triggered();             // Slot to perform 'Refresh Current Form' action
    void on_actionOpen_Current_Form_In_Designer_triggered();    // Slot to perform 'Open Current Form In Designer' action
    void on_actionDesigner_triggered();                         // Slot to perform 'Open Designer' action
    void on_actionNew_Window_triggered();                       // Slot to perform 'New Window' action
    void on_actionNew_Tab_triggered();                          // Slot to perform 'New Tab' action
    void on_actionNew_Dock_triggered();                          // Slot to perform 'New Dock' action
    void on_actionOpen_triggered();                             // Slot to perform 'Open' action
    void on_actionClose_triggered();                            // Slot to perform 'Close' action
    void on_actionAbout_triggered();                            // Slot to perform 'About' action
    void onWindowMenuSelection( QAction* action );              // Slot to receive requests to change focus to a specific gui

    void requestAction( const QEActionRequests & request );     // Slot to receive (new style) requests to launch a new GUI.


    void tabCurrentChanged( int index );               // Slot to act on user changing tabs
    void tabCloseRequest( int index );                 // Slot to act on user closing a tab

    void tabContextMenuRequest( const QPoint & pos );  // Slot for custom tab menu requests
    void tabContextMenuTrigger( QAction * action );    // Slot for custom tab menu actions

    void processError( QProcess::ProcessError error );  // An error occured starting designer process
    void startDesignerAlternate();                      // Timer signal used to attempt restarting designer from outside a QProcess error signal

    void on_actionSave_Configuration_triggered();       // Slot to perform 'Save Configuration' action
    void on_actionRestore_Configuration_triggered();    // Slot to perform 'Save Configuration' action

    void saveRestore( SaveRestoreSignal::saveRestoreOptions option );  // A save or restore has been requested (Probably by QEGui itself)

    void setGeom();                                 // Timer slot to set the window geometry on a restore
    void scrollTo();                                // Timer slot to set the gui scroll positions on a restore
    void on_actionSet_Passwords_triggered();

    void deleteConfigs( manageConfigDialog* mcd, const QStringList names );        // Delete configurations

    void dockComponentDestroyed( QObject* component );  // A component hosted by the application has gone away, delete the dock that was holding it.
};

#endif // MAINWINDOW_H
