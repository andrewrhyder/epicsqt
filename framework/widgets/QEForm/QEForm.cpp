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

/* This class is used as a container for QE widgets.

    It adds any variable name macro substitutions to the current environment profile, creates a form widget and
    reads a UI file which can contain QE widgets.

    As QE widgets are created, they note the current environment profile, including variable name substitutions.
    QE widgets also register themselves with this class so this class can activate them once they are fully created.
    QE widgets can't activate themselves. The Qt form loader creates each widget and calls the appropriate property
    functions to set it up. The widget itself does not know what properties are going to be set and when they have
    all been set. For this reason the QE widgets don't know when to request CA data. Both variable name properties
    and variable name substitution properties must be set up to request data and other properties may need to be set
    up before udates can be used.

    This class can be used directly (within a GUI application) as the top level form, or as a designer plugin class.
*/

#include <QUiLoader>
#include <QtGui>
#include <QString>
#include <QDir>
#include <QtDebug>
#include <QEForm.h>
#include <ContainerProfile.h>
#include <QEWidget.h>

// Constructor.
// No UI file is read. uiFileName must be set and then readUiFile() called after construction
QEForm::QEForm( QWidget* parent ) : QWidget( parent ), QEWidget( this ) {
    commonInit( false );
}

// Constructor.
// UI filename is supplied and UI file is read as part of construction.
QEForm::QEForm( const QString& uiFileNameIn, QWidget* parent ) : QWidget( parent ), QEWidget( this ) {
    commonInit( true );
    uiFileName = uiFileNameIn;
}

// Common construction
void QEForm::commonInit( const bool alertIfUINoFoundIn )
{
    setAcceptDrops(true);

    ui = NULL;
    alertIfUINoFound = alertIfUINoFoundIn;
    handleGuiLaunchRequests = false;
    resizeContents = true;

    // Set up the UserMessage class
    setFormFilter( MESSAGE_FILTER_MATCH );
    setSourceFilter( MESSAGE_FILTER_NONE );
    childMessageFormId = getNextMessageFormId();
    setChildFormId( childMessageFormId );

    // Setup a valid local profile if no profile was published
    if( !isProfileDefined() )
    {
        QStringList tempPathList;
        tempPathList.append( QDir::currentPath() );
        setupLocalProfile( this, tempPathList, "", "" );
    }

    // Prepare to recieve notification that the ui file being displayed has changed
    QObject::connect( &fileMon, SIGNAL( fileChanged( const QString & ) ), this, SLOT( fileChanged( const QString & ) ) );

    // Set up a connection to recieve variable name property changes (Actually only interested in substitution changes
    QObject::connect( &variableNamePropertyManager, SIGNAL( newVariableNameProperty( QString, QString, unsigned int ) ),
                      this, SLOT( useNewVariableNameProperty( QString, QString, unsigned int) ) );
}

// Destructor.
QEForm::~QEForm()
{
    // Close any existing form
    if( ui )
        ui->close();
}

// Debug function to list the widget hierarchy
//void showObjects( QObject* obj )
//{
//    static int depth = 0;

//    QWidget* w;
//    if( obj->isWidgetType() )
//    {
//        w = (QWidget*)obj;
//        qDebug() << depth << obj->metaObject()->className() << w->pos() << w->size();
//    }
//    QObjectList objList = obj->children();
//    depth++;
//    for( int i = 0; i < objList.size(); i++ )
//    {
//        showObjects( objList[i] );
//    }
//    depth--;
//}

