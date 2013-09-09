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
 * Named configurations are saved in .xml files.
 * Any number of configuration .xml files can be read, building up a directory of named configurations.
 * Configurations can be requested by name.
 * Configurations may include other configurations by name.
 * A configuration can be aliased by defining a configuration that only includes another named configuration.
 *
 * REFER TO menuConfig.h for more details on how to use this module.
 */

#include <menuConfig.h>
#include <QDebug>
#include <QFile>
#include <QMenuBar>
#include <QToolBar>

//==============================================================================================
// windowConfigItem
//==============================================================================================

// Construct instance of class defining an individual item (base class for button or menu item)
windowConfigItem::windowConfigItem(
    QString uiFileIn,                 // UI to display
    QString programIn,                // Program to run
    QStringList programArgumentsIn,   // Arguments for 'program'
    QString macroSubstitutionsIn,     // Substitutions for ui file, program and arguments
    QString configNameIn )            // New window configuration name (menu, buttons, etc)
         : QAction( 0 )
{
    // Save the item details
    uiFile = uiFileIn;
    program = programIn;
    programArguments = programArgumentsIn;
    macroSubstitutions = macroSubstitutionsIn;
    configName = configNameIn;

    // Set up an action to respond to the user
//    connect( this, SIGNAL( triggered()), this, SLOT(itemAction()));
}

// A user has triggered the menu item or button
void windowConfigItem::itemAction()
{
    qDebug() << "windowConfigItem::itemAction():  "
             << "uiFileIn = " << uiFile
             << "programIn = " << program
             << "macroSubstitutionsIn = " << macroSubstitutions
             << "configNameIn = " << configName;
}

//==============================================================================================
// windowConfigMenuItem
//==============================================================================================

// Construct instance of class defining an individual menu item
windowConfigMenuItem::windowConfigMenuItem( QStringList menuHierarchyIn,      // Location in menus to place this item. for example: 'Imaging'->'Region of interest'
                                            QString titleIn,                  // Name of this item. for example: 'Region 1'
                                            QString uiFileIn,                 // UI to display
                                            QString programIn,                // Program to run
                                            QStringList programArgumentsIn,   // Arguments for 'program'
                                            QString macroSubstitutionsIn,     // Substitutions for ui file, program and arguments
                                            QString configNameIn )           // New window configuration name (menu, buttons, etc)
         : windowConfigItem( uiFileIn, programIn, programArgumentsIn, macroSubstitutionsIn, configNameIn )
{
    menuHierarchy = menuHierarchyIn;
    title = titleIn;
    setText(titleIn);
    setParent(this);
    // Set up an action to respond to the user
    connect( this, SIGNAL( triggered()), this, SLOT(itemAction()));
}

//==============================================================================================
// windowConfigButtonItem
//==============================================================================================

// Construct instance of class defining an individual button item
windowConfigButtonItem::windowConfigButtonItem( QString buttonGroupIn,            // Name of toolbar button group in which to place a button
                                                QString buttonTextIn,             // Text to place in button
                                                QString buttonIconIn,             // Icon for button
                                                QString uiFileIn,                 // UI to display
                                                QString programIn,                // Program to run
                                                QStringList programArgumentsIn,   // Arguments for 'program'
                                                QString macroSubstitutionsIn,     // Substitutions for ui file, program and arguments
                                                QString configNameIn )           // New window configuration name (menu, buttons, etc)
                            : windowConfigItem( uiFileIn, programIn, programArgumentsIn, macroSubstitutionsIn, configNameIn )
{
    buttonGroup = buttonGroupIn;
    buttonText = buttonTextIn;
    buttonIcon = buttonIconIn;
    setText(buttonTextIn);
    setParent(this);
    // Set up an action to respond to the user
    connect( this, SIGNAL( triggered()), this, SLOT(itemAction()));
}

//==============================================================================================
// windowConfig
//==============================================================================================

// Class defining a configuration for a window.
// Construction - create a named, empty, configuration
windowConfig::windowConfig( QString nameIn )
{
    name = nameIn;
}

// Destruction - release config items
windowConfig::~windowConfig()
{
    // Release config items
    while( !menuItems.isEmpty() )
    {
        delete menuItems.takeFirst();
    }
    while( !buttons.isEmpty() )
    {
        delete buttons.takeFirst();
    }
}

// Add a menu item to the configuration
// NOTE! windowConfig TAKES OWNERSHIP of menuItem
void windowConfig::addItem( windowConfigMenuItem* menuItem )
{
    menuItems.append( menuItem );
}

// Add a button to the configuration
// NOTE! windowConfig TAKES OWNERSHIP of button
void windowConfig::addItem( windowConfigButtonItem* button )
{
    buttons.append( button );
}

//==============================================================================================
// windowConfigList
//==============================================================================================

windowConfigList::windowConfigList()
{
    // !!! Nothing yet
}

