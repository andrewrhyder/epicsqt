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
 * Helps QE widgets add and remove menu options from the main window menu bar.
 *
 */

#ifndef MENUCONFIG_H
#define MENUCONFIG_H

#include <QAction>
#include <QList>
#include <QString>
#include <QStringList>

class menuConfigItem : public QAction
{
    Q_OBJECT
public:
    menuConfigItem( QString titleIn );

private:
    // Menu location
    QStringList menuHierarchy;  // Location in menus to place this item. for example: 'Imaging'->'Region of interest'
    QString title;              // Name of this item. for example: 'Region 1'

    // Item action
    QString uiFile;                 // UI to display
    QString program;                // Program to run
    QStringList programArguments;   // Arguments for 'program'
    QString macroSubstitutions;     // Substitutions for ui file, program and arguments
    QString configName;             // New window configuration name (menu, buttons, etc)
    QString configFile;             // New window configuration (menu, buttons, etc)
    QString configXml;              // New window configuration (menu, buttons, etc)

private slots:
     void itemAction();

};

class menuConfig
{
public:
    menuConfig();
private:
    QStringList names;
    QList<menuConfigItem> configItems;

};

#endif // MENUCONFIG_H
