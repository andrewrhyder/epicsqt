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
#include <QUiLoader>
#include <QDockWidget>

//==============================================================================================
// windowCustomisationItem
//==============================================================================================

// Construct instance of class defining an individual item when none exists (for example, a menu placeholder)
windowCustomisationItem::windowCustomisationItem() : QAction( 0 )
{
//    creationOption = QEActionRequests::OptionNewWindow;
}

// Construct instance of class defining a built in application action
windowCustomisationItem::windowCustomisationItem( const QString builtInActionIn ) : QAction( 0 )
{
//    creationOption = QEActionRequests::OptionNewWindow;
    builtInAction = builtInActionIn;
}

// Construct instance of class defining an individual item (base class for button or menu item)
windowCustomisationItem::windowCustomisationItem(
    const QObject* /*launchRequestReceiver*/,                // Object (typically QEGui application) which will accept requests to launch a new GUI
    const QList<windowCreationListItem>& windowsIn,      // Windows to display (centrals and docks)
    const QString programIn,                             // Program to run
    const QStringList programArgumentsIn )               // Arguments for 'program'
         : QAction( 0 )
{
    // Save the item details
    for( int i = 0; i < windowsIn.count(); i++ )
    {
        windows.append( windowsIn.at(i));
    }
    program = programIn;
    programArguments = programArgumentsIn;
}

// Construct instance of class defining an individual item (base class for button )or menu item)
windowCustomisationItem::windowCustomisationItem(windowCustomisationItem* item): QAction( 0 )
{
    // Save the item details
    for( int i = 0; i < item->windows.count(); i++ )
    {
        windows.append( item->windows.at(i));
    }
    program = item->getProgram();
    programArguments = item->getProgramArguments();
    builtInAction = item->getBuiltInAction();
}

// A user has triggered the menu item or button
void windowCustomisationItem::itemAction()
{
    profile.publishOwnProfile();

    //    if( !uiFile.isEmpty() )
    if( windows.count() )
    {
        emit newGui( QEActionRequests( windows ));
    }
    else if ( !builtInAction.isEmpty() )
    {
        emit newGui( QEActionRequests( builtInAction, "" )  );
    }

    profile.releaseProfile();
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
                          const QStringList programArgumentsIn )               // Arguments for 'program'
                          : windowCustomisationItem( launchRequestReceiver, windowsIn, programIn, programArgumentsIn )
{
    type = typeIn;
    menuHierarchy = menuHierarchyIn;
    title = titleIn;
    separator = separatorIn;
}

// Construct instance of class defining a placeholder for items the application might add
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
                        const QStringList programArgumentsIn )                // Arguments for 'program'
                            : windowCustomisationItem( launchRequestReceiver, windowsIn, programIn, programArgumentsIn )
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
void windowCustomisation::addItem( windowCustomisationMenuItem* menuItem )
{
    menuItems.append( menuItem );
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
         if( creationOption.compare( "Open"          ) == 0 ) { return QEActionRequests::OptionOpen;                   }
    else if( creationOption.compare( "NewTab"        ) == 0 ) { return QEActionRequests::OptionNewTab;                 }
    else if( creationOption.compare( "NewWindow"     ) == 0 ) { return QEActionRequests::OptionNewWindow;              }

    else if( creationOption.compare( "LeftDock"      ) == 0 ) { return QEActionRequests::OptionLeftDockWindow;         }
    else if( creationOption.compare( "RightDock"     ) == 0 ) { return QEActionRequests::OptionRightDockWindow;        }
    else if( creationOption.compare( "TopDock"       ) == 0 ) { return QEActionRequests::OptionTopDockWindow;          }
    else if( creationOption.compare( "BottomDock"    ) == 0 ) { return QEActionRequests::OptionBottomDockWindow;       }
    else if( creationOption.compare( "FloatingDock"  ) == 0 ) { return QEActionRequests::OptionFloatingDockWindow;     }

                                                                return QEActionRequests::OptionNewWindow;  // Default
}

//==============================================================================================
// windowCustomisationList
//==============================================================================================

windowCustomisationList::windowCustomisationList()

