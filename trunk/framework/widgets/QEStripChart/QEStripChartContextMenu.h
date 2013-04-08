/*  QEStripChartContextMenu.h
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

#ifndef QESTRIPCHARTCONTEXTMENU_H
#define QESTRIPCHARTCONTEXTMENU_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QWidget>
#include <QStringList>
#include <contextMenu.h>

#include "QEStripChartNames.h"

class QEStripChartContextMenu : public QMenu {
Q_OBJECT
public:
   /// Construct strip chart item context menu.
   /// This menu item creates all required sub menu items.
   /// inUse set true for an inuse slot, i.e. already has a PV allocated.
   /// inUse set false for an empty slot.
   explicit QEStripChartContextMenu (bool inUse, QWidget *parent = 0);
   virtual ~QEStripChartContextMenu ();

   void setPredefinedNames (const QStringList & pvList);

   // Wrapper exec function.
   //
   QAction *exec(const unsigned int slot, const QPoint &pos, QAction *at=0);

   // IDs for all menu options
   // Each menu option has a unique ID across all menus
   // These IDs are in addition to standard context menu IDs and so start after
   // contextMenu::CM_SPECIFIC_WIDGETS_START_HERE
   //
   enum Options {
      SCCM_NONE = contextMenu::CM_SPECIFIC_WIDGETS_START_HERE,
      SCCM_READ_ARCHIVE,
      //
      SCCM_SCALE_CHART_AUTO,
      SCCM_SCALE_CHART_PLOTTED,
      SCCM_SCALE_CHART_BUFFERED,
      //
      SCCM_SCALE_PV_RESET,
      SCCM_SCALE_PV_GENERAL,
      SCCM_SCALE_PV_AUTO,
      SCCM_SCALE_PV_PLOTTED,
      SCCM_SCALE_PV_BUFFERED,
      SCCM_SCALE_PV_CENTRE,
      //
      SCCM_PLOT_RECTANGULAR,
      SCCM_PLOT_SMOOTH,
      SCCM_PLOT_SERVER_TIME,
      SCCM_PLOT_CLIENT_TIME,
      //
      SCCM_ARCH_LINEAR,
      SCCM_ARCH_PLOTBIN,
      SCCM_ARCH_RAW,
      SCCM_ARCH_SHEET,
      SCCM_ARCH_AVERAGED,
      //
      SCCM_LINE_HIDE,
      SCCM_LINE_REGULAR,
      SCCM_LINE_BOLD,
      SCCM_LINE_COLOUR,
      //
      SCCM_PV_EDIT_NAME,
      SCCM_ADD_TO_PREDEFINED,
      SCCM_PV_WRITE_TRACE,
      SCCM_PV_STATS,
      SCCM_PV_CLEAR,
      //
      SCCM_PV_ADD_NAME,
      SCCM_PV_PASTE_NAME,
      //
      SCCM_PREDEFINED_01,
      SCCM_PREDEFINED_02,
      SCCM_PREDEFINED_03,
      SCCM_PREDEFINED_04,
      SCCM_PREDEFINED_05,
      SCCM_PREDEFINED_06,
      SCCM_PREDEFINED_07,
      SCCM_PREDEFINED_08,
      SCCM_PREDEFINED_09,
      SCCM_PREDEFINED_10,
      //
      SCCM_LAST
   };

   // Must be consistant with number of SCCM_PREDEFINED_XX items.
   //
   static const int numberPrefefinedItems = 10;

signals:
   // All the triggered actions from the various sub-menu items are
   // converted to an Options value.
   void contextMenuSelected (const unsigned int, const QEStripChartContextMenu::Options);

private:
   bool inUse;
   unsigned int slot;

   QAction *predefinedPVs [numberPrefefinedItems];

   // Utility function to create and set up an action.
   //
   QAction* make (QMenu *parent,
                  const QString &caption,
                  const bool checkable,
                  const Options option);

private slots:
   void contextMenuTriggered (QAction* selectedItem);
};

#endif  // QESTRIPCHARTCONTEXTMENU_H 
