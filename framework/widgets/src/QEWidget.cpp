/*  QEWidget.cpp
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
 *    Anthony Owen
 *  Contact details:
 *    anthony.owen@gmail.com
 */

/*
  This class is used as a base for all CA aware wigets, such as QELabel, QESpinBox, etc.
  It manages common issues including creating a source of CA data updates, handling error,
  warning and status messages, and setting tool tips based on variable names.

  Note, there is tight integration between the CA aware widget classes, this class, and its
  base classes, especially VariableNameManager and QEToolTip.

  Refer to the class description in QEWidget.h for further details
 */

#include <QDebug>
#include <QEWidget.h>
#include <QEFrameworkVersion.h>

/*
    Constructor
*/
QEWidget::QEWidget( QWidget *ownerIn ) : QEToolTip( ownerIn ), QEDragDrop( ownerIn ), styleManager( ownerIn ), standardProperties( ownerIn )
{
    // Sanity check.
    if( ownerIn == NULL )
    {
        qWarning( "QEWidget constructor called with a null 'owner'" );
        exit( EXIT_FAILURE );
    }

    // Initially flag no variables array is defined.
    // This will be corrected when the first variable is declared
    numVariables = 0;
    qcaItem = 0;

    // Default properties
    subscribe = true;
    setSourceId( 0 );

    // Set the UserMessage form ID to be whatever has been published in the ContainerProfile
    setFormId( getMessageFormId() );

    // If there is a profile defining the environment containing this widget add this widget to the
    // list of contained widgets so whatever is managing the container can activate this widget.
    //
    // Although a widget is self contained, whatever is creating the widget has the option of providing
    // a list of services and other information through a containerProfile that QEWidgets can use.
    // For example, an application creating QEWidgets can provide a mechanism to display error
    // messages in a manner appropriate for the application.
    // In this case, the widget is taking the oppertunity to tell its creator it exists, and also to
    // get any variable name macro substitutions offered by its creator.
    if( isProfileDefined() )
    {
        addContainedWidget( this );
        setVariableNameSubstitutionsOverride( getMacroSubstitutions() );
    }

    // Setup to respond to requests to save or restore persistant data
    saveRestoreReceiver.setOwner( this );
    PersistanceManager* persistanceManager = getPersistanceManager();
    if( persistanceManager )
    {
        QObject::connect( persistanceManager->getSaveRestoreObject(),
                          SIGNAL( saveRestore( SaveRestoreSignal::saveRestoreOptions ) ),
                          &saveRestoreReceiver,
                          SLOT( saveRestore( SaveRestoreSignal::saveRestoreOptions ) ),
                          Qt::DirectConnection );
    }
}

/*
    Destruction:
    Delete all variable sources for the widgeet
*/
QEWidget::~QEWidget() {
    // Remove this widget remove this widget from the list of contained widgets if it is there.
    // The list is only used during form construction and generally widgets are not destroyed during form
    // construction, but there are exceptions. A typical exception is QEMotor, which creates and sometimes
    // destroys QELabels during contruction. These QELabels get added to the contained widgets list
    // but are then destroyed. Unless they are removed from the list, the form will attempt to activate them.
    removeContainedWidget( this );

    // Delete all the QCaObject instances
    for( unsigned int i = 0; i < numVariables; i++ ) {
        deleteQcaItem( i );
    }

    // Release the list
    delete[] qcaItem;
    qcaItem = NULL;
}

/*
    Set the number of variables that will be used for this widget.
    Create an array of QCaObject based objects to suit.
    This is called by the CA aware widgets based on this class, such as a QELabel.
*/
void QEWidget::setNumVariables( unsigned int numVariablesIn ) {

    // Get the number of variables that will be used by this widget
    // Don't accept zero or the qca array will be invalid
    if( numVariablesIn ) {
        numVariables = numVariablesIn;
    } else {
        numVariables = 1;
    }

    // Set up the number of variables managed by the variable name manager
    variableNameManagerInitialise( numVariables );

    // Allocate the array of QCa objects
    qcaItem = new qcaobject::QCaObject* [numVariables];
    for( unsigned int i = 0; i < numVariables; i++ ) {
        qcaItem[i] = NULL;
    }
}

