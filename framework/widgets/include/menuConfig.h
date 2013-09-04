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
 * Main class descriptions:
 *  - windowConfigList contains a list of windowConfig configurations.
 *
 *  - windowConfig contains a named configuration consisting of a name and a list of windowConfigMenuItem and
 *    a list of windowConfigButtonItem classes.
 *
 *  - windowConfigMenuItem and windowConfigButtonItem classes are both based on windowConfigItem which holds
 *    details about what to do (which .ui file to open, etc)
 *
 * To use this module:
 * Create a single instance of windowConfigList class.
 * Call windowConfigList::loadConfig() at any time to load .xml files containing one or more named configurations.
 * When starting GUIs, or creating QE widgets (or any other appropriate time) call windowConfigList::applyConfig()
 * to apply a named configuration to a QMainWindow
 *
 * The following example allows for the following:
 *
 * - IMBL main GUIs such as the general beamline overview GUI request configuration 'IBML_MAIN'
 *   which has a large set of menu options.
 *
 * - IMBL minor GUIs such as transient status GUIs request configuration 'IBML_MINOR' which adds
 *   a single menu option to return to the main beamline GUI.
 *
 * - IMBL Region Of Interest (ROI) GUIs request configuration 'ROI' which adds some options relevent
 *   for region of interest images (including both menu items and tool bar buttons) and also includes
 *   the 'IBML_MINOR' configuration.
 *

    <QEWindowConfig>
        <Config Name="IMBL_MAIN">
            <Menu Name="Imaging">
                <Menu Name="Regions">
                    <Item Name="ROI 1">
                        <UiFile>"ROI.ui"</path>
                        <Program Name="firefox">
                            <Arguments>"www.google.com"</Arguments>
                        </Program>
                        <macroSubstitutions>"REGION=1"</macroSubstitutions>
                        <config>"ROI"</config>
                    </Item>

                    <Item Name="ROI 2">
                        <UiFile>"ROI.ui"</path>
                        <macroSubstitutions>"REGION=2"</macroSubstitutions>
                        <config>"ROI"</config>
                    </Item>

                </Menu>
            </Menu>
        </ConfigwindowConfigList>


        <Config Name="IMBL_MINOR">
            <Menu Name="File">
                <Item Name="Main Window">
                    <UiFile>"IMBL.ui"</path>
                    <config>"IMBL_MAIN"</config>
                </Item>
            </Menu>
        </Config>


        <Config Name="ROI">
            <Menu Name="Imaging">
                <Item Name="Plot">
                    <UiFile>"ROI_Plot.ui"</path>
                    <config>"IMBL_MINOR"</config>
                </Item>
            </Menu>
            <Button>
                <Icon>"plot.png"</Icon>
                <UiFile>"ROI_Plot.ui"</path>
                <config>"IMBL_MINOR"</config>
            </Button>
            <IncludeConfig> Name="IMBL_MINOR"</IncludeConfig>
        </Config>

    <QEWindowConfig>
 */

#ifndef WINDOWCONFIG_H
#define WINDOWCONFIG_H

#include <QObject>
#include <QAction>
#include <QList>
#include <QString>
#include <QStringList>
#include <QMainWindow>

// Class defining an individual item (base class for button or menu item)
class windowConfigItem : public QAction
{
    Q_OBJECT
public:
    windowConfigItem( // Construction
                      const QString uiFileIn,                 // UI to display
                      const QString programIn,                // Program to run
                      const QStringList programArgumentsIn,   // Arguments for 'program'
                      const QString macroSubstitutionsIn,     // Substitutions for ui file, program and arguments
                      const QString configNameIn );           // New window configuration name (menu, buttons, etc)


private:
    // Item action
    QString uiFile;                 // UI to display
    QString program;                // Program to run
    QStringList programArguments;   // Arguments for 'program'
    QString macroSubstitutions;     // Substitutions for ui file, program and arguments
    QString configName;             // New window configuration name (menu, buttons, etc)

private slots:
     void itemAction();             // Slot to call when actiowindowConfign is triggered

};

// Class defining an individual menu item
class windowConfigMenuItem : public windowConfigItem
{
public:
    windowConfigMenuItem( // Construction
                          const QStringList menuHierarchyIn,      // Location in menus to place this item. for example: 'Imaging'->'Region of interest'
                          const QString titleIn,                  // Name of this item. for example: 'Region 1'

                          const QString uiFileIn,                 // UI to display
                          const QString programIn,                // Program to runwindowConfig
                          const QStringList programArgumentsIn,   // Arguments for 'program'
                          const QString macroSubstitutionsIn,     // Substitutions for ui file, program and arguments
                          const QString configNameIn );           // New window configuration name (menu, buttons, etc)

private:
    // Menu bar details.
    // All details are optional.
    // A menu item is created if menuHierarchy contains at least one level and title exists
    QStringList menuHierarchy;  // Location in menus to place this item. for example: 'Imaging'->'Region of interest'
    QString title;              // Name of this item. for example: 'Region 1'
};

// Class defining an individual button item
class windowConfigButtonItem : public windowConfigItem
{
public:
    windowConfigButtonItem( // Construction
                            const QString buttonGroupIn,            // Name of toolbar button group in which to place a button
                            const QString buttonTextIn,             // Text to place in button
                            const QString buttonIconIn,             // Icon for button

                            const QString uiFileIn,                 // UI to display
                            const QString programIn,                // Program to run
                            const QStringList programArgumentsIn,   // Arguments for 'program'
                            const QString macroSubstitutionsIn,     // Substitutions for ui file, program and arguments
                            const QString configNameIn );           // New window configuration name (menu, buttons, etc)

private:
    // Button details.
    // All details are optional.
    // A button is created if buttonText or buttonIcon is available
    QString buttonGroup;        // Name of toolbar button group in which to place a button
    QString buttonText;         // Text to place in button
    QString buttonIcon;         // Icon for button
};

// Class defining a configuration for a window.
// Generated from XML, generaly from a configuration file.
class windowConfig
{
public:
    windowConfig( const QString nameIn );       // Construction - create a named, empty, configuration
    ~windowConfig();                            // Destruction

    void addItem( windowConfigMenuItem* menuItem );      // Add a menu item to the configuration
    void addItem( windowConfigButtonItem* button );    // Add a button to the configuration

private:
    QString name;                               // Configuration name
    QList<windowConfigMenuItem*> menuItems;      // Menu items to be added to menu bar to implement configuration
    QList<windowConfigButtonItem*> buttons;      // Buttons to be added to tool bar to implement configuration
};

// Class managing all configurations
// Only instance of this class is instantiated (unless groups of configurations are requried)
// Multiple .xml files may be loaded, each defining one or more named configurations.
class windowConfigList
{
public:
    windowConfigList();

    void loadConfig( QString xmlFile );                      // Load a set of configurations
    bool applyConfig( QMainWindow* mw, QString configName ); // Add the named configuration to a main window. Return true if named configuration found and loaded.

private:
    QList<windowConfig> configList;                         // List of configurations
};

#endif // WINDOWCONFIG_H
