/*  MainWindow.cpp
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
 Window and GUI construction paths

        Core GUI construction should always be performed by MainWindow::createGui(). This function is used when:
         - creating a GUI in a main window
         - creating a GUI in a new tab in a main window
         - replacing a GUI
         - Opening a GUI in a dock
         - etc


          MainWindow::createGui()
            manage form IDs
            create a new QEForm
            manage restoration
            manage scaling
            read ui
            add gui to application's list of windows


        A new GUI may be created by creating a new Main Window (based on a QMainWindow):

          MainWindow::MainWindow()
            manage form ID, form filter, source filter
            manage scaling
            add to the window selection menu
            connect to the persistance manager
            manage the application's window list
            set the title
            update the recent file menu
            set up the menu bar (enable edit, etc)
            present the 'Open' dialog if required
            call MainWindow::createGui()
            perform restore if required


        A new GUI may be created by calling the slot supporting the 'File...' -> 'New Tab' menu item
          MainWindow::on_actionNew_Tab_triggered()
            call MainWindow::createGui()
            put gui in new tab

        A new GUI may be created by calling the slot supporting the 'File...' -> 'New Dock' menu item
          MainWindow::on_actionNew_Dock_triggered()
            call MainWindow::createGui()
            put gui in new dock


        A new GUI may be created by calling the slot supporting the 'File...' -> 'Open' menu item

          MainWindow::on_actionOpen_triggered()
            call MainWindow::createGui()
            put gui in current window


        A new GUI may be created by calling the slot supporting the 'Edit...' -> 'Refresh Current Form' menu item

          MainWindow::on_actionRefresh_Current_Form_triggered()
            call MainWindow::createGui()
            put gui in current window


        A new GUI may be created by calling when performing a 'restore' on a main window

          MainWindow::saveRestore()
            call MainWindow::createGui()


        A new main window may be created by the slot supporting the 'File...' -> 'New Window' menu item

          MainWindow::on_actionNew_Window_triggered()
            call MainWindow::MainWindow()


        A new window may be created by the slots connected to actions for displaying inbuilt forms

          MainWindow::on_actionPVProperties_triggered()
          MainWindow::on_actionStrip_Chart_triggered()
          MainWindow::on_actionMessage_Log_triggered()
          MainWindow::on_actionPlotter_triggered()
          MainWindow::on_actionScratch_Pad_triggered()
          MainWindow::on_actionArchive_Status_triggered()
            call MainWindow::launchLocalGui()
              call MainWindow::MainWindow()


        A new window may be created by the 'Detatch Tab' in the main window's context menu:

          MainWindow::tabContextMenuTrigger()
            get details from appropriate tabbed GUI
            close tabbed GUI
            call MainWindow::MainWindow()


        A new window, or a new GUI in an existing main window, may be created by the slot for
        requesting new GUIs (signaled from QE Buttons and other QE widgets, and custom menu items)

          MainWindow::requestAction()
            either
              call MainWindow::launchGui()
                standardise path name
                search for existing open .ui file (display it if found)
                either
                  call MainWindow::createGui()
                  put gui in current main window
                or
                  call MainWindow::createGui()
                  put gui in new tab
                or
                  call MainWindow::MainWindow( app, "", "", true );
            or
              call MainWindow::launchLocalGui()
                call MainWindow::MainWindow()

 */

#include <QtGui>
#include <QDebug>
#include <QString>
#include <QUiLoader>

#include <MainWindow.h>
#include <QEForm.h>
#include <QEFrameworkVersion.h>
#include <QECommon.h>
#include <QEScaling.h>
#include <QMessageBox>
#include <ContainerProfile.h>
#include <QVariant>
#include <saveDialog.h>
#include <restoreDialog.h>
#include <PasswordDialog.h>
#include <QEGui.h>
#include <aboutDialog.h>
#include <DetectorEditor.h>

// Before Qt 4.8, the command to start designer is 'designer'.
// Qt 4.8 later uses the command 'designer-qt4'
// Try both before giving up starting designer
#define DESIGNER_COMMAND_1 "designer-qt4"
#define DESIGNER_COMMAND_2 "designer"
Q_DECLARE_METATYPE( QEForm* )

//=================================================================================
// Methods for construction, destruction, initialisation
//=================================================================================

// Constructor
// A profile should have been defined before calling this constructor
MainWindow::MainWindow(  QEGui* appIn, QString fileName, QString customisationName, bool openDialog, QString title, QWidget *parent )  : QMainWindow( parent ), forceDockSize(false)
{
    app = appIn;

    tabMenu = NULL;
    windowMenu = NULL;
    recentMenu = NULL;
    editMenu = NULL;
    windowCustomisationName = customisationName;

    // A published profile should always be available, but the various signal consumers will always be either NULL (if the
    // profile was set up by the QEGui application) or objects in another main window (if the profile was published by a button in a gui)
    // Replace the signal consuming objects
    profile.updateConsumers( this );

    // Initialise
    usingTabs = false;

    beingDeleted = false;
    waitForX11WindowManagerCount = 0;

    // Give the main window's UserMessage class a unique form ID so only messages from
    // the form in each main window are displayed that main window's status bar
    setFormId( getNextMessageFormId() );
    setFormFilter( MESSAGE_FILTER_MATCH );
    setSourceFilter( MESSAGE_FILTER_NONE );

    // Present the main form's ui
    ui.setupUi( this );

    // Apply scaling to main window proper.
    //
    QEScaling::applyToWidget( this );

    // Setup to respond to requests to save or restore persistant data
    PersistanceManager* persistanceManager = profile.getPersistanceManager();
    QObject::connect( persistanceManager->getSaveRestoreObject(), SIGNAL( saveRestore( SaveRestoreSignal::saveRestoreOptions ) ), this, SLOT( saveRestore( SaveRestoreSignal::saveRestoreOptions ) ), Qt::DirectConnection );

    // Save this instance of a main window in the global list of main windows
    app->addMainWindow( this );

    if (fileName.isEmpty()){
        // Load any required window customisation
        applyCustomisation( customisationName, &customisationInfo, false );
        tabifyDocks();
    }

    // Set the defined title
    setTitle( title );

    // Hide the main tool bar (nothing in it yet)
    ui.mainToolBar->hide();

    // Enable the menu bar as required
    menuBar()->setVisible( !app->getParams()->disableMenu );

    // If no filename was supplied, and an 'Open...' dialog is required, open the file selection dialog
    // Do it after the creation of the main window is complete
    if( fileName.isEmpty() )
    {
        if (!profile.getPersistanceManager()->isRestoring())
            setDefaultCustomisation();
        if (openDialog)
            QTimer::singleShot( 0, this, SLOT(on_actionOpen_triggered()));
    }

    // If a filename was supplied, load it
    // This may also load customisations. These customisations may include built in menus like 'Windows' and 'Recent'
    else
    {
        QEForm* gui = createGui( fileName, customisationName ); // A profile should have been published before calling this constructor.
        loadGuiIntoCurrentWindow( gui, true );
        if (fileName == "RealEmpty.ui") centralWidget()->hide();
    }

    // Setup to allow user to change focus to a window from the 'Windows' menu
    if( windowMenu )
    {
        QObject::connect( windowMenu, SIGNAL( triggered( QAction* ) ), this, SLOT( onWindowMenuSelection( QAction* ) ) );
    }

    // Enable the edit menu if requested
    if( editMenu )
    {
        editMenu->setEnabled( app->getParams()->enableEdit  );
    }

    // Set up signals for starting the 'designer' process
    QObject::connect( &process, SIGNAL(error(QProcess::ProcessError)), this, SLOT( processError(QProcess::ProcessError) ) );
    QObject::connect( &processTimer, SIGNAL(timeout()), this, SLOT( startDesignerAlternate() ) );

    // Ensure this class destructor gets called.
    setAttribute ( Qt::WA_DeleteOnClose );

    // Setup the main window icon
    setWindowIcon( QIcon (":/qe/gui/icons/QEGuiIcon.png" ));

    // Ensure no widget in the loaded gui has focus (and therefore will not update)
    setFocus();

    // Restore (will only do anything if this main window is being created during a restore)
    saveRestore( SaveRestoreSignal::RESTORE_APPLICATION );

    // Set up request action to form name and clss name maps.
    createActionMaps();
}

// Destructor
MainWindow::~MainWindow()
{
    // Remove the GUIs shown in this main window from the GUIs listed in the 'Windows' menus of all other main windows
    removeAllGuisFromGuiList();

    // Remove this main window from the global list of main windows
    // Note, this may have already been done to hide the the main window if deleting using deleteLater()
    app->removeMainWindow( this );
    customisationInfo.clear();
}

// Set up the initial default customisation
// Used when first creating a main window, or after closing a GUI (the customisations for the GUI just closed will no longer apply)
void MainWindow::setDefaultCustomisation()
{
    QString defaultCustomisation = app->getParams()->customisationName;
    if( defaultCustomisation.isEmpty() )
    {
        defaultCustomisation = "QEGui_Default";
    }

    // Apply any required window customisations
    windowCustomisationName = defaultCustomisation;
    applyCustomisation( defaultCustomisation, &customisationInfo, true );
    setupPlaceholderMenus();
}

// Get whatever placeholder menus are available from the current customisation and use them
// (for example, populate a 'Recent' menu if present)
void MainWindow::setupPlaceholderMenus()
{
    if( windowMenu )
    {
        windowMenu->clear();
    }
    if( recentMenu )
    {
        recentMenu->clear();
    }
    windowMenu = customisationInfo.placeholderMenus.value( "Windows", NULL );
    recentMenu = customisationInfo.placeholderMenus.value( "Recent", NULL );
    editMenu = customisationInfo.placeholderMenus.value( "Edit", NULL );

    // Populate the 'Windows' menu to include all current guis in any main window
    buildWindowsMenu();

    // Populate the 'Recent...' menu to include recent guis
    buildRecentMenu();
}

//=================================================================================
// Methods for responding to user actions
//=================================================================================

// Open a gui in a new window.
// Present a file open dialog box and after generate the gui based on the ui file the user selects
void MainWindow::on_actionNew_Window_triggered()
{
    profile.publishOwnProfile();
    MainWindow* w = new MainWindow( app, "", app->getParams()->customisationName, true );
    profile.releaseProfile();
    w->show();
}

// Open a gui in a new tab.
// Present a file open dialog box and after generate the gui based on the ui file the user selects
void MainWindow::on_actionNew_Tab_triggered()
{
    // Create the GUI
    profile.publishOwnProfile();
    QEForm* gui = createGui( GuiFileNameDialog( "Open" ), app->getParams()->customisationName );
    profile.releaseProfile();

    // If a GUI was created, ensure tab mode is in effect and loadf the GUI into a new tab
    if( gui )
    {

        // If not using tabs, start tabs and migrate any single gui to the first tab
        if( !usingTabs )
            setTabMode();

        // Add the GUI in a new tab
        loadGuiIntoNewTab( gui );
    }
}

// Open a gui in a new dock.
// Present a file open dialog box and after generate the gui based on the ui file the user selects
void MainWindow::on_actionNew_Dock_triggered()
{
    // Create the GUI
    profile.publishOwnProfile();
    QEForm* gui = createGui( GuiFileNameDialog( "Open" ), app->getParams()->customisationName, true );
    profile.releaseProfile();
    loadGuiIntoNewDock( "", gui );
}

// User requested a new gui to be opened
// Present a file open dialog box and after generate the gui based on the ui file the user selects
void MainWindow::on_actionOpen_triggered()
{
    // Create the GUI
    profile.publishOwnProfile();
    QEForm* gui = createGui( GuiFileNameDialog( "Open" ), app->getParams()->customisationName, false, true );
    profile.releaseProfile();
    loadGuiIntoCurrentWindow( gui, true );
}

// Close a gui
void MainWindow::on_actionClose_triggered()
{
    // If using tabs, delete the currently selected tab
    if( usingTabs )
    {
        // Delete the tab
        // The tab change code will look after changing the title to whatever gui becomes current.
        QTabWidget* tabs = getCentralTabs();
        if( tabs )
            tabCloseRequest( tabs->currentIndex() );
    }

    // Using a single window, just delete the gui
    else
    {
        // Close the GUI
        // (Create an empty central widget)
        QEForm* gui = getCentralGui();
        if( gui )
        {
            removeGuiFromGuiList( gui );
            setCentralWidget( new QWidget() );
        }

        // Set up the default customisations as any customisations from the GUI just closed no longer apply
        setDefaultCustomisation();

        // Clear the title as the title for the GUI just closed no longer applies
        setTitle( "" );
    }
 }

// User is changing the user level
void MainWindow::on_actionUser_Level_triggered()
{
    // Present the login dialog to the user
    app->login( this );
}

// Close this window event
void MainWindow::closeEvent(QCloseEvent *event)
 {
    // If this main window is being deleted, just close
    if( beingDeleted )
    {
        event->accept();
        return;
    }

    // If there is only one GUI open (max), just exit
    int count = 0;
    for( int i = 0; i < guiList.count(); i++ )
    {
        if( !guiList[i].getIsDock() )
        {
            count++;
        }
    }
    if( count <= 1 )
    {
        event->accept();
    }

    // If more than one GUI is open, check what the user wants to do
    else
    {
        QMessageBox msgBox;
        msgBox.setText( "This window has more than one form open. Do you want to close them all?" );
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        switch ( msgBox.exec() )
        {
            case QMessageBox::Yes:
                // Yes, close the window.
                event->accept();
                break;

            case QMessageBox::Cancel:
            default:
                // Cancel, do nothing
                event->ignore();
                break;
         }
    }
}