/*
   Initiate updates.
   This is only required when QE widgets are loaded within a form and not directly by 'designer'.
   When loaded directly by 'designer' they are activated (a CA connection is established) as soon as either
   the variable name or variable name substitution properties are set
 */
void QEWidget::activate()
{
    // For each variable, ask the CA aware widget based on this class to initiate updates and to set up
    // whatever signal/slot connections are required to make use of data updates.
    // Note, establish connection is a virtual function of the VariableNameManager class and is normally
    // called by that class when a variable name is defined or changed
    for( unsigned int i = 0; i < numVariables; i++ )
        establishConnection( i );
}

/*
   Terminate updates.
   This has been provided for third party (non QEGui) applications using the framework.
 */
void QEWidget::deactivate()
{
    // Delete all the QCaObject instances
    for( unsigned int i = 0; i < numVariables; i++ ) {
        deleteQcaItem( i );
    }
}


/*
    Create a CA connection and initiates updates if required.
    This is called by the establishConnection function of CA aware widgets based on this class, such as a QELabel.
    If successfull it will return the QCaObject based object supplying data update signals
*/
qcaobject::QCaObject* QEWidget::createConnection( unsigned int variableIndex ) {

    // If the index is invalid do nothing
    // This same test is also valid if qcaItem has never been set up yet as numVariables will be zero
    if( variableIndex >= numVariables ) {
        return NULL;
    }

    // Update the variable names in the tooltip if required
    setToolTipFromVariableNames();

    // Remove any existing QCa connection
    deleteQcaItem( variableIndex );

    // Connect to new variable.
    // If a new variable name is present, ask the CA aware widget based on this class to create an
    // appropriate object based on a QCaObject (by calling its createQcaItem() function).
    // If that is successfull, supply it with a mechanism for handling errors and subscribe
    // to the new variable if required.
    if( getSubstitutedVariableName( variableIndex ).length() > 0 ) {
        qcaItem[variableIndex] = createQcaItem( variableIndex );
        if( qcaItem[variableIndex] ) {

            qcaItem[variableIndex]->setUserMessage( (UserMessage*)this );

            if( subscribe )
                qcaItem[variableIndex]->subscribe();
        }
    }

    // Return the QCaObject, if any
    return qcaItem[variableIndex];
}

// Default implementation of createQcaItem().
// Usually a QE widgets will request a connection be established by this class and this class will
// call back the QE widgets for it to create the specific flavour of QCaObject required using this function.
// Since this class can also be used as a base class for widgets that don't establish any CA connection,
// this default implementation is here to always return NULL when asked to create a QCaObject
//
qcaobject::QCaObject* QEWidget::createQcaItem( unsigned int )
{
    return NULL;
}

// Default implementation of establishConnection().
// Usually a QE widgets will request a connection be established by this class and this class will
// call back the QE widgets for it to establish a connection on a newly created QCaObject using this function.
// Since this class can also be used as a base class for widgets that don't establish any CA connection,
// this default implementation is here as a default when not implemented
//
void QEWidget::establishConnection( unsigned int )
{
}


/*
    Return a reference to one of the qCaObjects used to stream CA data updates to the widget
    This is called by CA aware widgets based on this class, such as a QELabel, mainly when they
    want to connect to its signals to recieve data updates.
*/
qcaobject::QCaObject* QEWidget::getQcaItem( unsigned int variableIndex ) {
    // If the index is invalid return NULL.
    // This same test is also valid if qcaItem has never been set up yet as numVariables will be zero
    if( variableIndex >= numVariables )
        return NULL;

    // Return the QCaObject used for the specified variable name
    return qcaItem[variableIndex];
}

