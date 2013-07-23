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

#include <QtGui>
#include <QDebug>
#include <QString>

#include <MainWindow.h>
#include <QEForm.h>
#include <QEFrameworkVersion.h>
#include <QECommon.h>
#include <QMessageBox>
#include <ContainerProfile.h>
#include <QVariant>
#include <saveDialog.h>
#include <restoreDialog.h>
#include <PasswordDialog.h>
#include <QEGui.h>

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
MainWindow::MainWindow(  QEGui* appIn, QString fileName, bool openDialog, QWidget *parent )  : QMainWindow( parent )
{
    app = appIn;

    tabMenu = NULL;

    // A published profile should always be available, but the various signal consumers will always be either NULL (if the
    // profile was set up by the QEGui application) or objects in another main window (if the profile was published by a button in a gui)
    // Replace the signal consuming objects
    profile.updateConsumers( this );

    // Initialise
    usingTabs = false;
//    nativeSize = QSize( 0, 0 );

    beingDeleted = false;
    scrollToCount = 0;

    // Give the main window's USerMessage class a unique form ID so only messages from
    // the form in each main window are displayed that main window's status bar
    setFormId( getNextMessageFormId() );
    setFormFilter( MESSAGE_FILTER_MATCH );
    setSourceFilter( MESSAGE_FILTER_NONE );

    // Present the main form's ui
    ui.setupUi( this );

    // Apply scaling to main window proper.
    //
    QEUtilities::adjustWidgetScale( this, int (app->getParams()->adjustScale) , 100 );

    // Setup to allow user to change focus to a window from the 'Windows' menu
    QObject::connect( ui.menuWindows, SIGNAL( triggered( QAction* ) ), this, SLOT( onWindowMenuSelection( QAction* ) ) );

    // Setup to respond to requests to save or restore persistant data
    PersistanceManager* persistanceManager = profile.getPersistanceManager();
    QObject::connect( persistanceManager->getSaveRestoreObject(), SIGNAL( saveRestore( SaveRestoreSignal::saveRestoreOptions ) ), this, SLOT( saveRestore( SaveRestoreSignal::saveRestoreOptions ) ), Qt::DirectConnection );

    // Save this instance of a main window in the global list of main windows
    app->addMainWindow( this );

//    ui.menuUser_Level->add
    // Set the default title
    setTitle( "" );

    // Populate the 'windows' menu to include all current guis in any main window
    buildWindowsMenu();

    // Enable the edit menu if requested
    ui.menuEdit->setEnabled( app->getParams()->enableEdit  );

    // Hide the main tool bar (nothing in it yet)
    ui.mainToolBar->hide();

    // Hide the menu bar if not required
    if( app->getParams()->disableMenu )
        ui.menuBar->hide();

    // If no filename was supplied, and an 'Open...' dialog is required, open the file selection dialog
    // Do it after the creation of the main window is complete
    if( fileName.isEmpty() && openDialog )
    {
        QTimer::singleShot( 0, this, SLOT(on_actionOpen_triggered()));
    }

    // If a filename was supplied, load it
    else
    {
        QEForm* gui = createGui( fileName ); // A profile should have been published before calling this constructor.
        loadGuiIntoCurrentWindow( gui, true );
    }

    // Set up signals for starting the 'designer' process
    QObject::connect( &process, SIGNAL(error(QProcess::ProcessError)), this, SLOT( processError(QProcess::ProcessError) ) );
    QObject::connect( &processTimer, SIGNAL(timeout()), this, SLOT( startDesignerAlternate() ) );

    // Ensure this class destructor gets called.
    setAttribute ( Qt::WA_DeleteOnClose );

    // Setup the main window icon
    setWindowIcon( QIcon (":/icons/QEGuiIcon.png" ));

    // Ensure no widget in the loaded gui has focus (and therefore will not update)
    setFocus();

    // Restore (will only do anything if this main window is being created during a restore)
    saveRestore( SaveRestoreSignal::RESTORE_APPLICATION );
}

// Destructor
MainWindow::~MainWindow()
{
    // Remove the GUIs shown in this main window from the GUIs listed in the 'Windows' menus of all other main windows
    removeAllGuisFromWindowsMenu();

    // Remove this main window from the global list of main windows
    // Note, this may have already been done to hide the the main window if deleting using deleteLater()
    app->removeMainWindow( this );
}