// Exit.
// If more than one window is present, offer to close the current window, or all of them
void MainWindow::on_actionExit_triggered()
{
    // If there is only one window open (max), just exit
    if( app->getMainWindowCount() <= 1 )
    {
        QCoreApplication::exit(0);
    }

    QString msg;
    if( app->getMainWindowCount() == 2 )
    {
        msg ="You are closing this window, but QEGui has another open. Do you want to close the other as well?";
    }
    else
    {
        msg ="You are closing this window, but QEGui has others open. Do you want to close the others as well?";
    }

    // If more than one main window is open, check what the user wants to do
    QMessageBox msgBox;
    msgBox.setText( msg );
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    switch ( msgBox.exec() )
    {
       case QMessageBox::Yes:
            // Yes, close all windows.
            // Simply exit!
            QCoreApplication::exit(0);
            break;

       case QMessageBox::No:
            // No, just close the one window
           close();
           break;

       case QMessageBox::Cancel:
           // Cancel, do nothing
           break;

        default:
           // Do nothing
           break;
     }
}

// Launch a new gui from, e.g. the 'File' menu
MainWindow* MainWindow::launchLocalGui( const QString& filename )
{
    profile.publishOwnProfile();
// Zai no customisation    MainWindow* w = new MainWindow( app, filename, app->getParams()->customisationName, true );
    MainWindow* w = new MainWindow( app, filename, "", true );
    profile.releaseProfile();
    w->show();
    return w;
}

// Launch a new gui, find QE widget by class name and paste PV name.
// Used by gui requests.
MainWindow* MainWindow::launchLocalGui( const QString& filename,
                                        const QString& className,
                                        const QString& pvName )
{
    MainWindow* newWindow = NULL;
    QWidget* widget = NULL;
    QEWidget* qeWidget = NULL;

    newWindow = launchLocalGui (filename);
    widget = QEUtilities::findWidget (newWindow, className);
    qeWidget = dynamic_cast< QEWidget* > (widget);

    if (qeWidget) {
       qeWidget->paste (QVariant (pvName));
    }
    return newWindow;
}

// Raise the window selected in the 'Window' menu
// Note, On Qt 4.7 this is called once for each action in the menu, but with the action being the action selected.
// This appears to be a bug in Qt QTBUG-25669. Additional calls are redundant but cheap and harmless as the desired window has already been rasied.
void MainWindow::onWindowMenuSelection( QAction* action )
{
    // Extract the gui from the action data
    QEForm* gui = action->data().value<QEForm*>();

    // Raise it to be the current window
    raiseGui( gui );
}

// Raise a gui and select the right tab so the user can see it.
// It may not be in this main window
void MainWindow::raiseGui( QEForm* gui )
{
    // Prepare to look for gui
    int tabIndex = 0;
    QTabWidget* tabs = NULL;

    // Search each main window for the gui
    int i = 0;
    MainWindow* mw;
    while( (mw = app->getMainWindow( i )) )
    {
        // If the main window is not using tabs, just check the central widget
        if( !mw->usingTabs )
        {
            QWidget* cw = mw->centralWidget();

            // If the central widget is the gui, we have found it
            if( cw == gui )
            {
                break;
            }

            // The central widget may be a scroll area holding the gui
            // If the central widget is a scroll area holding the gui, we have found it
            if ( QString::compare( cw->metaObject()->className(), "QScrollArea" ) == 0 )
            {
                QScrollArea* sa = (QScrollArea*)cw;
                if( sa->widget() == gui )
                    break;
            }
        }

        // If the main window is using tabs, check each tab
        else
        {
            tabs = mw->getCentralTabs();
            if( tabs )
            {
                // Compare the gui in each tab
                for( tabIndex = 0; tabIndex < tabs->count(); tabIndex++ )
                {
                    QWidget* tw = tabs->widget( tabIndex );
                    // If the tab's widget is the gui, we have found it
                    if( tw == gui )
                    {
                        break;
                    }

                    // The tab's widget may be a scroll area holding the gui
                    // If the tab's widget is a scroll area holding the gui, we have found it
                    if ( QString::compare( tw->metaObject()->className(), "QScrollArea" ) == 0 )
                    {
                        QScrollArea* sa = (QScrollArea*)tw;
                        if( sa->widget() == gui )
                            break;
                    }
                }

                // If fond a matching tab, stop looking in more forms
                if( tabIndex < tabs->count() )
                {
                    break;
                }
            }
        }

        // Next main window
        i++;
    }

    // If the gui was found in a main form, show it

    if( i < app->getMainWindowCount() )
    {
        // Ensure the main form is visible and the active form
        // Zai: restore it as well
        mw->showNormal();
        mw->raise();
        mw->activateWindow();

        // If using tabs, and a tab was found, the go to that tab
        if( mw->usingTabs && tabIndex < tabs->count() )
        {
            tabs->setCurrentIndex( tabIndex );
        }
    }
}

void MainWindow::toggleCenterWidget(){
    if (centralWidget()->isHidden()){
        centralWidget()->show();
    }
    else{
        centralWidget()->hide();
    }
}

void MainWindow::runDetectorEditor(){
    QList<QStringList> detectorList;
    // read customisation file and get detector list
    DetectorEditor::loadDetectorData("DetectorMainCustomisations.xml", detectorList);
    DetectorEditor dEditor(detectorList, "DetectorMainCustomisations.xml");

    while (true){
        if ( dEditor.exec() == QDialog::Rejected )
        {
            return;
        }
        // update the system customisation: reload
        if (dEditor.save()) return;
    }
}

void MainWindow::lockLayoutToggled(){
    bool lockStatus = getLockLayoutStatus();
    setLockLayoutStatus(!lockStatus);
    // checked and unchecked
    QMap<QString, windowCustomisationMenuItem*>::const_iterator items_i = customisationInfo.menuItems.find("LockLayout");
    if (items_i != customisationInfo.menuItems.end()){
        windowCustomisationMenuItem* actionItem = items_i.value();
        actionItem->setChecked(!lockStatus);
    }
}

// Present the 'About' dialog
void MainWindow::on_actionAbout_triggered()
{
    // Build the QE framework version string
    QString QEFrameworkVersionQEGui = QString( QEFrameworkVersion::getString() ).append(" ").append( QEFrameworkVersion::getDateTime() );

    // Build the user level string
    userLevelTypes::userLevels level = profile.getUserLevel();
    QString userLevel;
    switch( level )
    {
        case userLevelTypes::USERLEVEL_USER:      userLevel = "User";      break;
        case userLevelTypes::USERLEVEL_SCIENTIST: userLevel = "Scientist"; break;
        case userLevelTypes::USERLEVEL_ENGINEER:  userLevel = "Engineer";  break;
    }

    // Build a list of GUI windows and their files
    QStringList windowTitles;
    QStringList windowFiles;
    int i = 0;
    MainWindow* mw;
    while( (mw = app->getMainWindow( i )) )
    {
        for( int j = 0; j < mw->guiList.count(); j++ )
        {
            QString docked;
            if( mw->guiList[j].getIsDock() )
            {
                docked = " (Docked)";
            }
            windowTitles.append( mw->guiList[j].getForm()->getQEGuiTitle().append( docked ) );
            windowFiles.append( mw->guiList[j].getForm()->getFullFileName() );
        }

        // Next main window
        i++;
    }

    // Present the dialog
    aboutDialog ad( QString( QE_VERSION_STRING " " QE_VERSION_DATE_TIME ), // Version info and the build date/time at compile time of QEGui
                    QEFrameworkVersionQEGui,                               // Version info and the build date/time at compile time of the copy of QEPlugin library loaded by QEGui
                    UILoaderFrameworkVersion,                              // Version info and the build date/time at compile time of the copy of QEPlugin library loaded by QUiLoader while creating QE widgets

                    profile.getMacroSubstitutions(),                       // Macro substitutions (-m parameter)
                    profile.getPathList(),                                 // Path list (-p parameter)
                    profile.getEnvPathList(),                              // Path list (environment variable)
                    userLevel,                                             // Current user level

                    windowTitles,                                          // Window titles (must be same length as windowFiles)
                    windowFiles,                                           // Window file name (must be same length as windowTitles)

                    app->getParams()->configurationFile,                   // Configuration file
                    app->getParams()->configurationName,                   // Configuration name

                    app->getParams()->customisationFile,                   // Default Window customisation file
                    app->getParams()->customisationName,                   // Default window customisation name
                    QString( "not done yet" )                              // Default window customisation name
                    );
    ad.exec();
}

// Change the current tab
void MainWindow::tabCurrentChanged( int index )
{
    // Do nothing when called while changing from single scroll area to using tabs.
    if( !usingTabs )
        return;

    // Update the main window title
    QTabWidget* tabs = getCentralTabs();
    if( tabs )
    {
        setTitle( tabs->tabText( index ) );
    }
}

// Delete a tab
void MainWindow::tabCloseRequest( int index )
{
    QTabWidget* tabs = getCentralTabs();
    if( !tabs )
        return;

    // Get a reference to the scroll area for the tab being deleted
    tabs->setCurrentIndex( index );
    QEForm* gui = extractGui( tabs->currentWidget() );

    // Remove the gui from the 'windows' menus
    removeGuiFromGuiList( gui );

    // Remove the tab
    tabs->removeTab( index );

    // If there is no need for tabs (only one GUI) stop using tabs
    if( tabs->count() == 1 )
        setSingleMode();
}

//
void MainWindow::tabContextMenuRequest( const QPoint& posIn )
{
    QTabWidget* tabs = getCentralTabs();

    // Sanity checks ....
    if (!usingTabs || !tabs || !tabMenu) {
        return;
    }

    // NOTE: We want access to the tab widget's tabBar so that find the tab index
    // associated the the postion. But the tabBar () function IS protected. So we
    // get around this by deriving our own tab widget class that can see the
    // protected tabBar () function and expose this as a public function. We then
    // cast the QTabWidget pointer to a ExposedTabWidget pointer and call the public
    // function.
    //
    class ExposedTabWidget : QTabWidget {
    public:
        QTabBar* getTabBar() const { return tabBar(); }
    };

    // Find and switch to the appropriate tab.
    //
    ExposedTabWidget* exposed = (ExposedTabWidget*) tabs;
    QTabBar* tabBar = exposed->getTabBar();
    int index;
    QPoint golbalPos;

    index = tabBar->tabAt (posIn);
    if (index >= 0) {
        tabs->setCurrentIndex (index);
        golbalPos = tabs->mapToGlobal (posIn);
        tabMenu->exec (golbalPos, 0);
    }
}

// Process context menu action.
// Currently there is only one action - detach - no need to use the action parameter.
void MainWindow::tabContextMenuTrigger( QAction* )
{
    QTabWidget* tabs = getCentralTabs();

    // Sanity checks ....
    if (!tabs  || !usingTabs) {
       return;
    }

    int index = tabs->currentIndex ();

    // Similar to tab close request
    QEForm* gui = extractGui( tabs->currentWidget() );

    if (!gui) {
        return;
    }

    // Extract and save the filename.
    QString fileName = gui->getFullFileName ();

    // Look for the GUI and save the window customisation name if found
    QString customisationName;
    for( int i = 0; i < guiList.count(); i++ )
    {
        if( guiList[i].getForm() == gui )
        {
            customisationName = guiList[i].getCustomisationName();
            break;
        }
    }

    // Remove the gui from the 'windows' menus
    removeGuiFromGuiList( gui );

    // Remove the tab - note this does not delete the page widget.
    tabs->removeTab( index );

    // If there is no need for tabs (only one GUI) stop using tabs
    if( tabs->count() == 1 )
        setSingleMode();

    // Use extracted filename to open the new window - we assume the file still exists.
    profile.publishOwnProfile();
    MainWindow* w = new MainWindow( app, fileName, customisationName, false, NULL);
    profile.releaseProfile();
    w->show();
}

// Open designer
void MainWindow::on_actionDesigner_triggered()
{
    // Start designer
    processOpenGui = false;
    process.setWorkingDirectory( profile.getPath() );
    startDesigner();
}

// Open the current form in designer
void MainWindow::on_actionOpen_Current_Form_In_Designer_triggered()
{
    // Start designer specifying the current gui file name
    processOpenGui = true;
    process.setWorkingDirectory( profile.getPath() );
    startDesigner();
}

// Common 'designer' startup
// Called if starting designer with or without a filename
void MainWindow::startDesigner()
{
    // If not already running, start designer
    if( process.state() == QProcess::NotRunning )
    {
        processSecondAttempt = false;
        startDesignerCore( DESIGNER_COMMAND_1 );
    }

    // If already running, tell the user
    else
    {
        QMessageBox::about(this, "QEGui", "Designer (started by QEGui) is already running.");
    }
}