{
    // !!! Nothing yet
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
    QFile file( xmlFile );
    if (!file.open(QIODevice::ReadOnly))
    {
        QString error = file.errorString();
        qDebug() << "Could not open customisation file" << xmlFile << error;
        return false;
    }
    // if named customisation exists, replace it
    if ( !doc.setContent( &file ) )
    {
        qDebug() << "Could not parse the XML in the customisations file" << xmlFile;
        file.close();
        return false;
    }
    file.close();
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
                    QString includeCustomisationName = element.attribute( "Name" );
                    // get the customisation info
                    windowCustomisation* includeCustomisation = getCustomisation(includeCustomisationName);
                    if (includeCustomisation){
                        // add all customisation items to the current customisation set
                        addIncludeCustomisation(customisation, includeCustomisation);
                    }
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
                customisation->addItem(item);
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


// Add details for a menu item to customisation set
windowCustomisationMenuItem* windowCustomisationList::createMenuBuiltIn( QDomElement itemElement, QStringList menuHierarchy)
{
    QString name = itemElement.attribute( "Name" );
    if( name.isEmpty() )
        return NULL;

    // Add details for a built in menu item to customisation set
    windowCustomisationMenuItem* item = new windowCustomisationMenuItem( menuHierarchy, name, windowCustomisationMenuItem::MENU_BUILT_IN, requiresSeparator( itemElement ) );
    return item;
}

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
                                                      QString& program, QStringList& programArguments )
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
            programArguments = argumentsElement.text().split(" ");
        }
    }

    // Read Built In function
    QDomElement builtInElement = itemElement.firstChildElement( "BuiltIn" );
    if( !builtInElement.isNull() )
    {
        // read Built In function name
        builtIn = builtInElement.attribute( "Name" );
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
                parseDockItems( windowElement, windows );
            }
        }
        windowElement = windowElement.nextSiblingElement( "Window" );
    }

    parseDockItems( itemElement, windows );
    return true;
}

// ???!!!
void windowCustomisationList::parseDockItems( QDomElement itemElement, QList<windowCreationListItem>& windows )
{
    // Read Docks to create
    QDomElement dockElement = itemElement.firstChildElement( "Dock" );
    while( !dockElement.isNull() )
    {
        // Read UiFile name
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
        dockElement = dockElement.nextSiblingElement( "Dock" );
    }
}


// Add details for a menu item to customisation set
windowCustomisationMenuItem* windowCustomisationList::createMenuItem( QDomElement itemElement, QStringList menuHierarchy)
{
    QString title;
    QString program;
    QStringList programArguments;
    QList<windowCreationListItem> windows;
    QString builtIn;

    if( parseMenuAndButtonItem( itemElement, title, windows, builtIn, program, programArguments ) )
    {
        if( !builtIn.isEmpty() )
        {
            // Add details for a built in menu item to customisation set
            windowCustomisationMenuItem* item = new windowCustomisationMenuItem( menuHierarchy, builtIn, windowCustomisationMenuItem::MENU_BUILT_IN, requiresSeparator( itemElement ) );
            return item;

        }
        else
        {
            // Add details for a menu item to customisation set
            windowCustomisationMenuItem* item = new windowCustomisationMenuItem( menuHierarchy, title, windowCustomisationMenuItem::MENU_ITEM,
                                                                                 requiresSeparator( itemElement ), NULL/*!!! needs launch receiver object*/, windows, program,
                                                                  programArguments );
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

        QString title;
         QString program;
         QStringList programArguments;
         QString macroSubstitutions;
         QString customisationName;
         QList<windowCreationListItem> windows;
         QString builtIn;

         if( parseMenuAndButtonItem( itemElement, title, windows, builtIn, program, programArguments ) )
         {
             // Add details for a button item to customisation set
             windowCustomisationButtonItem* item = new windowCustomisationButtonItem(buttonGroup, title, buttonIcon, NULL/*!!! needs launch receiver object*/, windows, program,
                                                                   programArguments );

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
 void windowCustomisationList::addIncludeCustomisation(windowCustomisation* customisation, windowCustomisation* include)
 {
     QList<windowCustomisationMenuItem*> menuItems = include->getMenuItems();
     QList<windowCustomisationButtonItem*> buttons = include->getButtons();

     // add menu items
     for (int i = 0; i < menuItems.length(); i++)
     {
         customisation->addItem(menuItems.at(i));
     }

     // add button items
     for (int i = 0; i < buttons.length(); i++)
     {
         customisation->addItem(buttons.at(i));
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

// Add the named customisation to a main window.
// Return true if named customisation found and loaded.
void windowCustomisationList::applyCustomisation( QMainWindow* mw, QString customisationName, windowCustomisationInfo* customisationInfo, bool clearExisting )
{
    // Clear the existing customisation if requested (but only if we have a customisation name to replace it with)
    if( !customisationName.isEmpty() && clearExisting )
    {
        // Remove all current menus
        mw->menuBar()->clear();
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
//                          mw, SLOT( requestGui( const QEActionRequests& ) ) );
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
                // Add the item to the correct menu
                // (if no menu, don't add to the menu bar - this could change)
                if( menu )
                {
                    if( menuItem->hasSeparator() )
                    {
                        menu->addSeparator();
                    }
                    menu->addAction( menuItem );

                    // Set up an action to respond to the user
                    QObject::connect( menuItem, SIGNAL( newGui( const QEActionRequests& ) ),
                                      mw, SLOT( requestGui( const QEActionRequests& ) ) );
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
                if( menuItem->hasSeparator() )
                {
                    menu->addSeparator();
                }
                menu->addAction( menuItem );
                QObject::connect( menuItem, SIGNAL( newGui( const QEActionRequests& ) ),
                                  mw, SLOT( requestGui( const QEActionRequests& ) ) );

                break;
        }
    }
}