//=================================================================================
// Methods for responding to user actions
//=================================================================================

// Open a gui in a new window.
// Present a file open dialog box and after generate the gui based on the ui file the user selects
void MainWindow::on_actionNew_Window_triggered()
{
    profile.publishOwnProfile();
    MainWindow* w = new MainWindow( app, "", true );
    profile.releaseProfile();
    w->show();
}

// Open a gui in a new tab.
// Present a file open dialog box and after generate the gui based on the ui file the user selects
void MainWindow::on_actionNew_Tab_triggered()
{
    // If not using tabs, start tabs and migrate any single gui to the first tab
    if( !usingTabs )
        setTabMode();

    // Create the GUI
    profile.publishOwnProfile();
    QEForm* gui = createGui( GuiFileNameDialog( "Open" ) );
    profile.releaseProfile();
    loadGuiIntoNewTab( gui );
}

// User requested a new gui to be opened
// Present a file open dialog box and after generate the gui based on the ui file the user selects
void MainWindow::on_actionOpen_triggered()
{
    // Create the GUI
    profile.publishOwnProfile();
    QEForm* gui = createGui( GuiFileNameDialog( "Open" ) );
    profile.releaseProfile();
    loadGuiIntoCurrentWindow( gui, true );
}

// Close a gui
void MainWindow::on_actionClose_triggered()
{
    // If using tabs, delete the currenttly selected tab
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
        QEForm* gui = getCentralGui();
        if( gui )
        {
            removeGuiFromWindowsMenu( gui );
            setCentralWidget( new QWidget() );
        }

        setTitle( "" );
    }
 }

// Standard GUIs available from the 'File' menu
void MainWindow::on_actionPVProperties_triggered(){ launchLocalGui( ":/forms/PVProperties.ui" ); }
void MainWindow::on_actionStrip_Chart_triggered() { launchLocalGui( ":/forms/StripChart.ui"   ); }
void MainWindow::on_actionMessage_Log_triggered() { launchLocalGui( ":/forms/MessageLog.ui"   ); }

// User is changing the user level
void MainWindow::on_actionUser_Level_triggered()
{
    // Present the login dialog to the user
    app->login();
}

