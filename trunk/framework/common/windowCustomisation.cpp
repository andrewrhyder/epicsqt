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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/* Description:
 *
 * Helps applications such as QEGui and QE widgets add menu items from the main window menu bar
 * and push buttons to the main window tool bar.
 * Named customisations are saved in .xml files.
 * Any number of customisation .xml files can be read, building up a directory of named customisations.
 * Customisations can be requested by name.
 * Customisations may include other customisations by name.
 * A customisation can be aliased by defining a customisation that only includes another named customisation.
 *
 * REFER TO windowCustomisation.h for more details on how to use this module.
 */

#include <windowCustomisation.h>
#include <QDebug>
#include <QFile>
#include <QMenuBar>
#include <QToolBar>
#include <QEWidget.h>
#include <QDockWidget>

//==============================================================================================
// windowCustomisationItem
//==============================================================================================

// Construct instance of class defining an individual item when none exists (for example, a menu placeholder)
windowCustomisationItem::windowCustomisationItem() : QAction( 0 )
{
    profile.takeLocalCopy();

//    creationOption = QEActionRequests::OptionNewWindow;
}

// Construct instance of class defining a built in application action
windowCustomisationItem::windowCustomisationItem( const QString builtInActionIn )
                                                  : QAction( 0 )
{
    profile.takeLocalCopy();

    builtInAction = builtInActionIn;
}

windowCustomisationItem::windowCustomisationItem( const QString builtInActionIn,
                                                  const QString widgetNameIn )                           // widget name if built in function is for a widget, not the application
                                                  : QAction( 0 )
{
    profile.takeLocalCopy();

    builtInAction = builtInActionIn;
    widgetName = widgetNameIn;
}

// Construct instance of class defining an individual item (base class for button or menu item)
windowCustomisationItem::windowCustomisationItem(
    const QObject* /*launchRequestReceiver*/,            // Object (typically QEGui application) which will accept requests to launch a new GUI
    const QList<windowCreationListItem>& windowsIn,      // Windows to display (centrals and docks)
    const QString programIn,                             // Program to run
    const QStringList argumentsIn )                      // Arguments for 'program'
         : QAction( 0 )
{
    profile.takeLocalCopy();

    // Save the item details
    for( int i = 0; i < windowsIn.count(); i++ )
    {
        windows.append( windowsIn.at(i));
    }
    program = programIn;
    arguments = argumentsIn;
}

// Construct instance of class defining an individual item (base class for button )or menu item)
windowCustomisationItem::windowCustomisationItem(windowCustomisationItem* item): QAction( 0 )
{
    profile.takeLocalCopy();

    // Save the item details
    for( int i = 0; i < item->windows.count(); i++ )
    {
        windows.append( item->windows.at(i));
    }
    program = item->getProgram();
    arguments = item->getArguments();
    builtInAction = item->getBuiltInAction();

    widgetName = item->widgetName;

    dockTitle = item->dockTitle;
}

// Construct instance of class defining a link to an existing dock
windowCustomisationItem::windowCustomisationItem( const QString dockTitleIn, bool /*unused*/ ): QAction( 0 )
{
    profile.takeLocalCopy();

    dockTitle = dockTitleIn;
}

// A menu item or button has been created, let the application or widget know about it
void windowCustomisationItem::initialise()
{
    if( !builtInAction.isEmpty() && !widgetName.isEmpty() )
    {
        emit newGui( QEActionRequests( builtInAction, widgetName, arguments, true, this ) );
    }
}

// Return true if at least one dock is created by this item
bool windowCustomisationItem::createsDocks()
 {
     for( int i = 0; i < windows.count(); i++ )
     {
         if( QEActionRequests::isDockCreationOption( windows.at(i).creationOption ) )
         {
             return true;
         }
     }
     return false;
 }

// A user has triggered the menu item or button
void windowCustomisationItem::itemAction()
{
    if( windows.count() )
    {
        profile.publishOwnProfile();
        emit newGui( QEActionRequests( windows ));
        profile.releaseProfile();
    }
    else if ( !builtInAction.isEmpty() )
    {
        // If no widget name, then assume the action is for the application
        if( widgetName.isEmpty() )
        {
            profile.publishOwnProfile();
            emit newGui( QEActionRequests( builtInAction, "" )  );
            profile.releaseProfile();
        }
        // A widget name is present, assume the action is for a QE widget created by the application
        else
        {
            emit newGui( QEActionRequests( builtInAction, widgetName, arguments, false, this ) );
        }
    }
}

//==============================================================================================
// windowCustomisationMenuItem
//==============================================================================================

