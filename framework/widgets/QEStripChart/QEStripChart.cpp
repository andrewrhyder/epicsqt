/*  QEStripChart.cpp
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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <math.h>

#include <QBoxLayout>
#include <QBrush>
#include <QCursor>
#include <QDebug>
#include <QDockWidget>
#include <QFont>
#include <QIcon>
#include <QLabel>
#include <QList>
#include <QMenu>
#include <QPen>
#include <QPushButton>
#include <QScrollArea>
#include <QStringList>
#include <QToolButton>

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>

#include <alarm.h>

#include <QCaObject.h>
#include <QELabel.h>
#include <QCaVariableNamePropertyManager.h>
#include <QEResizeableFrame.h>


#include "QEStripChart.h"
#include "QEStripChartToolBar.h"
#include "QEStripChartItem.h"


#define MAX(a, b)           ((a) >= (b) ? (a) : (b))
#define MIN(a, b)           ((a) <= (b) ? (a) : (b))
#define LIMIT(x,low,high)   (MAX(low, MIN(x, high)))

#define DEBUG  qDebug () << __FILE__  << "::" << __FUNCTION__ << ":" << __LINE__


static const QColor clWhite (0xFF, 0xFF, 0xFF, 0xFF);
static const QColor clBlack (0x00, 0x00, 0x00, 0xFF);

// Chart time mode options.
//
enum ChartTimeMode {
   tmRealTime,
   tmPaused
};

#define PV_DELTA_HEIGHT    18

#define PV_FRAME_HEIGHT    (6 + (NUMBER_OF_PVS / 2) * PV_DELTA_HEIGHT)
#define PV_SCROLL_HEIGHT   (PV_FRAME_HEIGHT + 6)

// Hold the copy of the chart configurations
//
struct ChartState {
   bool isNormalVideo;
   bool isLinearScale;
   QEStripChartNames::ChartYRanges chartYScale;
   double yMinimum;
   double yMaximum;

   ChartTimeMode chartTimeMode;
   int duration;
   QDateTime endDateTime;
};

#define MAXIMUM_CHART_STATES   40

typedef QList<ChartState>  ChartStateList;


//==============================================================================
// Local support classes.
//==============================================================================
//
// The imperative to create this class is to hold references to created QWidgets.
// If this are declared directly in the header, either none of the widget defined
// in the plugin are visible in designer or designer seg faults. I think the moc
// file generation and other Qt SDK stuff gets very confused.
//
// But given that the class does it exist it is a convient place holder for
// some additional data and associated functions. The placement of there artefacts
// here (PrivateData) or in the main widget (QEStripChart) is somewhat arbitary.
//
class QEStripChart::PrivateData : public QObject {
public:
   // Constructor
   //
   PrivateData (QEStripChart *chartIn);
   ~PrivateData ();
   QEStripChartItem * getItem (unsigned int slot);
   QwtPlotCurve *allocateCurve ();
   void calcDisplayMinMax ();
   void plotData ();
   void setReadOut (const QString & text);
   void setNormalBackground (bool state);
   QEStripChartNames::ChartYRanges chartYScale;
   enum ChartTimeMode chartTimeMode;
   bool isLinearScale;              // false implies isLogScale
   double timeScale;                // 1 => units are seconds, 60 => x units are minutes, etc.
   QString timeUnits;
   void pushState ();
   void prevState ();
   void nextState ();

protected:
   bool eventFilter (QObject *obj, QEvent *event);

private:
   QEStripChart *chart;
   QEStripChartToolBar *toolBar;
   QEResizeableFrame *toolBarResize;

   QFrame *pvFrame;
   QScrollArea *pvScrollArea;
   QEResizeableFrame *pvResizeFrame;

   QwtPlot *plot;
   QFrame *plotFrame;

   QVBoxLayout *layout1;
   QVBoxLayout *layout2;

   QLabel *pvNames [NUMBER_OF_PVS];
   QELabel *caLabels [NUMBER_OF_PVS];
   QEStripChartItem *items [NUMBER_OF_PVS];

   ChartStateList chartStateList;
   int chartStatePointer;
   void applyState (const ChartState & chartState);

   bool isNormalVideo;
   QVector<QwtPlotCurve *> curve_list;
   void releaseCurves ();
   void onCanvasMouseMove (QMouseEvent * event);
   static double selectStep (const double step);
};

//------------------------------------------------------------------------------
//
QEStripChart::PrivateData::PrivateData (QEStripChart *chartIn) : QObject (chartIn)
{
   unsigned int slot;
   int x, y;

   this->chart = chartIn;

   // Create tool bar frame and tool buttons.
   //
   this->toolBar = new QEStripChartToolBar (); // this will become parented by toolBarResize

   // Connect various tool bar signals to the chart.
   //
   QObject::connect (this->toolBar, SIGNAL (stateSelected  (const QEStripChartNames::StateModes)),
                     this->chart,   SLOT   (stateSelected  (const QEStripChartNames::StateModes)));

   QObject::connect (this->toolBar, SIGNAL (videoModeSelected  (const QEStripChartNames::VideoModes)),
                     this->chart,   SLOT   (videoModeSelected  (const QEStripChartNames::VideoModes)));

   QObject::connect (this->toolBar, SIGNAL (yScaleModeSelected  (const QEStripChartNames::YScaleModes)),
                     this->chart,   SLOT   (yScaleModeSelected  (const QEStripChartNames::YScaleModes)));

   QObject::connect (this->toolBar, SIGNAL (yRangeSelected  (const QEStripChartNames::ChartYRanges)),
                     this->chart,   SLOT   (yRangeSelected  (const QEStripChartNames::ChartYRanges)));

   QObject::connect (this->toolBar, SIGNAL (durationSelected  (const int)),
                     this->chart,   SLOT   (durationSelected  (const int)));

   QObject::connect (this->toolBar, SIGNAL (playModeSelected  (const QEStripChartNames::PlayModes)),
                     this->chart,   SLOT   (playModeSelected  (const QEStripChartNames::PlayModes)));

   QObject::connect (this->toolBar, SIGNAL (readArchiveSelected  ()),
                     this->chart,   SLOT   (readArchiveSelected  ()));


   // Create user controllable resize area
   //
   this->toolBarResize = new QEResizeableFrame (8, 38, this->chart);
   this->toolBarResize->setFixedHeight (38);
   this->toolBarResize->setFrameShape (QFrame::Panel);
   this->toolBarResize->setGrabberToolTip ("Re size tool bar display area");
   this->toolBarResize->setWidget (this->toolBar);

   // Create PV frame and PV name labels and associated CA labels.
   //
   this->pvFrame = new QFrame ();  // this will become parented by pvScrollArea
   this->pvFrame->setFixedHeight (PV_FRAME_HEIGHT);

   // Create widgets (parented by chart) and chart ittem that manages these.
   //
   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {

      this->pvNames [slot] = new QLabel (this->pvFrame);
      this->caLabels [slot] = new QELabel (this->pvFrame);

      x = 6 + (slot % 2) * 492;
      y = 6 + (slot / 2) * PV_DELTA_HEIGHT;

      this->pvNames [slot]->setGeometry   (x, y, 344, 15); x += 348;
      this->caLabels [slot]->setGeometry  (x, y, 128, 15);

      this->items [slot] = new QEStripChartItem (this->chart,
                                                 this->pvNames [slot],
                                                 this->caLabels [slot],
                                                 slot);
   }

   // Create scrolling area and add pv frame.
   //
   this->pvScrollArea = new QScrollArea ();          // this will become parented by pvResizeFrame
   this->pvScrollArea->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOn);
   this->pvScrollArea->setWidgetResizable (true);    // MOST IMPORTANT
   this->pvScrollArea->setWidget (this->pvFrame);

   // Create user controllable resize area
   //
   this->pvResizeFrame = new QEResizeableFrame (12, PV_SCROLL_HEIGHT + 8, this->chart);
   this->pvResizeFrame->setFixedHeight (PV_SCROLL_HEIGHT + 8);
   this->pvResizeFrame->setFrameShape (QFrame::Panel);
   this->pvResizeFrame->setGrabberToolTip ("Re size PV display area");
   this->pvResizeFrame->setWidget (this->pvScrollArea);

   // Create plotting frame and plot area.
   //
   this->plotFrame = new QFrame (this->chart);
   this->plotFrame->setFrameShape (QFrame::Panel);

   this->plot = new QwtPlot (this->plotFrame);
   this->plot->setCanvasLineWidth (1);
   this->plot->setLineWidth (1);

   this->plot->canvas()->setMouseTracking (true);
   // Use the privateData object as the event filter object.
   this->plot->canvas()->installEventFilter (this);

   // Create layouts.
   //
   this->layout1 = new QVBoxLayout (this->chart);
   this->layout1->setMargin (4);
   this->layout1->setSpacing (4);
   this->layout1->addWidget (this->toolBarResize);
   this->layout1->addWidget (this->pvResizeFrame);
   this->layout1->addWidget (this->plotFrame);

   this->layout2 = new QVBoxLayout (this->plotFrame);
   this->layout2->setMargin (4);
   this->layout2->setSpacing (4);
   this->layout2->addWidget (this->plot);

   // Clear / initialise plot.
   //
   this->chartYScale = QEStripChartNames::manual;
   this->chartTimeMode = tmRealTime;
   this->isLinearScale = true;
   this->timeScale = 1.0;
   this->timeUnits = "secs";

   this->chartStateList.clear ();
   this->chartStatePointer = 0;
   setNormalBackground (true);
   this->pushState ();        // baseline state - there is always at leasts one.
}

//------------------------------------------------------------------------------
//
QEStripChart::PrivateData::~PrivateData ()
{
   this->releaseCurves ();

   // all the created QWidget are (indirectly) parented by the PrivateData,
   // they are automatically deleted.
}

//------------------------------------------------------------------------------
//
QEStripChartItem * QEStripChart::PrivateData::getItem (unsigned int slot)
{
   return (slot < NUMBER_OF_PVS) ? this->items [slot] : NULL;
}

void QEStripChart::PrivateData::setNormalBackground (bool isNormalVideoIn)
{
   QColor background;

   this->isNormalVideo = isNormalVideoIn;

   background = this->isNormalVideo ? clWhite : clBlack;
#if QWT_VERSION >= 0x060000
   this->plot->setCanvasBackground (QBrush (background));
#else
   this->plot->setCanvasBackground (background);
#endif
   this->plotData ();
}

//------------------------------------------------------------------------------
//
QwtPlotCurve * QEStripChart::PrivateData::allocateCurve ()
{
   QwtPlotCurve * result = NULL;

   result = new QwtPlotCurve ();
   result->attach (this->plot);

   this->curve_list.append (result);
   return result;
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::releaseCurves ()
{
   int j;
   QwtPlotCurve *curve;

   for (j = 0; j < this->curve_list.count(); j++) {
      curve = this->curve_list.value (j);
      curve->detach ();
      delete curve;
   }

   this->curve_list.clear ();
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::calcDisplayMinMax ()
{
   int slot;
   TrackRange tr;
   double min;
   double max;

   if (this->chartYScale == QEStripChartNames::manual) return;

   tr.clear ();

   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {

      QEStripChartItem * item = this->getItem (slot);
      if (item->isInUse() == true) {
         switch (this->chartYScale) {
         case QEStripChartNames::operatingRange:  tr.merge (item->getLoprHopr (true));         break;
         case QEStripChartNames::plotted:         tr.merge (item->getDisplayedMinMax (true));  break;
         case QEStripChartNames::buffered:        tr.merge (item->getBufferedMinMax (true));   break;
         case QEStripChartNames::dynamic:         tr.merge (item->getDisplayedMinMax (true));  break;
         default:       DEBUG << "Well this is unexpected"; return; break;
         }
      }
   }

   if (tr.getMinMax (min, max) == true) {
      this->chart->yMinimum = min;
      this->chart->yMaximum = MAX (max, min + 1.0E-3);
   } // else do not change.
}

//------------------------------------------------------------------------------
//
double QEStripChart::PrivateData::selectStep (const double step)
{
   static const double min_step = 1.0E-10;
   static const double max_step = 1.0E+20;
   double result;
   double im;
   double m;

   result = max_step;

   // We use 1.001 in case of any rounding errors
   // It's more accurate if m starts big and is divided by 10 to get smaller.
   //
   for (im = 1.0/min_step; im > 1.001/max_step; im = im / 10.0) {
      m = 1.0/im;

      if (step <= 1.0 * m) {
         result = 1.0 * m; break;
      }
      if (step <= 2.0 * m) {
         result = 2.0 * m; break;
      }
      if (step <= 5.0 * m) {
         result = 5.0 * m; break;
      }
   }
   return result;
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::plotData ()
{
   unsigned int slot;
   double d;
   double ylo, yhi;
   double step;
   QString format;
   QString times;

   d = this->chart->getDuration ();
   if (d <= 1.0) {
      this->timeScale = 0.001;
      this->timeUnits = "mS";
   } else if (d <= 60.0) {
      this->timeScale = 1.0;
      this->timeUnits = "secs";
   } else if (d <= 3600.0) {
      this->timeScale = 60.0;
      this->timeUnits = "mins";
   } else if (d <= 86400.0) {
      this->timeScale = 3600.0;
      this->timeUnits = "hrs";
   } else {
      this->timeScale = 86400.0;
      this->timeUnits = "days";
   }

   this->plot->setAxisScale (QwtPlot::xBottom, -d/this->timeScale, 0.0, (d/this->timeScale)/5.0);

   // Update the plot
   // Allocate curve and call curve-setSample/setData.
   //
   this->releaseCurves ();
   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
      if (this->getItem (slot)->isInUse ()) {
          this->getItem (slot)->plotData (this->timeScale, this->isLinearScale);
      }
   }

   if (this->chartYScale == QEStripChartNames::dynamic) {
      // Re-calculate chart range.
      //
      this->calcDisplayMinMax ();
   }

   if (this->isLinearScale) {
      step = (this->chart->getYMaximum () - this->chart->getYMinimum ())/5.0;
      step = this->selectStep (step);
      ylo = this->chart->getYMinimum ();
      yhi =this->chart->getYMaximum ();
   } else {
      // therefore log scale
      step = 1.0;
      ylo = floor (LOG10 (this->chart->getYMinimum ()));
      yhi = ceil  (LOG10 (this->chart->getYMaximum ()));
   }
   this->plot->setAxisScale (QwtPlot::yLeft, ylo, yhi, step);
   this->plot->replot ();

   format = "yyyy-MM-dd hh:mm:ss";
   times = " ";
   times.append (this->chart->getStartDateTime().toUTC().toString (format));
   times.append (" UTC");
   times.append (" to ");
   times.append (this->chart->getEndDateTime().toUTC().toString (format));
   times.append (" UTC");

   // set on tool bar
   this->toolBar->setTimeStatus (times);
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::setReadOut (const QString & text)
{
   this->chart->sendMessage (text,
                             message_types (MESSAGE_TYPE_INFO, MESSAGE_KIND_STATUS_BAR));
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::onCanvasMouseMove (QMouseEvent * event)
{
   double x;
   double y;
   QDateTime t;
   QString format;
   QString mouseReadOut;
   QString f;

   // Convert pixel (x, y) to plot values (x, y)
   //
   x = this->plot->invTransform(QwtPlot::xBottom, event->x ());
   y = this->plot->invTransform(QwtPlot::yLeft,   event->y ());

   // Convert cursor x to absolute cursor time.
   // x is the time (in seconds/minutes/hours) relative to the chart end time.
   //
   t = this->chart->getEndDateTime ().toUTC ().addMSecs ((qint64)(1000.0 * x * this->timeScale));

   // Raise 10^y if needs be
   //
   if (this->isLinearScale == false) {
       y = EXP10 (y);
   }
   // Keep only most significant digit of the milli-seconds,
   // i.e. tenths of a second.
   //
   format = "yyyy-MM-dd hh:mm:ss.zzz";
   mouseReadOut = t.toString (format).left (format.length() - 2);
   mouseReadOut.append (" UTC");

   f.sprintf (" %12.1f ", x);
   mouseReadOut.append (f);
   mouseReadOut.append (this->timeUnits);

   f.sprintf ("    %+.10g", y);
   mouseReadOut.append (f);

   this->setReadOut (mouseReadOut);
}

//------------------------------------------------------------------------------
//
bool QEStripChart::PrivateData::eventFilter (QObject *obj, QEvent *event)
{
   QMouseEvent * mouseEvent = NULL;

   // case on type first else we get a seg fault.
   //
   switch (event->type ()) {

      case QEvent::MouseMove:
         mouseEvent = static_cast<QMouseEvent *> (event);

         if (obj == this->plot->canvas ()) {
            this->onCanvasMouseMove (mouseEvent);
            return true;
         }

      default:
         // Just fall through
         break;
   }

   // standard event processing
   //
   return QObject::eventFilter (obj, event);
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::applyState (const ChartState & chartState)
{
    this->setNormalBackground (chartState.isNormalVideo);
    this->isLinearScale = chartState.isLinearScale;
    this->chartYScale = chartState.chartYScale;
    this->chart->setYRange (chartState.yMinimum, chartState.yMaximum);
    this->chartTimeMode =  chartState.chartTimeMode;
    this->chart->setEndDateTime (chartState.endDateTime);
    this->chart->setDuration (chartState.duration);
    this->plotData ();
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::pushState ()
{
   ChartState chartState;

   // Capture current state.
   //
   chartState.isNormalVideo = this->isNormalVideo;
   chartState.isLinearScale = this->isLinearScale;
   chartState.chartYScale = this->chartYScale;
   chartState.yMinimum = this->chart->getYMinimum ();
   chartState.yMaximum = this->chart->getYMaximum ();
   chartState.chartTimeMode = this->chartTimeMode;
   chartState.duration = this->chart->getDuration ();
   chartState.endDateTime = this->chart->getEndDateTime ();

   // New state - all potential next states are lost.
   //
   while (this->chartStateList.count () > this->chartStatePointer) {
      this->chartStateList.removeLast ();
   }

   this->chartStateList.append (chartState);

   // Don't let this list get tooo big.
   //
   if (this->chartStateList.count () > MAXIMUM_CHART_STATES) {
       this->chartStateList.removeFirst ();  // remove oldest
   }
   this->chartStatePointer = this->chartStateList.count ();

   // Enable/disble buttons according to availability.
   //
   this->toolBar->setStateSelectionEnabled (QEStripChartNames::previous, (this->chartStatePointer > 1));
   this->toolBar->setStateSelectionEnabled (QEStripChartNames::next,     (this->chartStatePointer < this->chartStateList.count ()));
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::prevState ()
{
   ChartState chartState;

   if (this->chartStateList.count () < 1) return;    // sanity check.

   // Ensure in range.
   //
   this->chartStatePointer = LIMIT (this->chartStatePointer, 1, this->chartStateList.count ());

   if (this->chartStatePointer > 1) {
      this->chartStatePointer--;
      chartState = this->chartStateList.value (this->chartStatePointer - 1);  // zero indexed
      this->applyState (chartState);
   }

   // Enable/disble buttons according to availability.
   //
   this->toolBar->setStateSelectionEnabled (QEStripChartNames::previous, (this->chartStatePointer > 1));
   this->toolBar->setStateSelectionEnabled (QEStripChartNames::next,     (this->chartStatePointer < this->chartStateList.count ()));
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::nextState ()
{
   ChartState chartState;

   if (this->chartStateList.count () < 1) return; // sanity check.

   // Ensure in range.
   //
   this->chartStatePointer = LIMIT (this->chartStatePointer, 1, this->chartStateList.count ());

   if (this->chartStatePointer < this->chartStateList.count ()) {
      this->chartStatePointer++;
      chartState = this->chartStateList.value (this->chartStatePointer - 1);  // zero indexed
      this->applyState (chartState);
   }

   // Enable/disble buttons according to availability.
   //
   this->toolBar->setStateSelectionEnabled (QEStripChartNames::previous, (this->chartStatePointer > 1));
   this->toolBar->setStateSelectionEnabled (QEStripChartNames::next,     (this->chartStatePointer < this->chartStateList.count ()));
}


//==============================================================================
// QEStripChart class functions
//==============================================================================
//
QEStripChart::QEStripChart (QWidget * parent) : QFrame (parent), QEWidget (this)
{
   // Configure the panel and create contents
   //
   this->setFrameShape (Panel);
   this->setMinimumSize (1032, 400);

   this->duration = 600;  // ten minites.

   // We always use UTC (EPICS) time within the strip chart.
   // Set directly here as using setEndTime has side effects.
   //
   this->endDateTime = QDateTime::currentDateTime ().toUTC ();

   this->yMinimum = 0.0;
   this->yMaximum = 100.0;

   // construct private data for this chart.
   //
   this->privateData = new PrivateData (this);

   // Refresh the stip chart at 1Hz.
   //
   this->tickTimer = new QTimer (this);
   connect (this->tickTimer, SIGNAL (timeout ()), this, SLOT (tickTimeout ()));
   this->tickTimer->start (1000);  // mSec = 1.0 s

   // Enable drag drop onto this widget.
   //
   this->evaluateAllowDrop ();

   // Use default context menu (for now).
   //
   this->setupContextMenu (this);

   // OR: override contextMenuEvent to deal of context calls.
   //     this->setContextMenuPolicy (Qt::DefaultContextMenu);
   // OR: set up signal/slot
   //     this->setContextMenuPolicy (Qt::CustomContextMenu);
}

//------------------------------------------------------------------------------
//
QEStripChart::~QEStripChart ()
{
   // privateData is a QObject parented by this, so it is automatically deleted.
}

//------------------------------------------------------------------------------
//
QSize QEStripChart::sizeHint () const {
   return QSize (1000, 400);
}

//------------------------------------------------------------------------------
//
void QEStripChart::setVariableNameProperty (unsigned int slot, QString pvName)
{
   if (slot < NUMBER_OF_PVS) {
      QEStripChartItem * item = this->privateData->getItem (slot);
      item->pvNameProperyManager.setVariableNameProperty (pvName);
   } else {
      DEBUG << "slot out of range " << slot;
   }
}

//------------------------------------------------------------------------------
//
QString QEStripChart::getVariableNameProperty (unsigned int slot)
{
   if (slot < NUMBER_OF_PVS) {
      QEStripChartItem * item = this->privateData->getItem (slot);
      return item->pvNameProperyManager.getVariableNameProperty ();
   } else {
      DEBUG << "slot out of range " << slot;
      return "";
   }
}

//------------------------------------------------------------------------------
//
void QEStripChart::setColourProperty (unsigned int slot, QColor colour)
{
   if (slot < NUMBER_OF_PVS) {
      QEStripChartItem * item = this->privateData->getItem (slot);
      item->setColour (colour);
   } else {
      DEBUG << "slot out of range " << slot;
   }
}

//------------------------------------------------------------------------------
//
QColor QEStripChart::getColourProperty (unsigned int slot)
{
   if (slot < NUMBER_OF_PVS) {
      QEStripChartItem * item = this->privateData->getItem (slot);
      return item->getColour ();
   } else {
      DEBUG << "slot out of range " << slot;
      return QColor (0x00, 0x00, 0x00, 0xFF);
   }
}

//------------------------------------------------------------------------------
//
void QEStripChart::addPvName (QString pvName)
{
   unsigned int slot;

   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
      QEStripChartItem * item = this->privateData->getItem (slot);
      if (item->isInUse() == false) {
         // Found an empty slot.
         //
         item->setPvName (pvName, "");
         break;
      }
   }

   // Determine if we are now full.
   //
   this->evaluateAllowDrop ();
}

//------------------------------------------------------------------------------
//
void QEStripChart::addPvNameSet (QString pvNameSet)
{
   QStringList pvNameList;
   int j;

   // Split input string using space as delimiter.
   // Could extend to use regular expression and split on any white space character,
   // and or comma, and or semi colon.
   //
   pvNameList = pvNameSet.split (' ', QString::SkipEmptyParts);
   for (j = 0; j < pvNameList.count (); j++) {
      this->addPvName (pvNameList.value (j));
   }
}

//------------------------------------------------------------------------------
//
void QEStripChart::tickTimeout ()
{
   if (this->privateData->chartTimeMode == tmRealTime) {
      // Note: when end time changes - setEndTime calls plotData ().
      this->setEndDateTime (QDateTime::currentDateTime ());
   } else {
      this->privateData->plotData ();
   }
}

//=============================================================================
// Handle toolbar signals
//
void QEStripChart::stateSelected (const QEStripChartNames::StateModes mode)
{
   if (mode == QEStripChartNames::previous) {
      this->privateData->prevState ();
   } else {
      this->privateData->nextState ();
   }

   this->privateData->plotData ();
}

//------------------------------------------------------------------------------
//
void QEStripChart::videoModeSelected (const QEStripChartNames::VideoModes mode)
{
   this->privateData->setNormalBackground (mode == QEStripChartNames::normal);
   this->privateData->pushState ();
}

//------------------------------------------------------------------------------
//
void QEStripChart::yScaleModeSelected (const QEStripChartNames::YScaleModes mode)
{
   this->privateData->isLinearScale = (mode == QEStripChartNames::linear);
   this->privateData->plotData ();
   this->privateData->pushState ();
}


//------------------------------------------------------------------------------
//
void QEStripChart::yRangeSelected (const QEStripChartNames::ChartYRanges scale)
{
   int n;

   switch (scale) {
      case QEStripChartNames::manual:
         this->yRangeDialog.setRange (this->getYMinimum (), this->getYMaximum ());
         n = this->yRangeDialog.exec ();
         if (n == 1) {
            this->privateData->chartYScale = scale;
            // User has selected okay.
            //
            this->setYRange (this->yRangeDialog.getMinimum (),
                             this->yRangeDialog.getMaximum ());
         }
         this->privateData->pushState ();
         break;

      case QEStripChartNames::operatingRange:
      case QEStripChartNames::plotted:
      case QEStripChartNames::buffered:
      case QEStripChartNames::dynamic:
         this->privateData->chartYScale = scale;
         this->privateData->calcDisplayMinMax ();
         this->privateData->plotData ();
         this->privateData->pushState ();
         break;

      case QEStripChartNames::normalised:
         this->privateData->chartYScale = scale;
         this->setYRange (0.0, 100.0);
         this->privateData->pushState ();
         // TODO - set each PVs m, d and c parameters to map operating range to 0 .. 100
         break;

      default:
         DEBUG << "Well this is unexpected:" << (int) scale;
         break;
   }
}

//------------------------------------------------------------------------------
//
void QEStripChart::durationSelected (const int seconds)
{
   this->setDuration (seconds);
   this->privateData->pushState ();
}

//------------------------------------------------------------------------------
//
void QEStripChart::playModeSelected (const QEStripChartNames::PlayModes mode)
{
   int n;
   int d;

   switch (mode) {

      case QEStripChartNames::play:
         this->privateData->chartTimeMode = tmRealTime;
         // Note: using setEndTime causes a replot.
         this->setEndDateTime (QDateTime::currentDateTime ());
         this->privateData->pushState ();
         break;

      case QEStripChartNames::pause:
         this->privateData->chartTimeMode = tmPaused;
         this->privateData->pushState ();
         break;

      case QEStripChartNames::forward:
         this->privateData->chartTimeMode = tmPaused;
         this->setEndDateTime (this->getEndDateTime ().addSecs (+this->duration));
         this->privateData->pushState ();
         break;

      case QEStripChartNames::backward:
         this->privateData->chartTimeMode = tmPaused;
         this->setEndDateTime (this->getEndDateTime ().addSecs (-this->duration));
         this->privateData->pushState ();
         break;

      case QEStripChartNames::selectTimes:
         this->timeDialog.setMaximumDateTime (QDateTime::currentDateTime ().toUTC ());
         this->timeDialog.setStartDateTime (this->getStartDateTime());
         this->timeDialog.setEndDateTime (this->getEndDateTime());

         n = this->timeDialog.exec ();
         if (n == 1) {
            // User has selected okay.
            //
            this->privateData->chartTimeMode = tmPaused;
            this->setEndDateTime (this->timeDialog.getEndDateTime ());

            // We use the possibly limited chart end time in order to calculate the
            // duration.
            //
            d = this->timeDialog.getStartDateTime ().secsTo (this->getEndDateTime());
            this->setDuration (d);
            this->privateData->pushState ();
         }
         break;
   }
}

//------------------------------------------------------------------------------
//
void QEStripChart::readArchiveSelected ()
{
   unsigned int slot;

   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
      QEStripChartItem * item = this->privateData->getItem (slot);
      if (item->isInUse ()) {
         item->readArchive ();
      }
   }
}
//
// end of tool bar handlers ====================================================

//------------------------------------------------------------------------------
//
void QEStripChart::plotData ()
{
   this->privateData->plotData ();
}

//------------------------------------------------------------------------------
// Start/end time
//
QDateTime QEStripChart::getStartDateTime ()
{
   return this->getEndDateTime().addSecs (-this->duration);
}

//------------------------------------------------------------------------------
//
QDateTime QEStripChart::getEndDateTime ()
{
   return this->endDateTime;
}

//------------------------------------------------------------------------------
//
void QEStripChart::setEndDateTime (QDateTime endDateTimeIn)
{
   QDateTime useUTC = endDateTimeIn.toUTC ();
   QDateTime nowUTC = QDateTime::currentDateTime ().toUTC ();

   // No peeking into the future.
   //
   if (useUTC > nowUTC) {
      useUTC = nowUTC;
   }

   if (this->endDateTime != useUTC) {
      this->endDateTime = useUTC;
      this->privateData->plotData ();
   }
}

//------------------------------------------------------------------------------
//
int QEStripChart::getDuration ()
{
   return this->duration;
}

//------------------------------------------------------------------------------
//
void QEStripChart::setDuration (int durationIn)
{
   // A duration of less than 1 second is not allowed.
   //
   if (durationIn < 1) {
      durationIn = 1;
   }

   if (this->duration != durationIn) {
      this->duration = durationIn;
      this->privateData->plotData ();
   }
}

//----------------------------------------------------------------------------
//
double QEStripChart::getYMinimum ()
{
   return this->yMinimum;
}

//----------------------------------------------------------------------------
//
void QEStripChart::setYMinimum (const double yMinimumIn)
{
   this->yMinimum = yMinimumIn;
   this->yMaximum = MAX (this->yMaximum, this->yMinimum + 1.0E-3);
   this->privateData->chartYScale = QEStripChartNames::manual;
   this->privateData->plotData ();
}

//----------------------------------------------------------------------------
//
double QEStripChart::getYMaximum ()
{
   return this->yMaximum;
}

//----------------------------------------------------------------------------
//
void QEStripChart::setYMaximum (const double yMaximumIn)
{
   this->yMaximum = yMaximumIn;
   this->yMinimum = MIN (this->yMinimum, this->yMaximum - 1.0E-3);
   this->privateData->chartYScale = QEStripChartNames::manual;
   this->privateData->plotData ();
}

//----------------------------------------------------------------------------
//
void QEStripChart::setYRange (const double yMinimumIn, const double yMaximumIn)
{
    this->yMinimum = yMinimumIn;
    this->yMaximum = MAX (yMaximumIn, this->yMinimum + 1.0E-3);
    this->privateData->chartYScale = QEStripChartNames::manual;
    this->privateData->plotData ();
}


//----------------------------------------------------------------------------
//
void QEStripChart::setDrop (QVariant drop)
{
   // Use dropped text to add a PV(s) to the chart.
   //
   this->addPvNameSet (drop.toString());
}

//----------------------------------------------------------------------------
//
QVariant QEStripChart::getDrop ()
{
   if (this->isDraggingVariable ()) {
      return QVariant (this->copyVariable ());
   } else {
      return this->copyData ();
   }
}

//----------------------------------------------------------------------------
// Copy and paste
//
QString QEStripChart::copyVariable ()
{
   QString result;
   unsigned int slot;

   // Copy string delimited set of PV names.
   //
   result = "";
   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
      QEStripChartItem * item = this->privateData->getItem (slot);

      if ((item) && (item->isInUse () == false)) {
         if (!result.isEmpty()) {
            result = result.append (" ");
         };
         result = result.append (item->getPvName());
      }
   }
   return result;
}

//----------------------------------------------------------------------------
//
QVariant QEStripChart::copyData ()
{
   // Place holder for now.
   // How can we sensibley interpret this? Image?
   //
   return QVariant ("");
}

//----------------------------------------------------------------------------
//
void QEStripChart::paste (QVariant s)
{
   // Use pasted text to add PV(s) to the chart.
   //
   this->addPvNameSet (s.toString ());
}

//----------------------------------------------------------------------------
// Determine if user allowed to drop new PVs into this widget.
//
void QEStripChart::evaluateAllowDrop ()
{
   unsigned int slot;
   bool allowDrop;

   // Hypoyhesize that the strip chart is full.
   //
   allowDrop = false;
   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
      QEStripChartItem * item = this->privateData->getItem (slot);

      if ((item) && (item->isInUse () == false)) {
         // Found an empty slot.
         //
         allowDrop = true;
         break;
      }
   }

   this->setAcceptDrops (allowDrop);
}

//----------------------------------------------------------------------------
//
QwtPlotCurve * QEStripChart::allocateCurve () {
   return this->privateData->allocateCurve ();
}

//------------------------------------------------------------------------------
//
void QEStripChart::setup ()
{
   DEBUG << "unexpected call";
}

//------------------------------------------------------------------------------
//
qcaobject::QCaObject* QEStripChart::createQcaItem (unsigned int variableIndex)
{
   DEBUG << "unexpected call, variableIndex = " << variableIndex;
   return NULL;
}

//------------------------------------------------------------------------------
//
void QEStripChart::establishConnection (unsigned int variableIndex)
{
   DEBUG << "unexpected call, variableIndex = " << variableIndex;
}

// end
