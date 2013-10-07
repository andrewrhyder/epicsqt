/*  QEPlotterMenu.h
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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QEPLOTTERMENU_H
#define QEPLOTTERMENU_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QWidget>
#include <contextMenu.h>

class QEPlotterMenu : public QMenu {
Q_OBJECT
public:
   explicit QEPlotterMenu (const int slot, QWidget* parent = 0);
   virtual ~QEPlotterMenu ();

   // IDs for all menu options
   // Each menu option has a unique ID across all menus
   // These IDs are in addition to standard context menu IDs and so start after
   // contextMenu::CM_SPECIFIC_WIDGETS_START_HERE
   //
   enum ContextMenuOptions {
      PLOTTER_NONE = contextMenu::CM_SPECIFIC_WIDGETS_START_HERE,

      PLOTTER_LINE_BOLD,
      PLOTTER_LINE_DOTS,
      PLOTTER_LINE_VISIBLE,
      PLOTTER_LINE_COLOUR,

      PLOTTER_DATA_SELECT,
      PLOTTER_DATA_DIALOG,
      PLOTTER_DATA_CLEAR,

      PLOTTER_SCALE_TO_MIN_MAX,
      PLOTTER_SCALE_TO_ZERO_MAX
   };

   // These MUST be consistant with above declaration.
   //
   static const ContextMenuOptions ContextMenuItemFirst = PLOTTER_LINE_BOLD;
   static const ContextMenuOptions ContextMenuItemLast  = PLOTTER_SCALE_TO_ZERO_MAX;
   static const int NumberContextMenuItems = ContextMenuItemLast - ContextMenuItemFirst + 1;

   // Allow setting of specific action attributes using option as index.
   //
   void setActionChecked (const ContextMenuOptions option, const bool visible);
   void setActionEnabled (const ContextMenuOptions option, const bool visible);
   void setActionVisible (const ContextMenuOptions option, const bool visible);

   void setState (const bool isDisplayed, const bool isBold, const bool showDots);

signals:
   // All the triggered actions from the various sub-menu items are
   // converted to an ContextMenuOptions value.
   //
   void contextMenuSelected (const int slot, const QEPlotterMenu::ContextMenuOptions);

private:
   int slot;
   QAction* actionList [NumberContextMenuItems];

   // Utility function to create and set up an action.
   //
   QAction* make (QMenu *parent,
                  const QString &caption,
                  const bool checkable,
                  const QEPlotterMenu::ContextMenuOptions option);

private slots:
   void contextMenuTriggered (QAction* selectedItem);
};

#endif  // QEPLOTTERMENU_H