// Construct instance of class defining an individual menu item (not a placeholder for items the application might add)
windowCustomisationMenuItem::windowCustomisationMenuItem(
                          const QStringList menuHierarchyIn,                   // Location in menus to place this item. for example: 'Imaging'->'Region of interest'
                          const QString titleIn,                               // Name of this item. for example: 'Region 1'
                          const menuObjectTypes typeIn,                        // type of menu object - must be MENU_ITEM
                          const bool separatorIn,                              // Separator required before this

                          const QObject* launchRequestReceiver,                // Object (typically QEGui application) which will accept requests to launch a new GUI
                          const QList<windowCreationListItem>& windowsIn,      // Windows to display (centrals and docks)
                          const QString programIn,                             // Program to run
                          const QStringList argumentsIn )                      // Arguments for 'program'
                          : windowCustomisationItem( launchRequestReceiver, windowsIn, programIn, argumentsIn )
{
    type = typeIn;
    menuHierarchy = menuHierarchyIn;
    title = titleIn;
    separator = separatorIn;
}

// Construct instance of class defining an item that will request the application (or a QE widget) take a named action
windowCustomisationMenuItem::windowCustomisationMenuItem(
                          const QStringList menuHierarchyIn,                   // Location in menus for application to place future items. for example: 'File' -> 'Recent'
                          const QString titleIn,                               // Title for this item. for example: 'Region 1' Usually same as name of built in function. (for example, function='Copy' and title='Copy', but may be different (function='LaunchApplication1' and title='paint.exe')
                          const menuObjectTypes typeIn,                        // type of menu object - must be MENU_PLACEHOLDER or MENU_BUILT_IN
                          const bool separatorIn,                              // Separator required before this

                          const QString builtIn,                               // Name of built in function (built into the application or a QE widget). For example: 'Region 1'
                          const QString widgetNameIn )                         // widget name if built in function is for a widget, not the application

                          : windowCustomisationItem( builtIn, widgetNameIn )
{
    type = typeIn;
    menuHierarchy = menuHierarchyIn;
    title = titleIn;
    separator = separatorIn;
}

// Construct instance of class defining an item that will be a placeholder. The application can locate placeholder menu items and use them directly
windowCustomisationMenuItem::windowCustomisationMenuItem(
                          const QStringList menuHierarchyIn,                   // Location in menus for application to place future items. for example: 'File' -> 'Recent'
                          const QString titleIn,                               // Identifier of placeholder. for example: 'Recent'
                          const menuObjectTypes typeIn,                        // type of menu object - must be MENU_PLACEHOLDER or MENU_BUILT_IN
                          const bool separatorIn )                             // Separator required before this

                          : windowCustomisationItem( titleIn )
{
    type = typeIn;
    menuHierarchy = menuHierarchyIn;
    title = titleIn;
    separator = separatorIn;
}

// Construct instance of class defining an item that will be associated with an existing dock (assocaition is by dock title)
windowCustomisationMenuItem::windowCustomisationMenuItem(
                      const QStringList menuHierarchyIn,                   // Location in menus for application to place future items. for example: 'File' -> 'Recent'
                      const QString titleIn,                               // Title for this item. for example: 'Brightness/Contrast' Must match the title of the dock widget it is to be associated with.
                      const menuObjectTypes typeIn,                        // type of menu object - must be MENU_ITEM
                      const bool separatorIn,                              // Separator required before this

                      const QString dockTitleIn )                          // Title of existing dock widget to assocaite the menu item with
                      : windowCustomisationItem( dockTitleIn, true )
{
    type = typeIn;
    menuHierarchy = menuHierarchyIn;
    title = titleIn;
    separator = separatorIn;
}


// Copy constructor
windowCustomisationMenuItem::windowCustomisationMenuItem(windowCustomisationMenuItem* menuItem)                  // New window customisation name (menu, buttons, etc)
                          : windowCustomisationItem( menuItem )
{
    type = menuItem->type;
    menuHierarchy = menuItem->getMenuHierarchy();
    title = menuItem->getTitle();
    setText(title);
    setParent(this);
    separator = menuItem->separator;


    // Set up an action to respond to the user
    connect( this, SIGNAL( triggered()), this, SLOT(itemAction()));
}

// Add an initial menu hierarchy.
// Used when including a customisation set at a particuar point in another customisation set
void windowCustomisationMenuItem::prependMenuHierarchy( QStringList preMenuHierarchy )
{
    int count = preMenuHierarchy.count();
    for( int i = count - 1; i >= 0; i-- )
    {
        menuHierarchy.prepend( preMenuHierarchy.at(i) );
    }
}

//==============================================================================================
// windowCustomisationButtonItem
//==============================================================================================

// Construct instance of class defining an individual button item
windowCustomisationButtonItem::windowCustomisationButtonItem(
                        const QString buttonGroupIn,                         // Name of toolbar button group in which to place a button
                        const QString buttonTextIn,                          // Text to place in button
                        const QString buttonIconIn,                          // Icon for button

                        const QObject* launchRequestReceiver,                // Object (typically QEGui application) which will accept requests to launch a new GUI
                        const QList<windowCreationListItem>& windowsIn,      // Windows to display (centrals and docks)
                        const QString programIn,                             // Program to run
                        const QStringList argumentsIn )                      // Arguments for 'program'
                            : windowCustomisationItem( launchRequestReceiver, windowsIn, programIn, argumentsIn )
{
    buttonGroup = buttonGroupIn;
    buttonText = buttonTextIn;
    buttonIcon = buttonIconIn;
}

