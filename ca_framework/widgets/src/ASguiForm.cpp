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

/*! This class is used as a container for QCa widgets.

    It adds any variable name macro substitutions to the current environment profile, creates a form widget and
    reads a UI file which can contain QCa widgets.

    As QCa widgets are created, they note the current environment profile, including variable name substitutions.
    QCa widgets also register themselves with this class so this class can activate them once they are fully created.
    QCa widgets can't activate themselves. The Qt form loader creates each widget and calls the appropriate property
    functions to set it up. The widget itself does not know what properties are going to be set and when they have
    all been set. For this reason the QCa widgets don't know when to request CA data. Both variable name properties
    and variable name substitution properties must be set up to request data and other properties may need to be set
    up before udates can be used.

    This class can be used directly (within a GUI application) as the top level form, or as a base class for
    the ASguiFormPlugin class. The ASguiFormPlugin plugin class is used to store a gui form in a UI file.
*/

#include <QUiLoader>
#include <QtGui>
#include <QString>
#include <QDir>
#include <QtDebug>
#include <ASguiForm.h>
#include <ContainerProfile.h>
#include <QCaWidget.h>

/// Constructor.
/// No UI file is read. uiFileName must be set and then readUiFile() called after construction
ASguiForm::ASguiForm( QWidget* parent ) : QScrollArea( parent ) {
    commonInit( false );
}

/// Constructor.
/// UI filename is supplied and UI file is read as part of construction.
ASguiForm::ASguiForm( const QString& uiFileNameIn, QWidget* parent ) : QScrollArea( parent ) {
    commonInit( true );
    uiFileName = uiFileNameIn;
}

/// Common construction
void ASguiForm::commonInit( const bool alertIfUINoFoundIn )
{
    setAcceptDrops(true);

    ui = NULL;
    alertIfUINoFound = alertIfUINoFoundIn;
    handleGuiLaunchRequests = false;

    // Setup a valid local profile if no profile was published
    if( !profile.isProfileDefined() )
    {
        userMessage.setup( this );
        profile.setupLocalProfile( this, this, this, this, QDir::currentPath(), "", "" );
    }

    // Prepare to recieve notification that the ui file being displayed has changed
    QObject::connect( &fileMon, SIGNAL( fileChanged( const QString & ) ), this, SLOT( fileChanged( const QString & ) ) );
}

/// Destructor.
ASguiForm::~ASguiForm()
{
    // Close any existing form
    if( ui )
        ui->close();
}

// Read a UI file.
// The file read depends on the value of uiFileName
bool ASguiForm::readUiFile()
{
    // Assume file is bad
    bool fileLoaded = false;

    // If any name has been provided...
    if (!uiFileName.isEmpty()) {

        // Set up the environment profile for any QCa widgets created by the form
        QObject* savedGuiLaunchConsumer = NULL;

        // Build a list of all the places we expect to find the file
        // Use a single location if an absolute path was specified.
        // Use the following list of locations if a relative path was specified:
        //  - The directory where the parent object (form) was read from (set up in the application profile)
        //  - The application's path (set up in the application profile) (the -p sewitch for ASgui)
        //  - The current directory
        QStringList searchList;
        if(  QDir::isAbsolutePath( uiFileName ) )
        {
            searchList.append( uiFileName );
        }
        else
        {
            QFileInfo fileInfo;

            fileInfo.setFile( profile.getParentPath(), uiFileName );
            searchList.append( fileInfo.filePath() );

            fileInfo.setFile( profile.getPath(), uiFileName );
            searchList.append(  fileInfo.filePath() );

            fileInfo.setFile( QDir::currentPath(), uiFileName );
            searchList.append(  fileInfo.filePath() );
        }

        // Attempt to open the file
        QFile* uiFile = NULL;
        for( int i = 0; i < searchList.count(); i++ )
        {
            uiFile = new QFile( searchList[i] );
            if( uiFile->open( QIODevice::ReadOnly ) )
                break;
            delete uiFile;
            uiFile = NULL;
        }

        // If the file was not found and opened, notify as appropriate
        if( !uiFile )
        {
            if( alertIfUINoFound )
            {
                QString msg;
                QTextStream(&msg) << "User interface file '" << uiFileName << "' could not be opened";
                //!!!??? currently not working. profile not set up yet so user messages can't be displayed this early perhaps
                userMessage.sendWarningMessage( msg, "ASguiForm::readUiFile"  );
                qDebug() << msg;
            }
        }

        // If the file was found and opened, load it
        else
        {
            // Close any pre-existing gui in the form
            if( ui )
            {
                delete ui;
                ui = NULL;
            }
            QDir uiDir;

            // Load the UI file if opened
            /// Ensure no other files are being monitored (belt and braces)
            QStringList monitoredPaths = fileMon.files();
            if( monitoredPaths.count())
            {
                fileMon.removePaths( monitoredPaths );
            }

            /// Monitor the opened file
            fileMon.addPath( uiDir.absoluteFilePath( uiFile->fileName()) );

            /// Extract the file name part used for the window title
            QFileInfo fileInfo( uiFile->fileName() );
            title = fileInfo.fileName();
            if( title.endsWith( ".ui" ) )
                title.chop( 3 );

            // If profile has been published (for example by an application creating this form), then publish our own local profile
            bool localProfile = false;
            if( !profile.isProfileDefined() )
            {
                // Flag the profile was set up in this function (and so should be released in this function)
                localProfile = true;

                profile.publishOwnProfile();
            }

            // Add this form's macro substitutions for all it's children to use
            profile.addMacroSubstitutions( variableNameSubstitutions );

            // Temporarily update the published current object's path to the path of the form being created.
            // Any objects created within the form (for example sub forms) can then know where their parent form is located.
            profile.setPublishedParentPath( fileInfo.absolutePath() );

            // If this form is handling form launch requests from object created within it, replace any form launcher with our own
            if( handleGuiLaunchRequests )
                savedGuiLaunchConsumer = profile.replaceGuiLaunchConsumer( this );

            // Load the gui
            QUiLoader loader;
            ui = loader.load( uiFile );
            uiFile->close();

            // Remove this form's macro substitutions now all it's children are created
            profile.removeMacroSubstitutions();

            // Reset the published current object's path to what ever it was
            profile.setPublishedParentPath( profile.getParentPath() );

            // If this form is handling form launch requests from object created within it, put back any original
            // form launcher now all objects have been created
            if ( handleGuiLaunchRequests )
                 profile.replaceGuiLaunchConsumer( savedGuiLaunchConsumer );

            /// Any QCa widgets that have just been created need to be activated.
            /// Note, this is only required when QCa widgets are not loaded within a form and not directly by 'designer'.
            /// When loaded directly by 'designer' they are activated (a CA connection is established) as soon as either
            /// the variable name or variable name substitution properties are set
            QCaWidget* containedWidget;
            while( (containedWidget = profile.getNextContainedWidget()) )
                containedWidget->activate();

            // If the published profile was published within this method, release it so nothing created later tries to use this object's services
            if( localProfile )
            {
                profile.releaseProfile();
            }

            /// Present the gui to the user
            setWidget( ui );

            // Release the QFile
            delete uiFile;
            uiFile = NULL;
            fileLoaded = true;
        }


    }
    return fileLoaded;
}

