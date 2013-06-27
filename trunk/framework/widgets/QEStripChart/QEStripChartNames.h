/*  QEStripChartNames.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
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

#ifndef QESTRIPCHARTNAMES_H
#define QESTRIPCHARTNAMES_H

#include <QObject>
#include <contextMenu.h>

// Define strip chart specific names (enumerations)
// We use a QObject as opposed to a namespace as this allows us leverage off the
// meta object compiler output, specifically allows us to use the enumToString
// and stringToEnum functions in QEUtilities.
//
class QEStripChartNames : public QObject {
   Q_OBJECT
public:

   // Chart time mode options.
   //
   enum ChartTimeModes {
      tmRealTime,      //
      tmPaused,        //
      tmHistorical     //
   };

   Q_ENUMS (ChartTimeModes)

   enum ChartYRanges {
      manual,          // chart set manually via dialog
      operatingRange,  // i.e. HOPR/LOPR values
      plotted,         // what is currently displayed
      buffered,        // what is buffered - include waht is displayed and what of "off screen".
      dynamic,         // chart range adhjuested dynamically (based on plotted values)
      normalised       // chart range set to 0 .. 100, PVs scaled
   };

   Q_ENUMS (ChartYRanges)

   enum PlayModes {
      play,            // run chart real time
      pause,           // pause chart updates
      forward,         // move start/end time forards
      backward,        // move start/end time backwards
      selectTimes      // set start/end time via dialog
   };

   Q_ENUMS (PlayModes)

   enum StateModes {
      previous,         // previous state
      next              // next state
   };

   Q_ENUMS (StateModes)

   enum VideoModes {
      normal,          // white background
      reverse          // black backgound
   };

   Q_ENUMS (VideoModes)

   enum YScaleModes {
      linear,          // lineary Y scale
      log              // log Y scale - limited to -20
   };

   Q_ENUMS (YScaleModes)


   enum LineDrawModes {
      ldmHide,         // do not draw
      ldmRegular,      // normal draw
      ldmBold          // bold draw
   };

   Q_ENUMS (LineDrawModes)


   // IDs for all menu options
   // Each menu option has a unique ID across all menus
   // These IDs are in addition to standard context menu IDs and so start after
   // contextMenu::CM_SPECIFIC_WIDGETS_START_HERE
   //
   enum ContextMenuOptions {
      SCCM_NONE = contextMenu::CM_SPECIFIC_WIDGETS_START_HERE,

      // general chart items.
      //
      SCCM_COPY_PV_NAMES,
      SCCM_PASTE_PV_NAMES,

      // Item specific menu options.
      //
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
      SCCM_PREDEFINED_10
   };

   // These MUST be consistant with above declaration.
   //
   static const ContextMenuOptions ContextMenuItemFirst = SCCM_READ_ARCHIVE;
   static const ContextMenuOptions ContextMenuItemLast  = SCCM_PREDEFINED_10;
};

#endif   // QESTRIPCHARTNAMES_H

