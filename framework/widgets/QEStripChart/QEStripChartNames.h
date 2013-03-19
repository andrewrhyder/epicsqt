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

// Define strip chart specific names (enumerations)
//
namespace QEStripChartNames {

typedef enum {
   manual,          // chart set manually via dialog
   operatingRange,  // i.e. HOPR/LOPR values
   plotted,         // what is currently displayed
   buffered,        // what is buffered - include waht is displayed and what of "off screen".
   dynamic,         // chart range adhjuested dynamically (based on plotted values)
   normalised       // chart range set to 0 .. 100, PVs scaled
} ChartYRanges;


typedef enum {
   play,            // run chart real time
   pause,           // pause chart updates
   forward,         // move start/end time forards
   backward,        // move start/end time backwards
   selectTimes      // set start/end time via dialog
} PlayModes;

typedef enum {
   previous,         //
   next              //
} StateModes;


typedef enum {
   normal,          // white background
   reverse          // black backgound
} VideoModes;

typedef enum {
   linear,          // lineary Y scale
   log              // log Y scale - limited to -20
} YScaleModes;


typedef enum {
   shrink,          // Shrink widget/frame
   expand           // Expmand widget/frame
} SizeActions;

}

#endif   // QESTRIPCHARTNAMES_H

