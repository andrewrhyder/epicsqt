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
 * REFER TO menuConfig.h for more details on how to use this module.
 */

#include <menuConfig.h>
#include <QDebug>
#include <QFile>
#include <QMenuBar>
#include <QToolBar>
#include <QUiLoader>
#include <QDockWidget>

//==============================================================================================
// windowCustomisationItem
//==============================================================================================

// Construct instance of class defining an individual item (base class for button or menu item)
windowCustomisationItem::windowCustomisationItem(
    const QObject* /*launchRequestReceiver*/,                // Object (typically QEGui application) which will accept requests to launch a new GUI
    const QString uiFileIn,                              // UI to display
    const QString programIn,                             // Program to run
    const QStringList programArgumentsIn,                // Arguments for 'program'
    const QString macroSubstitutionsIn,                  // Substitutions for ui file, program and arguments
    const QEGuiLaunchRequests::Options creationOptionIn, // Window creation options
    const QString customisationNameIn )                  // New window customisation name (menu, buttons, etc)
         : QAction( 0 )
{
    // Save the item details
    uiFile = uiFileIn;
    program = programIn;
    programArguments = programArgumentsIn;
    macroSubstitutions = macroSubstitutionsIn;
    creationOption = creationOptionIn;
    customisationName = customisationNameIn;

    // Set up an action to respond to the user
//    QObject::connect( this, SIGNAL( newGui( const QEGuiLaunchRequests& ) ),
//                      launchRequestReceiver, SLOT( requestGui( const QEGuiLaunchRequests& ) ) );
}

// Construct instance of class defining an individual item (base class for button )or menu item)
windowCustomisationItem::windowCustomisationItem(windowCustomisationItem* item): QAction( 0 )
{
    // Save the item details
    uiFile = item->getUiFile();
    program = item->getProgram();
    programArguments = item->getProgramArguments();
    macroSubstitutions = item->getMacroSubstitutions();
    creationOption = item->getCreationOption();
    customisationName = item->getCustomisationName();

    // Set up an action to respond to the user
//    QObject::connect( this, SIGNAL( newGui( const QEGuiLaunchRequests& ) ),
//                      launchRequestReceiver, SLOT( requestGui( const QEGuiLaunchRequests& ) ) );
}

// A user has triggered the menu item or button
void windowCustomisationItem::itemAction()
{
    qDebug() << "windowCustomisationItem::itemAction():  "
             << "uiFile = " << uiFile
             << "program = " << program
             << "macroSubstitutions = " << macroSubstitutions
             << "creationOption" << creationOption
             << "customisationName = " << customisationName;

    emit newGui( QEGuiLaunchRequests( uiFile, customisationName, creationOption )  );
}

//==============================================================================================
// windowCustomisationMenuItem
//==============================================================================================

// Construct instance of class defining an individual menu item
windowCustomisationMenuItem::windowCustomisationMenuItem(
                          const QStringList menuHierarchyIn,                   // Location in menus to place this item. for example: 'Imaging'->'Region of interest'
                          const QString titleIn,                               // Name of this item. for example: 'Region 1'

                          const QObject* launchRequestReceiver,                // Object (typically QEGui application) which will accept requests to launch a new GUI
                          const QString uiFileIn,                              // UI to display
                          const QString programIn,                             // Program to run
                          const QStringList programArgumentsIn,                // Arguments for 'program'
                          const QString macroSubstitutionsIn,                  // Substitutions for ui file, program and arguments
                          const QEGuiLaunchRequests::Options creationOptionIn, // Window creation options
                          const QString customisationNameIn )                  // New window customisation name (menu, buttons, etc)
                          : windowCustomisationItem( launchRequestReceiver, uiFileIn, programIn, programArgumentsIn, macroSubstitutionsIn, creationOptionIn, customisationNameIn )
{
    menuHierarchy = menuHierarchyIn;
    title = titleIn;
//    setText(titleIn);
//    setParent(this);

    // Set up an action to respond to the user
//    connect( this, SIGNAL( triggered()), this, SLOT(itemAction()));
}

