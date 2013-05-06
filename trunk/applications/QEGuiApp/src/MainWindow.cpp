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
#include <QString>

#include <MainWindow.h>
#include <QEForm.h>
#include <QEFrameworkVersion.h>
#include <QMessageBox>
#include <ContainerProfile.h>
#include <QVariant>

// Before Qt 4.8, the command to start designer is 'designer'.
// Qt 4.8 later uses the command 'designer-qt4'
// Try both before giving up starting designer
#define DESIGNER_COMMAND_1 "designer-qt4"
#define DESIGNER_COMMAND_2 "designer"

#define QE_CONFIG_NAME "QEGuiConfig"

// Shared list of all main windows
QList<MainWindow*> MainWindow::mainWindowList;

// Shared list of all GUIs being displayed in all main windows
QList<guiListItem> MainWindow::guiList;

int MainWindow::nextUniqueId = 0;

Q_DECLARE_METATYPE( QEForm* )

//=================================================================================
// Methods for construction, destruction, initialisation
//=================================================================================

// Constructor
// A profile should have been defined before calling this constructor
MainWindow::MainWindow( QString fileName, bool openDialog, bool enableEditIn, bool disableMenuIn, QWidget *parent )  : QMainWindow( parent )
{
    // A published profile should always be available, but the various signal consumers will always be either NULL (if the
    // profile was set up by the QEGui application) or objects in another main window (if the profile was published by a button in a gui)
    // Replace the signal consuming objects
    profile.updateConsumers( this );

    // Initialise
    usingTabs = false;
    nativeSize = QSize( 0, 0 );

    // Give the main window's USerMessage class a unique form ID so only messages from
    // the form in each main window are displayed that main window's status bar
    setFormId( getNextMessageFormId() );
    setFormFilter( MESSAGE_FILTER_MATCH );
    setSourceFilter( MESSAGE_FILTER_NONE );

    // Present the main form's ui
    ui.setupUi( this );

    // Setup to allow user to change focus to a window from the 'Windows' menu
    QObject::connect( ui.menuWindows, SIGNAL( triggered( QAction* ) ), this, SLOT( onWindowMenuSelection( QAction* ) ) );

    // Setup to respond to requests to save or restore persistant data
    PersistanceManager* persistanceManager = profile.getPersistanceManager();
    QObject::connect( persistanceManager->getSaveRestoreObject(), SIGNAL( saveRestore( SaveRestoreSignal::saveRestoreOptions ) ), this, SLOT( saveRestore( SaveRestoreSignal::saveRestoreOptions ) ), Qt::DirectConnection );

    // Save this instance of a main window in the global list of main windows
    mainWindowList.append( this );

//    ui.menuUser_Level->add
    // Set the default title
    setTitle( "" );

    // Populate the 'windows' menu to include all current guis in any main window
    buildWindowsMenu();

    // Enable the edit menu if requested
    enableEdit = enableEditIn;
    if( enableEdit )
        ui.menuEdit->setEnabled( true );

    // Hide the main tool bar (nothing in it yet)
    ui.mainToolBar->hide();

    // Hide the menu bar if not required
    disableMenu = disableMenuIn;
    if( disableMenu )
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

    // Clear unique Id
    uniqueId = 0;

    // Restore (will only do anything if this main window is being created during a restore)
    saveRestore( SaveRestoreSignal::RESTORE );

    // If we got a restored unique ID, then go with it, otherwise, generate a unique id for this main window.
    if( !uniqueId )
    {
        getUniqueId();
    }
}

// Destructor
MainWindow::~MainWindow()
{
    // Remove the GUIs shown in this main window from the GUIs listed in the 'Windows' menus of all other main windows
    removeAllGuisFromWindowsMenu();

    // Remove this main window from the global list of main windows
    // Note, this may have already been done to hide the the main window if deleting using deleteLater()
    for (int i = 0; i < mainWindowList.size(); ++i)
    {
        if( mainWindowList[i] == this )
        {
            mainWindowList.removeAt( i );
            break;
        }
    }
}

//=================================================================================
// Methods for responding to user actions
//=================================================================================

// Open a gui in a new window.
// Present a file open dialog box and after generate the gui based on the ui file the user selects
void MainWindow::on_actionNew_Window_triggered()
{
    profile.publishOwnProfile();
    MainWindow* w = new MainWindow( "", true, enableEdit, disableMenu );
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
            setCentralWidget( NULL );
        }

        setTitle( "" );
    }
 }