/*
    Remove any previous QCaObject created to supply CA data updates for a variable name
*/
void QEWidget::deleteQcaItem( unsigned int variableIndex ) {
    // If the index is invalid do nothing.
    // This same test is also valid if qcaItem has never been set up yet as numVariables will be zero
    if( variableIndex >= numVariables )
        return;

    // Delete the QCaObject used for the specified variable name
    if( qcaItem[variableIndex] )
        delete qcaItem[variableIndex];

    // Remove the reference to the deleted object to prevent accidental use
    qcaItem[variableIndex] = NULL;
}

/*
  Take a menu widgt and add it as the context menu for this widget
 */
void QEWidget::setupContextMenu( QWidget* w )
{
    // Set up context sensitive menu (right click menu)
    addContextMenuToWidget( w );
}

/*
  Return a colour to update the widget's look to reflect the current alarm state
  Note, the color is determined by the alarmInfo class, but since that class is used in non
  gui applications, it can't return a QColor
 */
QColor QEWidget::getColor( QCaAlarmInfo& alarmInfo, int saturation )
{
    QColor result(alarmInfo.getColorName());

    int h, s, v;
    result.getHsv( &h, &s, &v );
    result.setHsv( h, saturation, 255 );
    return result;
}

// Update the variable name list used in tool tips if requried
void QEWidget::setToolTipFromVariableNames()
{
    // Set the variable name that will be used as the tool tip if required
    // Determine what seperator to place between variable names. To avoid long tool tips, use line breaks if over two variables
    QString seperator;
    if( numVariables > 2 )
    {
        seperator = "\n";
    }
    else
    {
        seperator = " ";
    }

    // Build tip
    QString tip;
    for( unsigned int i = 0; i < numVariables; i++ ) {
        // If a variable name is present, add it to the tip
        QString variableName = getSubstitutedVariableName( i );
        if( variableName.size() ) {
            // Add a seperator between variable names
            if( tip.size() > 0 ) {
                tip.append( seperator );
            }
            // Add the variable name to the tip
            tip.append( variableName );
        }
    }

    // If no variables, note that fact in the tip
    if( tip.size() == 0 ) {
        tip = "No variables defined";
    }

    // Update the top variable names (will only take effect if variable names are actually being used as the tool tip.
    updateToolTipVariable( tip );
}

// Returns true if running within the Qt Designer application.
// used when the behaviour needs to be different in designer.
// For example, a run-time-visible property - always visible in designer, visible at run time dependant on the property.
bool QEWidget::inDesigner()
{
    // check if the current executable has 'designer' in the name
    // Note, depending on Qt version, (and installation?) designer image may be 'designer' or 'designer-qt4'
    QString appPath = QCoreApplication::applicationFilePath();
    QFileInfo fi( appPath );
    return fi.baseName().contains( "designer" );
}

//==============================================================================
// User level

// The user level has changed
// Modify the label properties accordingly
void QEWidget::userLevelChanged( userLevels level )
{
    styleUserLevelChanged( level );
    checkVisibilityEnabledLevel( level );
}

// Perform a single shot read on all variables.
// Widgets may be write only and do not need to subscribe (subscribe property is false).
// When not subscribing it may still be usefull to do a single shot read to get initial
// values, or perhaps confirm a write.
void QEWidget::readNow()
{
    // Perform a single shot read on all variables.
    qcaobject::QCaObject* qca;
    for( unsigned int i = 0; i < numVariables; i++ )
    {
        qca = getQcaItem( i );
        if( qca ) // If variable exists...
        {
            qca->singleShotRead();
        }
    }
}

// Access functions for variableName and variableNameSubstitutions
// variable substitutions Example: SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
void QEWidget::setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
{
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
    setVariableName( variableNameIn, variableIndex );
    if( !getDontActivateYet() )
    {
        establishConnection( variableIndex );
    }
}

