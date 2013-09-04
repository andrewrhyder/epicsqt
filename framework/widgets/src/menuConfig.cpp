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
        : QAction( this )
{
    // Save the item details
    uiFile = uiFileIn;
    program = programIn;
    programArguments = programArgumentsIn;
    macroSubstitutions = macroSubstitutionsIn;
    configName = configNameIn;

    // Set up an action to respond to the user
    connect( this, SIGNAL( triggered()), this, SLOT(itemAction()));
}

// A user has triggered the menu item or button
void windowConfigItem::itemAction()
{

}

//==============================================================================================
// windowConfigMenuItem
//==============================================================================================

// Construct instance of class defining an individual menu item
windowConfigMenuItem::windowConfigMenuItem( QStringList menuHierarchyIn,       // Location in menus to place this item. for example: 'Imaging'->'Region of interest'
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
void windowConfigList::loadConfig( QString xmlFile )
{
    qDebug() << "windowConfigList::loadConfig()" << xmlFile;
    // !!! Read and parse xmlFile
    // !!! if named configuration exists, replace it
    // !!! Parse XML using Qt's Document Object Model. Refer to code in persistanceManager.cpp/.h
}

// Add the named configuration to a main window.
// Return true if named configuration found and loaded.
bool windowConfigList::applyConfig( QMainWindow* mw, QString configName )
{
     qDebug() << "windowConfigList::applyConfig()" << mw << configName;
    //!!! Extend the QMainWindow menu bar and tool bar with the named configuration
    //!!! Add required menus, menu items, button groups and buttons
    //!!! Use the windowConfigItem as the QAction to use for each menu item and button
    return true;
}