// Core 'designer' startup
// Called first and second time designer startup is attempted
void MainWindow::startDesignerCore( QString command )
{
    // If opening the current gui, get the name and start designer with the name
    if( processOpenGui )
    {
        // Get the gui file name (left empthy if no gui)
        QStringList guiFileName;
        QEForm* gui = getCurrentGui();
        if( gui )
        {
            guiFileName.append( gui->getUiFileName() );
        }

        // Start designer
        process.start( command, guiFileName );
    }

    // If just opening designer, then start it with no file name
    else
    {
        process.start( command );
    }
}

// An error occured starting designer.
// One possibility is that a Qt version older then 4.8 is in use and the name is different
// (Before Qt 4.8, command is 'designer'. Qt 4.8 later uses the command 'designer-qt4')
// So, try again with the alternate name.
// However, the process can't be started while still in the error function for the last process,
// so set a timer for 0mS and start it in the signal from that
void MainWindow::processError( QProcess::ProcessError error )
{
    if( error == QProcess::FailedToStart )
    {
        // Do nothing if this was the second attempt using an alternate command
        if( processSecondAttempt )
        {
            QMessageBox::about(this, "QEGui", "Sorry, an error occured starting designer.");
            return;
        }

        // Signal startDesignerAlternate() immedietly to try starting designer again
        // with an alternate command
        processTimer.setSingleShot(true);
        processTimer.setInterval(0);
        processTimer.start();
    }
}

// Try starting designer again with an alternate command.
// See description of processError() for more details.
void MainWindow::startDesignerAlternate()
{
    // Try starting designer again with an alternate command.
    processSecondAttempt = true;
    startDesignerCore( DESIGNER_COMMAND_2 );
}

// Refresh the current window (reload the ui file)
void MainWindow::on_actionRefresh_Current_Form_triggered()
{
    // Get the gui file name (left empty if no gui)
    QString guiFileName;
    QEForm* currentGui = getCurrentGui();
    QString guiPath;
    if( currentGui )
    {
        guiFileName = currentGui->getUiFileName();
        QDir directory( profile.getPath() );
        guiPath = directory.filePath( guiFileName );
    }

    // Recreate the gui and load it in place of the current window
    if( guiFileName.size() )
    {
        profile.publishOwnProfile();
        QEForm* newGui = createGui( guiPath, "" ); // no customisation name so customisations remain unaltered
        loadGuiIntoCurrentWindow( newGui, true );
        profile.releaseProfile();
    }
}

// Allow the user to change user level passwords
void MainWindow::on_actionSet_Passwords_triggered()
{
ContainerProfile profile;
    PasswordDialog pd;
    pd.exec();
}


//=================================================================================
// Methods for managing GUI windows
//=================================================================================

// Given a QEForm, return a widget that will manage being resized.
// If the QEForm has a scroll area as its top level child, or if its top level child has
// a layout, it is managing its own size so just return the QEForm, otherwise return a
// scroll area containing the QEForm.
QWidget* MainWindow::resizeableGui( QEForm* gui, QSize* preferedSize )
{
    // Determine if the top level widget in the ui is a scroll area
    QObjectList children = gui->children();
    bool topLevelScrollArea = false;
    if( children.size() && QString::compare( children[0]->metaObject()->className(), "QScrollArea" ) == 0 )
    {
        topLevelScrollArea = true;
    }

    // If the widget is managing its own size (it is in in a scroll or has a layout), return it as is
    if( topLevelScrollArea || gui->layout() )
    {
        // Set the prefered size to the gui size.
        if( preferedSize )
        {
            *preferedSize = gui->size();
        }

        // Return the gui as is
        return gui;
    }
    // If the widget is not managing its own size return it within a scroll area
    else
    {
        // Add the gui to a scroll area
        QScrollArea* sa = new QScrollArea();
        sa->setWidget( gui );

        // Set the prefered size to the gui size plus the scroll area margins.
        if( preferedSize )
        {
#if defined(Q_OS_WIN32)
            preferedSize->setWidth( gui->size().width() + sa->contentsMargins().left() + sa->contentsMargins().right() + 2 );
            preferedSize->setHeight( gui->size().height() + sa->contentsMargins().top() + sa->contentsMargins().bottom() + 2 );
#else
            preferedSize->setWidth( gui->size().width() + sa->contentsMargins().left() + sa->contentsMargins().right() );
            preferedSize->setHeight( gui->size().height() + sa->contentsMargins().top() + sa->contentsMargins().bottom() );
#endif
        }

        // Return the scroll area
        return sa;
    }
}

// Return a QEForm from a widget that may be a QEForm, or a QScrollArea containg a QEForm
QEForm* MainWindow::extractGui( QWidget* rGui )
{
    // Assume the resizable gui supplied is the actual QEForm
    QWidget* w = rGui;

    // If the resizable gui is a scroll area, then extract the widget the scroll area is managing - it should be a QEForm
    if ( QString::compare( w->metaObject()->className(), "QScrollArea" ) == 0 )
    {
        QScrollArea* sa = (QScrollArea*)w;
        w = sa->widget();
    }

    // We should now have the QEForm. Return it
    if ( QString::compare( w->metaObject()->className(), "QEForm" ) == 0 )
    {
        return (QEForm*)w;
    }
    else
    {
        return NULL;
    }
}

// Open a gui in a new tab
// Either as a result of the gui user requesting a new tab, or a contained object (gui push button) requesting a new tab
void MainWindow::loadGuiIntoNewTab( QEForm* gui )
{
    // Do nothing if couldn't create gui
    if( !gui )
        return;

    // Ensure the gui can be resized
    QWidget* rGui = resizeableGui( gui );

    // Add a tab
    QTabWidget* tabs = getCentralTabs();
    if( tabs )
    {
        int index = tabs->addTab( rGui, gui->getQEGuiTitle() );
        tabs->setCurrentIndex( index );
    }

    // Initialise customisation items.
    app->getMainWindowCustomisations()->initialise( &customisationInfo );
}

// Open a gui in the current window
// Either as a result of the gui user requesting a new window, or a contained object (gui push button) requesting a new window
void MainWindow::loadGuiIntoCurrentWindow( QEForm* gui, bool resize )
{
    // Do nothing if couldn't create gui
    if( !gui )
        return;

    // If using tabs, load the gui into the current tab
    if( usingTabs )
    {
        QTabWidget* tabs = getCentralTabs();
        if( tabs )
        {
            // Remove the gui from the 'windows' menus and delete it
            QEForm* oldGui = extractGui( tabs->currentWidget() );
            if( oldGui )
            {
                removeGuiFromGuiList( oldGui );
            }

            // Ensure the gui can be resized
            QWidget* rGui = resizeableGui( gui );

            // Remove the tab
            int i = tabs->currentIndex();
            tabs->removeTab( i );

            // Replace the tab
            tabs->insertTab( i, rGui, gui->getQEGuiTitle() );
            tabs->setCurrentWidget( rGui );
        }
    }

    // Using a single window, just set the gui as the central widget
    // (note, resize with a timer event to allow all widget sizes to be recalculated before we use them
    else
    {
        // Remove the old gui from the 'windows' menus, if any
        if( centralWidget() )
        {
            QEForm* oldGui = extractGui( centralWidget() );
            if( oldGui )
            {
                removeGuiFromGuiList( oldGui );
            }
        }

        // If nothing else is looking after resizing (such as a restore) resize here
        if( resize )
        {
            // Ensure the gui can be resized (Ensure it has a layout or is in a scroll area)
            QSize preferedSize;
            QWidget* rGui = resizeableGui( gui, &preferedSize );

            // Resize the main window to neatly fit the new gui.
            // (Note, this is done using the old central widget as we are fiddling with the
            //  size properties to force a specific size. By using the the old (current) central
            //  widget we can avoid having to restore the size properties)
            centralWidget()->setFixedSize( preferedSize );
            adjustSize();

            // Load the new gui into the main window
            setCentralWidget( rGui );
        }
        else
        {
            QWidget* rGui = resizeableGui( gui );
            setCentralWidget( rGui );
        }
    }

    // Set the title
    if (app->getParams()->applicationTitle.isEmpty())
        setTitle( gui->getQEGuiTitle() );

    // Initialise customisation items.
    app->getMainWindowCustomisations()->initialise( &customisationInfo );
}

// Open a gui in a new dock
// Either as a result of the gui user requesting a new dock, or a contained object (gui push button) requesting a new dock
QAction* MainWindow::loadGuiIntoNewDock( QString dockTitle,
                                     QEForm* gui,
                                     bool hidden,
                                     bool tabbed,
                                     QEActionRequests::Options createOption,
                                     Qt::DockWidgetArea allowedAreas,
                                     QDockWidget::DockWidgetFeature features,
                                     QRect geom )
{
    // Do nothing if couldn't create gui
    if( !gui )
    {
        return NULL;
    }

    // Ensure the gui can be resized
    QWidget* rGui = resizeableGui( gui );


    QDockWidget *dock = new QDockWidget( this );
    dock->setAllowedAreas( allowedAreas );
    dock->setFeatures( features );


    Qt::DockWidgetArea dockLocation = creationOptionToDockLocation( createOption,  dock);


    // If the dock is loating and geometry has been supplied (non zero width and height), set the geometry
    if( createOption == QEActionRequests::OptionFloatingDockWindow && geom.width() && geom.height() )
    {
        dock->setGeometry( geom );
    }

    // Add the dock to the appropriate main window
    addDockWidget(dockLocation, dock);

    if( tabbed )
    {
        QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();
        for( int i = 0; i < dockWidgets.count(); i++ )
        {
            if( dockWidgetArea( dockWidgets[i] ) == dockLocation )
            {
                if( dock != dockWidgets[i] )
                {
                    tabifyDockWidget ( dockWidgets[i], dock );
                    break;
                }
            }
        }
    }

    if (createOption != QEActionRequests::OptionFloatingDockWindow && geom.width() && geom.height()){
        // qt workaround for dock size
        dock->setMinimumWidth(geom.width());
        dock->setMinimumHeight(geom.height());
        if (!forceDockSize){
            // flag for restoring the default min size
            forceDockSize = true;
        }
    }

    // Load the GUI into the dock
    dock->setWidget( rGui );

    dock->setWindowTitle( dockTitle.isEmpty()? gui->getQEGuiTitle():dockTitle );

    // Set floating if requested
    dock->setFloating( createOption == QEActionRequests::OptionFloatingDockWindow);

    // Set hidden if required
    dock->setVisible( !hidden );

    // Initialise customisation items.
    app->getMainWindowCustomisations()->initialise( &customisationInfo );

    if (!dockTitle.isEmpty())
        replaceMenuAction(dockTitle, dock->toggleViewAction());
    return dock->toggleViewAction();
}

// Translate a creation option to a dock location.
// This is not a one for one. For example if a floating option is requested, the dock will still need a location.
Qt::DockWidgetArea MainWindow::creationOptionToDockLocation( QEActionRequests::Options createOption, QDockWidget *dock )
{
    Qt::DockWidgetArea dockLocation;

    switch( createOption )
    {
        case QEActionRequests::OptionLeftDockWindow:
            dockLocation = Qt::LeftDockWidgetArea;
            if (dock) customisationInfo.dockLeftWidgetList.append(dock);
            break;
        case QEActionRequests::OptionRightDockWindow:
            dockLocation = Qt::RightDockWidgetArea;
            if (dock) customisationInfo.dockRightWidgetList.append(dock);
            break;
        case QEActionRequests::OptionTopDockWindow:
            dockLocation = Qt::TopDockWidgetArea;
            if (dock) customisationInfo.dockTopWidgetList.append(dock);
            break;
        case QEActionRequests::OptionBottomDockWindow:
            dockLocation = Qt::BottomDockWidgetArea;
            if (dock) customisationInfo.dockBottomWidgetList.append(dock);
            break;
        case QEActionRequests::OptionFloatingDockWindow:
        if (dock) customisationInfo.dockFloatingWidgetList.append(dock);
        default:
            dockLocation = Qt::LeftDockWidgetArea;
    }

    return dockLocation;
}

// Translate a dock location to a creation option
QEActionRequests::Options MainWindow::dockLocationToCreationOption( Qt::DockWidgetArea dockLocation )
{
    switch( dockLocation )
    {
        default:
        case Qt::BottomDockWidgetArea: return QEActionRequests::OptionBottomDockWindow;
        case Qt::TopDockWidgetArea:    return QEActionRequests::OptionTopDockWindow;
        case Qt::LeftDockWidgetArea:   return QEActionRequests::OptionLeftDockWindow;
        case Qt::RightDockWidgetArea:  return QEActionRequests::OptionRightDockWindow;
    }
}

//=================================================================================
// Reimplementation of UserMessage method for presenting messages
//=================================================================================

void MainWindow::newMessage( QString msg, message_types type )
{
    // Change the message in the status bar
    if ( ( type.kind_set & MESSAGE_KIND_STATUS ) != 0 ) {
        statusBar()->showMessage( getMessageTypeName( type ).append( ": ").append( msg ) );
    }
}

//=================================================================================
// Slots and methods for launching new GUIs on behalf of objects in the gui (typically buttons)
//=================================================================================