// Construct instance of class defining an individual button item
windowCustomisationButtonItem::windowCustomisationButtonItem(
                        const QString buttonGroupIn,                         // Name of toolbar button group in which to place a button
                        const QString buttonTextIn,                          // Text to place in button
                        const QString buttonIconIn )                         // Icon for button
                            : windowCustomisationItem()
{
    buttonGroup = buttonGroupIn;
    buttonText = buttonTextIn;
    buttonIcon = buttonIconIn;
}

// Copy construct
windowCustomisationButtonItem::windowCustomisationButtonItem(windowCustomisationButtonItem* buttonItem)                  // New window customisation name (menu, buttons, etc)
                            : windowCustomisationItem( buttonItem )
{
    buttonGroup = buttonItem->getButtonGroup();
    buttonText = buttonItem->getButtonText();
    buttonIcon = buttonItem->getButtonIcon();
    setText(buttonText);
    setParent(this);

    // Set up an action to respond to the user
    connect( this, SIGNAL( triggered()), this, SLOT(itemAction()));
}

//==============================================================================================
// windowCustomisation
//==============================================================================================

// Class defining the customisation for a window.
// Construction - create a named, empty, customisation
windowCustomisation::windowCustomisation( QString nameIn )//: recentMenuPoint(NULL), windowMenuPoint(NULL), recentMenuSet(false), windowMenuSet(false)
{
    name = nameIn;
}

// Destruction - release customisation items
windowCustomisation::~windowCustomisation()
{
    // Release customisation items
    while( !menuItems.isEmpty() )
    {
        delete menuItems.takeFirst();
    }
    while( !buttons.isEmpty() )
    {
        delete buttons.takeFirst();
    }
}

// Add a menu item to the customisation
// NOTE! windowCustomisation TAKES OWNERSHIP of menuItem
void windowCustomisation::addItem( windowCustomisationMenuItem* menuItem, QStringList preMenuHierarchy )
{
    menuItems.append( menuItem );
    menuItems.last()->prependMenuHierarchy( preMenuHierarchy );
}

// Add a button to the customisation
// NOTE! windowCustomisation TAKES OWNERSHIP of button
void windowCustomisation::addItem( windowCustomisationButtonItem* button )
{
    buttons.append( button );
}

// Translate creation option text from .xml file to enumeration in QEActionRequests
QEActionRequests::Options windowCustomisation::translateCreationOption( QString creationOption )
{
         if( creationOption.compare( "Open"              ) == 0 ) { return QEActionRequests::OptionOpen;                   }
    else if( creationOption.compare( "NewTab"            ) == 0 ) { return QEActionRequests::OptionNewTab;                 }
    else if( creationOption.compare( "NewWindow"         ) == 0 ) { return QEActionRequests::OptionNewWindow;              }

    else if( creationOption.compare( "FloatingDock"      ) == 0 ) { return QEActionRequests::OptionFloatingDockWindow;     }

    else if( creationOption.compare( "LeftDock"          ) == 0 ) { return QEActionRequests::OptionLeftDockWindow;         }
    else if( creationOption.compare( "RightDock"         ) == 0 ) { return QEActionRequests::OptionRightDockWindow;        }
    else if( creationOption.compare( "TopDock"           ) == 0 ) { return QEActionRequests::OptionTopDockWindow;          }
    else if( creationOption.compare( "BottomDock"        ) == 0 ) { return QEActionRequests::OptionBottomDockWindow;       }

    else if( creationOption.compare( "LeftDockTabbed"    ) == 0 ) { return QEActionRequests::OptionLeftDockWindowTabbed;   }
    else if( creationOption.compare( "RightDockTabbed"   ) == 0 ) { return QEActionRequests::OptionRightDockWindowTabbed;  }
    else if( creationOption.compare( "TopDockTabbed"     ) == 0 ) { return QEActionRequests::OptionTopDockWindowTabbed;    }
    else if( creationOption.compare( "BottomDockTabbed"  ) == 0 ) { return QEActionRequests::OptionBottomDockWindowTabbed; }

    return QEActionRequests::OptionNewWindow;  // Default
}

//==============================================================================================
// windowCustomisationList
//==============================================================================================

windowCustomisationList::windowCustomisationList()
{
    // Initialise
    toBeActivatedMW = NULL;
    toBeActivatedMenu = NULL;

    // Load QE widget customisations.
    loadCustomisation( ":/qe/configuration/QEImageCustomisationDefault.xml" );
    // Add other QE widget's customisation files here as required
}

