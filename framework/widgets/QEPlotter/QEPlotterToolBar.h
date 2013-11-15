/*  QEPlotterToolBar.h
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
 *
 */

#ifndef QEPLOTTERTOOLBAR_H
#define QEPLOTTERTOOLBAR_H

//  ??? #include <qnamespace.h>
#include <QFrame>
#include <QPushButton>
#include <QObject>
#include <QWidget>

#include <QEStripChartNames.h>


/// This class holds all the QEPlotter tool bar widgets.
class QEPlotterToolBar : public QFrame {
Q_OBJECT
public:  
   explicit QEPlotterToolBar (QWidget *parent = 0);
   virtual ~QEPlotterToolBar ();
   static const int designHeight = 32;


   enum ToolBarOptions {
      TOOLBAR_NONE = 0,

      TOOLBAR_PREV,                // Previous state
      TOOLBAR_NEXT,                // Previous state

      TOOLBAR_NORMAL_VIDEO,        //
      TOOLBAR_REVERSE_VIDEO,       //

      TOOLBAR_LINEAR_Y_SCALE,      //
      TOOLBAR_LOG_Y_SCALE,         //

      TOOLBAR_MANUAL_Y_RANGE,      // User selected YMin YMax
      TOOLBAR_CURRENT_Y_RANGE,     // YMin/YMax based on overal min/max of current data set
      TOOLBAR_DYNAMIC_Y_RANGE,     // As TOOLBAR_CURRENT_Y_RANGE, but dynamic per update
      TOOLBAR_NORAMLISED_Y_RANGE,  // Range 0 to 1: Data mapped Min => 0, Max => 1
      TOOLBAR_FRACTIONAL_Y_RANGE,  // Range 0 to 1: Data mapped (value / Max)

      TOOLBAR_PLAY,                //
      TOOLBAR_PAUSE,               //

      TOOLBAR_NUMBER_ITEMS         // Must be last
   };

   void setEnabled (const ToolBarOptions option, const bool value);

signals:
   void selected (const QEPlotterToolBar::ToolBarOptions);


protected:
   void resizeEvent (QResizeEvent * event);

private:
   QPushButton *pushButtons [TOOLBAR_NUMBER_ITEMS];

private slots:

   void prevStateClicked (bool checked = false);
   void nextStateClicked (bool checked = false);

   void normalVideoClicked (bool checked = false);
   void reverseVideoClicked (bool checked = false);

   void linearScaleClicked (bool checked = false);
   void logScaleClicked (bool checked = false);

   void manualYScaleClicked (bool checked = false);
   void automaticYScaleClicked (bool checked = false);
   void dynamicYScaleClicked (bool checked = false);
   void normalisedYScaleClicked (bool checked = false);
   void fractionalYScaleClicked (bool checked = false);

   void playClicked (bool checked = false);
   void pauseClicked (bool checked = false);
};

#endif  // QEPLOTTERTOOLBAR_H