// Read a UI file.
// The file read depends on the value of uiFileName
bool QEForm::readUiFile()
{
    // Assume file is bad
    bool fileLoaded = false;

    // If any name has been provided...
    if (!uiFileName.isEmpty()) {

        // Set up the environment profile for any QE widgets created by the form
        QObject* savedGuiLaunchConsumer = NULL;

        // Try to open the UI file
        QFile* uiFile = openQEFile( uiFileName, QIODevice::ReadOnly );

        // If the file was not found and opened, notify as appropriate
        if( !uiFile )
        {
            if( alertIfUINoFound )
            {
                QString msg;
                QTextStream(&msg) << "User interface file '" << uiFileName << "' could not be opened";
                sendMessage( msg, "QEForm::readUiFile", MESSAGE_TYPE_WARNING );
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

            // Get filename info
            QFileInfo fileInfo( uiFile->fileName() );

            // Note the full file path
            QDir uiDir;
            fullUiFileName = uiDir.cleanPath( uiDir.absoluteFilePath( uiFile->fileName() ) );

            // Ensure no other files are being monitored (belt and braces)
            QStringList monitoredPaths = fileMon.files();
            if( monitoredPaths.count())
            {
                fileMon.removePaths( monitoredPaths );
            }

            // Monitor the opened file
            fileMon.addPath( fullUiFileName );

            // If profile has been published (for example by an application creating this form), then publish our own local profile
            bool localProfile = false;
            if( !isProfileDefined() )
            {
                // Flag the profile was set up in this function (and so should be released in this function)
                localProfile = true;

                publishOwnProfile();
            }

            // Add this form's macro substitutions for all it's children to use
            // Note, any macros in the substitutions are themselves substituted before applying the substitutions to the form
            addMacroSubstitutions( substituteThis( variableNameSubstitutions ) );

            // Temporarily update the published current object's path to the path of the form being created.
            // Any objects created within the form (for example sub forms) can then know where their parent form is located.
            setPublishedParentPath( fileInfo.absolutePath() );

            // If this form is handling form launch requests from object created within it, replace any form launcher with our own
            if( handleGuiLaunchRequests )
                savedGuiLaunchConsumer = replaceGuiLaunchConsumer( this );

            // Note the current published message form ID, and set up a new
            // message form ID for widgets created within this form.
            // This new message form ID will also be used when matching the
            // form ID of received messages
            unsigned int parentMessageFormId = getPublishedMessageFormId();
            setPublishedMessageFormId( childMessageFormId );

            // Load the gui
            QUiLoader loader;

            ui = loader.load( uiFile );
            uiFile->close();

            // Restore the original published message form ID
            setPublishedMessageFormId( parentMessageFormId );

            // Remove this form's macro substitutions now all it's children are created
            removeMacroSubstitutions();

            // Reset the published current object's path to what ever it was
            setPublishedParentPath( getParentPath() );

            // If this form is handling form launch requests from object created within it, put back any original
            // form launcher now all objects have been created
            if ( handleGuiLaunchRequests )
                 replaceGuiLaunchConsumer( savedGuiLaunchConsumer );

            // Any QE widgets that have just been created need to be activated.
            // Note, this is only required when QE widgets are not loaded within a form and not directly by 'designer'.
            // When loaded directly by 'designer' they are activated (a CA connection is established) as soon as either
            // the variable name or variable name substitution properties are set
            QEWidget* containedWidget;
            while( (containedWidget = getNextContainedWidget()) )
                containedWidget->activate();

            // If the published profile was published within this method, release it so nothing created later tries to use this object's services
            if( localProfile )
            {
                releaseProfile();
            }

            // If the form contents should take all its sizing clues from the form, then set the top ui widget to match
            // the form's size related properties.
            if( resizeContents )
            {
                QRect formRect = ui->geometry();
                ui->setGeometry( formRect.x(), formRect.y(), width(), height() );
                ui->setSizePolicy( sizePolicy() );
                ui->setMinimumSize( minimumSize() );
                ui->setMaximumSize( maximumSize() );
                ui->setSizeIncrement( sizeIncrement() );
                ui->setBaseSize( baseSize() );
                ui->setContentsMargins( contentsMargins() );
            }
            // If the form should take all its sizing clues from the form's contents, then set the form to match
            // the top ui widget's size related properties.
            else
            {
                QRect formRect = geometry();
                setGeometry( formRect.x(), formRect.y(), ui->width(), ui->height() );
                setSizePolicy( ui->sizePolicy() );
                setMinimumSize( ui->minimumSize() );
                setMaximumSize( ui->maximumSize() );
                setSizeIncrement( ui->sizeIncrement() );
                setBaseSize( ui->baseSize() );
                setContentsMargins( ui->contentsMargins() );
            }
            // Reset the user interface's position.
            // Not sure why, but the loaded user interface does not always have a position of 0,0
            // When debugged, the particular example was a QDialog with a position of 0,0 when viewed in designer.
            QRect uiRect = ui->geometry();
            ui->setGeometry(0, 0, uiRect.width(), uiRect.height());

            // Set the title to the name of the top level widget title, if it has one
            title.clear();
            QVariant windowTitleV = ui->property( "windowTitle" );
            if( windowTitleV.isValid() && windowTitleV.type() == QVariant::String )
            {
                QString windowTitle = windowTitleV.toString();
                if( !windowTitle.isEmpty() )
                {
                    title = substituteThis( windowTitle );
                }
            }

            // If no title was obtained from the ui, use the file name
            if( title.isEmpty() )
            {
                // Extract the file name part used for the window title
                QFileInfo fileInfo( uiFile->fileName() );
                title = QString( "QEGui " ).append( fileInfo.fileName() );
                if( title.endsWith( ".ui" ) )
                    title.chop( 3 );
            }

            // Load the user interface into the QEForm widget
            ui->setParent( this );
            ui->show();         // note, this show is only needed when replacing ui in existing QEForm

            // If the ui is managed by a layout, add a layout to the QEform (if not already present) and add
            // the ui to the layout so layout requests are passed down
            if( ui->layout() )
            {
                QLayout* lo = layout();
                if( !lo )
                {
                    lo = new QVBoxLayout;
                    setLayout( lo );
                }
                lo->addWidget( ui );
            }

            // Release the QFile
            delete uiFile;
            uiFile = NULL;
            fileLoaded = true;

// Debuging only  showObjects( this );

        }
    }
    return fileLoaded;
}


// Get the form title
QString QEForm::getQEGuiTitle(){
    return title;
}

// Get the standard, absolute UI file name
QString QEForm::getFullFileName()
{
    return fullUiFileName;
}

// Set the variable name substitutions used by all QE widgets within the form
void QEForm::setVariableNameSubstitutions( QString variableNameSubstitutionsIn )
{
    variableNameSubstitutions = variableNameSubstitutionsIn;

    // The macro substitutions have changed. Reload the form to pick up new substitutions.
    // NOTE an alternative to this would be to find all QE widgets contained in the form and it's descentand forms, modify the macro substitutions and reconnect.
    // This is a realistic option since contained widgets now register themselves with the form on creation so the fomr can activate them once all properties have been set up
    if( ui )
    {
        ui->close();
        readUiFile();
    }
}

// Slot for reloading the file if it has changed.
// It doesn't matter if it has been deleted, a reload attempt will still tell the user what they need to know - that the file has gone.
void QEForm::fileChanged ( const QString & /*path*/ )
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

// Slot for launching another form.
// Normally a gui will have provided it's own GUI launch mechanism.
// This is only used if no environment profile has been set up when a form is created. This is the case if created within 'designer'
// Launch a GUI.
// Note, creation options are ignored as the guiForm has no application wide context to know
// what 'creating a new tab', etc, means. A new window is always created.
 void QEForm::launchGui( QString guiName, QEForm::creationOptions )
 {
     // Build the gui
     // Build it in a new window.
     //??? This could use the create options as follows: (instead of always creating a new window)
     //       - Wind up through parents until the parent of the first scroll
     //       - Replace the scroll area's widget with the new gui
     QMainWindow* w = new QMainWindow;
     QEForm* gui = new QEForm( guiName );
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

// Receive new log messages.
// This widget doesn't do anything itself with messages, but it can regenerate the message as if it came from itself.
void QEForm::newMessage( QString msg, message_types type )
{
    // A QEForm deals with any message it receives from widgets it contains by resending it with its own form and source ids.
    // This way messages from widgets in QEForm widgets will be filtered as if they came from the form. This means a widget can
    // treat a sibling QEForm as a single message generating entity (and set up filters accordingly) and not worry about
    // exactly what widget within the form generated the message.
    sendMessage( msg, type );
}

// The form is being resized.
// Resize the ui to match.
// (not required if a layout is present)
void QEForm::resizeEvent ( QResizeEvent * event )
{
    // If the form's ui does not have a layout, resize it to match the QEForm
    // If it does have a layout, then the QEForm will also have given itself a
    // layout to ensure layout requests are propogated. In this case a resize is not nessesary.
    if( ui && !ui->layout() )
    {
        ui->resize( event->size() );
    }
}

//==============================================================================
// Property convenience functions

// Access functions for variableName and variableNameSubstitutions
// variable substitutions Example: SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
void QEForm::setVariableNameAndSubstitutions( QString, QString variableNameSubstitutionsIn, unsigned int ) {

    // Set new variable name substitutions
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
}

// UI file name
void    QEForm::setUiFileName( QString uiFileNameIn )
{
    uiFileName = uiFileNameIn;
    readUiFile();
}
QString QEForm::getUiFileName()
{
    return uiFileName;
}

// Flag indicating form should handle gui form launch requests
void QEForm::setHandleGuiLaunchRequests( bool handleGuiLaunchRequestsIn )
{
    handleGuiLaunchRequests = handleGuiLaunchRequestsIn;
}
bool QEForm::getHandleGuiLaunchRequests()
{
    return handleGuiLaunchRequests;
}

// Flag indicating form should resize contents to match form size (otherwise resize form to match contents)
void QEForm::setResizeContents( bool resizeContentsIn )
{
    resizeContents = resizeContentsIn;
}
bool QEForm::getResizeContents()
{
    return resizeContents;
}