// Load a set of customisations
bool windowCustomisationList::loadCustomisation( QString xmlFile )
{
    // If no file specified, silently return (no error)
    if( xmlFile.isEmpty() )
    {
        return false;
    }

    QDomDocument doc;
    QDomNodeList rootNodeList;

    // Read and parse xmlFile
    QFile* file = QEWidget::findQEFile( xmlFile );
    if( !file )
    {
        qDebug() << "Could not find customisation file" << xmlFile;
        return false;
    }

    if( !file->open(QIODevice::ReadOnly) )
    {
        QString error = file->errorString();
        qDebug() << "Could not open customisation file" << xmlFile << error;
        return false;
    }
    // if named customisation exists, replace it
    if ( !doc.setContent( file ) )
    {
        qDebug() << "Could not parse the XML in the customisations file" << xmlFile;
        file->close();
        delete file;
        return false;
    }
    file->close();
    delete file;
    file = NULL; // Ensure no further reference
    QDomElement docElem = doc.documentElement();

    // Load customisation include file
    QDomElement customisationIncludeFileElement = docElem.firstChildElement( "CustomisationIncludeFile" );
    while( !customisationIncludeFileElement.isNull() )
    {
        QString includeFileName = customisationIncludeFileElement.text();
        if( !includeFileName.isEmpty() )
        {
            // load customisation file
            loadCustomisation(includeFileName);
        }
        customisationIncludeFileElement = customisationIncludeFileElement.nextSiblingElement( "CustomisationIncludeFile" );
    }

    // Parse XML using Qt's Document Object Model.
    QDomElement customisationElement = docElem.firstChildElement( "Customisation" );
    while( !customisationElement.isNull() )
    {
        QString customisationName = customisationElement.attribute( "Name" );
        if( !customisationName.isEmpty() )
        {
            // create a window customisation
            windowCustomisation* customisation = new windowCustomisation(customisationName);
            // add the window customisation to the list
            customisationList.prepend( customisation );
            // get a first node
            QDomNode node = customisationElement.firstChild();
            // check if the item is a menu or a button item
            while (!node.isNull())
            {
                QDomElement element = node.toElement();

                // Add a menu
                if( element.tagName() == "Menu" )
                {
                    QString menuName = element.attribute( "Name" );
                    QStringList menuHierarchy;
                    menuHierarchy.append(menuName);

                    // parse menu customisation
                    parseMenuElement( element, customisation, menuHierarchy );
                }

                // Create a menu item if required
                else if( element.tagName() == "Item" )
                {
                    QStringList menuHierarchy;
                    customisation->addItem( createMenuItem( element, menuHierarchy ));
                }

                // Create a placeholder item if required
                else if( element.tagName() == "PlaceHolder" )
                {
                    QStringList menuHierarchy;
                    customisation->addItem( createMenuPlaceholder( element, menuHierarchy ));
                }


                // Add a tool bar button
                else if (element.tagName() == "Button")
                {
                    // create and add a button item
                    customisation->addItem( createButtonItem( element ));
                }

                // Add an include file
                else if (element.tagName() == "IncludeCustomisation")
                {
                    // add all customisation items to the current customisation set
                    addIncludeCustomisation( element, customisation );
                }
                node = node.nextSibling();
            }
        }
        customisationElement = customisationElement.nextSiblingElement( "Customisation" );
    }
    return true;
}
// Parse menu customisation data
void windowCustomisationList::parseMenuElement( QDomElement element, windowCustomisation* customisation, QStringList menuHierarchy)
{
    QDomElement childElement = element.firstChildElement();
    while (!childElement.isNull())
    {
         // check if the item is a submenu or an item
        if (childElement.tagName() == "Menu")
        {
            // get the menu name
            QString menuName = childElement.attribute( "Name" );
            if( !menuName.isEmpty() )
            {
                // copy it over
                QStringList hierarchy = menuHierarchy;
                // update menu hierarchy
                hierarchy.append(menuName);
                // parse menu customisation
                parseMenuElement( childElement, customisation, hierarchy );
            }
        }

        else
        {
            // Add an include file
            if (childElement.tagName() == "IncludeCustomisation")
            {
                // add all customisation items to the current customisation set
                addIncludeCustomisation( childElement, customisation, menuHierarchy );
            }

            // Item to add if found
            windowCustomisationMenuItem* item = NULL;

            // Create a menu item if required
            if( childElement.tagName() == "Item" )
            {
                item = createMenuItem( childElement, menuHierarchy );
            }

            // Create a placeholder item if required
            else if( childElement.tagName() == "PlaceHolder" )
            {
                item = createMenuPlaceholder( childElement, menuHierarchy );
            }

            // If an item was created, add it
            if( item )
            {
                customisation->addItem( item );
            }
        }

        childElement = childElement.nextSiblingElement();
    }
}

// Determine if an item contains a 'separator' tag
bool windowCustomisationList::requiresSeparator( QDomElement itemElement )
{
    // Determine if separator is required
    QDomElement separatorElement = itemElement.firstChildElement( "Separator" );
    return !separatorElement.isNull();
}