// Standard GUIs available from the 'File' menu
void MainWindow::on_actionPVProperties_triggered(){ launchLocalGui( ":/forms/PVProperties.ui" ); }
void MainWindow::on_actionStrip_Chart_triggered() { launchLocalGui( ":/forms/StripChart.ui"   ); }
void MainWindow::on_actionUser_Level_triggered()  { launchLocalGui( ":/forms/UserLevel.ui"    ); }
void MainWindow::on_actionMessage_Log_triggered() { launchLocalGui( ":/forms/MessageLog.ui"   ); }

// Exit.
// If more than one window is present, offer to close the current window, or all of them
void MainWindow::on_actionExit_triggered()
{
    // If there is only one GUI open (max), just exit
    if( mainWindowList.count() <= 1 )
    {
        exit(0);
    }

    QString msg;
    if( mainWindowList.count() == 2 )
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
            exit(0);
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

// Launch a new gui from the 'File' menu
void MainWindow::launchLocalGui( QString filename )
{
    profile.publishOwnProfile();
    MainWindow* w = new MainWindow( filename, true, enableEdit, disableMenu );
    profile.releaseProfile();
    w->show();
}

// Raise the window selected in the 'Window' menu
void MainWindow::onWindowMenuSelection( QAction* action )
{
    // Extract the gui from the action data
    QEForm* gui = action->data().value<QEForm*>();

    // Prepare to look for gui
    int mwIndex;
    int tabIndex = 0;
    MainWindow* mw = NULL;
    QTabWidget* tabs = NULL;

    // Search each main window for the gui
    for( mwIndex = 0; mwIndex < mainWindowList.size(); mwIndex++ )
    {
        mw = mainWindowList[mwIndex];

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

    if( mwIndex < mainWindowList.size() )
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
    userLevels level = profile.getUserLevel();
    switch( level )
    {
        case USERLEVEL_USER:      about.append( "User" );      break;
        case USERLEVEL_SCIENTIST: about.append( "Scientist" ); break;
        case USERLEVEL_ENGINEER:  about.append( "Engineer" );  break;
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

//=================================================================================
// Methods for managing GUI windows
//=================================================================================

// Given a QEForm, return a widget that will manage being resized.
// If the QEForm has a scroll area as its top level child, or if its top level child has
// a layout, it is managing its own size so just return the QEForm, otherwise return a
// scroll area containing the QEForm.
QWidget* MainWindow::resizeableGui( QEForm* gui )
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
        return gui;
    }
    // If the widget is not managing its own size return it within a scroll area
    else
    {
        QScrollArea* sa = new QScrollArea();
        sa->setWidget( gui );
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
    nativeSize = gui->geometry().size();

    // Ensure the gui can be resized
    QWidget* rGui = resizeableGui( gui );

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

        // Use the gui
        setCentralWidget( rGui );

        // If nothing else is looking after resizing (such as a restore) resize here
        if( resize )
        {
            QTimer::singleShot( 1, this, SLOT(resizeToFitGui())); // note 1mS rather than zero. recalculates size correctly if opening a new window from the file menu
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

//=================================================================================
// Slots for managing resizing
//=================================================================================

// Resize the form.
// This is done as a timer event once all processing has completed after creating a new gui.
void MainWindow::resizeToFitGui()
{
    // It is (ever so slightly) possible that the central window has been removed
    // before the timer event occurs. If so, do nothing.
    if( !centralWidget() )
        return;

    // Get dimensions
    int main_w = width();
    int main_h = height();
    int central_w = centralWidget()->width();
    int central_h = centralWidget()->height();

    int ui_w = nativeSize.width();
    int ui_h = nativeSize.height();
    int frame_w = 0;

    // If the central widget is a QEForm, then the area to size around is simply the form size
    if ( QString::compare( centralWidget()->metaObject()->className(), "QEForm" ) == 0)
    {
        frame_w = 0;
    }

    // If the central widget is a scroll area (containing a QEForm), then the area to size around is
    // the area of the scroll area's managed widget
    else if ( QString::compare( centralWidget()->metaObject()->className(), "QScrollArea" ) == 0 )
    {
        QScrollArea* sa = (QScrollArea*)centralWidget();
        frame_w = sa->frameWidth();
    }

    // If no user interface height or width (probably gui file not found) then create an empty window of a reasonable size
    if( !ui_w || !ui_h )
    {
        ui_w = 400;
        ui_h = 200;
    }

    // The size required is the size of the user interface plus any
    // difference between the main window size and the central widget size
    resize( ui_w + main_w - central_w + frame_w * 2, ui_h + main_h - central_h + frame_w * 2 );
}

//=================================================================================
// Slots and methods for launching new GUIs on behalf of objects in the gui (typically buttons)
//=================================================================================

// Slot for launching a new gui from a contained object.
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
        for( int i = 0; i < guiList.size(); i++ )
        {
            QEForm* form = guiList[i].getForm();
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
                QWidget* w = guiList[i].getForm()->parentWidget();
                while( w )
                {
                    // Ensure the correct tab is selected
                    if( QString::compare( w->metaObject()->className(), "QTabWidget" ) == 0 )
                    {
                        QTabWidget* tw = (QTabWidget*)w;
                        int j;
                        j = tw->indexOf( guiList[i].getForm() ) ;
                        if( j < 0 )
                        {
                            j = tw->indexOf( guiList[i].getForm()->parentWidget() ) ;
                            if( j < 0 )
                            {
                                j = tw->indexOf( guiList[i].getForm()->parentWidget()->parentWidget() ) ;
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
                MainWindow* w = new MainWindow( guiName, true, enableEdit, disableMenu ); // Note, profile should have been published by signal code
                w->show();
            }
            break;

        default:
            qDebug() << "MainWindow::launchGui() Unexpected gui creation option: " << createOption;
            break;
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
    // Don't do anything if no filename was supplied
    if (fileName.isEmpty())
        return NULL;

    // Publish the main window's form Id so the new QEForm will pick it up
    setChildFormId( getNextMessageFormId() );
    profile.setPublishedMessageFormId( getChildFormId() );

    // Inform user
    newMessage( QString( "Opening %1 in new window " ).arg( fileName ), message_types ( MESSAGE_TYPE_INFO ) );

    profile.addIdRoot( QString( "MW-%1" ).arg( uniqueId ));

    // Build the gui
    QEForm* gui = new QEForm( fileName );
    gui->setResizeContents( false );

    profile.removeIdRoot();

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
    guiList.append( guiListItem( gui, this ) );

    // For each main window, add a new action to the window menu
    for( int i = 0; i < mainWindowList.size(); i++ )
    {
        addWindowMenuAction(  mainWindowList[i]->ui.menuWindows, gui );
    }
}

// Build new 'windows' menu
// Used when creating a new main window and there are already other main windows present with GUIs
void MainWindow::buildWindowsMenu()
{
    // Add all current guis to the 'windows' menu
    for( int i = 0; i < guiList.size(); i++ )
    {
        addWindowMenuAction(  ui.menuWindows, guiList[i].getForm() );
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
    bool guiFound = false;
    int i;
    for( i = 0; i < guiList.size(); i++ )
    {
        if( guiList[i].getForm() == gui )
        {
            guiList.removeAt( i );
            guiFound = true;
            break;
        }
    }

    // For each main window, also remove the gui
    if( guiFound )
    {
        for( int j = 0; j < mainWindowList.size(); j++ )
        {
            QList<QAction*> actions = mainWindowList[j]->ui.menuWindows->actions();
            mainWindowList[j]->ui.menuWindows->removeAction( actions[i] );
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
/*
        QMessageBox::warning( this,"QEGui",
                              "Under Construction.\n"
                              "'Save'' is not implemented yet",
                              QMessageBox::Cancel );

        return;
*/
    // Ask the persistance manager to save the current configuration.
    // The persistance manager will signal all interested objects (including this application) that
    // they should present anything they wish to save.
    PersistanceManager* persistanceManager = profile.getPersistanceManager();
    persistanceManager->save( QString( QE_CONFIG_NAME ).append( ".xml" ), QE_CONFIG_NAME, "Default" );
}

// The user has requested a restore of a saved configuration
void MainWindow::on_actionRestore_Configuration_triggered()
{
    /*
            QMessageBox::warning( this,"QEGui",
                                  "Under Construction.\n"
                                  "'Restore'' is not implemented yet",
                                  QMessageBox::Cancel );

            return;
    */

    // Restoring a configuration will close all current windows, check with the user this is OK
    QMessageBox msgBox;
    msgBox.setText( "All existing windows will be closed prior to restoring a configuration. Do you want to continue?" );
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    switch ( msgBox.exec() )
    {
       case QMessageBox::Yes:
            // Yes, continue
            break;

        case QMessageBox::No:
        case QMessageBox::Cancel:
        default:
            // No, do nothing
            return;
     }

    // Ask the persistance manager to restore a configuration.
    // The persistance manager will signal all interested objects (including this application) that
    // they should collect and apply restore data.
    PersistanceManager* persistanceManager = profile.getPersistanceManager();
    persistanceManager->restore( QString( QE_CONFIG_NAME ).append( ".xml" ), QE_CONFIG_NAME, "Default"  );
}

// A save or restore has been requested (Probably by QEGui itself)
void MainWindow::saveRestore( SaveRestoreSignal::saveRestoreOptions option )
{
    PersistanceManager* pm = profile.getPersistanceManager();

    // Note where this main window is in the list of main windows.
    // This will be used to generate a unique name
    int mainWindowId;
    for( mainWindowId = 0; mainWindowId < mainWindowList.count(); mainWindowId++ )
    {
        if( mainWindowList[mainWindowId] == this )
            break;
    }

    QString mainWindowName = QString( "QEGuiMainWindow_%1" ).arg( mainWindowId );

    switch( option )
    {
    case SaveRestoreSignal::SAVE:
        {
            // Start with the top level element - the main windows
            PMElement mw =   pm->addElement( mainWindowName );

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

            for( int j = 0; j < guiList.count(); j++ )
            {
                if( guiList[j].getMainWindow() == this )
                {
                    PMElement form =  mw.addElement( "Gui" );
                    form.addAttribute( "Name", guiList[j].getForm()->getFullFileName() );

                    // If QEGui is managing the scrolling of the QEForm and has placed it in a scroll area,
                    // then note the scroll position
                    QScrollArea* sa = guiScrollArea( guiList[j].getForm() );
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


    case SaveRestoreSignal::RESTORE:
        {
            // Get the data for this window
            PMElement data = pm->getMyData( mainWindowName );

            // If none, do nothing
            if( data.isNull() )
            {
                return;
            }

            PMElement id = data.getElement( "Identity" );
            id.getElementAttribute( "id", uniqueId );

            PMElement geometry = data.getElement( "Geometry" );
            int x, y, w, h;
            if( geometry.getElementAttribute( "X", x ) &&
                geometry.getElementAttribute( "Y", y ) &&
                geometry.getElementAttribute( "Width", w ) &&
                geometry.getElementAttribute( "Height", h ) )
            {
                // Set the geometry in a timer event. This will occur after events due to creation have finished arriving.
                setGeomRect = QRect( x, y, w, h );
                QTimer::singleShot(1, this, SLOT(setGeom()));
            }

            PMElement pos = data.getElement( "State" );
            int flags;
            if( pos.getElementAttribute( "Flags", flags ) )
            {
                  setWindowState( (Qt::WindowStates)flags );
            }

            // Get a list of guis
            PMElementList guiElements = data.getElementList( "Gui" );

            for(int i = 0; i < guiElements.count(); i++ )
            {
                PMElement guiElement = guiElements.getElement( i );
                {
                    QString name;
                    if( guiElement.getElementAttribute( "Name", name ) )
                    {
                        QEForm* gui = createGui( name );
                        if( i == 0)
                        {
                            loadGuiIntoCurrentWindow( gui, false );
                        }
                        else
                        {
                            loadGuiIntoNewTab( gui );
                        }

                        PMElement scroll = guiElement.getElement( "Scroll" );
                        int x, y;
                        if( scroll.getElementAttribute( "X", x ) &&
                            scroll.getElementAttribute( "Y", y ) )
                        {
                            guiList.last().setScroll( QPoint( x, y ) );
                        }
                    }
                }
           }
        }
        break;
    }

}

// Static function to close all main windows.
// Used when restoring a configuration
void MainWindow::closeAll()
{
    // Queue all windows for closure.
    // Need to use deleteLater() as this function is usually called from an event from one of the windows
    // Also, hide the widget from all code by removing it from the main window list
    while( mainWindowList.count() )
    {
        mainWindowList[0]->deleteLater();
        mainWindowList.removeFirst();
    }
}

// Static function to report the number of main windows
// Used when saving a configuration
int MainWindow::count()
{
    return mainWindowList.count();
}

// Generate a unique main window ID
void MainWindow::getUniqueId()
{
    // Keep incrementing the unique ID until one is found that isn't in use.
    // (the next increment for uniqueId may not be unique since IDs are restored
    // during a restore operation)
    while( true )
    {
        uniqueId = ++nextUniqueId;
        int i;
        for( i = 0; i < mainWindowList.count(); i++ )
        {
            MainWindow* mw = mainWindowList[i];
            if( mw != this && mw->uniqueId == uniqueId )
                break;
        }
        if( i == mainWindowList.count() )
        {
            break;
        }
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
    if( QCoreApplication::hasPendingEvents() ||
        setGeomRect.width() != width() ||
        setGeomRect.height() != height() )
    {
        QTimer::singleShot(10, this, SLOT(scrollTo()));
        return;
    }

    // The window is ready for scrolling if required.
    for( int i = 0; i < guiList.count(); i++ )
    {
        // If QEGui is managing the scrolling of the QEForm and has placed it in a scroll area,
        // then note the scroll position
        QScrollArea* sa = guiScrollArea( guiList[i].getForm() );
        if( sa )
        {
            sa->horizontalScrollBar()->setValue( guiList[i].getScroll().x() );
            sa->verticalScrollBar()->setValue( guiList[i].getScroll().y() );
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
    QTimer::singleShot(1, this, SLOT(scrollTo()));
}
