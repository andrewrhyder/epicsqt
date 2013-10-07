/*  QEPlotterMenu.cpp
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

#include <QDebug>
#include <QECommon.h>
#include <QEPlotterMenu.h>

#define DEBUG  qDebug () << "QEPlotterMenu::" <<  __FUNCTION__  << ":" << __LINE__


//------------------------------------------------------------------------------
//
QEPlotterMenu::QEPlotterMenu (const int slotIn, QWidget* parent) : QMenu (parent)
{
   QMenu* menu = NULL;

   this->slot = slotIn;

   // Ensure all actions are null unless otherwise defined.
   //
   for (int j = 0; j < ARRAY_LENGTH (this->actionList); j++) {
      this->actionList [j] = 0;
   }

   this->setTitle ("Plotter Item");

   if (this->slot > 0) {
      // These only apply to Y data.
      //
      menu = new QMenu ("Line", this);
      this->addMenu (menu);
      this->make (menu, "Bold",      true, PLOTTER_LINE_BOLD);
      this->make (menu, "Dots",      true, PLOTTER_LINE_DOTS);
      this->make (menu, "Visible",   true, PLOTTER_LINE_VISIBLE);
      this->make (menu, "Colour...", false, PLOTTER_LINE_COLOUR)->setEnabled (slot < 16);
   }

   menu = new QMenu ("Data", this);
   this->addMenu (menu);
   this->make (menu, "Select",    false, PLOTTER_DATA_SELECT)->setEnabled (slot > 0);
   this->make (menu, "Define...", false, PLOTTER_DATA_DIALOG);
   this->make (menu, "Clear",     false, PLOTTER_DATA_CLEAR);

   menu = new QMenu ("Scale to", this);
   this->addMenu (menu);
   this->make (menu, "Min to Max values",  false, PLOTTER_SCALE_TO_MIN_MAX);
   this->make (menu, "Zero to Max values", false, PLOTTER_SCALE_TO_ZERO_MAX);

   QObject::connect (this, SIGNAL (triggered             (QAction* ) ),
                     this, SLOT   (contextMenuTriggered  (QAction* )));
}

//------------------------------------------------------------------------------
//
QEPlotterMenu::~QEPlotterMenu ()
{
}


//------------------------------------------------------------------------------
//
#define SET_ACTION(attribute)                                                  \
void QEPlotterMenu::setAction##attribute (const ContextMenuOptions option,     \
                                          const bool value)                    \
{                                                                              \
   const int t =  option - ContextMenuItemFirst;                               \
   if (t >= 0 && t < ARRAY_LENGTH (this->actionList)) {                        \
      QAction* action = this->actionList [t];                                  \
      if (action) action->set##attribute (value);                              \
   }                                                                           \
}


SET_ACTION (Checked)
SET_ACTION (Enabled)
SET_ACTION (Visible)

#undef SET_ACTION


//------------------------------------------------------------------------------
//
void QEPlotterMenu::setState (const bool isDisplayed, const bool isBold, const bool showDots)
{
   if (this->slot > 0) {
      this->setActionChecked (PLOTTER_LINE_VISIBLE, isDisplayed);
      this->setActionChecked (PLOTTER_LINE_BOLD, isBold);
      this->setActionChecked (PLOTTER_LINE_DOTS, showDots);
   }
}

//------------------------------------------------------------------------------
//
QAction* QEPlotterMenu::make (QMenu* parent,
                              const QString& caption,
                              const bool checkable,
                              const QEPlotterMenu::ContextMenuOptions option)
{
   QAction* action;

   action = new QAction (caption, parent);
   action->setCheckable (checkable);
   action->setData (QVariant (int (option)));
   parent->addAction (action);

   this->actionList [option - ContextMenuItemFirst] = action;
   return action;
}

//------------------------------------------------------------------------------
//
void QEPlotterMenu::contextMenuTriggered (QAction* selectedItem)
{
   bool okay;
   QEPlotterMenu::ContextMenuOptions option;

   option = QEPlotterMenu::ContextMenuOptions (selectedItem->data ().toInt (&okay));

   if (okay &&
       (option >= QEPlotterMenu::ContextMenuItemFirst) &&
       (option <= QEPlotterMenu::ContextMenuItemLast)) {
      emit this->contextMenuSelected (this->slot, option);
   }
}

// end