//// Add details for a menu item to customisation set
//windowCustomisationMenuItem* windowCustomisationList::createMenuBuiltIn( QDomElement itemElement, QStringList menuHierarchy)
//{
//    QString name = itemElement.attribute( "Name" );
//    if( name.isEmpty() )
//        return NULL;

//    // Add details for a built in menu item to customisation set
//    windowCustomisationMenuItem* item = new windowCustomisationMenuItem( menuHierarchy, name, windowCustomisationMenuItem::MENU_BUILT_IN, requiresSeparator( itemElement ) );
//    return item;
//}

// Add details for a menu item to customisation set
windowCustomisationMenuItem* windowCustomisationList::createMenuPlaceholder( QDomElement itemElement, QStringList menuHierarchy)
{
    QString name = itemElement.attribute( "Name" );
    if( name.isEmpty() )
        return NULL;

    // Add details for a placeholder (where the applicaiton can add menu items) to customisation set
    windowCustomisationMenuItem* item = new windowCustomisationMenuItem( menuHierarchy, name, windowCustomisationMenuItem::MENU_PLACEHOLDER, requiresSeparator( itemElement ) );
    return item;
}

// ???!!!
bool windowCustomisationList::parseMenuAndButtonItem( QDomElement itemElement,
                                                      QString& title,
                                                      QList<windowCreationListItem>& windows,
                                                      QString& builtIn,
                                                      QString& program,
                                                      QString& widgetName,
                                                      QStringList& arguments,
                                                      QString& dockTitle )
{
    title = itemElement.attribute( "Name" );
    if( title.isEmpty() )
    {
        return false;
    }

    // Read Program
    QDomElement programElement = itemElement.firstChildElement( "Program" );
    if( !programElement.isNull() )
    {
        // read Program name and args
        program = programElement.attribute( "Name" );
        QDomElement argumentsElement = programElement.firstChildElement( "Arguments" );
        if( !argumentsElement.isNull() )
        {
            arguments = argumentsElement.text().split(" ");
        }
    }

    // Read Built In function
    QDomElement builtInElement = itemElement.firstChildElement( "BuiltIn" );
    if( !builtInElement.isNull() )
    {
        // Read Built In function name
        builtIn = builtInElement.attribute( "Name" );

        QDomElement childElement = builtInElement.firstChildElement();
        while (!childElement.isNull())
        {
            QString tagName = childElement.tagName();

             // Note the widget target, if any
            if( tagName == "WidgetName" )
            {
                // Get the name of the target widget
                widgetName = childElement.text();
            }

           childElement = childElement.nextSiblingElement();
        }
    }

    // Read windows to create
    QDomElement windowElement = itemElement.firstChildElement( "Window" );
    while( !windowElement.isNull() )
    {
        // Read UiFile name
        QDomElement uiFileElement = windowElement.firstChildElement( "UiFile" );
        if( !uiFileElement.isNull() )
        {
            QString uiFile = uiFileElement.text();
            if( !uiFile.isEmpty() )
            {
                windowCreationListItem windowItem;
                windowItem.uiFile = uiFile;

                // Read optional macro substitutions
                QDomElement macroSubstitutionsElement = windowElement.firstChildElement( "MacroSubstitutions" );
                if( !macroSubstitutionsElement.isNull() )
                {
                    windowItem.macroSubstitutions = macroSubstitutionsElement.text();
                }

                // Read optional customisation name
                QDomElement customisationNameElement = windowElement.firstChildElement( "CustomisationName" );
                if( !customisationNameElement.isNull() )
                {
                    windowItem.customisationName = customisationNameElement.text();
                }

                // Read optional creation option
                QDomElement creationOptionElement = windowElement.firstChildElement( "CreationOption" );
                windowItem.creationOption = QEActionRequests::OptionNewWindow;
                if( !creationOptionElement.isNull() )
                {
                    windowItem.creationOption = windowCustomisation::translateCreationOption( creationOptionElement.text() );
                }

                // Read optional title (This title will override any title in the title property of the top widget in the .ui file)
                QDomElement titleElement = windowElement.firstChildElement( "Title" );
                if( !titleElement.isNull() )
                {
                    windowItem.title = titleElement.text();
                }

                // Add a window to the list of windows to create
                windows.append( windowItem );

                // Read any docks to be added to this window
                parseDockItems( windowElement, windows, dockTitle );
            }
        }
        windowElement = windowElement.nextSiblingElement( "Window" );
    }

    parseDockItems( itemElement, windows, dockTitle );
    return true;
}

