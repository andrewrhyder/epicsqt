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
 * Main class descriptions:
 *  - windowCustomisationList contains a list of windowCustomisation customisations.
 *
 *  - windowCustomisation contains a named customisation consisting of a name and a list of windowCustomisationMenuItem and
 *    a list of windowCustomisationButtonItem classes.
 *
 *  - windowCustomisationMenuItem and windowCustomisationButtonItem classes are both based on windowCustomisationItem which holds
 *    details about what to do (which .ui file to open, etc)
 *
 * To use this module:
 * Create a single instance of windowCustomisationList class.
 * Call windowCustomisationList::loadCustomisation() at any time to load .xml files containing one or more named customisations.
 * When starting GUIs, or creating QE widgets (or any other appropriate time) call windowCustomisationList::applyCustomisation()
 * to apply a named customisation to a QMainWindow
 *
 * The following example allows for the following:
 *
 * - IMBL main GUIs such as the general beamline overview GUI request customisation 'IBML_MAIN'
 *   which has a large set of menu options.
 *
 * - IMBL minor GUIs such as transient status GUIs request customisation 'IBML_MINOR' which adds
 *   a single menu option to return to the main beamline GUI.
 *
 * - IMBL Region Of Interest (ROI) GUIs request customisation 'ROI' which adds some options relevent
 *   for region of interest images (including both menu items and tool bar buttons) and also includes
 *   the 'IBML_MINOR' customisation.
 *

    <QEWindowCustomisation>
        <Customisation Name="IMBL_MAIN">
            <Menu Name="Imaging">
                <Menu Name="Regions">
                    <Item Name="ROI 1">
                        <UiFile>ROI.ui</UiFile>
                        <Program Name="firefox">
                            <Arguments>www.google.com</Arguments>
                        </Program>
                        <MacroSubstitutions>REGION=1</MacroSubstitutions>
                        <Customisation>ROI</Customisation>
                    </Item>

                    <Item Name="ROI 2">
                        <UiFile>ROI.ui</UiFile>
                        <MacroSubstitutions>REGION=2</MacroSubstitutions>
                        <Customisation>ROI</Customisation>
                    </Item>

                </Menu>
            </Menu>
        </Customisation>


        <Customisation Name="IMBL_MINOR">
            <Menu Name="File">
                <Item Name="Main Window">
                    <UiFile>IMBL.ui</UiFile>
                    <Customisation>IMBL_MAIN</Customisation>
                </Item>
            </Menu>
        </Customisation>


        <Customisation Name="ROI">
            <Menu Name="Imaging">
                <Item Name="Plot">
                    <UiFile>ROI_Plot.ui</UiFile>
                    <Customisation>IMBL_MINOR</Customisation>
                </Item>
            </Menu>
            <Button Name="Plot">
                <Icon>plot.png</Icon>
                <UiFile>ROI_Plot.ui</UiFile>
                <Customisation>IMBL_MINOR</Customisation>
            </Button>
            <IncludeCustomisation Name="IMBL_MINOR"></IncludeCustomisation>
        </Customisation>

        <CustomisationIncludeFile> </CustomisationIncludeFile>

    </QEWindowCustomisation>
 */

#ifndef WINDOWCUSTOMISATION_H
#define WINDOWCUSTOMISATION_H

#include <QObject>
#include <QAction>
#include <QList>
#include <QString>
#include <QStringList>
#include <QMainWindow>
#include <QDomDocument>
#include <QEGuiLaunchRequests.h>

// Class defining an individual item (base class for button or menu item)
class windowCustomisationItem : public QAction
{
    Q_OBJECT
public:
    windowCustomisationItem( // Construction
                      const QObject* launchRequestReceiver,                // Object (typically QEGui application) which will accept requests to launch a new GUI
                      const QString uiFileIn,                              // UI to display
                      const QString programIn,                             // Program to run
                      const QStringList programArgumentsIn,                // Arguments for 'program'
                      const QString macroSubstitutionsIn,                  // Substitutions for ui file, program and arguments
                      const QEGuiLaunchRequests::Options creationOptionIn, // Window creation options
                      const QString customisationNameIn );                 // New window customisation name (menu, buttons, etc)


private:
    // Item action
    QString uiFile;                                 // UI to display
    QString program;                                // Program to run
    QStringList programArguments;                   // Arguments for 'program'
    QString macroSubstitutions;                     // Substitutions for ui file, program and arguments
    QEGuiLaunchRequests::Options creationOption;    // Window creation options
    QString customisationName;                      // New window customisation name (menu, buttons, etc)

private slots:
    void itemAction();              // Slot to call when action is triggered

signals:
    void newGui( const QEGuiLaunchRequests& request );

};