// Load a set of configurations
bool windowConfigList::loadConfig( QString xmlFile )
{
    QDomDocument doc;

    qDebug() << "windowConfigList::loadConfig()" << xmlFile;
    // !!! Read and parse xmlFile
    QFile file( xmlFile );
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open config file" << xmlFile;
        return false;
    }
    // !!! if named configuration exists, replace it
    if ( !doc.setContent( &file ) )
    {
        qDebug() << "Could not parse the XML in the config file" << xmlFile;
        file.close();
        return false;
    }
    file.close();
    QDomElement docElem = doc.documentElement();
    // !!! Parse XML using Qt's Document Object Model. Refer to code in persistanceManager.cpp/.h
    QDomNodeList rootNodeList = docElem.elementsByTagName( "Config" );
    for( int i = 0; i < rootNodeList.count(); i++ )
    {
        QDomNode configNode = rootNodeList.at(i);
        QString configName = configNode.toElement().attribute( "Name" );
        if( !configName.isEmpty() )
        {
            // create a window configuration
            windowConfig* config = new windowConfig(configName);
            // add the window configuration to the list
            configList.append(config);
            // get a menu list and button list
            QDomNodeList menuButtonList = configNode.toElement().childNodes();
            QDomNode node = configNode.firstChild();
            // check if the item is a menu or a button item
            while (!node.isNull()){
                QDomElement itemElement = node.toElement();
                if (itemElement.tagName() == "Menu"){
                    QString menuName = itemElement.attribute( "Name" );
                    QStringList menuHierarchy;
                    menuHierarchy.append(menuName);
                    // parse menu configuration
                    parseMenuCfg(node, config, menuHierarchy);
                }
                else if (itemElement.tagName() == "Button"){
                    // create and add a button item
                    config->addItem(createButtonItem(itemElement));
                }
                else if (itemElement.tagName() == "IncludeConfig"){
                    QString includeConfigName = itemElement.attribute( "Name" );
                    // get the config info
                    windowConfig* includeConfig = getConfig(includeConfigName);
                    if (includeConfig){
                        // add all config items to the current configuration
                        addIncludeConfig(config, includeConfig);
                    }
                }
                node = node.nextSibling();
            }
        }
    }
    // load ConfigIncludeFile
    rootNodeList = docElem.elementsByTagName( "ConfigIncludeFile" );
    for( int i = 0; i < rootNodeList.count(); i++ )
    {
        QDomNode configNode = rootNodeList.at(i);
        QString includeFileName = configNode.toElement().text();
        if( !includeFileName.isEmpty() )
        {
            // load config file
            loadConfig(includeFileName);
        }
    }

    return true;
}

// Parse menu configuration data
void windowConfigList::parseMenuCfg( QDomNode menuNode, windowConfig* config, QStringList menuHierarchy){
    QDomNode node = menuNode.firstChild();
    while (!node.isNull()){
        QDomElement itemElement = node.toElement();
        // check if the item is a submenu or an item
        if (itemElement.tagName() == "Menu"){
            // get the menu name
            QString menuName = itemElement.attribute( "Name" );
            if( !menuName.isEmpty() )
            {
                // update menu hierarchy
                menuHierarchy.append(menuName);
                // parse menu configuration
                parseMenuCfg(node, config, menuHierarchy);
            }
        }
        else if (itemElement.tagName() == "Item"){
            // create and add a menu item
            config->addItem(createMenuItem(itemElement, menuHierarchy));
        }
        node = node.nextSibling();
    }
}