// Parse a Dock element.
// If the dock has a title, return the title: This item is to be linked to a pre-existing dock with the given title.
// If the dock has a UI file, return a single 'window creation list item' containing the UI file to be opened as a dock.
void windowCustomisationList::parseDockItems( QDomElement itemElement, QList<windowCreationListItem>& windows, QString& dockTitle )
{
    // Get the dock element
    QDomElement dockElement = itemElement.firstChildElement( "Dock" );
    if( dockElement.isNull() )
    {
        return;
    }

    // If first Dock has a title, we are not creating one or more windows, we are just linking up with a dock with the given title.
    QDomElement titleElement = dockElement.firstChildElement( "Title" );
    if( !titleElement.isNull() )
    {
        dockTitle = titleElement.text();
        return;
    }

    // If first Dock has a UI file, note the UI file name and other details required for starting a new GUI as a dock.
    QDomElement uiFileElement = dockElement.firstChildElement( "UiFile" );
    if( !uiFileElement.isNull() )
    {
        QString uiFile = uiFileElement.text();
        if( !uiFile.isEmpty() )
        {
            windowCreationListItem windowItem;
            windowItem.uiFile = uiFile;

            QDomElement macroSubstitutionsElement = dockElement.firstChildElement( "MacroSubstitutions" );
            if( !macroSubstitutionsElement.isNull() )
            {
                windowItem.macroSubstitutions = macroSubstitutionsElement.text();
            }

            QDomElement creationOptionElement = dockElement.firstChildElement( "CreationOption" );
            windowItem.creationOption = QEActionRequests::OptionFloatingDockWindow;

            if( !creationOptionElement.isNull() )
            {
                windowItem.creationOption = windowCustomisation::translateCreationOption( creationOptionElement.text() );
            }

            QDomElement hiddenElement = dockElement.firstChildElement( "Hidden" );
            if( !hiddenElement.isNull() )
            {
                windowItem.hidden = true;
            }

            windows.append( windowItem );
        }
    }
}


// Add details for a menu item to customisation set
windowCustomisationMenuItem* windowCustomisationList::createMenuItem( QDomElement itemElement, QStringList menuHierarchy)
{
    QString title;                          // Menu item title
    QString program;                        // Program to run when the user selects this menu item
    QStringList arguments;                  // Arguments to supply to 'program'
    QList<windowCreationListItem> windows;  // Windows to create (displaying .UI files) when the user selects this menu item
    QString builtIn;                        // Function (built in to the application, or a QE widget) to call when the user selects this menu item
    QString widgetName;                     // QE widget name to pass built in function request to. If not provided, the built in function is assumed to be handled by the application
    QString dockTitle;                      // Title of existing dock to associate this menu item with

    if( parseMenuAndButtonItem( itemElement, title, windows, builtIn, program, widgetName, arguments, dockTitle ) )
    {
        if( !dockTitle.isEmpty() )
        {
            // Add details for a existing dock menu item to customisation set
            windowCustomisationMenuItem* item = new windowCustomisationMenuItem( menuHierarchy,
                                                                                 title,
                                                                                 windowCustomisationMenuItem::MENU_ITEM,
                                                                                 requiresSeparator( itemElement ),

                                                                                 dockTitle );
            return item;

        }
        else if( !builtIn.isEmpty() )
        {
            // Add details for a built in menu item to customisation set
            windowCustomisationMenuItem* item = new windowCustomisationMenuItem( menuHierarchy,
                                                                                 title,
                                                                                 windowCustomisationMenuItem::MENU_BUILT_IN,
                                                                                 requiresSeparator( itemElement ),

                                                                                 builtIn,
                                                                                 widgetName );
            return item;

        }
        else
        {
            // Add details for a menu item to customisation set
            windowCustomisationMenuItem* item = new windowCustomisationMenuItem( menuHierarchy,
                                                                                 title,
                                                                                 windowCustomisationMenuItem::MENU_ITEM,
                                                                                 requiresSeparator( itemElement ),

                                                                                 NULL/*!!! needs launch receiver object*/,
                                                                                 windows,
                                                                                 program,
                                                                                 arguments );
            return item;
        }
    }
    else
    {
        return NULL;
    }
}