// Copy constructor
windowCustomisationMenuItem::windowCustomisationMenuItem(windowCustomisationMenuItem* menuItem)                  // New window customisation name (menu, buttons, etc)
                          : windowCustomisationItem( menuItem )
{
    menuHierarchy = menuItem->getMenuHierarchy();
    title = menuItem->getTitle();
    setText(title);
    setParent(this);

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
                        const QString uiFileIn,                              // UI to display
                        const QString programIn,                             // Program to run
                        const QStringList programArgumentsIn,                // Arguments for 'program'
                        const QString macroSubstitutionsIn,                  // Substitutions for ui file, program and arguments
                        const QEGuiLaunchRequests::Options creationOptionIn, // Window creation options
                        const QString customisationNameIn )                  // New window customisation name (menu, buttons, etc)
                            : windowCustomisationItem( launchRequestReceiver, uiFileIn, programIn, programArgumentsIn, macroSubstitutionsIn, creationOptionIn, customisationNameIn )
{
    buttonGroup = buttonGroupIn;
    buttonText = buttonTextIn;
    buttonIcon = buttonIconIn;
//    setText(buttonTextIn);
//    setParent(this);

    // Set up an action to respond to the user
//    connect( this, SIGNAL( triggered()), this, SLOT(itemAction()));
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
windowCustomisation::windowCustomisation( QString nameIn )
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

// Translate creation option text from .xml file to enumeration in QEGuiLaunchRequests
QEGuiLaunchRequests::Options windowCustomisation::translateCreationOption( QString creationOption )
{
    if( creationOption.compare( "Open") == 0 )
    {
        return QEGuiLaunchRequests::OptionOpen;
    }

    else if( creationOption.compare( "NewTab") == 0 )
    {
        return QEGuiLaunchRequests::OptionNewTab;
    }

    else if( creationOption.compare( "NewLeftDock") == 0 )
    {
        return QEGuiLaunchRequests::OptionLeftDockWindow;
    }
    else if( creationOption.compare( "NewRightDock") == 0 )
    {
        return QEGuiLaunchRequests::OptionRightDockWindow;
    }
    else if( creationOption.compare( "NewTopDock") == 0 )
    {
        return QEGuiLaunchRequests::OptionTopDockWindow;
    }
    else if( creationOption.compare( "NewBottomDock") == 0 )
    {
        return QEGuiLaunchRequests::OptionBottomDockWindow;
    }
    else if( creationOption.compare( "NewDock") == 0 )
    {
        return QEGuiLaunchRequests::OptionBottomDockWindow;
    }
    else if( creationOption.compare( "NewChildWindow") == 0 )
    {
        return QEGuiLaunchRequests::OptionNewChildWindow;
    }

    // default is NewWindow
    return QEGuiLaunchRequests::OptionNewWindow;
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
    QDomDocument doc;

    qDebug() << "windowCustomisationList::loadCustomisation()" << xmlFile;
    // !!! Read and parse xmlFile
    QFile file( xmlFile );
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open customisation file" << xmlFile;
        return false;
    }
    // !!! if named customisation exists, replace it
    if ( !doc.setContent( &file ) )
    {
        qDebug() << "Could not parse the XML in the customisations file" << xmlFile;
        file.close();
        return false;
    }
    file.close();
    QDomElement docElem = doc.documentElement();
    // !!! Parse XML using Qt's Document Object Model. Refer to code in persistanceManager.cpp/.h
    QDomNodeList rootNodeList = docElem.elementsByTagName( "Customisation" );
    for( int i = 0; i < rootNodeList.count(); i++ )
    {
        QDomNode customisationNode = rootNodeList.at(i);
        QString customisationName = customisationNode.toElement().attribute( "Name" );
        if( !customisationName.isEmpty() )
        {
            // create a window customisation
            windowCustomisation* customisation = new windowCustomisation(customisationName);
            // add the window customisation to the list
            customisationList.append(customisation);
            // get a first node
            QDomNode node = customisationNode.firstChild();
            // check if the item is a menu or a button item
            while (!node.isNull()){
                QDomElement itemElement = node.toElement();
                if (itemElement.tagName() == "Menu"){
                    QString menuName = itemElement.attribute( "Name" );
                    QStringList menuHierarchy;
                    menuHierarchy.append(menuName);
                    // parse menu customisation
                    parseMenuCfg(node, customisation, menuHierarchy);
                }
                else if (itemElement.tagName() == "Button"){
                    // create and add a button item
                    customisation->addItem(createButtonItem(itemElement));
                }
                else if (itemElement.tagName() == "IncludeCustomisation"){
                    QString includeCustomisationName = itemElement.attribute( "Name" );
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
    }
    // load customisation include file
    rootNodeList = docElem.elementsByTagName( "CustomisationIncludeFile" );
    for( int i = 0; i < rootNodeList.count(); i++ )
    {
        QDomNode customisationNode = rootNodeList.at(i);
        QString includeFileName = customisationNode.toElement().text();
        if( !includeFileName.isEmpty() )
        {
            // load customisation file
            loadCustomisation(includeFileName);
        }
    }

    return true;
}

// Parse menu customisation data
void windowCustomisationList::parseMenuCfg( QDomNode menuNode, windowCustomisation* customisation, QStringList menuHierarchy){
    QDomNode node = menuNode.firstChild();
    while (!node.isNull()){
        QDomElement itemElement = node.toElement();
        // check if the item is a submenu or an item
        if (itemElement.tagName() == "Menu"){
            // get the menu name
            QString menuName = itemElement.attribute( "Name" );
            if( !menuName.isEmpty() )
            {
                // copy it over
                QStringList hierarchy = menuHierarchy;
                // update menu hierarchy
                hierarchy.append(menuName);
                // parse menu customisation
                parseMenuCfg(node, customisation, hierarchy);
            }
        }
        else if (itemElement.tagName() == "Item"){
            // create and add a menu item
            customisation->addItem(createMenuItem(itemElement, menuHierarchy));
        }
        else if (itemElement.tagName() == "Separator"){
            // create and add a menu item
            // create a menu item and add it to the customisation set
            windowCustomisationMenuItem* item = new windowCustomisationMenuItem(menuHierarchy, itemElement.tagName(), NULL,
                                                                                QString(), QString(), QStringList(), QString(),
                                                                                QEGuiLaunchRequests::OptionOpen, QString());
            customisation->addItem(item);
        }
        node = node.nextSibling();
    }
}

windowCustomisationMenuItem* windowCustomisationList::createMenuItem( QDomElement itemElement, QStringList menuHierarchy){
    QString title = itemElement.attribute("Name");
    QString uiFile = "";
    QString program = "";
    QStringList programArguments;
    QString macroSubstitutions = "";
    QEGuiLaunchRequests::Options creationOption = QEGuiLaunchRequests::OptionNewWindow;
    QString customisationName = "";

    // read UiFile name
    QDomNodeList list = itemElement.elementsByTagName("UiFile" );
    if (list.count() > 0){
        uiFile = list.at(0).toElement().text();
    }
    // read Program
    list = itemElement.elementsByTagName("Program" );
    if (list.count() > 0){
        QDomElement programElement = list.at(0).toElement();
        // read Program name and args
        program = programElement.attribute( "Name" );
        list = programElement.elementsByTagName("Arguments");
        if (list.count() > 0){
            programArguments = list.at(0).toElement().text().split(" ");
        }
    }

    // read macro substitutions
    list = itemElement.elementsByTagName("MacroSubstitutions" );
    if (list.count() > 0){
        macroSubstitutions = list.at(0).toElement().text();
    }

    // read customisation name
    list = itemElement.elementsByTagName("CustomisationName" );
    if (list.count() > 0){
        customisationName = list.at(0).toElement().text();
    }

    // read creation option
    list = itemElement.elementsByTagName("CreationOption" );
    if (list.count() > 0)
    {
        creationOption = windowCustomisation::translateCreationOption( list.at(0).toElement().text() );
    }

    // create a menu item and add it to the customisation set
    windowCustomisationMenuItem* item = new windowCustomisationMenuItem(menuHierarchy, title, NULL/*!!! needs launch receiver object*/, uiFile, program,
                                                          programArguments, macroSubstitutions, creationOption, customisationName);
    return item;
}

 windowCustomisationButtonItem*  windowCustomisationList::createButtonItem( QDomElement itemElement ){
    QString buttonGroup = "";
    QString buttonText = itemElement.attribute("Name" );
    QString buttonIcon = "";

    QString uiFile = "";
    QString program = "";
    QStringList programArguments;
    QString macroSubstitutions = "";
    QEGuiLaunchRequests::Options creationOption = QEGuiLaunchRequests::OptionNewWindow;
    QString customisationName = "";

    // read GroupName
    QDomNodeList list = itemElement.elementsByTagName("GroupName" );
    if (list.count() > 0){
        buttonGroup = list.at(0).toElement().text();
    }
    // read Icon
    list = itemElement.elementsByTagName("Icon" );
    if (list.count() > 0){
        buttonIcon = list.at(0).toElement().text();
    }

    // read UiFile name
    list = itemElement.elementsByTagName("UiFile" );
    if (list.count() > 0){
        uiFile = list.at(0).toElement().text();
    }
    // read Program
    list = itemElement.elementsByTagName("Program" );
    if (list.count() > 0){
        QDomElement programElement = list.at(0).toElement();
        // read Program name and args
        program = programElement.attribute( "Name" );
        list = programElement.elementsByTagName("Arguments");
        if (list.count() > 0){
            programArguments = list.at(0).toElement().text().split(" ");
        }
    }

    // read macro substitutions
    list = itemElement.elementsByTagName("MacroSubstitutions" );
    if (list.count() > 0){
        macroSubstitutions = list.at(0).toElement().text();
    }

    // read customisation name
    list = itemElement.elementsByTagName("CustomisationName" );
    if (list.count() > 0){
        customisationName = list.at(0).toElement().text();
    }

    // read creation option
    list = itemElement.elementsByTagName("CreationOption" );
    if (list.count() > 0)
    {
        creationOption = windowCustomisation::translateCreationOption( list.at(0).toElement().text() );
    }

    // create a menu item and add it to customisation set
    windowCustomisationButtonItem* item = new windowCustomisationButtonItem(buttonGroup, buttonText, buttonIcon, NULL/*!!! needs launch receiver object*/, uiFile, program,
                                                          programArguments, macroSubstitutions, creationOption, customisationName);
    return item;
}

 windowCustomisation* windowCustomisationList::getCustomisation(QString name){
     windowCustomisation* customisation = 0;
     for (int i = 0; i < customisationList.length(); i++){
         if (customisationList.at(i)->getName() == name){
            customisation = customisationList.at(i);
            break;
         }
     }
     return customisation;
 }

 void windowCustomisationList::addIncludeCustomisation(windowCustomisation* customisation, windowCustomisation* include){
     QList<windowCustomisationMenuItem*> menuItems = include->getMenuItems();
     QList<windowCustomisationButtonItem*> buttons = include->getButtons();
     // add menu items
     for (int i = 0; i < menuItems.length(); i++){
         customisation->addItem(menuItems.at(i));
     }
     // add button items
     for (int i = 0; i < buttons.length(); i++){
         customisation->addItem(buttons.at(i));
     }
}

// Add the named customisation to a main window.
// Return true if named customisation found and loaded.
bool windowCustomisationList::applyCustomisation( QMainWindow* mw, QString customisationName )
{

    qDebug() << "windowCustomisationList::applyCustomisation()" << mw << customisationName;
    windowCustomisation* customisation = getCustomisation(customisationName);
    if (!customisation)
        return false;

    QToolBar* mainToolBar = new QToolBar("Toolbar", mw);
    mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
    mw->addToolBar(Qt::TopToolBarArea, mainToolBar);
    QList<windowCustomisationButtonItem*> bList = customisation->getButtons();
    for ( int i = 0; i < bList.length(); i++ ){
        windowCustomisationButtonItem* item = new windowCustomisationButtonItem(bList.at(i));
        // add button action
        mainToolBar->addAction(item);
        // Set up an action to respond to the user
        QObject::connect( item, SIGNAL( newGui( const QEGuiLaunchRequests& ) ),
                          mw, SLOT( requestGui( const QEGuiLaunchRequests& ) ) );
    }


    QMenuBar* menuBar = mw->menuBar();
    QMap<QString, QMenu*> menuCreated;
    QList<QDockWidget*> dockWidgetList;

    QList<windowCustomisationMenuItem*> mList = customisation->getMenuItems();

    for (int i = 0; i < mList.length(); i++){
        windowCustomisationMenuItem* menuItem = new windowCustomisationMenuItem(mList.at(i));
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
        if (menuPoint){
            if (menuItem->getTitle() != "Separator"){
                QEGuiLaunchRequests::Options option = menuItem->getCreationOption();
                bool isDockItem = option == QEGuiLaunchRequests::OptionLeftDockWindow? true:
                              option == QEGuiLaunchRequests::OptionRightDockWindow? true:
                              option == QEGuiLaunchRequests::OptionTopDockWindow? true:
                              option == QEGuiLaunchRequests::OptionBottomDockWindow? true:false;
                if (isDockItem){
                    QAction* dockAction = createDockWidget(mw, menuItem, dockWidgetList);
                    menuPoint->addAction(dockAction);
                }
                else{
                    menuPoint->addAction(menuItem);
                    // Set up an action to respond to the user
                    QObject::connect( menuItem, SIGNAL( newGui( const QEGuiLaunchRequests& ) ),
                                      mw, SLOT( requestGui( const QEGuiLaunchRequests& ) ) );
                }
            }
            else{
                menuPoint->addSeparator();
            }
        }
    }
    // tabify dock widgets
    for ( int i = 0; i < dockWidgetList.length() - 1; i++ ){
        mw->tabifyDockWidget(dockWidgetList.at(i), dockWidgetList.at(i+1));
    }

    //!!! Extend the QMainWindow menu bar and tool bar with the named customisation
    //!!! Add required menus, menu items, button groups and buttons
    //!!! Use the windowCustomisationItem as the QAction to use for each menu item and button

//    qDeleteAll(menuCreated);
    menuCreated.clear();

    return true;
}

QAction* windowCustomisationList::createDockWidget( QMainWindow* mw,  windowCustomisationMenuItem* menuItem, QList<QDockWidget*>& dockWidgetList )
{
    QEGuiLaunchRequests::Options option = menuItem->getCreationOption();
    QString guiName = menuItem->getUiFile();
    QString dockTitle = menuItem->getCustomisationName();
    QFile file( guiName );
    if (!file.open(QIODevice::ReadOnly))
    {
         qDebug() << "Could not open ui file" << guiName;
         return false;
    }

    Qt::DockWidgetArea dockLocation = Qt::BottomDockWidgetArea;
    QUiLoader loader;
    QWidget* ui = loader.load( &file );
    QDockWidget *dock = new QDockWidget(dockTitle, mw);

    dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    if (option == QEGuiLaunchRequests::OptionLeftDockWindow){
        dockLocation = Qt::LeftDockWidgetArea;
    }
    else if (option == QEGuiLaunchRequests::OptionRightDockWindow){
        dockLocation = Qt::RightDockWidgetArea;
    }
    else if (option == QEGuiLaunchRequests::OptionTopDockWindow){
        dockLocation = Qt::TopDockWidgetArea;
    }
    mw->addDockWidget(dockLocation, dock);
    dock->setWidget(ui);
    dockWidgetList.append(dock);

    dock->hide();
//    dock->isHidden();

    return dock->toggleViewAction();
}