// Launching a new gui given a .ui filename
MainWindow* MainWindow::launchGui( QString guiName, QString title, QString customisationName, QString actionName, QEActionRequests::Options createOption, bool hidden )
{
    // Get the profile published by whatever is launching a new GUI (probably a QEPushButton)
    ContainerProfile publishedProfile;

    // Get a standard absolute path for the file name
    QFile* uiFile =  QEWidget::findQEFile( guiName, &publishedProfile );

    // If a file was found, check if it is the same as any already open
    if( uiFile )
    {

    //!!! Note, repeated substitutions should be removed leaving only the first
    //!!! If a button re-launches the form it is in (with different macro substitutions) the list just grows

        // If the form already exists (with the same substitutions), just display that one.
        // Note, even if the gui is found, if the main window is not located and raised, then a new gui will be launched.
        MainWindow* mw = app->raiseGui( uiFile->fileName(), publishedProfile.getMacroSubstitutions().trimmed() );
        if( mw )
        {
            delete uiFile;
            return mw;
        }
    }

    // Load the new gui as required
    switch( createOption )
    {
        // Open the specified gui in the current window
        case QEActionRequests::OptionOpen:
            {
                // cleanup window and its data - close widgets and elete them all
                this->removeAllGuisFromGuiList();
                qDeleteAll(this->findChildren<QDockWidget*>());
                QList<QMainWindow*> winList = findChildren<QMainWindow*>();
                QList<QMainWindow*> winListToBeDeleted;
                for (int i = 0; i < winList.count(); i++){
                    if (winList.at(i)->parent() == this)
                    {
                        winListToBeDeleted.append(winList.at(i));
                    }
                }
                qDeleteAll(winListToBeDeleted);

                // update window title
                if (!title.isEmpty() && title != windowTitle()){
                    setTitle(title);
                }
                QEForm* gui = createGui( guiName, customisationName, false, true );  // Note, profile should have been published by signal code
                loadGuiIntoCurrentWindow( gui, true );
                lockLayoutStatusChanged(getLockLayoutStatus());
                QMap<QString, windowCustomisationMenuItem*>::const_iterator items_i = customisationInfo.menuItems.find(actionName);
                if (items_i != customisationInfo.menuItems.end()){
                    windowCustomisationMenuItem* actionItem = items_i.value();
                    actionItem->setCheckable(true);
                    actionItem->setChecked(true);
                }
                return this;
            }

        // Open the specified gui in a new tab
        case QEActionRequests::OptionNewTab:
            {
                // Create the gui and if created, load it into a new tab
                QEForm* gui = createGui( guiName, customisationName );  // Note, profile should have been published by signal code
                if( gui )
                {
                    // If not using tabs, start tabs and migrate any single gui to the first tab
                    if( !usingTabs )
                        setTabMode();

                    // Load the new GUI into a tab
                    loadGuiIntoNewTab( gui );
                }
                return this;
            }

        // Open the specified gui in a new window
        case QEActionRequests::OptionNewWindow:
            {
                MainWindow* w = new MainWindow( app, guiName, customisationName, true, title ); // Note, profile should have been published by signal code
                w->lockLayoutStatusChanged(getLockLayoutStatus());
                w->show();
                return w;
            }

        // Open the specified gui in a new child window
        case QEActionRequests::OptionNewChildWindow:
            {
                MainWindow* w = new MainWindow( app, guiName, customisationName, false, title, this ); // Note, profile should have been published by signal code
                w->lockLayoutStatusChanged(getLockLayoutStatus());
                w->show();
                return w;
            }

        // Create the specified gui in a new dock
        case QEActionRequests::OptionLeftDockWindow:
        case QEActionRequests::OptionRightDockWindow:
        case QEActionRequests::OptionTopDockWindow:
        case QEActionRequests::OptionBottomDockWindow:
        case QEActionRequests::OptionFloatingDockWindow:
            {
                // Create the gui and load it into a new dock
                QEForm* gui = createGui( guiName, title, true );  // Note, customisationName is the title of the Dock
                replaceMenuAction(actionName, loadGuiIntoNewDock( title, gui, hidden, false, createOption ));

                return this;
            }

        default:
            sendMessage( QString( "Unexpected gui creation option: " ).append( createOption ), "QEGui application. MainWindow::launchGui()" );
            return NULL;
    }
}

void MainWindow::createActionMaps()
{
    inbuiltFormMap.clear();
    inbuiltFormMap.insert( "PV Properties...",       ":/qe/gui/forms/PVProperties.ui" );
    inbuiltFormMap.insert( "Strip Chart...",         ":/qe/gui/forms/StripChart.ui" );
    inbuiltFormMap.insert( "Scratch Pad...",         ":/qe/gui/forms/ScratchPad.ui" );
    inbuiltFormMap.insert( "Message Log...",         ":/qe/gui/forms/MessageLog.ui" );
    inbuiltFormMap.insert( "Plotter...",             ":/qe/gui/forms/Plotter.ui" );
    inbuiltFormMap.insert( "PV Load/Save...",        ":/qe/gui/forms/PVLoadSave.ui" );
    inbuiltFormMap.insert( "Archive Status...",      ":/qe/gui/forms/ArchiveStatus.ui" );
    inbuiltFormMap.insert( "Archive Name Search...", ":/qe/gui/forms/ArchiveNameSearch.ui" );

    classNameMap.clear();
    classNameMap.insert( "PV Properties...",         "QEPvProperties" );
    classNameMap.insert( "Strip Chart...",           "QEStripChart" );
    classNameMap.insert( "Scratch Pad...",           "QEScratchPad" );
    classNameMap.insert( "Message Log...",           "QEMessageLog" );
    classNameMap.insert( "Plotter...",               "QEPlotter" );
    classNameMap.insert( "PV Load/Save...",          "QEPvLoadSave" );
    classNameMap.insert( "Archive Status...",        "QEArchiveStatus" );
    classNameMap.insert( "Archive Name Search...",   "QEArchiveNameSearch" );
}

// Slot for launching a new gui from a contained object.
void  MainWindow::requestAction( const QEActionRequests & request )
{
    QStringList arguments =  request.getArguments();

    switch( request.getKind () )
    {

        // Launching a new gui given a .ui file name
        case QEActionRequests::KindOpenFile:
            if (arguments.count() >= 1)
            {
                QList<windowCreationListItem> windows = request.getWindows();
                if (windows.count() > 0){
                    windowCreationListItem* window = &windows[0];
                    launchGui ( arguments.first(),  window->title, request.getCustomisation(), request.getAction(), request.getOption(), true );
                }
                else{
                    launchGui ( arguments.first(), "", request.getCustomisation(), request.getAction(), request.getOption(), true );
                }
            }
            break;

        case QEActionRequests::KindOpenFiles:
            {
                QList<windowCreationListItem> windows = request.getWindows();
                MainWindow* mw = this;
                for( int i = 0; i < windows.count(); i ++ )
                {
                    windowCreationListItem* window = &windows[i];
                    profile.addPriorityMacroSubstitutions( window->macroSubstitutions );
                    if (window->creationOption == QEActionRequests::OptionBottomDockWindow ||
                        window->creationOption == QEActionRequests::OptionFloatingDockWindow ||
                        window->creationOption == QEActionRequests::OptionLeftDockWindow ||
                        window->creationOption == QEActionRequests::OptionRightDockWindow ||
                        window->creationOption == QEActionRequests::OptionTopDockWindow){
                        mw = mw->launchGui ( window->uiFile, window->title, "", request.getAction(), window->creationOption, window->hidden );
                    }
                    else{
                        mw = mw->launchGui ( window->uiFile, window->title, window->customisationName, request.getAction(), window->creationOption, window->hidden );
                    }
                    profile.removePriorityMacroSubstitutions();
                }
            }
            break;

        case QEActionRequests::KindAction:
            if (arguments.count() >= 1)
            {
                QString action = request.getAction();

                // Handle actions that launch inbuilt forms
                if( inbuiltFormMap.contains( action ) )
                {
                    QString inbuiltForm = inbuiltFormMap.value (action, "");

                    if( arguments.count() >= 1 )
                    {
                        QString className = classNameMap.value (action, "");
                        launchLocalGui( inbuiltForm, className, arguments[0] );
                    }
                    else
                    {
                        launchLocalGui( inbuiltForm );
                    }
                    break;
                }

                // Handle other actions
                     if (action == "New Window..."                     ) { on_actionNew_Window_triggered();                     }
                else if (action == "New Tab..."                        ) { on_actionNew_Tab_triggered();                        }
                else if (action == "New Dock..."                       ) { on_actionNew_Dock_triggered();                       }
                else if (action == "Open..."                           ) { on_actionOpen_triggered();                           }
                else if (action == "Close"                             ) { on_actionClose_triggered();                          }
                else if (action == "Save Configuration..."             ) { on_actionSave_Configuration_triggered();             }
                else if (action == "Restore Configuration..."          ) { on_actionRestore_Configuration_triggered();          }
                else if (action == "Manage Configuration..."           ) { on_actionManage_Configurations_triggered();          }
                else if (action == "User Level..."                     ) { on_actionUser_Level_triggered();                     }
                else if (action == "Exit"                              ) { on_actionExit_triggered();                           }
                else if (action == "Open Designer..."                  ) { on_actionDesigner_triggered();                       }
                else if (action == "Open Current Form In Designer..."  ) { on_actionOpen_Current_Form_In_Designer_triggered();  }
                else if (action == "Refresh Current Form"              ) { on_actionRefresh_Current_Form_triggered();           }
                else if (action == "Set Passwords..."                  ) { on_actionSet_Passwords_triggered();                  }
                else if (action == "About..."                          ) { on_actionAbout_triggered();                          }
                else if (action == "ToggleCenterWidget"                ) { toggleCenterWidget();                                }
                else if (action == "DetectorEditor"                    ) { runDetectorEditor();                                 }
                else if (action == "LockLayout"                        ) { lockLayoutToggled();                                 }
                else  sendMessage( "Unhandled gui action request, action = '" + action + "'",
                                   message_types( MESSAGE_TYPE_ERROR, MESSAGE_KIND_EVENT ) );
            }
            break;

        case QEActionRequests::KindWidgetAction:
            {
                QEWidget::doAction( this, request.getWidgetName(), request.getAction(), request.getArguments(), request.getInitialise(), request.getOriginator() );
            }
            break;

        case QEActionRequests::KindHostComponents:
            {
                QList<componentHostListItem> components = request.getComponents();
                for( int i = 0; i < components.count(); i ++ )
                {
                    componentHostListItem* component = &components[i];

                    QDockWidget *dock = new QDockWidget( this );
                    // default to all areas allowed    dock->setAllowedAreas( ... );
                    // default features                dock->setFeatures( ... );

                    // Add the dock to the appropriate main window
                    addDockWidget( creationOptionToDockLocation( component->creationOption ), dock );

                    // Load the component into the dock
                    dock->setWidget( component->widget );
                    QObject::connect( component->widget, SIGNAL(destroyed(QObject*)), this, SLOT(dockComponentDestroyed(QObject*)));


                    dock->setWindowTitle( component->title );
                    dock->adjustSize();

                    // Set floating if requested
                    dock->setFloating( component->creationOption == QEActionRequests::OptionFloatingDockWindow );

                    // Set hidden if required
                    dock->setVisible( !component->hidden );

                    replaceMenuAction(component->title, dock->toggleViewAction());
                }
            }
            break;

       default:
            sendMessage( "Unhandled gui request kind", message_types( MESSAGE_TYPE_ERROR, MESSAGE_KIND_EVENT ) );

    }
}

// Slot to delete a dock holding a component that the application is hosting on
// behalf of a QE widget.
// this is called when the component being hosted is destroyed by the QE widget.
// This should be done here since the component should not need to know how the
// application is hosting it.
void MainWindow::dockComponentDestroyed( QObject* component )
{
    // Do nothing if no widget is available
    if( !component->isWidgetType() )
    {
        return;
    }

    // Get parent (should be the dock)
    QWidget* dock = (QWidget*)(component)->parent();

    // Do nothing if no parent or it is not a dock
    if( !dock || QString( "QDockWidget").compare( dock->metaObject()->className() ) )
    {
        return;
    }

    // Delete the dock holding the component.
    // (Delete it later when returned to the event loop. We are only here as we are
    //  currently deleting the component within the dock. Starting another chain of
    //  deletion higher up the widget tree at this time would be fatal)
    dock->deleteLater();
}

//=================================================================================
// Methods for common support tasks
//=================================================================================

// Set up to use only a single gui
// This is the case when the main window is started or the number of tabs has reduced to one
void MainWindow::setSingleMode()
{
    // Sanity check
    if( !usingTabs )
        return;

    // Get the tabs widget
    QTabWidget* tabs = getCentralTabs();
    if( !tabs )
        return;

    // Move the gui from the first (and only) tab to be the central widget
    QEForm* gui = extractGui( tabs->currentWidget() );
    if( gui )
    {
        // Make the gui the central widget.
        // Note, ownership of widgets set as the central widget is claimed by the main window.
        // So when a widget is set as the central widget, any previous central widget is deleted by the main window.
        // As the previous central widget was the tab widget, and gui we are about to set as the central
        // widget is part of the tab widget's hierarchy, there is a risk that the gui may be deleted before
        // it is reparented as part of being set as the central widget. In practice, this does not happen.
        // It appears it is removed from the tab widget hierarchy before the tab widget is deleted.
        // If this proves not be the case, it may be nessesary to reparent the gui widget manually before
        // calling setCentralWidget() here.
        QWidget* w = resizeableGui( gui );
        setCentralWidget( w );

        // Set the title to the remaining window
        setTitle( gui->getQEGuiTitle() );

        // Must 'show' the gui following the reparenting inherent in the above invocation of setCentralWidget
        // where the gui is reparented from from the tab widget to the main window.
        w->show();
    }

    // Flag tabs are no longer in use
    usingTabs = false;
    tabMenu = NULL;
}