// Add details for a tool bar button item to customisation set
 windowCustomisationButtonItem*  windowCustomisationList::createButtonItem( QDomElement itemElement )
 {
    QString buttonGroup;
    QString buttonIcon;
//    // Read GroupName
//    QDomNodeList list = itemElement.elementsByTagName( "GroupName" );
//    if (list.count() > 0)
//    {
//        buttonGroup = list.at(0).toElement().text();
//    }

//    // Read Icon
//    list = itemElement.elementsByTagName( "Icon" );
//    if (list.count() > 0)
//    {
//        buttonIcon = list.at(0).toElement().text();
//    }

    QString title;                          // Menu item title
    QString program;                        // Program to run when the user selects this menu item
    QStringList arguments;                  // Arguments to supply to 'program'
    QList<windowCreationListItem> windows;  // Windows to create (displaying .UI files) when the user selects this menu item
    QString builtIn;                        // Function (built in to the application, or a QE widget) to call when the user selects this menu item
    QString widgetName;                     // QE widget name to pass built in function request to. If not provided, the built in function is assumed to be handled by the application
    QString dockTitle;                      // Title of existing dock to associate this menu item with


         if( parseMenuAndButtonItem( itemElement, title, windows, builtIn, program, widgetName, arguments, dockTitle ) )
         {
             // Add details for a button item to customisation set
             windowCustomisationButtonItem* item = new windowCustomisationButtonItem(buttonGroup, title, buttonIcon, NULL/*!!! needs launch receiver object*/, windows, program,
                                                                   arguments );

             return item;
         }
         else
         {
             return NULL;
         }
}

 // Return a named set of customisations
 windowCustomisation* windowCustomisationList::getCustomisation(QString name)
 {
     // search for and return customisation
     for( int i = 0; i < customisationList.length(); i++ )
     {
         if( customisationList.at(i)->getName() == name )
         {
            return customisationList.at(i);
         }
     }

     // Customisations not found
     return NULL;
 }

 // Parse a customisation include file
 void windowCustomisationList::addIncludeCustomisation( QDomElement includeCustomisationElement, windowCustomisation* customisation, QStringList menuHierarchy )
 {
     QString includeCustomisationName = includeCustomisationElement.attribute( "Name" );

     // Attempt to get the named customisation set
     windowCustomisation* includeCustomisation = getCustomisation(includeCustomisationName);

     // If the named customisation set was found, then add it
     if (includeCustomisation)
     {
         // add menu items
         QList<windowCustomisationMenuItem*> menuItems = includeCustomisation->getMenuItems();
         for (int i = 0; i < menuItems.length(); i++)
         {
             customisation->addItem(  menuItems.at(i), menuHierarchy );
         }

         // add button items
         QList<windowCustomisationButtonItem*> buttons = includeCustomisation->getButtons();
         for (int i = 0; i < buttons.length(); i++)
         {
             customisation->addItem(buttons.at(i));
         }
     }
}

// Ensure a menu path exists in the menu bar.
// For example, if the menu path required is 'File' -> 'Edit', and a
// 'File' menu exists but does not contain an 'Edit' menu, then add an 'Edit' menu to the 'File' menu.
// Return the end menu (newly created, or found)
QMenu* windowCustomisationList::buildMenuPath( windowCustomisationInfo* customisationInfo, QMenuBar* menuBar, const QStringList menuHierarchy )
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

void windowCustomisationList::initialise( windowCustomisationInfo* customisationInfo )
{
    int count = customisationInfo->items.count();
    for( int i = 0; i < count; i++ )
    {
        customisationInfo->items.at(i)->initialise();
    }
}