// Close this window
void MainWindow::closeEvent(QCloseEvent *event)
 {
     // If there is only one GUI open (max), just exit
     int guiCount = 0;
     for( int i = 0; i < app->getGuiCount(); i++ )
     {
         if( app->getGuiMainWindow(i) == this )
         {
             guiCount++;
         }
     }
     if( guiCount <= 1 )
     {
         event->accept();
         return;
     }

     // If more than one GUI is open, check what the user wants to do
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
    MainWindow* w = new MainWindow( app, filename, true );
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
void MainWindow::onWindowMenuSelection( QAction* action )
{
    // Extract the gui from the action data
    QEForm* gui = action->data().value<QEForm*>();

    // Raise it to be the current window
    raiseGui( gui );
}

// Raise a gui.
void MainWindow::raiseGui( QEForm* gui )
{
    // Prepare to look for gui
    int mwIndex;
    int tabIndex = 0;
    MainWindow* mw = NULL;
    QTabWidget* tabs = NULL;

    // Search each main window for the gui
    for( mwIndex = 0; mwIndex < app->getMainWindowCount(); mwIndex++ )
    {
        mw = app->getMainWindow( mwIndex );

        // If the main window is not using tabs, just check the central widget
        if( !mw->usingTabs )
        {
            if( mw->centralWidget() == gui )
            {
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
                    if( tabs->widget( tabIndex ) == gui )
                    {
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
    }

    // If the gui was found in a main form, show it

    if( mwIndex < app->getMainWindowCount() )
    {
        // Ensure the main form is visible and the active form
        mw->raise();
        mw->activateWindow();

        // If using tabs, and a tab was found, the go to that tab
        if( mw->usingTabs && tabIndex < tabs->count() )
        {
            tabs->setCurrentIndex( tabIndex );
        }
    }
}

// Present the 'About' dialog
void MainWindow::on_actionAbout_triggered()
{

    // Add the version info and the build date/time at compile time of QEGui
    QString about = QString ("QEGui version:\n      ").append(QE_VERSION_STRING " " QE_VERSION_DATE_TIME);

    // Add the version info and the build date/time at compile time of
    // the copy of QEPlugin library loaded by QEGui
    about.append( "\n\n\nQE Framework version (loaded by QEGui):\n      " ).append( QEFrameworkVersion::getString() ).append(" ").append( QEFrameworkVersion::getDateTime() );


    // Add the version info and the build date/time at compile time of
    // the copy of QEPlugin library loaded by QUiLoader while creating QE widgets
    about.append( "\n\n\nQE Framework version (loaded by QUiLoader):\n      " ).append( UILoaderFrameworkVersion );

    // Add Macro substitutions (-m parameter)
    about.append( "\n\n\nMacro Substitutions:\n      " ).append( profile.getMacroSubstitutions() );


    // Add current working directory
    about.append( "\n\n\nCurrent path:" );
    about.append( "\n      " ).append( QDir::currentPath() );

    // Add any path list (-p parameter)
    QStringList paths =  profile.getPathList();
    about.append( "\n\n\nPath List Parameter:" );
    for( int i = 0; i < paths.size(); i++ )
    {
        about.append( "\n      " ).append( paths[i] );
    }

    // Add any path list (environment variable)
    QStringList envPaths =  profile.getEnvPathList();
    about.append( "\n\n\nPath List Environment Variable QE_UI_PATH:" );
    for( int i = 0; i < envPaths.size(); i++ )
    {
        about.append( "\n      " ).append( envPaths[i] );
    }

    // Add the current user level
    about.append( "\n\n\nCurrent User Level:\n      " );
    userLevelTypes::userLevels level = profile.getUserLevel();
    switch( level )
    {
        case userLevelTypes::USERLEVEL_USER:      about.append( "User" );      break;
        case userLevelTypes::USERLEVEL_SCIENTIST: about.append( "Scientist" ); break;
        case userLevelTypes::USERLEVEL_ENGINEER:  about.append( "Engineer" );  break;
    }

    // Add the configuration file details
    about.append( "\n\n\nConfiguration file:\n      " ).append( app->getParams()->configurationFile );

    // Add the current forms
    about.append( "\n\n\nOpen GUI files:\n" );
    for( int i = 0; i < app->getGuiCount(); i++ )
    {
        about.append( "\n      " ).append( app->getGuiForm(i)->getQEGuiTitle() );
        about.append( "\n      " ).append( app->getGuiForm(i)->getFullFileName() );
    }

    // Display the 'about' text
    QMessageBox::about(this, "About QEGui", about );
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
        setTitle( tabs->tabText( index ) );
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
    removeGuiFromWindowsMenu( gui );

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
    QString fileName;

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
    fileName = gui->getFullFileName ();

    // Remove the gui from the 'windows' menus
    removeGuiFromWindowsMenu( gui );

    // Remove the tab - note this does not delete the page widget.
    tabs->removeTab( index );

    // If there is no need for tabs (only one GUI) stop using tabs
    if( tabs->count() == 1 )
        setSingleMode();

    // Use extracted filename to open the new window - we assume the file still exists.
    profile.publishOwnProfile();
    MainWindow* w = new MainWindow( app, fileName, false, NULL);
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
        QEForm* newGui = createGui( guiPath );
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
            preferedSize->setWidth( gui->size().width() + sa->contentsMargins().left() + sa->contentsMargins().right() );
            preferedSize->setHeight( gui->size().height() + sa->contentsMargins().top() + sa->contentsMargins().bottom() );
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
}

// Open a gui in the current window
// Either as a result of the gui user requesting a new window, or a contained object (gui push button) requesting a new window
void MainWindow::loadGuiIntoCurrentWindow( QEForm* gui, bool resize )
{
    // Do nothing if couldn't create gui
    if( !gui )
        return;

    // Note the native size of the gui
//    nativeSize = gui->geometry().size();

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
                removeGuiFromWindowsMenu( oldGui );
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
                removeGuiFromWindowsMenu( oldGui );
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

//            QTimer::singleShot( 1, this, SLOT(resizeToFitGui())); // note 1mS rather than zero. recalculates size correctly if opening a new window from the file menu
        }
        else
        {
            QWidget* rGui = resizeableGui( gui );
            setCentralWidget( rGui );
        }
    }

    // Set the title
    setTitle( gui->getQEGuiTitle() );
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

//===========================================================================================
//The following code was used to ensure a main window fitted a new gui.
// This is now performed by setting a minimum size and calling adjustSize() on the main window.
////=================================================================================
//// Slots for managing resizing
////=================================================================================

//// Resize the form.
//// This is done as a timer event once all processing has completed after creating a new gui.
//void MainWindow::resizeToFitGui()
//{
//    // It is (ever so slightly) possible that the central window has been removed
//    // before the timer event occurs. If so, do nothing.
//    if( !centralWidget() )
//        return;

//    // Get dimensions
//    int main_w = width();
//    int main_h = height();
//    int central_w = centralWidget()->width();
//    int central_h = centralWidget()->height();

//    int ui_w = nativeSize.width();
//    int ui_h = nativeSize.height();
//    int frame_w = 0;

//    // If the central widget is a QEForm, then the area to size around is simply the form size
//    if ( QString::compare( centralWidget()->metaObject()->className(), "QEForm" ) == 0)
//    {
//        frame_w = 0;
//    }

//    // If the central widget is a scroll area (containing a QEForm), then the area to size around is
//    // the area of the scroll area's managed widget
//    else if ( QString::compare( centralWidget()->metaObject()->className(), "QScrollArea" ) == 0 )
//    {
//        QScrollArea* sa = (QScrollArea*)centralWidget();
//        frame_w = sa->frameWidth();
//    }

//    // If no user interface height or width (probably gui file not found) then create an empty window of a reasonable size
//    if( !ui_w || !ui_h )
//    {
//        ui_w = 400;
//        ui_h = 200;
//    }

//    // The size required is the size of the user interface plus any
//    // difference between the main window size and the central widget size
//    resize( ui_w + main_w - central_w + frame_w * 2, ui_h + main_h - central_h + frame_w * 2 );
//}

//=================================================================================
// Slots and methods for launching new GUIs on behalf of objects in the gui (typically buttons)
//=================================================================================

// Slot for launching a new gui from a contained object (old style).
void MainWindow::launchGui( QString guiName, QEForm::creationOptions createOption )
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
        for( int i = 0; i < app->getGuiCount(); i++ )
        {
            QEForm* form = app->getGuiForm( i );
            if( !form->getFullFileName().compare( uiFile->fileName() ) && !form->getMacroSubstitutions().trimmed().compare( publishedProfile.getMacroSubstitutions().trimmed() ) )
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

                        delete uiFile;

                        return;
                    }

                    // Move up a generation
                    w = w->parentWidget();
                }

                // Sanity check. If gui was found, should always have found it in a main window
                break;
            }
        }
        if( uiFile )
        {
            delete uiFile;
        }
    }

    // Load the new gui as required
    switch( createOption )
    {
        // Open the specified gui in the current window
        case QEForm::CREATION_OPTION_OPEN:
            {
                QEForm* gui = createGui( guiName );  // Note, profile should have been published by signal code
                loadGuiIntoCurrentWindow( gui, true );
            }
            break;

        // Open the specified gui in a new tab
        case QEForm::CREATION_OPTION_NEW_TAB:
            {
                // If not using tabs, start tabs and migrate any single gui to the first tab
                if( !usingTabs )
                    setTabMode();

                // Create the gui and load it into a new tab
                QEForm* gui = createGui( guiName );  // Note, profile should have been published by signal code
                loadGuiIntoNewTab( gui );
            }
            break;

        // Open the specified gui in a new window
        case QEForm::CREATION_OPTION_NEW_WINDOW:
            {
                MainWindow* w = new MainWindow( app, guiName, true ); // Note, profile should have been published by signal code
                w->show();
            }
            break;

        default:
            qDebug() << "MainWindow::launchGui() Unexpected gui creation option: " << createOption;
            break;
    }
}


