/*  QEStripChartContextMenu.cpp
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

#include <QVariant>
#include <QDebug>
#include "QEStripChartContextMenu.h"
#include <QECommon.h>


#define DEBUG  qDebug () << "QEStripChartContextMenu::" <<  __FUNCTION__  << ":" << __LINE__


//------------------------------------------------------------------------------
//
QEStripChartContextMenu::QEStripChartContextMenu (bool inUseIn, QWidget *parent) : QMenu (parent)
{
   QMenu *menu;
   QAction *action;

   unsigned int j;

   this->inUse = inUseIn;

   this->setTitle ("PV Item");

   // Note: action items are not enabled until corresponding functionallity is implemented.
   //
   if (inUseIn) {
      this->make (this, "Read Archive",                        false, SCCM_READ_ARCHIVE);

      menu = new QMenu ("Scale chart to this PV's", this);
      this->addMenu (menu);
      this->make (menu, "HOPR/LOPR values",                    false, SCCM_SCALE_CHART_AUTO);
      this->make (menu, "Plotted min/max values",              false, SCCM_SCALE_CHART_PLOTTED);
      this->make (menu, "Buffered min/max values",             false, SCCM_SCALE_CHART_BUFFERED);

      menu = new QMenu ("Adjust/Scale this PV", this);
      this->addMenu (menu);
      this->make (menu, "Reset",                               false, SCCM_SCALE_PV_RESET);
      this->make (menu, "General...",                          false, SCCM_SCALE_PV_GENERAL);
      this->make (menu, "HOPR/LOPR values map to chart range", false, SCCM_SCALE_PV_AUTO);
      this->make (menu, "Plotted values map to chart range",   false, SCCM_SCALE_PV_PLOTTED);
      this->make (menu, "Buffered values map to chart range",  false, SCCM_SCALE_PV_BUFFERED);
      this->make (menu, "First value maps to chart centre",    false, SCCM_SCALE_PV_CENTRE);

      menu = new QMenu ("Mode", this);
      this->addMenu (menu);
      this->make (menu, "Rectangular",                         false, SCCM_PLOT_RECTANGULAR)->setEnabled (false);
      this->make (menu, "Smooth",                              false, SCCM_PLOT_SMOOTH)->setEnabled (false);
      this->make (menu, "User PV Process Time",                false, SCCM_PLOT_SERVER_TIME)->setEnabled (false);
      this->make (menu, "Use Receive Time",                    false, SCCM_PLOT_CLIENT_TIME)->setEnabled (false);
      menu->addSeparator();
      this->make (menu, "Linear",                              false, SCCM_ARCH_LINEAR)->setEnabled (false);
      this->make (menu, "Plot Binning",                        false, SCCM_ARCH_PLOTBIN)->setEnabled (false);
      this->make (menu, "Raw",                                 false, SCCM_ARCH_RAW)->setEnabled (false);
      this->make (menu, "Spread Sheet",                        false, SCCM_ARCH_SHEET)->setEnabled (false);
      this->make (menu, "Averaged",                            false, SCCM_ARCH_AVERAGED)->setEnabled (false);

      menu = new QMenu ("Line", this);
      this->addMenu (menu);
      this->make (menu, "Hide",                                true,  SCCM_LINE_HIDE)->setEnabled (false);
      this->make (menu, "Regular",                             true,  SCCM_LINE_REGULAR)->setEnabled (false);
      this->make (menu, "Bold",                                true,  SCCM_LINE_BOLD)->setEnabled (false);
      this->make (menu, "Colour...",                           false, SCCM_LINE_COLOUR);

      this->make (this, "Edit PV Name...",                     false, SCCM_PV_EDIT_NAME);

      this->make (this, "Write PV trace to file...",           false, SCCM_PV_WRITE_TRACE)->setEnabled (false);

      this->make (this, "Generate Statistics",                 false, SCCM_PV_STATS)->setEnabled (false);

      this->make (this, "Add to predefined PV names",          false, SCCM_ADD_TO_PREDEFINED);

      this->make (this, "Clear",                               false, SCCM_PV_CLEAR);

      for (j = 0 ; j < ARRAY_LENGTH (this->predefinedPVs); j++) {
         this->predefinedPVs [j] = NULL;
      }

   } else {

      this->make (this, "Add PV Name...",                      false, SCCM_PV_ADD_NAME);
      this->make (this, "Paste PV Name ",                      false, SCCM_PV_PASTE_NAME);
      this->make (this, "Colour...",                           false, SCCM_LINE_COLOUR);
      this->addSeparator ();

      for (j = 0 ; j < ARRAY_LENGTH (this->predefinedPVs); j++) {
         action = this->make (this, "", false, Options (SCCM_PREDEFINED_01 + j));
         action->setVisible (false);
         this->predefinedPVs [j] = action;
      }
   }

   QObject::connect (this, SIGNAL (triggered             (QAction* ) ),
                     this, SLOT   (contextMenuTriggered  (QAction* )));
}

//------------------------------------------------------------------------------
//
QEStripChartContextMenu::~QEStripChartContextMenu ()
{
   // no special action
}

//------------------------------------------------------------------------------
//
void QEStripChartContextMenu::setPredefinedNames (const QStringList & pvList)
{
   unsigned int j;
   QAction *action;

   if (!this->inUse) {
      for (j = 0 ; j < ARRAY_LENGTH (this->predefinedPVs); j++) {
         action = this->predefinedPVs [j];
         if (!action) continue;
         if ((int) j < pvList.count ()) {
            action->setText (pvList.value(j));
            action->setVisible (true);
         } else {
            action->setVisible (false);
         }
      }
   }
}

//------------------------------------------------------------------------------
//
QAction *QEStripChartContextMenu::exec (const unsigned int slotIn, const QPoint &pos, QAction *at)
{
   this->slot = slotIn;  // save context

   // Call super class function.
   //
   return QMenu::exec (pos, at);
}

//------------------------------------------------------------------------------
//
void QEStripChartContextMenu::contextMenuTriggered (QAction* selectedItem)
{
   Options option;

   option = Options (selectedItem->data ().toInt ());

   if ((option > SCCM_NONE) && (option < SCCM_LAST)) {
      emit this->contextMenuSelected (this->slot, option);
   }
}

//------------------------------------------------------------------------------
//
QAction* QEStripChartContextMenu::make (QMenu *parent,
                                        const QString & caption,
                                        const bool checkable,
                                        const Options option)
{
   QAction* action;

   action = new QAction (caption, parent);
   action->setCheckable (checkable);
   action->setData (QVariant (int (option)));
   parent->addAction (action);
   return action;
}

// end