// Get the form title
QString ASguiForm::getASGuiTitle(){
    return title;
}

// Get the UI file name used to build the gui
QString ASguiForm::getGuiFileName(){
    return uiFileName;
}

/// Set the variable name substitutions used by all QCa widgets wihtin the form
void ASguiForm::setVariableNameSubstitutions( QString variableNameSubstitutionsIn )
{
    variableNameSubstitutions = variableNameSubstitutionsIn;

    // The macro substitutions have changed. Reload the form to pick up new substitutions.
    // NOTE an alternative to this would be to find all QCa widgets contained in the form and it's descentand forms, modify the macro substitutions and reconnect.
    // This is a realistic option since contained widgets now register themselves with the form on creation so the fomr can activate them once all properties have been set up
    if( ui )
    {
        ui->close();
        readUiFile();
    }
}

// Slot for reloading the file if it has changed.
// It doesn't matter if it has been deleted, a reload attempt will still tell the user what they need to know - that the file has gone.
void ASguiForm::fileChanged ( const QString & /*path*/ )
{
    // Ensure we arn't monitoring files any more
    QStringList monitoredPaths = fileMon.files();
    fileMon.removePaths( monitoredPaths );

    // Close any existing form
    if( ui )
    {
        delete ui;
        ui = NULL;
    }

    // Re-open file
    readUiFile();
}

// Common function for all slots presenting messages to the user.
// Normally a gui will have provided it's own message and error handling.
// This is only used if no environment profile has been set up when a form is created. This is the case if created within 'designer'
void ASguiForm::onMessage( QString title, QString message )
{
    QMessageBox msgBox;
    msgBox.setText( title );
    msgBox.setInformativeText( message );
    msgBox.exec();
}

// Slot for launching another form.
// Normally a gui will have provided it's own GUI launch mechanism.
// This is only used if no environment profile has been set up when a form is created. This is the case if created within 'designer'
// Launch a GUI.
// Note, creation options are ignored as the guiForm has no application wide context to know
// what 'creating a new tab', etc, means. A new window is always created.
 void ASguiForm::launchGui( QString guiName, ASguiForm::creationOptions )
 {
     // Build the gui
     // Build it in a new window.
     //??? This could use the create options as follows: (instead of always creating a new window)
     //       - Wind up through parents until the parent of the first scroll
     //       - Replace the scroll area's widget with the new gui
     QMainWindow* w = new QMainWindow;
     ASguiForm* gui = new ASguiForm( guiName );
     if( gui )
     {
         if( gui->readUiFile())
         {
             w->setCentralWidget( gui );
             w->show();
         }
         else
         {
             delete gui;
             gui = NULL;
         }
     }
     else
     {
         delete w;
     }
}

// Slot same as default widget setEnabled slot, but renamed to match other QCa widgets where requestEnabled() will use our own setEnabled
// which will allow alarm states to override current enabled state
void ASguiForm::requestEnabled( const bool& state )
{
    setEnabled(state);
}

//==============================================================================
// Property convenience functions

// Access functions for variableName and variableNameSubstitutions
// variable substitutions Example: SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
void ASguiForm::setVariableNameAndSubstitutions( QString, QString variableNameSubstitutionsIn, unsigned int ) {

    /// Set new variable name substitutions
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
}

// UI file name
void    ASguiForm::setUiFileName( QString uiFileNameIn )
{
    uiFileName = uiFileNameIn;
    readUiFile();
}
QString ASguiForm::getUiFileName()
{
    return uiFileName;
}

// Flag indicating form should handle gui form launch requests
void ASguiForm::setHandleGuiLaunchRequests( bool handleGuiLaunchRequestsIn )
{
    handleGuiLaunchRequests = handleGuiLaunchRequestsIn;
}
bool ASguiForm::getHandleGuiLaunchRequests()
{
    return handleGuiLaunchRequests;
}