// Set up to use multiple guis in tabs
// This is the case when a single scroll area is in use and a new tab has been requested
void MainWindow::setTabMode()
{
    // Sanity check
    if( usingTabs )
        return;

    // Create the tab widget
    QTabWidget* tabs = new QTabWidget;
    tabs->setTabsClosable( true );
    QObject::connect( tabs, SIGNAL( tabCloseRequested ( int ) ), this, SLOT( tabCloseRequest( int ) ) );
    QObject::connect( tabs, SIGNAL( currentChanged ( int ) ), this, SLOT( tabCurrentChanged( int ) ) );

    // Set up tab context menus.
    //
    tabs->setContextMenuPolicy (Qt::CustomContextMenu);
    QObject::connect (tabs, SIGNAL (customContextMenuRequested (const QPoint &)),
                      this, SLOT   (tabContextMenuRequest      (const QPoint &)));

    tabMenu = new QMenu (tabs);  // note: menu deleted when tabs object deleted.

    QAction *action = new QAction ("Reopen tab as new window", tabMenu);
    action->setCheckable (false);
    action->setData (QVariant (0));
    action->setEnabled (true);
    tabMenu->addAction (action);

    QObject::connect (tabMenu, SIGNAL (triggered             (QAction* )),
                      this,    SLOT   (tabContextMenuTrigger (QAction* )));


    // If there was a single gui present, move it to the first tab
    QEForm* gui = getCentralGui();
    if( gui )
        tabs->addTab( resizeableGui( gui ), gui->getQEGuiTitle() );

    // Start using tabs as the main area of the main window
    setCentralWidget( tabs );
    usingTabs = true;
}

// Get a gui filename from the user
// Although this is only a single line, it standardises the filters and file types used
QString MainWindow::GuiFileNameDialog( QString caption )
{
    // Get the filename
    return QFileDialog::getOpenFileName( this, caption, profile.getPath(), "Interfaces(*.ui)" );
}

// Create a gui
// Performs gui opening tasks generic to new guis, including opening a new tab,
// replacing a gui in a tab, replacing a single gui in the main window,
// or creating a gui in a new main window.
// A profile should have been published before calling this method.
QEForm* MainWindow::createGui( QString fileName, QString customisationName, bool isDock, bool clearExistingCustomisations )
{
    return createGui( fileName, customisationName, QString(), isDock, clearExistingCustomisations );
}

QEForm* MainWindow::createGui( QString fileName, QString customisationName, QString restoreId, bool isDock, bool clearExistingCustomisations )
{
    bool isRestoring = !restoreId.isEmpty();
    // Don't do anything if no filename was supplied
    if (fileName.isEmpty())
        return NULL;

    // Perform tasks required by a main window, but not a dock
    if( !isDock )
    {
        // Use the default customisations if no customisation is specified
        if( customisationName.isEmpty() )
        {
//Zai            setDefaultCustomisation();
        }
        else{
            // Load any required window customisation
            applyCustomisation( customisationName, &customisationInfo, clearExistingCustomisations, isRestoring );
            tabifyDocks();
        }

        // Use whatever placeholder menus are available (for example, populate a 'Recent' menu if present)
        setupPlaceholderMenus();

        // Setup to allow user to change focus to a window from the 'Windows' menu
        if( windowMenu )
        {
            QObject::connect( windowMenu, SIGNAL( triggered( QAction* ) ), this, SLOT( onWindowMenuSelection( QAction* ) ) );
        }
    }
    // Publish the main window's form Id so the new QEForm will pick it up
    setChildFormId( getNextMessageFormId() );
    profile.setPublishedMessageFormId( getChildFormId() );

    // Inform user
    newMessage( QString( "Opening %1" ).arg( fileName ), message_types ( MESSAGE_TYPE_INFO ) );

    // Build the gui
    QEForm* gui = new QEForm( fileName );
    if( !restoreId.isNull() )
    {
        gui->setUniqueIdentifier( restoreId );
    }
    gui->setResizeContents( false );

    // Read the ui file.
    // This method may be called with or without a profile defined.
    // For example, when this method is the result of a QEButton launching a new GUI,
    // the button will have published its own profile. This is fine for some
    // things - such as picking up the required macro substitutions - but not
    // appropriate for other things, such as which widgets should be signaled
    // for error messages - The newly created window should be receiving those,
    // not the window the button lives in.
    bool profileDefinedHere = false;
    if( !profile.isProfileDefined() )
    {
        // Flag we are defining a profile here (we need to release it ourselves later)
        profileDefinedHere = true;

        // Publish our profile
        profile.publishOwnProfile();

    }

    // Regardless of who set up the profile, this window should be receiving
    // requests to do things such as display errors.
    profile.updateConsumers( this );

    // Load the .ui file into the GUI
    gui->readUiFile();

    // Apply any adjustments to the scaling of the loaded widget.
    //
    QEScaling::applyToWidget( gui );

    // Save the version of the QE framework used by the ui loader. (can be different to the one this application is linked against)
    UILoaderFrameworkVersion = gui->getContainedFrameworkVersion();

    // If a profile was defined in this method, release it now.
    if( profileDefinedHere )
    {
        profile.releaseProfile();
    }

    // Add the new gui to the list of windows
    if( !isDock ){
        // Create an action for the 'Window' menus
        QAction* windowMenuAction = new QAction( !windowTitle().isEmpty()? windowTitle():gui->getQEGuiTitle(), this );
        windowMenuAction->setData( qVariantFromValue( gui ) );

        // Add this gui to the application wide list of guis
        guiList.append( guiListItem( gui, this, windowMenuAction, customisationName, isDock ) );

        // For each main window, add a new action to the window menu
        int i = 0;
        MainWindow* mw;
        while( (mw = app->getMainWindow( i )) )
        {
            mw->addWindowMenuAction( windowMenuAction );

            // Next main window
            i++;
        }
    }
    else{
        // Add this gui to the application wide list of guis with no action
        guiList.append( guiListItem( gui, this, NULL, customisationName, isDock ) );
    }

    app->addGui( gui, customisationName );

    // Return the created gui if any
    return gui;
 }

// Set the main window title (default to 'QEGui' if no title supplied)
void MainWindow::setTitle( QString title )
{
    if( title.isEmpty() )
    {
        startupParams* params = app->getParams();
        if( !params->applicationTitle.isEmpty() )
        {
            setWindowTitle( params->applicationTitle );
        }
        else
        {
            setWindowTitle( "QEGui" );
        }
    }
    else
    {
        setWindowTitle( title );
    }
}

// Return the central widget if it is the tab widget, else return NULL
QTabWidget* MainWindow::getCentralTabs()
{
    // If no central widget, return NULL
    if( !centralWidget() )
        return NULL;

    QWidget* w = centralWidget();
    if( !w || QString( "QTabWidget").compare( w->metaObject()->className() ) )
        return NULL;
    else
        return (QTabWidget*)w;
}

// Return the central widget if it is a single gui, else return NULL
// Note, originally QEForm class did not implement QOBJECT so className() returned it's base class which was QScrollArea.
QEForm* MainWindow::getCentralGui()
{
    // If no central widget, return NULL
    if( !centralWidget() )
        return NULL;

    QWidget* w = centralWidget();
    if( !w || QString( "QTabWidget").compare( w->metaObject()->className() ) == 0 )
        return NULL;
    else
        return extractGui( w );
}

// Get the current gui if any
QEForm* MainWindow::getCurrentGui()
{
    // If using tabs, return the current tab if any
    if( usingTabs )
    {
        QTabWidget* tabs = getCentralTabs();
        if( tabs )
            return extractGui(tabs->currentWidget());
    }

    // Using a single window, return the gui
    else
    {
        QEForm* gui = getCentralGui();
        if( gui )
            return gui;
    }

    // No gui present
    return NULL;
 }

//=================================================================================
// Methods to manage the 'Windows' and 'Recent...' menus
//=================================================================================

// Build new 'Recent...' menu
// Used when creating a new main window
void MainWindow::buildRecentMenu()
{
    if( !recentMenu )
    {
        return;
    }

//    recentMenu->clear();

    QList<recentFile*> files = app->getRecentFiles();
    if (files.count() > 0){
        recentMenu->setEnabled(true);
    }
    for( int i = 0; i < files.count(); i++ )
    {
        recentMenu->addAction( files.at( i ) );
    }
}

// Build new 'Windows' menu
// Used when creating a new main window and there are already other main windows present with GUIs
void MainWindow::buildWindowsMenu()
{
    if( !windowMenu )
    {
        return;
    }

    // Add all current guis (except docked ones) to the 'windows' menu
    MainWindow* mw;
    int i = 0;

    while( (mw = app->getMainWindow( i )) )
    {
        windowMenu->setEnabled(true);
        for( int j = 0; j < mw->guiList.count(); j++ )
        {
            if( !mw->guiList[j].getIsDock() )
            {
                addWindowMenuAction( mw->guiList[j].getAction() );
            }
        }

        // Next main window
        i++;
    }
}

// Add a gui to a 'Recent...' menu
void MainWindow::addRecentMenuAction( QAction* action )
{
    if( !recentMenu )
    {
        return;
    }

    QAction* beforeAction = 0;
    if( recentMenu->actions().count() )
    {
        beforeAction = recentMenu->actions().at(0);
    }
    recentMenu->insertAction( beforeAction, action );
}

// Add a gui to a 'Window' menu
void MainWindow::addWindowMenuAction( QAction* action )
{
    if( !windowMenu )
    {
        return;
    }

    windowMenu->addAction( action );
}

//=================================================================================
// Methods to manage configurations
//=================================================================================

// The user has requested a save of the current configuration
void MainWindow::on_actionSave_Configuration_triggered()
{
    PersistanceManager* pm = profile.getPersistanceManager();
    startupParams* params = app->getParams();

    // Get the user selection
    saveDialog sd( pm->getConfigNames( params->configurationFile, QE_CONFIG_NAME ) );

    // Ensure scaling is consistent with the rest of the application's forms.
    QEScaling::applyToWidget( &sd );

    if ( sd.exec() == QDialog::Rejected )
    {
        return;
    }

    QString configName;
    if( sd.getUseDefault() )
    {
        configName = PersistanceManager::defaultName;
    }
    else if ( !sd.getName().isEmpty() )
    {
        configName = sd.getName();
    }
    else
    {
        // The OK button should only be enabled if there is a configuration selected
        sendMessage( "No configuration selected", "QEGui application. MainWindow::on_actionSave_Configuration_triggered()" );
        return;
    }

    // Give all main windows and top level QEForms (managed by this application) a unique identifier required for restoration
    int i = 0;
    MainWindow* mw;
    while( (mw = app->getMainWindow( i )) )
    {
        mw->identifyWindowAndForms( i );

        // Next main window
        i++;
    }

    // Ask the persistance manager to save the current configuration.
    // The persistance manager will signal all interested objects (including this application) that
    // they should present anything they wish to save.
    pm->save( params->configurationFile, QE_CONFIG_NAME, configName );
}

// The user has requested a restore of a saved configuration
void MainWindow::on_actionRestore_Configuration_triggered()
{
    PersistanceManager* pm = profile.getPersistanceManager();
    startupParams* params = app->getParams();

    // Get the list of restoration options
    bool hasDefault;
    QStringList configNames = pm->getConfigNames( params->configurationFile, QE_CONFIG_NAME, hasDefault );
    if( configNames.count() == 0 && !hasDefault )
    {
        QMessageBox::warning( this, "Configuration Restore", "There are no configurations available to restore." );
        return;
    }

    // Get the user selection
    restoreDialog rd( configNames, hasDefault );
    if ( rd.exec() == QDialog::Rejected )
    {
        return;
    }

    QString configName;
    if( rd.getUseDefault() )
    {
        configName = PersistanceManager::defaultName;
    }
    else if( !rd.getName().isEmpty() )
    {
        configName = rd.getName();
    }
    else
    {
        // The OK button should only be enabled if there is a configuration selected
        sendMessage( "No configuration selected", "QEGui application. MainWindow::on_actionRestore_Configuration_triggered()" );
        return;
    }

    // Close all current windows
    closeAll();

    // Ask the persistance manager to restore a configuration.
    // The persistance manager will signal all interested objects (including this application) that
    // they should collect and apply restore data.
    PersistanceManager* persistanceManager = profile.getPersistanceManager();
    persistanceManager->restore( params->configurationFile, QE_CONFIG_NAME, configName  );
}

// Manage the save/resore configurations
void MainWindow::on_actionManage_Configurations_triggered()
{
    PersistanceManager* pm = profile.getPersistanceManager();
    startupParams* params = app->getParams();

    // Get the list of configuration options
    bool hasDefault;
    QStringList configNames = pm->getConfigNames( params->configurationFile, QE_CONFIG_NAME, hasDefault );
    if( configNames.count() == 0 && !hasDefault )
    {
        QMessageBox::warning( this, "Configuration Management", "There are no configurations available to manage." );
        return;
    }

    // Present the dialog
    manageConfigDialog mcd( configNames, hasDefault );
    QObject::connect( &mcd, SIGNAL( deleteConfigs ( manageConfigDialog*, const QStringList ) ), this, SLOT( deleteConfigs(  manageConfigDialog*, const QStringList ) ) );
    mcd.exec();
}