// Returns the default location to create files.
// Use this to create files in a consistant location
QString QEWidget::defaultFileLocation()
{
    QString path = getParentPath();
    if( !path.isEmpty() )
    {
        return path;
    }

    path = getPath();
    if( !path.isEmpty() )
    {
        return path;
    }

    path = QDir::currentPath();
    if( !path.isEmpty() )
    {
        return path;
    }
    return "";
}

// Returns an open file given a file name.
// This uses findQEFile() to find files in a consistant set of locations. Refer to findQEFile() for details.
QFile* QEWidget::openQEFile( QString name, QFile::OpenModeFlag mode )
{
    // Find the file
    QFile* uiFile = findQEFile( name, this );
    if( uiFile )
    {
        if( !uiFile->open( mode ) )
        {
            delete uiFile;
            uiFile = NULL;
        }
    }
    return uiFile;
}

// Returns a QFile given a file name, or NULL if can't find the file
// Use this to find files in a consistant set of locations:
// If the file name contains an absolute path, then no options, just try to open it
// If the file name contains a relative path (including no path) look in the following locations:
//  - The directory where the parent object (form) was read from (set up in the application profile)
//  - The application's path (set up in the application profile) (the -p switch for QEGui)
//  - The current directory
//  - The environment variable QE_UI_PATH

QFile* QEWidget::findQEFile( QString name, ContainerProfile* profile )
{
        // Build a list of all the places we expect to find the file
        // Use a single location if an absolute path was specified.
        // Use the following list of locations if a relative path was specified:
        //  - The directory where the parent object (form) was read from (set up in the application profile)
        //  - The application's path list (set up in the application profile) (the -p switch for QEGui)
        //  - The current directory
        //  - The environment variable QE_UI_PATH
        QStringList searchList;
        if(  QDir::isAbsolutePath( name ) )
        {
            searchList.append( name );
        }
        else
        {
            QFileInfo fileInfo;

            // Add the parent path from any parent QEForm
            QString parentPath =  profile->getParentPath();
            if( !parentPath.isEmpty() )
            {
                fileInfo.setFile( parentPath, name );
                searchList.append( fileInfo.filePath() );
            }

            // Add the paths from the path list in the container profile
            QStringList pathList = profile->getPathList();
            for( int i = 0; i < pathList.count(); i++ )
            {
                fileInfo.setFile( pathList[i], name );
                searchList.append(  fileInfo.filePath() );
            }

            // Add the current directory
            fileInfo.setFile( QDir::currentPath(), name );
            searchList.append(  fileInfo.filePath() );

            // Add paths from environment variable
            QStringList envPathList = profile->getEnvPathList();
            for( int i = 0; i < envPathList.count(); i++ )
            {
                fileInfo.setFile( envPathList[i], name );
                searchList.append(  fileInfo.filePath() );
            }
        }

        // Attempt to open the file
        QFile* uiFile = NULL;
        for( int i = 0; i < searchList.count(); i++ )
        {
            uiFile = new QFile( searchList[i] );
            if( uiFile->exists() )
                break;
            delete uiFile;
            uiFile = NULL;
        }
        return uiFile;
}

// Returns the QE framework that built this instance of the widget.
QString QEWidget::getFrameworkVersion()
{
    return QE_VERSION_STRING " " QE_VERSION_DATE_TIME;
}

saveRestoreSlot::saveRestoreSlot()
{
    owner = NULL;
}
saveRestoreSlot::~saveRestoreSlot()
{
}

void saveRestoreSlot::setOwner( QEWidget* ownerIn )
{
    owner = ownerIn;
}

// A save or restore has been requested
void saveRestoreSlot::saveRestore( SaveRestoreSignal::saveRestoreOptions option )
{
    if( !owner )
    {
        return;
    }

    PersistanceManager* pm = owner->getPersistanceManager();
    if( !pm )
    {
        return;
    }

    switch( option )
    {
        case SaveRestoreSignal::SAVE:
            owner->saveConfiguration( pm );
            break;

        case SaveRestoreSignal::RESTORE:
            owner->restoreConfiguration( pm );
            break;
    }
}