windowConfigMenuItem* windowConfigList::createMenuItem( QDomElement itemElement, QStringList menuHierarchy){
    QString titleIn = itemElement.attribute("Name");
    QString uiFileIn;
    QString programIn;
    QStringList programArgumentsIn;
    QString macroSubstitutionsIn;
    QString configNameIn;

    // read UiFile name
    QDomNodeList list = itemElement.elementsByTagName("UiFile" );
    if (list.count() > 0){
        uiFileIn = list.at(0).toElement().text();
    }
    // read Program
    list = itemElement.elementsByTagName("Program" );
    if (list.count() > 0){
        QDomElement program = list.at(0).toElement();
        // read Program name and args
        programIn = program.attribute( "Name" );
        list = program.elementsByTagName("Arguments");
        if (list.count() > 0){
            programArgumentsIn = list.at(0).toElement().text().split(" ");
        }
    }

    // read macroSubstitutionsIn
    list = itemElement.elementsByTagName("macroSubstitutions" );
    if (list.count() > 0){
        macroSubstitutionsIn = list.at(0).toElement().text();
    }
    // read configNameIn
    list = itemElement.elementsByTagName("config" );
    if (list.count() > 0){
        configNameIn = list.at(0).toElement().text();
    }

    // create a menu item and add it to config
    windowConfigMenuItem* item = new windowConfigMenuItem(menuHierarchy, titleIn, uiFileIn, programIn,
                                                          programArgumentsIn, macroSubstitutionsIn, configNameIn);
    return item;
}

 windowConfigButtonItem*  windowConfigList::createButtonItem( QDomElement itemElement ){
    QString buttonGroupIn;
    QString buttonTextIn = itemElement.attribute("Name" );
    QString buttonIconIn;

    QString uiFileIn;
    QString programIn;
    QStringList programArgumentsIn;
    QString macroSubstitutionsIn;
    QString configNameIn;

    // read GroupName
    QDomNodeList list = itemElement.elementsByTagName("GroupName" );
    if (list.count() > 0){
        buttonGroupIn = list.at(0).toElement().text();
    }
    // read Icon
    list = itemElement.elementsByTagName("Icon" );
    if (list.count() > 0){
        buttonIconIn = list.at(0).toElement().text();
    }

    // read UiFile name
    list = itemElement.elementsByTagName("UiFile" );
    if (list.count() > 0){
        uiFileIn = list.at(0).toElement().text();
    }
    // read Program
    list = itemElement.elementsByTagName("Program" );
    if (list.count() > 0){
        QDomElement program = list.at(0).toElement();
        // read Program name and args
        programIn = program.attribute( "Name" );
        list = program.elementsByTagName("Arguments");
        if (list.count() > 0){
            programArgumentsIn = list.at(0).toElement().text().split(" ");
        }
    }

    // read macroSubstitutionsIn
    list = itemElement.elementsByTagName("macroSubstitutions" );
    if (list.count() > 0){
        macroSubstitutionsIn = list.at(0).toElement().text();
    }
    // read configNameIn
    list = itemElement.elementsByTagName("config" );
    if (list.count() > 0){
        configNameIn = list.at(0).toElement().text();
    }

    // create a menu item and add it to config
    windowConfigButtonItem* item = new windowConfigButtonItem(buttonGroupIn, buttonTextIn, buttonIconIn, uiFileIn, programIn,
                                                          programArgumentsIn, macroSubstitutionsIn, configNameIn);
    return item;
}

 windowConfig* windowConfigList::getConfig(QString name){
     windowConfig* config = 0;
     for (int i = 0; i < configList.length(); i++){
         if (configList.at(i)->getName() == name){
            config = configList.at(i);
            break;
         }
     }
     return config;
 }

 void windowConfigList::addIncludeConfig(windowConfig* config, windowConfig* include){
     QList<windowConfigMenuItem*> menuItems = include->getMenuItems();
     QList<windowConfigButtonItem*> buttons = include->getButtons();
     // add menu items
     for (int i = 0; i < menuItems.length(); i++){
         config->addItem(menuItems.at(i));
     }
     // add button items
     for (int i = 0; i < buttons.length(); i++){
         config->addItem(buttons.at(i));
     }
}

// Add the named configuration to a main window.
// Return true if named configuration found and loaded.
bool windowConfigList::applyConfig( QMainWindow* mw, QString configName )
{

    qDebug() << "windowConfigList::applyConfig()" << mw << configName;
    windowConfig* config = getConfig(configName);
    if (!config)
        return false;

    QToolBar* mainToolBar = new QToolBar(mw);
    mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
    mw->addToolBar(Qt::TopToolBarArea, mainToolBar);
    QList<windowConfigButtonItem*> bList = config->getButtons();
    for ( int i = 0; i < bList.length(); i++ ){
        windowConfigButtonItem* item = bList.at(i);
//        QAction* action = new QAction(item->getButtonText(), mw);
        mainToolBar->addAction(item);
//        mainToolBar->addAction(action);
    }


    QMenuBar* menuBar = mw->menuBar();
    QMap<QString, QMenu*> menuCreated;

    QList<windowConfigMenuItem*> mList = config->getMenuItems();

    for (int i = 0; i < mList.length(); i++){
        windowConfigMenuItem* menuItem = mList.at(i);
        QMenu* menuPoint = 0;
        QStringList menuHierarchy = menuItem->getMenuHierarchy();
        QString extendedMenuName;
        for ( int j = 0; j < menuHierarchy.length(); j++ ){
            QString menuName = menuHierarchy.at(j);
            extendedMenuName = extendedMenuName.append(menuName);
            if (!menuCreated.contains(extendedMenuName)){
                QMenu* newMenu = new QMenu(menuName);
                if (!menuPoint){
                    // top menu
                    menuBar->addMenu(newMenu);
                }
                else{ // submenu
                    menuPoint->addMenu(newMenu);
                }
                menuPoint = newMenu;
                menuCreated.insert(extendedMenuName, newMenu);
            }
            else{
                menuPoint = menuCreated.find(extendedMenuName).value();
            }
        }
//        QAction* action = new QAction(menuItem->getTitle(), mw);
        if (menuPoint)
    //        menuPoint->addMenu(menuItem->getTitle());
    //        menuPoint->addAction(action);
            menuPoint->addAction(menuItem);
    }
    //!!! Extend the QMainWindow menu bar and tool bar with the named configuration
    //!!! Add required menus, menu items, button groups and buttons
    //!!! Use the windowConfigItem as the QAction to use for each menu item and button

    return true;
}