// Slot for launching a new gui from a contained object (new style).
void  MainWindow::requestGui( const QEGuiLaunchRequests & request )
{
    QStringList arguments =  request.getArguments();

    switch (request.getKind ()) {

        case QEGuiLaunchRequests::KindFileName:
            if (arguments.count() >= 1) {
                // Just re-use old style slot.
                QString guiName = arguments.value( 0 );
                QEForm::creationOptions createOption = (QEForm::creationOptions) request.getOption ();
                launchGui ( guiName, createOption );
            }
            break;

        case QEGuiLaunchRequests::KindPvProperties:
            if (arguments.count() >= 1) {
                QString pvName = arguments.value( 0 );
                launchLocalGui (":/forms/PVProperties.ui", "QEPvProperties", pvName);
            }
            break;

        case QEGuiLaunchRequests::KindStripChart:
            if (arguments.count() >= 1) {
                QString pvName = arguments.value( 0 );
                launchLocalGui (":/forms/StripChart.ui", "QEStripChart", pvName);
            }
            break;

        default:
            sendMessage( "Unhandled gui request kind", message_types( MESSAGE_TYPE_ERROR, MESSAGE_KIND_EVENT ) );

  }
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
QEForm* MainWindow::createGui( QString fileName )
{
    return createGui( fileName, QString() );
}

QEForm* MainWindow::createGui( QString fileName, QString restoreId )
{
    // Don't do anything if no filename was supplied
    if (fileName.isEmpty())
        return NULL;

    // Publish the main window's form Id so the new QEForm will pick it up
    setChildFormId( getNextMessageFormId() );
    profile.setPublishedMessageFormId( getChildFormId() );

    // Inform user
    newMessage( QString( "Opening %1 in new window " ).arg( fileName ), message_types ( MESSAGE_TYPE_INFO ) );

    // Build the gui
    QEForm* gui = new QEForm( fileName );
    if( !restoreId.isNull() )
    {
        gui->setUniqueIdentifier( restoreId );
    }
    gui->setResizeContents( false );

    // If built ok, read the ui file
    if( gui )
    {
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
        QEUtilities::adjustWidgetScale( gui, int (app->getParams()->adjustScale) , 100 );

        UILoaderFrameworkVersion = gui->getContainedFrameworkVersion();

        // If a profile was defined in this method, release it now.
        if( profileDefinedHere )
        {
            profile.releaseProfile();
        }

        // Add the new gui to the list of windows
        addGuiToWindowsMenu( gui );
    }
    // Return the created gui if any
    return gui;
 }

// Set the main window title (default to 'QEGui' if no title supplied)
void MainWindow::setTitle( QString title )
{
    if( title.isEmpty() )
    {
        setWindowTitle( "QEGui" );
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
// Methods to manage the application wide 'Windows' menu
//=================================================================================

// Add a gui to the 'windows' menus
// Used when creating a new gui
void MainWindow::addGuiToWindowsMenu( QEForm* gui )
{
    // Add the gui to the list of guis
    app->addGui( gui, this );

    // For each main window, add a new action to the window menu
    for( int i = 0; i < app->getMainWindowCount(); i++ )
    {
        addWindowMenuAction(  app->getMainWindow( i )->ui.menuWindows, gui );
    }
}

// Build new 'windows' menu
// Used when creating a new main window and there are already other main windows present with GUIs
void MainWindow::buildWindowsMenu()
{
    // Add all current guis to the 'windows' menu
    for( int i = 0; i < app->getGuiCount(); i++ )
    {
        addWindowMenuAction(  ui.menuWindows, app->getGuiForm( i ) );
    }
}

// Add a gui to a 'window' menu
void MainWindow::addWindowMenuAction( QMenu* menu, QEForm* gui )
{
    // Create the action and add it to the window menu, setting the action data to be the gui
    QAction* action = new QAction( gui->getQEGuiTitle(), menu );
    action->setData( qVariantFromValue( gui ) );
    menu->addAction( action );
}

// Remove a gui from the 'windows' menus
// Used when deleting a single gui
void MainWindow::removeGuiFromWindowsMenu( QEForm* gui )
{
    // Remove the gui from the application wide list of guis
    bool guiFound = app->removeGui( gui );

    // For each main window, also remove the gui
    if( guiFound )
    {
        for( int i = 0; i < app->getMainWindowCount(); i++ )
        {
            MainWindow* mw = app->getMainWindow( i );
            QList<QAction*> actions = mw->ui.menuWindows->actions();
            for( int j = 0; j < actions.count(); j++ )
            {
                // Extract the gui from the action data
                QEForm* actionGui = actions[j]->data().value<QEForm*>();
                if( actionGui == gui )
                {
                    mw->ui.menuWindows->removeAction( actions[j] );
                    break;
                }
            }
        }
    }
}

// Remove all guis on a main window from the 'windows' menus of all main windows
// Used when deleting a main window
void MainWindow::removeAllGuisFromWindowsMenu()
{
    // If using tabs, delete the 'windows' menu references for each gui displayed in the tabs
    if( usingTabs )
    {
        QTabWidget* tabs = getCentralTabs();
        if( tabs )
        {
            while( tabs->widget( 0 ) )
            {
                QEForm* gui = extractGui( tabs->widget( 0 ) );
                removeGuiFromWindowsMenu( gui );
                tabs->removeTab( 0 );
            }
        }
    }

    // Not using tabs, so just remove the entry for the single gui displayed, if any
    else
    {
        QEForm* gui = getCentralGui();
        if( gui )
        {
            removeGuiFromWindowsMenu( gui );
        }
    }
}

// The user has requested a save of the current configuration
void MainWindow::on_actionSave_Configuration_triggered()
{
    PersistanceManager* pm = profile.getPersistanceManager();
    startupParams* params = app->getParams();

    // Get the user selection
    saveDialog sd( pm->getConfigNames( params->configurationFile, QE_CONFIG_NAME ) );
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
        qDebug() <<  "No configuration selected";
        return;
    }

    // Give all main windows and top level QEForms (managed by this application) a unique identifier required for restoration
    app->identifyWindowsAndForms();

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
        qDebug() <<  "No configuration selected";
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
                PMElement mw =   pm->addNamedConfiguration( mainWindowName );

                PMElement id =  mw.addElement( "Identity" );
                id.addAttribute( "id", uniqueId );

                PMElement geo =  mw.addElement( "Geometry" );
                QRect r = geometry();
                geo.addAttribute( "X", r.x() );
                geo.addAttribute( "Y", r.y() );
                geo.addAttribute( "Width", r.width() );
                geo.addAttribute( "Height", r.height() );

                PMElement state =  mw.addElement( "State" );
                state.addAttribute( "Flags", windowState() );

                // Note which GUI is the current GUI. Mainly if main window is displaying
                // more than one gui in a tab widget. Redundant but harmless if only one gui is present.
                QEForm* currentGui = getCurrentGui();

                // Save details for each GUI
                for( int i = 0; i < app->getGuiCount(); i++ )
                {
                    if( app->getGuiMainWindow( i ) == this )
                    {
                        // Gui name and ID
                        PMElement form =  mw.addElement( "Gui" );
                        form.addAttribute( "Name", app->getGuiForm( i )->getFullFileName() );
                        form.addAttribute( "ID", app->getGuiForm( i )->getUniqueIdentifier() );

                        // Current gui boolean
                        if( app->getGuiForm( i ) == currentGui )
                        {
                            form.addAttribute( "CurrentGui", true );
                        }

                        // Macro substitutions, if any
                        QString macroSubs = profile.getMacroSubstitutions();
                        if( !macroSubs.isEmpty() )
                        {
                            form.addValue( "MacroSubstitutions", profile.getMacroSubstitutions() );
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
                        QScrollArea* sa = guiScrollArea( app->getGuiForm( i ) );
                        if( sa )
                        {
                            PMElement pos =  form.addElement( "Scroll" );
                            pos.addAttribute( "X",  sa->horizontalScrollBar()->value() );
                            pos.addAttribute( "Y", sa->verticalScrollBar()->value() );
                        }
                    }
                }
            }
            break;


        // First restore phase.
        // This main window will position itself and create the GUIs it contains
        case SaveRestoreSignal::RESTORE_APPLICATION:
            {
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

                PMElement id = data.getElement( "Identity" );
                id.getAttribute( "id", uniqueId );

                PMElement geometry = data.getElement( "Geometry" );
                int x, y, w, h;
                //!!! Note, window position jumps down by the window offset in the decoration each save and restore. either saving or restoring does not include decoration???
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
                            QEForm* gui = createGui( name, restoreId );
                            if( i == 0)
                            {
                                // Load the gui into the main window
                                loadGuiIntoCurrentWindow( gui, false );
                            }
                            else
                            {
                                // If not using tabs, start tabs and migrate any single gui to the first tab
                                if( !usingTabs )
                                    setTabMode();
                                // Create gui as a new tab
                                loadGuiIntoNewTab( gui );
                            }

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
                                app->setGuiScroll( app->getGuiCount() - 1, QPoint( x, y ) );
                            }
                        }
                    }
                }

                // If this new GUI is the current one, make it so
                if( currentGui )
                {
                    raiseGui( currentGui );
                }
            }
            break;

        // Second resore phase.
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