// Add the named customisation to a main window.
// Return true if named customisation found and loaded.
void windowCustomisationList::applyCustomisation( QMainWindow* mw, QString customisationName, windowCustomisationInfo* customisationInfo, bool clearExisting, dockMap dockedComponents )
{
    // Clear the existing customisation if requested (but only if we have a customisation name to replace it with)
    if( !customisationName.isEmpty() && clearExisting )
    {
        // Remove all current menus
        mw->menuBar()->clear();
        customisationInfo->items.clear();
        mw->menuBar()->setVisible( false );
        customisationInfo->menus.clear();

        // Remove all current toolbars
        foreach (QToolBar* toolBar, customisationInfo->toolbars)
        {
            mw->removeToolBar( toolBar );
            delete toolBar;
        }
        customisationInfo->toolbars.clear();
    }

    // Get the customisations required
    // Do nothing if not found
    windowCustomisation* customisation = getCustomisation( customisationName );
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
        QMenu* menu = buildMenuPath( customisationInfo, mw->menuBar(), menuItem->getMenuHierarchy() );

        // Act on the type of customisation required
        switch( menuItem->getType() )
        {
            case windowCustomisationMenuItem::MENU_UNKNOWN:
                break;

            case windowCustomisationMenuItem::MENU_ITEM:
                {
                    // Set up an action to respond to the user
                    QObject::connect( menuItem, SIGNAL( newGui( const QEActionRequests& ) ),
                                      mw, SLOT( requestAction( const QEActionRequests& ) ) );

                    // Assume there is no action to add yet.
                    // This will remain the case if a dock is to created and the 'toggle view' action from the dock is required.
                    QAction* action = NULL;

                    if( !menuItem->getDockTitle().isEmpty() )
                    {
                        QDockWidget* component =  dockedComponents.value( menuItem->getDockTitle(), NULL );

                        // Use the dock toggle action from the existing dock matching the title as the menu action
                        if( component )
                        {
                            action =  component->toggleViewAction();
                        }
                        else
                        {
                            QMapIterator<QString, QDockWidget*> i(dockedComponents);
                            qDebug() << "When applying window customisations, could not find a dock titled: " << menuItem->getDockTitle() << ". Dock titles found were:";
                            while (i.hasNext())
                            {
                                i.next();
                                qDebug() << "   " << i.key();
                             }
                        }
                    }

                    // If the menu item creates at least one dock, prepare to activate the item (which will create the docked GUI).
                    // Once activated the dock's 'toggle view' action will be added to the menu noted here.
                    else if( menuItem->createsDocks() )
                    {
                        // Save this menu item, and the associated menu, in a transient list of dock related
                        // items to be activated as soon as this window's set of customisations has been applied
                        toBeActivatedList.append(menuItemToBeActivated( menuItem, menu ));
                    }

                    // Not dock related, just add the menu item as the action
                    else
                    {
                        action = menuItem;
                    }

                    // If the required action is available, add the item action to the correct menu.
                    // (it won't be available if the action is the 'toggle view' action of a dock that is yet to be created)
                    if( action )
                    {
                        if( menu )
                        {
                            if( menuItem->hasSeparator() )
                            {
                                menu->addSeparator();
                            }
                            menu->addAction( action );
                        }

                        // Or add the item action to the menu bar, if not in a menu
                        // (Unusual, but OK)
                        else
                        {
                            mw->menuBar()->addAction( action );
                        }
                    }
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
                        placeholderMenu = mw->menuBar()->addMenu( menuTitle );
                        mw->menuBar()->setVisible( true );
                    }
                    customisationInfo->placeholderMenus.insert( menuTitle, placeholderMenu );

                    // Save the menu for some future menu search
                    customisationInfo->menus.insert( menuTitle, placeholderMenu );
                }
                break;

            case windowCustomisationMenuItem::MENU_BUILT_IN:
                // Add the item to the correct menu
                if( menu )
                {
                    if( menuItem->hasSeparator() )
                    {
                        menu->addSeparator();
                    }
                    menu->addAction( menuItem );
                }

                // Or add the item to the menu bar, if not in a menu
                // (This is normal if the built-in function adds a menu to the action - such as the QEImage zoom menu,
                // but is a bit unusual otherwise)
                else
                {
                    mw->menuBar()->addAction( menuItem );
                }

                QObject::connect( menuItem, SIGNAL( newGui( const QEActionRequests& ) ),
                                  mw, SLOT( requestAction( const QEActionRequests& ) ) );
                customisationInfo->items.append( menuItem );

                break;
        }
    }

    // Activate any dock related items.
    // The dock related items have been added to the transient list 'toBeActivatedList'.
    // NOTE, the action is performed as a timer event not to delay it, but to ensure
    // is called after this customisation has been applied. This is required since
    // there is a currenly published profile (published for the creation of the new
    // window this customisation is being applied to). When the item acction is performed,
    // it will (if creating a gui) want to publish it's own profile, and can't do so while
    // one is already published.
    if( toBeActivatedList.count() )
    {
        toBeActivatedMW = mw;
        QTimer::singleShot( 0, this, SLOT(activateDocks()));
    }
}

// Slot to create any docks required to support dock menu items.
// Docked GUIs are created once customisation has been applied.
// They are created in this slot as a timer event, not because a delay is required,
// but to ensure is occurs after the customisation has been applied. This is required since
// there is a currenly published profile (published for the creation of the new
// window this customisation is being applied to). When the item acction is performed,
// it will (if creating a gui) want to publish it's own profile, and can't do so while
// one is already published.
void windowCustomisationList::activateDocks()
{
    // Sanity check. Do nothing if a main window was not noted
    if( !toBeActivatedMW )
    {
        return;
    }

    // Prepare to catch signals when a dock is created
    QObject::connect( toBeActivatedMW, SIGNAL( dockCreated( QDockWidget* ) ),
                      this, SLOT( useDock( QDockWidget* ) ) );

    // Activate each of the dock related menu items.
    for( int i = 0; i < toBeActivatedList.count(); i++ )
    {
        // Extract the next dock related item from the list
        menuItemToBeActivated mitba = toBeActivatedList.at(i);

        // Note the menu that an action should be added to
        toBeActivatedMenu = mitba.menu;

        // Perform the menu action (create the dock)
        mitba.item->itemAction();

    }
    QObject::disconnect(toBeActivatedMW, SIGNAL( dockCreated( QDockWidget* ) ), this, SLOT( useDock( QDockWidget* ) ));

    // To prevent accidental missuse, clear the transitory variables that
    // are only used while the docks are created
    toBeActivatedList.clear();
    toBeActivatedMenu = NULL;
    toBeActivatedMW = NULL;
}

// A dock has just been created in response to applying a customisation menu item.
// Add the dock's 'toggle view' action to the appropriate menu
// NOTE: This will result in all dock 'toggle view' actions appearing at the bottom of the menu.
// NOTE, Also, if the menu item indicates requests a seperator before this action, this is not honoured
void windowCustomisationList::useDock( QDockWidget* dock )
{
    // Add the dock's 'toggle view' action to the appropriate menu.
    if( toBeActivatedMenu )
    {
        toBeActivatedMenu->addAction( dock->toggleViewAction() );
    }
}

