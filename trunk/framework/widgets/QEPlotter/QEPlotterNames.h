/*  QEPlotterNames.h
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
 *  Copyright (c) 2013 Australian Synchrotron
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

// Provides common definitions of both conext menu and tool bar actions.
//
#ifndef QE_PLOTTER_NAMES_H
#define QE_PLOTTER_NAMES_H

#include <contextMenu.h>

// This is essentially a namespace
//
class QEPlotterNames {

public:

   QEPlotterNames () {}

   // IDs for all menu options
   // Each menu option has a unique ID across all plotter menus and toolbars
   // These IDs are in addition to standard context menu IDs and so start after
   // contextMenu::CM_SPECIFIC_WIDGETS_START_HERE
   //
   enum MenuActions {
      PLOTTER_FIRST = contextMenu::CM_SPECIFIC_WIDGETS_START_HERE,  // Must be first

      // General context menu items, many also available in the tool bar.
      //
      PLOTTER_SHOW_HIDE_TOOLBAR,   //
      PLOTTER_SHOW_HIDE_PV_ITEMS,  //
      PLOTTER_SHOW_HIDE_STATUS,    //

      PLOTTER_PREV,                // Previous state
      PLOTTER_NEXT,                // Previous state

      PLOTTER_NORMAL_VIDEO,        //
      PLOTTER_REVERSE_VIDEO,       //

      PLOTTER_LINEAR_Y_SCALE,      //
      PLOTTER_LOG_Y_SCALE,         //

      PLOTTER_MANUAL_Y_RANGE,      // User selected YMin YMax
      PLOTTER_CURRENT_Y_RANGE,     // YMin/YMax based on overal min/max of current data set
      PLOTTER_DYNAMIC_Y_RANGE,     // As PLOTTER_CURRENT_Y_RANGE, but dynamic per update
      PLOTTER_NORAMLISED_Y_RANGE,  // Range 0 to 1: Data mapped Min => 0, Max => 1
      PLOTTER_FRACTIONAL_Y_RANGE,  // Range 0 to 1: Data mapped (value / Max)

      PLOTTER_MANUAL_X_RANGE,      // User selected XMin XMax
      PLOTTER_CURRENT_X_RANGE,     // XMin/XMax based on overal min/max of current data set
      PLOTTER_DYNAMIC_X_RANGE,     // As PLOTTER_CURRENT_X_RANGE, but dynamic per update

      PLOTTER_PLAY,                //
      PLOTTER_PAUSE,               //

      // PV item context menu items.
      //
      PLOTTER_LINE_BOLD,           //
      PLOTTER_LINE_DOTS,           //
      PLOTTER_LINE_VISIBLE,        //
      PLOTTER_LINE_COLOUR,         //

      PLOTTER_PASTE_DATA_PV,       //
      PLOTTER_PASTE_SIZE_PV,       //
      PLOTTER_DATA_SELECT,         //
      PLOTTER_DATA_DIALOG,         //
      PLOTTER_DATA_CLEAR,          //

      PLOTTER_SCALE_TO_MIN_MAX,    //
      PLOTTER_SCALE_TO_ZERO_MAX,   //

      PLOTTER_LAST                 // Must be last
   };

};


#endif  // QE_PLOTTER_ACTIONS_H