// Function to close all main windows.
// Used when restoring a configuration
void MainWindow::closeAll()
{
    // Queue all windows for closure.
    // Need to use deleteLater() as this function is usually called from an event from one of the windows
    // Also, hide the widget from all code by removing it from the main window list
    while( app->getMainWindowCount() )
    {
        MainWindow* mw = app->getMainWindow( 0 );
        mw->beingDeleted = true;
        mw->deleteLater();
        app->removeMainWindow( 0 );
    }
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

// Scroll all guis in a main window.
// This is called as part of restoring a main window.
// When restoring a window, setting it's geometry and scroll position can't be
// performed until after it has been created, program flow has returned to the
// event loop, and various events related to geometry have occured.
// For this reason, setting specific geometry and scrolling is performed as a timer event.
void MainWindow::scrollTo()
{
    // When restoring a window scrolling can't be performed until after the window has been created, program flow has
    // returned to the event loop, and various events related to geometry changes have occured.
    // For this reason, scolling is performed as a timer event.
    // The timer period should not be important, it should just ensure all outstanding events are processed first.
    // However, it seems to require over 30mS on a test implementation which implies this is not the case.
    // Specifying a large timer period is dangerous since any 'reasonable' period may not be enough if the system is heavily loaded.
    // To work around this, scrolling is held off (rescheduled with a new timer event here) if there are still
    // outstanding events (which may be related to resizing) or if the resizing has not yet taken effect (if the
    // width and height specified in the geometry request have not yet been reflected in the current width and height)
    // As a safeguard, we won't wait for longer than 10 seconds. One possible reason for this is the following scenario:
    // a configuration was saved with a large window on a large screen. If the screen size is reduced the requested window
    // size may not be honoured by the display manager and this function will never see the window size has reached the required size.
    if( setGeomRect.width() != width() ||
        setGeomRect.height() != height() )
    {
        if( scrollToCount < 1000 )/* 1000 = 1000 x 10ms = 10 seconds */
        {
            QTimer::singleShot(10, this, SLOT( scrollTo() ));
        }
        scrollToCount++;
        return;
    }

    // The window is ready for scrolling if required.
    for( int i = 0; i < app->getGuiCount(); i++ )
    {
        // If QEGui is managing the scrolling of the QEForm and has placed it in a scroll area,
        // then note the scroll position
        QScrollArea* sa = guiScrollArea( app->getGuiForm( i ) );
        if( sa )
        {
            QPoint p = app->getGuiScroll( i );
            sa->horizontalScrollBar()->setValue( p.x() );
            sa->verticalScrollBar()->setValue( p.y() );
        }
    }
}

// Set a gui geometry.
// This is called as part of restoring a main window.
// When restoring a window, setting it's geometry and scroll position can't be
// performed until after it has been created, program flow has returned to the
// event loop, and various events related to geometry have occured.
// For this reason, setting specific geometry and scrolling is performed as a timer event.
void MainWindow::setGeom()
{
    // Set the geometry as noted during the restore
    setGeometry( setGeomRect );

    // Initiate scrolling of guis within the main window
    QTimer::singleShot( 10, this, SLOT(scrollTo() ));
}

