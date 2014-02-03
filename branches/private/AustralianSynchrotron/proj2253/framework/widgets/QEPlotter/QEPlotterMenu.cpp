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
 *  Copyright (c) 2013 Australian Synchrotron.
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
QEPlotterMenu::QEPlotterMenu (QWidget* parent) : QMenu (parent)
{
   QMenu* menu = NULL;

   this->slot = 0;

   // Ensure all actions are null unless otherwise defined.
   //
   for (int j = 0; j < ARRAY_LENGTH (this->actionList); j++) {
      this->actionList [j] = NULL;
   }

   this->setTitle ("Plotter General");

   menu = new QMenu ("Show", this);
   this->addMenu (menu);
   this->make (menu, "Show/Hide Tool Bar",  true,  QEPlotterNames::PLOTTER_SHOW_HIDE_TOOLBAR);
   this->make (menu, "Show/Hide PV Items",  true,  QEPlotterNames::PLOTTER_SHOW_HIDE_PV_ITEMS);
   this->make (menu, "Show/Hide Status",    true,  QEPlotterNames::PLOTTER_SHOW_HIDE_STATUS);

   menu = new QMenu ("General", this);
   this->addMenu (menu);
   this->make (menu, "Previous state",      false, QEPlotterNames::PLOTTER_PREV);
   this->make (menu, "Next state",          false, QEPlotterNames::PLOTTER_NEXT);

   this->make (menu, "White background",    false, QEPlotterNames::PLOTTER_NORMAL_VIDEO);
   this->make (menu, "Black background",    false, QEPlotterNames::PLOTTER_REVERSE_VIDEO);

   this->make (menu, "Linear scale",        false, QEPlotterNames::PLOTTER_LINEAR_Y_SCALE);
   this->make (menu, "Log Scale",           false, QEPlotterNames::PLOTTER_LOG_Y_SCALE);

   this->make (menu, "Play - Real time",    false, QEPlotterNames::PLOTTER_PLAY);
   this->make (menu, "Pause",               false, QEPlotterNames::PLOTTER_PAUSE);

   menu = new QMenu ("Y Scale", this);
   this->addMenu (menu);
   this->make (menu, "Manual Y Scale...",   false, QEPlotterNames::PLOTTER_MANUAL_Y_RANGE);
   this->make (menu, "Y Data Range Scale",  false, QEPlotterNames::PLOTTER_CURRENT_Y_RANGE);
   this->make (menu, "Dynamic Y Scale",     false, QEPlotterNames::PLOTTER_DYNAMIC_Y_RANGE);
   this->make (menu, "Noramalised Scale",   false, QEPlotterNames::PLOTTER_NORAMLISED_Y_RANGE);
   this->make (menu, "Fractional Scale",    false, QEPlotterNames::PLOTTER_FRACTIONAL_Y_RANGE);

   menu = new QMenu ("X Scale", this);
   this->addMenu (menu);
   this->make (menu, "Manual X Scale...",   false, QEPlotterNames::PLOTTER_MANUAL_X_RANGE);
   this->make (menu, "X Data Range Scale",  false, QEPlotterNames::PLOTTER_CURRENT_X_RANGE);
   this->make (menu, "Dynamic X Scale",     false, QEPlotterNames::PLOTTER_DYNAMIC_X_RANGE);

   QObject::connect (this, SIGNAL (triggered             (QAction* ) ),
                     this, SLOT   (contextMenuTriggered  (QAction* )));
}

//------------------------------------------------------------------------------
//
QEPlotterMenu::QEPlotterMenu (const int slotIn, QWidget* parent) : QMenu (parent)
{
   QMenu* menu = NULL;

   this->slot = slotIn;

   // Ensure all actions are null unless otherwise defined.
   //
   for (int j = 0; j < ARRAY_LENGTH (this->actionList); j++) {
      this->actionList [j] = NULL;
   }

   this->setTitle ("Plotter Item");

   menu = new QMenu ("Data", this);
   this->addMenu (menu);
   this->make (menu, "Define...",           false, QEPlotterNames::PLOTTER_DATA_DIALOG);
   this->make (menu, "Paste Data PV Name ", false, QEPlotterNames::PLOTTER_PASTE_DATA_PV);
   this->make (menu, "Paste Size PVName",   false, QEPlotterNames::PLOTTER_PASTE_SIZE_PV);
   this->make (menu, "Select",              false, QEPlotterNames::PLOTTER_DATA_SELECT)->setEnabled (slot > 0);
   this->make (menu, "Clear",               false, QEPlotterNames::PLOTTER_DATA_CLEAR);

   menu = new QMenu ("Scale to", this);
   this->addMenu (menu);
   this->make (menu, "Min to Max values",   false, QEPlotterNames::PLOTTER_SCALE_TO_MIN_MAX);
   this->make (menu, "Zero to Max values ", false, QEPlotterNames::PLOTTER_SCALE_TO_ZERO_MAX);

   menu = new QMenu ("Line", this);
   this->addMenu (menu);

   // This only apply to Y data.
   //
   menu->setEnabled (this->slot > 0);
   if (this->slot > 0) {
      this->make (menu, "Bold",             true, QEPlotterNames::PLOTTER_LINE_BOLD);
      this->make (menu, "Dots",             true, QEPlotterNames::PLOTTER_LINE_DOTS);
      this->make (menu, "Visible",          true, QEPlotterNames::PLOTTER_LINE_VISIBLE);
      this->make (menu, "Colour... ",       false,QEPlotterNames::PLOTTER_LINE_COLOUR)->setEnabled (slot < 16);
   }

   QObject::connect (this, SIGNAL (triggered             (QAction* )),
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
void QEPlotterMenu::setAction##attribute (const QEPlotterNames::MenuActions action,  \
                                          const bool value)                    \
{                                                                              \
   const int t =  action - QEPlotterNames::PLOTTER_FIRST;                      \
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
      this->setActionChecked (QEPlotterNames::PLOTTER_LINE_VISIBLE, isDisplayed);
      this->setActionChecked (QEPlotterNames::PLOTTER_LINE_BOLD, isBold);
      this->setActionChecked (QEPlotterNames::PLOTTER_LINE_DOTS, showDots);
   }
}

//------------------------------------------------------------------------------
//
QAction* QEPlotterMenu::make (QMenu* parent,
                              const QString& caption,
                              const bool checkable,
                              const QEPlotterNames::MenuActions menuAction)
{
   const int t =  menuAction - QEPlotterNames::PLOTTER_FIRST;

   QAction* action = NULL;

   if ((t >= 0) && (t < ARRAY_LENGTH (this->actionList))) {
      action = new QAction (caption + " ", parent);
      action->setCheckable (checkable);
      action->setData (QVariant (int (menuAction)));
      parent->addAction (action);

      this->actionList [t] = action;
   }
   return action;
}

//------------------------------------------------------------------------------
//
void QEPlotterMenu::contextMenuTriggered (QAction* selectedItem)
{
   bool okay;
   QEPlotterNames::MenuActions action;

   action = QEPlotterNames::MenuActions (selectedItem->data ().toInt (&okay));

   // Neither first nor last are actual actions.
   //
   if (okay && (action > QEPlotterNames::PLOTTER_FIRST) &&
               (action < QEPlotterNames::PLOTTER_LAST)) {
      emit this->selected (action, this->slot);
   }
}

// end