// Class defining an individual menu item
class windowCustomisationMenuItem : public windowCustomisationItem
{
public:
    windowCustomisationMenuItem( // Construction
                          const QStringList menuHierarchyIn,                   // Location in menus to place this item. for example: 'Imaging'->'Region of interest'
                          const QString titleIn,                               // Name of this item. for example: 'Region 1'

                          const QObject* launchRequestReceiver,                // Object (typically QEGui application) which will accept requests to launch a new GUI
                          const QString uiFileIn,                              // UI to display
                          const QString programIn,                             // Program to run
                          const QStringList programArgumentsIn,                // Arguments for 'program'
                          const QString macroSubstitutionsIn,                  // Substitutions for ui file, program and arguments
                          const QEGuiLaunchRequests::Options creationOptionIn, // Window creation options
                          const QString customisationNameIn );                 // New window customisation name (menu, buttons, etc)

    QStringList getMenuHierarchy(){return menuHierarchy;}
    QString getTitle(){return title;}
private:
    // Menu bar details.
    // All details are optional.
    // A menu item is created if menuHierarchy contains at least one level and title exists
    QStringList menuHierarchy;  // Location in menus to place this item. for example: 'Imaging'->'Region of interest'
    QString title;              // Name of this item. for example: 'Region 1'
};

// Class defining an individual button item
class windowCustomisationButtonItem : public windowCustomisationItem
{
public:
    windowCustomisationButtonItem( // Construction
                            const QString buttonGroupIn,                         // Name of toolbar button group in which to place a button
                            const QString buttonTextIn,                          // Text to place in button
                            const QString buttonIconIn,                          // Icon for button

                            const QObject* launchRequestReceiver,                // Object (typically QEGui application) which will accept requests to launch a new GUI
                            const QString uiFileIn,                              // UI to display
                            const QString programIn,                             // Program to run
                            const QStringList programArgumentsIn,                // Arguments for 'program'
                            const QString macroSubstitutionsIn,                  // Substitutions for ui file, program and arguments
                            const QEGuiLaunchRequests::Options creationOptionIn, // Window creation options
                            const QString customisationNameIn );                 // New window customisation name (menu, buttons, etc)

    QString getButtonText(){ return buttonText; }

private:
    // Button details.
    // All details are optional.
    // A button is created if buttonText or buttonIcon is available
    QString buttonGroup;        // Name of toolbar button group in which to place a button
    QString buttonText;         // Text to place in button
    QString buttonIcon;         // Icon for button
};

// Class defining the customisation of a window.
// Generated from an XML customisation file.
class windowCustomisation
{
public:
    windowCustomisation( const QString nameIn );       // Construction - create a named, empty, customisation
    ~windowCustomisation();                            // Destruction

    void addItem( windowCustomisationMenuItem* menuItem );      // Add a menu item to the customisation
    void addItem( windowCustomisationButtonItem* button );    // Add a button to the customisation

    QList<windowCustomisationMenuItem*> getMenuItems(){return menuItems;}      // get Menu items list
    QList<windowCustomisationButtonItem*> getButtons(){return buttons;}        // get Buttons list
    QString getName(){ return name; }

    static QEGuiLaunchRequests::Options translateCreationOption( QString creationOption);

private:
    QString name;                                  // Customisation name
    QList<windowCustomisationMenuItem*> menuItems; // Menu items to be added to menu bar to implement customisation
    QList<windowCustomisationButtonItem*> buttons; // Buttons to be added to tool bar to implement customisation
};

// Class managing all customisation sets
// Only instance of this class is instantiated (unless groups of customisation sets are required)
// Multiple .xml files may be loaded, each defining one or more named customisations.
class windowCustomisationList
{
public:
    windowCustomisationList();

    bool loadCustomisation( QString xmlFile );                      // Load a set of customisations
    bool applyCustomisation( QMainWindow* mw, QString customisationName ); // Add the named customisation set to a main window. Return true if named customisation found and loaded.


private:
    windowCustomisation* getCustomisation(QString name);
    void addIncludeCustomisation(windowCustomisation* customisation, windowCustomisation* include);
    void parseMenuCfg( // Parse menu customisation data
                       QDomNode menuNode,
                       windowCustomisation* customisation,
                       QStringList menuHierarchy );
    windowCustomisationMenuItem* createMenuItem( // Create a menu customisation item
                                          QDomElement itemElement,
                                          QStringList menuHierarchy);
    windowCustomisationButtonItem* createButtonItem( // Create a button customisation item
                                              QDomElement itemElement);
    QList<windowCustomisation*> customisationList;                         // List of customisations
};

#endif // WINDOWCUSTOMISATION_H
