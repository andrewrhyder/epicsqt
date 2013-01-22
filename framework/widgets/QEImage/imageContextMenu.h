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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/*
 This class manages the context menu for the QEImage widget
 It extends the standard QEFramework context menu
 Functions are available to add menu options, and sub menus.
 A function is available to present the menu to the user and return a user selection
 */

#ifndef IMAGECONTEXTMENU_H
#define IMAGECONTEXTMENU_H

#include <contextMenu.h>
#include <QMenu>

class imageContextMenu : public QMenu
{
    Q_OBJECT
public:

    explicit imageContextMenu( QWidget *parent = 0 );

    // IDs for all menu options
    // Each menu option has a unique ID across all menus
    // These IDs are in addition to standard context menu IDs and so start after contextMenu::CM_SPECIFIC_WIDGETS_START_HERE
    enum imageContextMenuOptions{ ICM_NONE = contextMenu::CM_SPECIFIC_WIDGETS_START_HERE,
                                  ICM_SAVE,
                                  ICM_PAUSE,
                                  ICM_ENABLE_TIME,
                                  ICM_ENABLE_CURSOR_PIXEL,
                                  ICM_ENABLE_CONTRAST_REVERSAL,
                                  ICM_ENABLE_VERT,
                                  ICM_ENABLE_HOZ,
                                  ICM_ENABLE_AREA,
                                  ICM_ENABLE_LINE,
                                  ICM_ENABLE_TARGET,
                                  ICM_DISPLAY_BUTTON_BAR,
                                  ICM_ZOOM_SELECTED,
                                  ICM_ZOOM_FIT,
                                  ICM_ZOOM_10,
                                  ICM_ZOOM_25,
                                  ICM_ZOOM_50,
                                  ICM_ZOOM_75,
                                  ICM_ZOOM_100,
                                  ICM_ZOOM_150,
                                  ICM_ZOOM_200,
                                  ICM_ZOOM_300,
                                  ICM_ZOOM_400,
                                  ICM_ROTATE_NONE,
                                  ICM_ROTATE_RIGHT,
                                  ICM_ROTATE_LEFT,
                                  ICM_ROTATE_180,
                                  ICM_FLIP_HORIZONTAL,
                                  ICM_FLIP_VERTICAL,
                                  ICM_SELECT_PAN,
                                  ICM_SELECT_HSLICE,
                                  ICM_SELECT_VSLICE,
                                  ICM_SELECT_AREA1,
                                  ICM_SELECT_AREA2,
                                  ICM_SELECT_AREA3,
                                  ICM_SELECT_AREA4,
                                  ICM_SELECT_PROFILE,
                                  ICM_SELECT_TARGET,
                                  ICM_SELECT_BEAM,
                                  ICM_CLEAR_MARKUP
                                };

    // Present the context menu, and return whatever the user selects
    void getContextMenuOption( const QPoint&, imageContextMenuOptions* option, bool* checked );
    // Add an item to the context menu
    void addMenuItem( const QString& title, const bool checkable, const bool checked, const imageContextMenuOptions option );
    // Add an item to the context menu's 'Option' sub menu
    void addOptionMenuItem( const QString& title, const bool checkable, const bool checked, const imageContextMenuOptions option );

signals:

public slots:

private:
    // Add a menu item to the either the context menu, or one of its sub menus
    void addMenuItemGeneric( QMenu* menu, const QString& title, const bool checkable, const bool checked, const imageContextMenuOptions option );

    QMenu* optionMenu;
};

#endif // IMAGECONTEXTMENU_H