// A save or restore has been requested (Probably by QEGui itself)
void MainWindow::saveRestore( SaveRestoreSignal::saveRestoreOptions option )
{
    PersistanceManager* pm = profile.getPersistanceManager();

    // Build a unique name based on the order of the main window in the main window list
    QString mainWindowName = QString( "QEGuiMainWindow_%1" ).arg( app->getMainWindowPosition( this ) );

    switch( option )
    {
        // Save the main window configuration
        case SaveRestoreSignal::SAVE:
            {
                // Start with the top level element - the main windows
                PMElement mw = pm->addNamedConfiguration( mainWindowName );
                mw.addValue( "CustomisationName", windowCustomisationName );
                mw.addValue( "Title", windowTitle() );
                // check if it has a window parent
                if (parent() != NULL){
                    QString className = parent()->metaObject()->className();
                    if (className == "MainWindow"){
                        mw.addValue( "WindowParent", ((QMainWindow*)parent())->windowTitle() );
                    }
                }

                PMElement id = mw.addElement( "Identity" );
                id.addAttribute( "id", uniqueId );

                PMElement geo = mw.addElement( "Geometry" );
                QRect r = geometry();
                geo.addAttribute( "X", r.x() );
                geo.addAttribute( "Y", r.y() );
                geo.addAttribute( "Width", r.width() );
                geo.addAttribute( "Height", r.height() );

                PMElement state =  mw.addElement( "State" );
                state.addAttribute( "Flags", windowState() );

                // Note which GUI is the current GUI. This is relevent if main window is displaying
                // more than one gui in a tab widget. Redundant but harmless if only one gui is present.
                QEForm* currentGui = getCurrentGui();
/*
                // Zai ??? workaround - put central widget first
                for( int i = 0; i < guiList.count(); i++ )
                {
                    if (!guiList[i].getIsDock()){
                        // assumption: central widget if not a dock - fix it for other cases: tabs
                        guiListItem centralUI = guiList[i];
                        guiList.removeAt(i);
                        guiList.prepend(centralUI);
                        break;
                    }
                }
*/
                // Save details for each GUI
                for( int i = 0; i < guiList.count(); i++ )
                {
                    QEForm* gui = guiList[i].getForm();
                    // Gui name and ID
                    PMElement form =  mw.addElement( "Gui" );
                    form.addAttribute( "Name", gui->getFullFileName() );
                    form.addAttribute( "ID", gui->getUniqueIdentifier() );

                    // Current gui boolean
                    if( gui == currentGui )
                    {
                        form.addAttribute( "CurrentGui", true );
                        form.addAttribute( "CentralWidgetVisibility", !centralWidget()->isHidden() );
                    }

                    // Macro substitutions, if any
                    QString macroSubs = profile.getMacroSubstitutions();
                    if( !macroSubs.isEmpty() )
                    {
                        form.addValue( "MacroSubstitutions", macroSubs );
                    }

                    // Window customisations, if any
                    QString customisationName = getCustomisationName( gui );
                    if( !customisationName.isEmpty() )
                    {
                        form.addValue( "CustomisationName", customisationName );
                    }

                    // Path list, if any
                    QStringList pathList = profile.getPathList();
                    for( int j = 0; j < pathList.count(); j++ )
                    {
                        PMElement pl = form.addElement( "PathListItem" );
                        pl.addAttribute( "Order", j );
                        pl.addValue( QString( "Path" ), pathList.at( j ) );
                    }

                    // If QEGui is managing the scrolling of the QEForm and has placed it in a scroll area,
                    // then note the scroll position
                    QScrollArea* sa = guiScrollArea( gui );
                    if( sa )
                    {
                        PMElement pos =  form.addElement( "Scroll" );
                        pos.addAttribute( "X", sa->horizontalScrollBar()->value() );
                        pos.addAttribute( "Y", sa->verticalScrollBar()->value() );
                    }

                    // Save the window presentation.
                    // The window may be presented within the main window's central widget (on its own
                    // or inside a tabbed widget with other GUIs), or the window may be presented as a docked window.
                    for( int i = 0; i < guiList.count(); i++ )
                    {
                        if( guiList[i].getForm() == gui )
                        {
                            // If GUI is in a dock...
                            if( guiList[i].getIsDock() )
                            {
                                // Set presentation as a dock
                                form.addValue( "Presentation", QString( "Dock" ));

                                // Save Docking info
                                QDockWidget* dock = getGuiDock( gui );
                                if( dock )
                                {
                                    PMElement docking =  form.addElement( "Docking" );
                                    docking.addAttribute( "Title", dock->windowTitle() );
                                    docking.addAttribute( "AllowedAreas", dock->allowedAreas() );
                                    docking.addAttribute( "Area", dockWidgetArea( dock ) );
                                    docking.addAttribute( "Features", dock->features() );
                                    docking.addAttribute( "Floating", dock->isFloating() );
                                    docking.addAttribute( "X", dock->x() );
                                    docking.addAttribute( "Y", dock->y() );
                                    docking.addAttribute( "Width", dock->width() );
                                    docking.addAttribute( "Height", dock->height() );
                                    docking.addAttribute( "Hidden", !dock->isVisible() );
                                    QList<QDockWidget *> tabbedDocks = tabifiedDockWidgets ( dock );
                                    if( tabbedDocks.count() )
                                    {
                                        docking.addAttribute( "Tabbed", true );
                                    }
                                }
                            }

                            // GUI is not in a dock. Determine if GUI is in a tab or not
                            else
                            {
                                // Count GUIs in central area (not docks)
                                // Only count enough to check if there is more than one
                                int count = 0;
                                for( int i = 0; i < guiList.count(); i++ )
                                {
                                    if( !guiList[i].getIsDock() )
                                    {
                                        count++;
                                        if( count > 1 )
                                        {
                                            break;
                                        }
                                    }
                                }

                                // If more than one, GUI is in a TAB
                                if( count > 1 )
                                {
                                    form.addValue( "Presentation", QString( "Tab" ));
                                    break;
                                }

                                // Only one GUI. GUI is on its own in the central window
                                else
                                {
                                    form.addValue( "Presentation", QString( "Central" ));
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            break;


        // First restore phase.
        // This main window will position itself and create the GUIs it contains
        case SaveRestoreSignal::RESTORE_APPLICATION:
            {
                QString windowTitle;
                // If this window is marked for deletion, (deleteLater() has been called on it when closing
                // all current windows before restoring a configuration) then do nothing
                if( beingDeleted )
                    return;

                // If not restoring, do nothing
                if( !pm->isRestoring() )
                {
                    return;
                }

                // Get the data for this window
                PMElement data = pm->getNamedConfiguration( mainWindowName );

                // If none, do nothing
                if( data.isNull() )
                {
                    return;
                }

                data.getValue( "Title", windowTitle );
                QString winCustomisationName;
                data.getValue( "CustomisationName", winCustomisationName );

                PMElement id = data.getElement( "Identity" );
                id.getAttribute( "id", uniqueId );

                PMElement geometry = data.getElement( "Geometry" );
                int x, y, w, h;
                if( geometry.getAttribute( "X", x ) &&
                    geometry.getAttribute( "Y", y ) &&
                    geometry.getAttribute( "Width", w ) &&
                    geometry.getAttribute( "Height", h ) )
                {
                    // Set the geometry in a timer event. This will occur after events due to creation have finished arriving.
                    setGeomRect = QRect( x, y, w, h );
                    QTimer::singleShot(10, this, SLOT(setGeom()));
                }

                // Get the state of the window (iconised, maximised, etc
                PMElement pos = data.getElement( "State" );
                int flags;
                if( pos.getAttribute( "Flags", flags ) )
                {
                      setWindowState( (Qt::WindowStates)flags );
                }

                // Get a list of guis
                PMElementList guiElements = data.getElementList( "Gui" );

                QEForm* currentGui = NULL;

                // If there are no GUIs in the this window, apply the default customisation
                if(  guiElements.count() == 0 )
                {
                    setDefaultCustomisation();
                }

                // Create all the guis required for this main window
                for(int i = 0; i < guiElements.count(); i++ )
                {
                    PMElement guiElement = guiElements.getElement( i );
                    {
                        QString macroSubstitutions;
                        guiElement.getValue( "MacroSubstitutions", macroSubstitutions );

                        // Restore the path list
                        PMElementList pl = guiElement.getElementList( "PathListItem" );
                        QVector<QString> paths( pl.count() );
                        for( int j = 0; j < pl.count(); j++ )
                        {
                            PMElement ple = pl.getElement( j );
                            int order;
                            if( ple.getAttribute( "Order", order ) )
                            {
                                QString path;
                                if( ple.getValue( "Path", path ) )
                                {
                                    paths[order] = path;
                                }
                            }
                        }

                        QStringList pathList;
                        for( int j = 0; j < paths.size(); j++  )
                        {
                            pathList.append( paths[j] );
                        }

                        // All normal window creation is over. Swap currently published profile with profile to restore under
                        profile.releaseProfile();
                        profile.setupLocalProfile( profile.getGuiLaunchConsumer(), pathList, profile.getParentPath(), macroSubstitutions );
                        profile.publishOwnProfile();

                        // Create the GUI
                        QString name;
                        if( guiElement.getAttribute( "Name", name ) )
                        {
                            QString restoreId;
                            guiElement.getAttribute( "ID", restoreId );

                            QString customisationName;
                            guiElement.getValue( "CustomisationName", customisationName );

                            QString presentation;
                            guiElement.getValue( "Presentation", presentation );

                            bool isDock = !presentation.compare( "Dock" );
                            QEForm* gui = createGui( name, customisationName, restoreId, isDock );

                            // If no presentation, assume the first gui is a central gui and any more are tabs.
                            // (in which case the first was also a tab and will be converted to a tab when the second tab is added)
                            if( presentation.isEmpty() )
                            {
                                if( i == 0 )
                                {
                                    presentation = QString( "Central" );
                                }
                                else
                                {
                                    presentation = QString( "Tab" );
                                }
                            }

                            // If the gui is the central widget, create it as such
                            if( presentation.compare( "Central" ) == 0 )
                            {
                                // Load the gui into the main window
                                loadGuiIntoCurrentWindow( gui, false );
                                bool visibility;
                                guiElement.getAttribute("CentralWidgetVisibility", visibility);
                                if (!visibility) {
                                    centralWidget()->hide();
                                    QMap<QString, windowCustomisationMenuItem*>::const_iterator items_i = customisationInfo.menuItems.find("ToggleCenterWidget");
                                    if (items_i != customisationInfo.menuItems.end()){
                                        windowCustomisationMenuItem* actionItem = items_i.value();
                                        actionItem->setChecked(false);
                                    }
                                }
                            }

                            // If the gui is a tab, create it as such
                            else if( presentation.compare( "Tab" ) == 0 )
                            {
                                if( gui )
                                {
                                    // If not using tabs, start tabs and migrate any single gui to the first tab
                                    if( !usingTabs )
                                        setTabMode();

                                    // Create gui as a new tab
                                    loadGuiIntoNewTab( gui );
                                }
                            }

                            // If the gui is a dock, create it as such
                            else if( presentation.compare( "Dock" ) == 0 )
                            {
                                if( gui )
                                {
                                    QString title;
                                    PMElement docking = guiElement.getElement( "Docking" );

                                    int allowedAreas = Qt::AllDockWidgetAreas;
                                    int features = QDockWidget::AllDockWidgetFeatures;
                                    bool floating = false;

                                    int x = 0;
                                    int y = 0;
                                    int width = 100;
                                    int height = 100;
                                    int area = Qt::BottomDockWidgetArea;
                                    bool hidden = false;
                                    bool tabbed = false;
                                    docking.getAttribute( QString( "Title" ), title );
                                    docking.getAttribute( QString( "AllowedAreas" ), allowedAreas );
                                    docking.getAttribute( QString( "Area" ), area );
                                    docking.getAttribute( "Features", features );
                                    docking.getAttribute( "Floating", floating );
                                    docking.getAttribute( "X", x );
                                    docking.getAttribute( "Y", y );
                                    docking.getAttribute( "Width", width );
                                    docking.getAttribute( "Height", height );
                                    docking.getAttribute( "Hidden", hidden );
                                    docking.getAttribute( "Tabbed", tabbed );

                                    // Create gui as a new dock
                                    QEActionRequests::Options createOption;
                                    if( floating )
                                    {
                                        createOption = QEActionRequests::OptionFloatingDockWindow;
                                    }
                                    else
                                    {
                                        createOption = dockLocationToCreationOption( (Qt::DockWidgetArea)area );
                                    }
                                    loadGuiIntoNewDock( title, gui, hidden, tabbed, createOption, (Qt::DockWidgetArea)allowedAreas, (QDockWidget::DockWidgetFeature)features, QRect( x, y, width, height ) );
                                }

                                // If not a dock...
                                if( presentation.compare( "Dock" ) )
                                {
                                    // Note if this gui is the current gui
                                    bool currentGuiFlag = false;
                                    guiElement.getAttribute( "CurrentGui", currentGuiFlag ) ;
                                    if( currentGuiFlag )
                                    {
                                        currentGui = gui;
                                    }

                                    // Note any scroll position
                                    // This scroll position will be applied by scrollTo(), which is a timer event
                                    // that will be run after the restored geometry has been applied.
                                    // (Note, the gui has just been placed at the end of the gui list)
                                    PMElement scroll = guiElement.getElement( "Scroll" );
                                    int x, y;
                                    if( scroll.getAttribute( "X", x ) &&
                                        scroll.getAttribute( "Y", y ) )
                                    {
                                        guiList.last().setScroll( QPoint( x, y ) );
                                    }
                                }
                            }
                        }
                    }
                }

                // If this new GUI is the current one, make it so
                if( currentGui )
                {
                    raiseGui( currentGui );
                }
                setWindowTitle(windowTitle);
            }
            break;

        // Second restore phase.
        // This main window has done it's work. The widgets that have been created will be able to act on the second phase
        case SaveRestoreSignal::RESTORE_QEFRAMEWORK:
            break;
    }
}

// Delete a configuration
void MainWindow::deleteConfigs( manageConfigDialog* mcd, const QStringList names )
{
    PersistanceManager* pm = profile.getPersistanceManager();
    startupParams* params = app->getParams();

    pm->deleteConfigs( params->configurationFile, QE_CONFIG_NAME, names );
    mcd->setCurrentNames( pm->getConfigNames( params->configurationFile, QE_CONFIG_NAME ) );
}

//=================================================================================

// Function to close all main windows.
// Used when restoring a configuration
void MainWindow::closeAll()
{
    // Queue all windows for closure.
    // Need to use deleteLater() as this function is usually called from an event from one of the windows
    // Also, hide the widget from all code by removing it from the main window list
    MainWindow* mw;
    while( (mw = app->getMainWindow( 0 )) )
    {
        mw->beingDeleted = true;

        app->removeMainWindow( 0 );
        //mw->deleteLater();
        mw->close();
    }
    app->mainWindowMap.clear();
}

// Return the scroll area a gui is in if it is in one.
// If a gui appears to be managing it's own resizing (such as having a
// layout manager for its top level widget or a scroll area as its top level
// widget, it is left to its own devices. If not, then QEGui places the QEForm
// widget presenting the gui within a scroll area. This function returns the
// scroll area added by QEGui if there is one.
QScrollArea* MainWindow::guiScrollArea( QEForm* gui )
{
    QWidget* w = gui->parentWidget();
    if( w && !QString::compare( w->metaObject()->className(), "QWidget" )  )
    {
        w = w->parentWidget();
        if( w && !QString::compare( w->metaObject()->className(), "QScrollArea" )  )
        {
            return (QScrollArea*)w;
        }
    }
    return NULL;
}

// Set a gui geometry.
// This is called as part of restoring a main window.
// When restoring a window under X11, setting its geometry and scroll position is complicated
// by the X11 window manager.
// At some time after window creation the window manager will add the window decorations
// (title bar, borders, etc)
// Since the position was saved with the window decorations the position needs to be restored
// after the window decorations have been added or the window will be moved to the wrong position
// when decorations are added.
// Since the X11 window manager operates asynchronously to the application the addition of the
// window decorations is infered when the origin of the window geometry frame geometry changes.
// This is reasonably robust, but would fail if there is no X11 window manager running, or
// some weird window manager is running which adds no decorations to the top or left of the
// window leaving both window geometry and frame geometry at the same position.
// To ensure a failure des not result in waiting forever, the wait is limited to a maximum.
void MainWindow::setGeom()
{
    // Loop until the X11 window manager has added window decorations.
    // When decorations have been added, position of window geometry and frame geometry will differ.
    // (Set the size regardless of decoration after maximum of 10 seconds)
    if(( geometry().x() == frameGeometry().x() )&&
       ( geometry().y() == frameGeometry().y() ) &&
       ( waitForX11WindowManagerCount < 1000 ) ) /* 1000 = 1000 x 10ms = 10 seconds */
    {
        QTimer::singleShot(10, this, SLOT( setGeom() ));
        waitForX11WindowManagerCount++;
        return;
    }
    waitForX11WindowManagerCount = 0;

    // Set the geometry as noted during the restore
    setGeometry( setGeomRect );

    // Initiate scrolling of guis within the main window
    QTimer::singleShot( 10, this, SLOT(scrollTo() ));
}

// Scroll all guis in a main window.
// This is called as part of restoring a main window.
// When restoring a window under X11, setting its geometry and scroll position is complicated
// by the X11 window manager.
// The X11 window manager services a request to resize the window some time after the request
// is made.
// Since the X11 window manager operates asynchronously to the application the resize of the
// window decorations is infered by the size changing to the required size.
// This is reasonably robust, but may fail if there is no X11 window manager running, or
// the display size has changed since the size was saved and the X11 window manager is not
// willing to set the size to that requested.
// To ensure a failure does not result in waiting forever, the wait is limited to a maximum.
void MainWindow::scrollTo()
{
    // Loop until the X11 window manager has set the size. (abandon scrolling after maximum of 10 seconds)
    if( setGeomRect.width() != width() ||
        setGeomRect.height() != height() )
    {
        if( waitForX11WindowManagerCount < 1000 )/* 1000 = 1000 x 10ms = 10 seconds */
        {
            QTimer::singleShot(10, this, SLOT( scrollTo() ));
            waitForX11WindowManagerCount++;
            return;
        }
        else
        {
            waitForX11WindowManagerCount = 0;
            return;
        }
    }

    // The window is ready for scrolling if required.
    for( int i = 0; i < guiList.count(); i++ )
    {
        // If QEGui is managing the scrolling of the QEForm and has placed it in a scroll area,
        // then note the scroll position
        QScrollArea* sa = guiScrollArea( guiList[i].getForm() );
        if( sa )
        {
            QPoint p = guiList[i].getScroll();
            sa->horizontalScrollBar()->setValue( p.x() );
            sa->verticalScrollBar()->setValue( p.y() );
        }
    }
}

// Remove all guis on a main window from the 'windows' menus of all main windows
// Used when deleting a main window
void MainWindow::removeAllGuisFromGuiList()
{
    for( int i = 0; i < guiList.count(); i++ )
    {
        removeGuiFromGuiList( i );
    }
}

// Remove a GUI from the application's list of GUIs
// Note, deleting the action will remove it from all the menus it was associated with
void MainWindow::removeGuiFromGuiList( QEForm* gui )
{
    for( int i = 0; i < guiList.count(); i++ )
    {
        if( guiList[i].getForm() == gui )
        {
            removeGuiFromGuiList( i );
            return;
        }
    }
}

// Remove a GUI from Windows menus.
// Note, deleting the action will remove it from all the menus it was associated with
void MainWindow::removeGuiFromGuiList( int i )
{
    // Avoid use of bad index
    if( guiList.count() <= i )
    {
        return;
    }

    // Delete the action. This will remove it from all the menus it was associated with
    guiList[i].deleteAction();
    guiList.removeAt( i );
}

// Get any customisation name for a GUI
QString MainWindow::getCustomisationName( QEForm* gui )
{
    for( int i = 0; i < guiList.count(); i++ )
    {
        if( guiList[i].getForm() == gui )
        {
            return guiList[i].getCustomisationName();
        }
    }
    return QString();
}

// Check if a GUI already exists in this main window (with matching macro substitutions) and ensure is visible and has focus
// Return true if found
bool MainWindow::showGui( QString guiFileName, QString macroSubstitutions )
{
    // Look for a form matching the gui name and with the same substitutions
    for( int i = 0; i < guiList.count(); i++ )
    {
        QEForm* form = guiList[i].getForm();
        if( !form->getFullFileName().compare( guiFileName ) &&
            !form->getMacroSubstitutions().trimmed().compare( macroSubstitutions ) )
        {
            // This code replaces the winding back up the widget hierarchy (below) looking for a tab widget and the MainWindow
            // This can be avoided now the guiList contains the MainWindow the gui is in, but this code isn't tested yet and a
            // quick run appeared to have problems: when opening many forms through a QEPushButton (when this is called) forms
            // ended up overlayed over each other in the same tab

            //                MainWindow* mw = guiList[i].getMainWindow();

            //                // If a tab widget is found, set the gui as the active tab
            //                QTabWidget* tw = mw->getCentralTabs();
            //                if( tw )
            //                {
            //                    int j;
            //                    j = tw->indexOf( form ) ;
            //                    if( j < 0 )
            //                    {
            //                        j = tw->indexOf( form->parentWidget() ) ;
            //                        if( j < 0 )
            //                        {
            //                            j = tw->indexOf( form->parentWidget()->parentWidget() ) ;
            //                        }
            //                    }
            //                    if( j >= 0 )
            //                    {
            //                        tw->setCurrentIndex( j );
            //                    }
            //                }

            //                // Display the main window
            //                mw->show();
            //                mw->raise();
            //                mw->activateWindow();
            //                return;


            // GUI found. Roll back up the widget hierarchy.
            // If a parent tab widget is found, set the child as the active tab, when the main window is found, display it.
            QWidget* w = form->parentWidget();
            while( w )
            {
                // Ensure the correct tab is selected
                if( QString::compare( w->metaObject()->className(), "QTabWidget" ) == 0 )
                {
                    QTabWidget* tw = (QTabWidget*)w;
                    int j;
                    j = tw->indexOf( form ) ;
                    if( j < 0 )
                    {
                        j = tw->indexOf( form->parentWidget() ) ;
                        if( j < 0 )
                        {
                            j = tw->indexOf( form->parentWidget()->parentWidget() ) ;
                        }
                    }
                    if( j >= 0 )
                    {
                        tw->setCurrentIndex( j );
                    }
                }

                // Display the main window
                if( QString::compare( w->metaObject()->className(), "MainWindow" ) == 0 )
                {
                    w->show();
                    w->raise();
                    w->activateWindow();

                    return (MainWindow*)w;
                }

                // Move up a generation
                w = w->parentWidget();
            }

            // GUI found, but could not locate the GUI in the widget hierarchy
            return NULL;
        }
    }

    // Gui not found
    return NULL;
}

// Ensure the main window and all its QEForms (top level forms only) have a unique identifier
void MainWindow::identifyWindowAndForms( int mwIndex )
{
    // Give the main window a unique ID for restoration purposes
    setUniqueId( mwIndex );

    // Give all top level QEForms (managed by this application - not sub forms) a unique ID for restoration purposes
    for( int i = 0; i < guiList.count(); i++ )
    {
        // Get form and main window details
        QString name = QString("QEGui_window_%1_form_%2" ).arg( getUniqueId() ).arg( i );
        guiList[i].getForm()->setUniqueIdentifier( name );
    }
}

// Determine the dock widget containing a docked GUI
// (Or NULL if not found)
QDockWidget* MainWindow::getGuiDock( QWidget* gui )
{
    // Look for doc widget containing GUI
    QWidget* child = gui;
    QWidget* parent;
    while( (parent = child->parentWidget()) )
    {
        if( QString( "QDockWidget").compare( parent->metaObject()->className() ) == 0 )
        {
            return (QDockWidget*)parent;
        }
        child = parent;
    }
    return NULL;
}

void MainWindow::replaceMenuAction(QString actionName, QAction* newAction){
    QMap<QString, QMenu*>::const_iterator menu_i = customisationInfo.actions.find(actionName);
    if (menu_i == customisationInfo.actions.end()) return;
    QMenu* menu = menu_i.value();
    int actionIndex = 0;
    bool found = false;
    QList<QAction*> actionlist = menu->actions();
    // find action location
    for ( int i = 0; i < actionlist.length(); i++ ){
        if (actionlist.at(i)->text() == actionName){
            actionIndex = i;
            found = true;
            break;
        }
    }
    if (!found) return;
    // delete all actions
    for ( int i = 0; i < actionlist.length(); i++ ){
        menu->removeAction(actionlist.at(i));
    }
    // update actions
    for ( int i = 0; i < actionlist.length(); i++ ){
        if (i == actionIndex){
            menu->addAction(newAction);
        }
        else{
            menu->addAction(actionlist.at(i));
        }
    }
}

void MainWindow::tabifyDocks(){
    if (profile.getPersistanceManager()->isRestoring()){
        return;
    }
    // tabify dock widgets
    for ( int i = 0; i < customisationInfo.dockLeftWidgetList.length() - 1; i++ ){
        tabifyDockWidget(customisationInfo.dockLeftWidgetList.at(i), customisationInfo.dockLeftWidgetList.at(i+1));
    }
    if (customisationInfo.dockLeftWidgetList.length() > 1){
        customisationInfo.dockLeftWidgetList.at(0)->raise();
    }
    for ( int i = 0; i < customisationInfo.dockRightWidgetList.length() - 1; i++ ){
        tabifyDockWidget(customisationInfo.dockRightWidgetList.at(i), customisationInfo.dockRightWidgetList.at(i+1));
    }
    if (customisationInfo.dockRightWidgetList.length() > 1){
        customisationInfo.dockRightWidgetList.at(0)->raise();
    }
    for ( int i = 0; i < customisationInfo.dockTopWidgetList.length() - 1; i++ ){
        tabifyDockWidget(customisationInfo.dockTopWidgetList.at(i), customisationInfo.dockTopWidgetList.at(i+1));
    }
    if (customisationInfo.dockTopWidgetList.length() > 1){
        customisationInfo.dockTopWidgetList.at(0)->raise();
    }
    for ( int i = 0; i < customisationInfo.dockBottomWidgetList.length() - 1; i++ ){
        tabifyDockWidget(customisationInfo.dockBottomWidgetList.at(i), customisationInfo.dockBottomWidgetList.at(i+1));
    }
    if (customisationInfo.dockBottomWidgetList.length() > 1){
        customisationInfo.dockBottomWidgetList.at(0)->raise();
    }
}

// Add the named customisation to a main window.
// Return true if named customisation found and loaded.
void MainWindow::applyCustomisation( QString customisationName, windowCustomisationInfo* customisationInfo, bool clearExisting, bool restoring )
{
    // Clear the existing customisation if requested (but only if we have a customisation name to replace it with)
    if( !customisationName.isEmpty() && clearExisting )
    {
        // Remove all current menus
        menuBar()->clear();
        menuBar()->setVisible( false );

        // Remove all current toolbars
        foreach (QToolBar* toolBar, customisationInfo->toolbars)
        {
            removeToolBar( toolBar );
            delete toolBar;
        }
        // clear all
        customisationInfo->clear();
    }

    // Get the customisations required
    // Do nothing if not found
    windowCustomisation* customisation = app->getCustomisation( customisationName );
    if (!customisation)
    {
        return;
    }

//    // Create the toolbar
//    QToolBar* mainToolBar = new QToolBar( "Toolbar", mw );
//    mainToolBar->setObjectName(QString::fromUtf8( "mainToolBar" ));
//    mw->addToolBar(Qt::TopToolBarArea, mainToolBar);

//    // Add the required toolbar buttons
//    QList<windowCustomisationButtonItem*> bList = customisation->getButtons();
//    for ( int i = 0; i < bList.length(); i++ )
//    {
//        windowCustomisationButtonItem* item = new windowCustomisationButtonItem(bList.at(i));

//        // add button action
//        mainToolBar->addAction(item);

//        // Set up an action to respond to the user
//        QObject::connect( item, SIGNAL( newGui( const QEActionRequests& ) ),
//                          mw, SLOT( requestAction( const QEActionRequests& ) ) );
//    }

    // Get the menu item customisations required
    QList<windowCustomisationMenuItem*> mList = customisation->getMenuItems();

    // Apply all the menu customisations
    for (int i = 0; i < mList.length(); i++)
    {
        // Get the next customisation required
        windowCustomisationMenuItem* menuItem = new windowCustomisationMenuItem( mList.at(i) );

        // Ensure the menu hierarchy is present.
        // For example if the hierarchy required is 'File' -> 'Recent' is required and a 'File' menu is
        // present but it does not contain a 'Recent' menu, then create a 'Recent' menu in the 'File' menu
        QMenu* menu = buildMenuPath( customisationInfo, menuBar(), menuItem->getMenuHierarchy() );

        // Act on the type of customisation required
        switch( menuItem->getType() )
        {
            case windowCustomisationMenuItem::MENU_UNKNOWN:
                break;

            case windowCustomisationMenuItem::MENU_ITEM:
                // Add the item to the correct menu
                // (if no menu, don't add to the menu bar - this could change)                
                if( menu )
                {   // Enable when add an item to placeholder
                    if (customisationInfo->placeholderMenus.contains(menu->title())){
                        menu->setEnabled(true);
                    }
                    if( menuItem->hasSeparator() )
                    {
                        menu->addSeparator();
                    }
                    // Zai - create a dock immediately if one dock view but not a restoring process
                    if (isDockView(menuItem) && !restoring){
                        // create the doce and add view action
                        menu->addAction(createDockWidget(menuItem));
                        customisationInfo->actions.insert(menuItem->getTitle(), menu);
                        customisationInfo->items.append( menuItem );
                        break;
                    }
                    menu->addAction( menuItem );
                    customisationInfo->actions.insert(menuItem->getTitle(), menu);
                    customisationInfo->items.append( menuItem );
                    customisationInfo->menuItems.insert( menuItem->getTitle(), menuItem );

                    // Set up an action to respond to the user
                    QObject::connect( menuItem, SIGNAL( newGui( const QEActionRequests& ) ),
                                      this, SLOT( requestAction( const QEActionRequests& ) ) );
                }
                break;

            case windowCustomisationMenuItem::MENU_PLACEHOLDER:
                {
                    QMenu* placeholderMenu;
                    QString menuTitle = menuItem->getTitle();

                    // Add the placeholder to the menu if there is one
                    if( menu )
                    {
                        if( menuItem->hasSeparator() )
                        {
                            menu->addSeparator();
                        }
                        placeholderMenu = menu->addMenu( menuTitle );
                    }
                    // If no menu, add the placeholder to the menu bar
                    else
                    {
                        placeholderMenu = menuBar()->addMenu( menuTitle );
                        menuBar()->setVisible( true );
                        // Disabled when created as no item yet
                        placeholderMenu->setEnabled(false);
                    }
                    customisationInfo->placeholderMenus.insert( menuTitle, placeholderMenu );

                    // Save the menu for some future menu search
                    customisationInfo->menus.insert( menuTitle, placeholderMenu );
                }
                break;

            case windowCustomisationMenuItem::MENU_BUILT_IN:
                if (menu){
                    if( menuItem->hasSeparator() )
                    {
                        menu->addSeparator();
                    }
                    menu->addAction( menuItem );
                    // if toggle required
                    if (menuItem->getBuiltInAction() == "ToggleCenterWidget"){
                        menuItem->setCheckable(true);
                        menuItem->setChecked(true);
                    }
                    QObject::connect( menuItem, SIGNAL( newGui( const QEActionRequests& ) ),
                                      this, SLOT( requestAction( const QEActionRequests& ) ) );

                    customisationInfo->items.append( menuItem );
                    customisationInfo->actions.insert(menuItem->getTitle(), menu);
                    customisationInfo->menuItems.insert( menuItem->getBuiltInAction(), menuItem );
                }
                else{
                    // add builtin menu from widgets - eg Image widget
                    menuBar()->addAction(menuItem);
                    QObject::connect( menuItem, SIGNAL( newGui( const QEActionRequests& ) ),
                                      this, SLOT( requestAction( const QEActionRequests& ) ) );

                    customisationInfo->items.append( menuItem );
                    customisationInfo->menuItems.insert( menuItem->getBuiltInAction(), menuItem );
                }
                break;
        }
    }
    userLevelChangedGeneral(getUserLevel());
    // initial layout lock status
    QMap<QString, windowCustomisationMenuItem*>::const_iterator items_i = customisationInfo->menuItems.find("LockLayout");
    bool lockStatus = getLockLayoutStatus();
    if (items_i != customisationInfo->menuItems.end()){
        windowCustomisationMenuItem* actionItem = items_i.value();
        actionItem->setCheckable(true);
        actionItem->setChecked(lockStatus);
    }
}

// Ensure a menu path exists in the menu bar.
// For example, if the menu path required is 'File' -> 'Edit', and a
// 'File' menu exists but does not contain an 'Edit' menu, then add an 'Edit' menu to the 'File' menu.
// Return the end menu (newly created, or found)
QMenu* MainWindow::buildMenuPath( windowCustomisationInfo* customisationInfo, QMenuBar* menuBar, const QStringList menuHierarchy )
{
    // Work through the anticipated menu hierarchy creating missing menus as required
    QMenu* menuPoint = NULL;
    QString hierarchyString;
    int count = menuHierarchy.count();
    bool skipSearch = false;
    for( int i = 0; i < count; i++ )
    {
        // Add the next menu level
        hierarchyString.append( menuHierarchy.at(i) );

        // Look for the next menu level
        QMenu* menu = NULL;
        if( !skipSearch )
        {
            menu = customisationInfo->menus.value( hierarchyString, NULL );
        }

        // If the next menu level is present, note it
        if( menu )
        {
            menuPoint = menu;
        }

        // The next menu level is not present, create it
        else
        {
            // No point looking for deeper menus from now on
            skipSearch = true;

            // Create the menu
            QMenu* newMenu = new QMenu( menuHierarchy.at(i) );

            // Add it to the next level up (if any)
            if( menuPoint )
            {
                // add the menu
                menuPoint->addMenu( newMenu );
            }

            // Otherwise add it to the menu bar
            else
            {
                menuBar->addMenu( newMenu );
                menuBar->setVisible( true );
            }

            // Save the menu just added as the menu to add the next menu to in this loop
            menuPoint = newMenu;

            // Save the menu for some future menu search
            customisationInfo->menus.insert( hierarchyString, newMenu );
        }

        // Add the menu separator.
        // Note, this is done so things are easier in the debugger (File>Edit is
        // clearly two levels of menu compared to FileEdit). Also, it may help
        // avoid conflicts; for example if there is a 'File' menu containing an
        // 'Edit' menu and there is also a top level menu is called 'FileEdit'.
        if( i < count - 1 )
        {
            hierarchyString.append( ">" );
        }
    }

    return menuPoint;
}

bool MainWindow::isDockView(windowCustomisationMenuItem* item){
    QEActionRequests::Options option = item->getWindows().at(0).creationOption;
    bool isDock = (option == QEActionRequests::OptionLeftDockWindow ||
            option == QEActionRequests::OptionRightDockWindow ||
            option == QEActionRequests::OptionTopDockWindow ||
            option == QEActionRequests::OptionBottomDockWindow ||
            option == QEActionRequests::OptionFloatingDockWindow);
    if (option == QEActionRequests::OptionFloatingDockWindow){
        return false;
    }
    if ((item->getWindows().count() == 1) && isDock)
    {
        return true;
    }
    return false;
}

QAction* MainWindow::createDockWidget(windowCustomisationMenuItem* item)
{
    profile.addPriorityMacroSubstitutions( item->getWindows().at(0).macroSubstitutions );

    QEActionRequests::Options createOption = item->getWindows().at(0).creationOption;
    QString customisationName = item->getWindows().at(0).customisationName;
    bool hidden = item->getWindows().at(0).hidden;
    QEForm* gui = createGui( item->getWindows().at(0).uiFile, customisationName, true );

    profile.removePriorityMacroSubstitutions();

    return loadGuiIntoNewDock( item->getWindows().at(0).title, gui, hidden, false, createOption );
}

void MainWindow::userLevelChangedGeneral( userLevelTypes::userLevels level ){
    int count = customisationInfo.items.count();
    for ( int i = 0; i < count; i++){
        windowCustomisationMenuItem* item = customisationInfo.items.at(i);
        if (level >= item->getUserLevel()){
            customisationInfo.items.at(i)->setEnabled(true);
        }
        else{
            customisationInfo.items.at(i)->setEnabled(false);
        }
    }
}

void MainWindow::lockLayoutStatusChanged( bool locked ){
    // apply user level restrictions on docks
    QDockWidget::DockWidgetFeatures features = QDockWidget::NoDockWidgetFeatures;
    if (!locked){
        // default policy for Docks
        features = QDockWidget::AllDockWidgetFeatures;
    }
    for ( int i = 0; i < customisationInfo.dockLeftWidgetList.length(); i++ ){
        customisationInfo.dockLeftWidgetList.at(i)->setFeatures(features);
    }
    for ( int i = 0; i < customisationInfo.dockRightWidgetList.length(); i++ ){
       customisationInfo.dockRightWidgetList.at(i)->setFeatures(features);
    }
    for ( int i = 0; i < customisationInfo.dockTopWidgetList.length(); i++ ){
        customisationInfo.dockTopWidgetList.at(i)->setFeatures(features);
    }
    for ( int i = 0; i < customisationInfo.dockBottomWidgetList.length(); i++ ){
        customisationInfo.dockBottomWidgetList.at(i)->setFeatures(features);
    }
    for ( int i = 0; i < customisationInfo.dockFloatingWidgetList.length(); i++ ){
        customisationInfo.dockFloatingWidgetList.at(i)->setFeatures(features);
    }
}

void MainWindow::showEvent ( QShowEvent* ){
    // restore dock min size settings
    if (forceDockSize){
        for ( int i = 0; i < customisationInfo.dockLeftWidgetList.length(); i++ ){
            customisationInfo.dockLeftWidgetList.at(i)->setMinimumWidth(0);
            customisationInfo.dockLeftWidgetList.at(i)->setMinimumHeight(0);
        }
        for ( int i = 0; i < customisationInfo.dockRightWidgetList.length(); i++ ){
            customisationInfo.dockRightWidgetList.at(i)->setMinimumWidth(0);
            customisationInfo.dockRightWidgetList.at(i)->setMinimumHeight(0);
        }
        for ( int i = 0; i < customisationInfo.dockTopWidgetList.length(); i++ ){
            customisationInfo.dockTopWidgetList.at(i)->setMinimumWidth(0);
            customisationInfo.dockTopWidgetList.at(i)->setMinimumHeight(0);
        }
        for ( int i = 0; i < customisationInfo.dockBottomWidgetList.length(); i++ ){
            customisationInfo.dockBottomWidgetList.at(i)->setMinimumWidth(0);
            customisationInfo.dockBottomWidgetList.at(i)->setMinimumHeight(0);
        }
        forceDockSize = false;
    }
}
