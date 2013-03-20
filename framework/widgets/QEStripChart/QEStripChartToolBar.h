/*  QEStripChartToolBar.h
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

#ifndef QESTRIPCHARTTOOLBAR_H
#define QESTRIPCHARTTOOLBAR_H

#include <QAction>
#include <QFrame>
#include <QObject>
#include <QWidget>

#include "QEStripChartNames.h"


/// This class holds all the StripChart tool bar widgets.
class QEStripChartToolBar : public QFrame {
Q_OBJECT
public:  
  explicit QEStripChartToolBar (QWidget *parent = 0);
  virtual ~QEStripChartToolBar ();

  void setStateSelectionEnabled (const QEStripChartNames::StateModes mode, const bool enabled);
  
signals:
   void stateSelected (const QEStripChartNames::StateModes mode);
   void videoModeSelected (const QEStripChartNames::VideoModes mode);
   void yScaleModeSelected (const QEStripChartNames::YScaleModes mode);
   void yRangeSelected (const QEStripChartNames::ChartYRanges scale);
   void durationSelected (const int seconds);
   void playModeSelected (const QEStripChartNames::PlayModes mode);
   void readArchiveSelected ();

protected:
   void resizeEvent (QResizeEvent * event);

private:
   // Internal widgets are held on ownWidgets. If these items are declared at
   // class level, there is a run time exception. I think the SDK moc file
   // generation gets very confused.
   //
   class OwnWidgets;
   OwnWidgets *ownWidgets;

private slots:

   void durationClicked (QAction *action);

   void prevStateClicked (bool checked = false);
   void nextStateClicked (bool checked = false);
   void normalVideoClicked (bool checked = false);
   void reverseVideoClicked (bool checked = false);
   void linearScaleClicked (bool checked = false);
   void logScaleClicked (bool checked = false);

   void manualYScaleClicked (bool checked = false);
   void automaticYScaleClicked (bool checked = false);
   void plottedYScaleClicked (bool checked = false);
   void bufferedYScaleClicked (bool checked = false);
   void dynamicYScaleClicked (bool checked = false);
   void normalisedYScaleClicked (bool checked = false);

   void playClicked (bool checked = false);
   void pauseClicked (bool checked = false);
   void forwardClicked (bool checked = false);
   void backwardClicked (bool checked = false);
   void selectTimeClicked (bool checked = false);
   void readArchiveClicked (bool checked = false);
};

#endif  // QESTRIPCHARTTOOLBAR_H
