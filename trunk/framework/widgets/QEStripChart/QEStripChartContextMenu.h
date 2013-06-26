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
#include <QEArchiveInterface.h>
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
   void setUseReceiveTime  (const bool useReceiveTime);
   void setArchiveReadHow  (const QEArchiveInterface::How how);

   // Wrapper exec function.
   //
   QAction *exec (const unsigned int slot,
                  const QPoint &pos, QAction *at=0);

   // Must be consistant with number of SCCM_PREDEFINED_XX items.
   //
   static const int numberPrefefinedItems = 10;

signals:
   // All the triggered actions from the various sub-menu items are
   // converted to an ContextMenuOptions value.
   void contextMenuSelected (const unsigned int, const QEStripChartNames::ContextMenuOptions);

private:
   bool inUse;
   unsigned int slot;

   QAction *predefinedPVs [numberPrefefinedItems];

   // Utility function to create and set up an action.
   //
   QAction* make (QMenu *parent,
                  const QString &caption,
                  const bool checkable,
                  const QEStripChartNames::ContextMenuOptions option);

   QAction* serverTime;
   QAction* clientTime;
   QAction* archiveModes [5];

private slots:
   void contextMenuTriggered (QAction* selectedItem);
};

#endif  // QESTRIPCHARTCONTEXTMENU_H 
