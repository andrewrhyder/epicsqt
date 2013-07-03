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
#include <QMutex>
#include <QPen>
#include <QPointF>
#include <QPushButton>
#include <QRegExp>
#include <QScrollArea>
#include <QStringList>
#include <QToolButton>

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>

#include <alarm.h>

#include <QECommon.h>
#include <QCaObject.h>
#include <QELabel.h>
#include <QCaVariableNamePropertyManager.h>
#include <QEResizeableFrame.h>

#include "QEStripChart.h"
#include "QEStripChartToolBar.h"
#include "QEStripChartItem.h"
#include "QEStripChartUtilities.h"


#define DEBUG  qDebug () << "QEStripChart::" << __FUNCTION__ << ":" << __LINE__

static const QColor clWhite (0xFF, 0xFF, 0xFF, 0xFF);
static const QColor clBlack (0x00, 0x00, 0x00, 0xFF);

#define PV_DELTA_HEIGHT    18

#define PV_FRAME_HEIGHT    (6 + (NUMBER_OF_PVS / 2) * PV_DELTA_HEIGHT)
#define PV_SCROLL_HEIGHT   (PV_FRAME_HEIGHT + 6)

#define MAXIMUM_CHART_STATES   40


//==============================================================================
// Local support classes.
//==============================================================================
//
// Hold the copy of the chart configurations
//
class ChartState {
public:
   ChartState ();
   void saveConfiguration (PMElement & parentElement);
   void restoreConfiguration (PMElement & parentElement);

   bool isNormalVideo;
   QEStripChartNames::ChartTimeModes chartTimeMode;
   QEStripChartNames::YScaleModes  yScaleMode;
   QEStripChartNames::ChartYRanges chartYScale;
   double yMinimum;
   double yMaximum;
   int duration;
   Qt::TimeSpec timeZoneSpec;
   QDateTime endDateTime;
};

//------------------------------------------------------------------------------
//
ChartState::ChartState () {
   this->isNormalVideo = true;
}

//------------------------------------------------------------------------------
//
void ChartState::saveConfiguration (PMElement & parentElement)
{
   QEStripChartNames meta;

   PMElement stateElement = parentElement.addElement ("ChartState");

   stateElement.addValue ("IsNormalVideo", this->isNormalVideo);
   stateElement.addValue ("ChartTimeMode", QEUtilities::enumToString (meta, "ChartTimeModes",this->chartTimeMode));
   stateElement.addValue ("YScaleMode", QEUtilities::enumToString (meta, "YScaleModes", this->yScaleMode));
   stateElement.addValue ("ChartYScale", QEUtilities::enumToString (meta, "ChartYRanges", this->chartYScale));
   stateElement.addValue ("YMinimum", this->yMinimum);
   stateElement.addValue ("YMaximum", this->yMaximum);
   stateElement.addValue ("Duration", this->duration);
   stateElement.addValue ("TimeZoneSpec", (int) this->timeZoneSpec);

   // We use double here as toTime_t returns a uint (as opposed to an int).
   //
   stateElement.addValue ("EndDateTime", (double) this->endDateTime.toTime_t ());
}

//------------------------------------------------------------------------------
//
void ChartState::restoreConfiguration (PMElement & parentElement)
{
   QEStripChartNames meta;

   PMElement stateElement = parentElement.getElement ("ChartState");
   bool status;
   bool boolVal;
   int intVal;
   double doubleVal;
   QString stringVal;

   if (stateElement.isNull ()) return;

   status = stateElement.getValue ("IsNormalVideo", boolVal);
   if (status) {
      this->isNormalVideo = boolVal;
   }

   status = stateElement.getValue ("ChartTimeMode", stringVal);
   if (status) {
      intVal = QEUtilities::stringToEnum (meta, "ChartTimeModes", stringVal, &status);
      if (status) {
         this->chartTimeMode = (QEStripChartNames::ChartTimeModes) intVal;
      }
   }

   status = stateElement.getValue ("YScaleMode", stringVal);
   if (status) {
      intVal = QEUtilities::stringToEnum (meta, "YScaleModes", stringVal, &status);
      if (status) {
         this->yScaleMode = (QEStripChartNames::YScaleModes) intVal;
      }
   }

   status = stateElement.getValue ("ChartYScale", stringVal);
   if (status) {
      intVal = QEUtilities::stringToEnum (meta, "ChartYRanges", stringVal, &status);
      if (status) {
         this->chartYScale = (QEStripChartNames::ChartYRanges) intVal;
      }
   }

   status = stateElement.getValue ("YMinimum", this->yMinimum);

   status = stateElement.getValue ("YMaximum", this->yMaximum);

   status = stateElement.getValue ("Duration", this->duration);

   status = stateElement.getValue ("TimeZoneSpec", intVal);
   if (status) {
      this->timeZoneSpec = (Qt::TimeSpec) intVal;
   }

   status = stateElement.getValue ("EndDateTime", doubleVal);
   if (status) {
      this->endDateTime.setTime_t ((uint) doubleVal);
   }
}

//==============================================================================
//
class QEChartStateLists : public QList<ChartState> {
};

//==============================================================================
//
class QEPVNameLists : public QStringList {
public:
   explicit QEPVNameLists ();
   virtual ~QEPVNameLists ();

   void prependOrMoveToFirst (const QString & item);
   void saveConfiguration (PMElement & parentElement);
   void restoreConfiguration (PMElement & parentElement);
private:
   QMutex *mutex;
};

//------------------------------------------------------------------------------
//
QEPVNameLists::QEPVNameLists ()
{
   this->mutex = new QMutex ();
   // inititialise from container and/or environment variable?
}

//------------------------------------------------------------------------------
//
QEPVNameLists::~QEPVNameLists ()
{
   delete this->mutex;
}

//------------------------------------------------------------------------------
//
void QEPVNameLists::prependOrMoveToFirst (const QString & item)
{
   QMutexLocker locker (this->mutex);

   int posn;

   posn = this->indexOf (item, 0);
   if (posn < 0) {
      this->prepend (item);
   } else if (posn > 0) {
      // item in list - move to front.
      this->swap (0, posn);
   }  // else posn = 0 - nothing to do.

   if (this->count () > QEStripChartContextMenu::numberPrefefinedItems) {
      this->removeLast ();
   }
}

//------------------------------------------------------------------------------
//
void QEPVNameLists::saveConfiguration (PMElement & parentElement)
{
   PMElement predefinedElement = parentElement.addElement ("Predefined");
   int number;
   int j;

   number = this->count ();
   predefinedElement.addAttribute ("Number", number);
   for (j = 0; j < number; j++) {
      PMElement pvElement = predefinedElement.addElement ("PV");
      pvElement.addAttribute ("id", j);
      pvElement.addValue ("Name", this->value (j));
   }

}

//------------------------------------------------------------------------------
//
void QEPVNameLists::restoreConfiguration (PMElement & parentElement)
{
   PMElement predefinedElement = parentElement.getElement ("Predefined");
   int number;
   int j;
   QString pvName;
   bool status;

   if (predefinedElement.isNull ()) return;

   status = predefinedElement.getAttribute ("Number", number);
   if (status) {
      this->clear ();

      // Read in reverse order (as use insert into list with prependOrMoveToFirst).
      //
      for (j = number - 1; j >= 0; j--) {
         PMElement pvElement = predefinedElement.getElement ("PV", "id", j);

         if (pvElement.isNull ()) continue;

         status = pvElement.getValue ("Name", pvName);
         if (status) {
            this->prependOrMoveToFirst (pvName);
         }
      }
   }
}

//------------------------------------------------------------------------------
// This is a static list shared amongst all instances of the strip chart widget.
//
static QEPVNameLists predefinedPVNameList;


//==============================================================================
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

   void chartContextMenuRequested (const QPoint & pos);
   void nullContextMenuRequested (const QPoint & pos);
   void chartContextMenuTriggered (QAction* action);

   void itemContextMenuRequested (const unsigned int slot, const QPoint & pos);
   void itemContextMenuSelected (const unsigned int, const QEStripChartNames::ContextMenuOptions option);

   void pushState ();
   void prevState ();
   void nextState ();

   void captureState (ChartState& chartState);
   void applyState (const ChartState& chartState);

   QEStripChartNames::ChartYRanges chartYScale;
   QEStripChartNames::YScaleModes yScaleMode;
   QEStripChartNames::ChartTimeModes chartTimeMode;
   double timeScale;             // 1 => units are seconds, 60 => x units are minutes, etc.
   QString timeUnits;

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

   QMenu* chartContextMenu;
   QEStripChartContextMenu *inUseMenu;
   QEStripChartContextMenu *emptyMenu;

   QEChartStateLists chartStateList;
   int chartStatePointer;

   bool isNormalVideo;
   QwtPlotGrid *grid;

   // Mouse button pressed postions and flags
   QPoint plotCurrent;          // last known mouse position of the plot.
   QPoint plotLeftButton;       // point at which left button pressed.
   bool   plotLeftIsDefined;
   QPoint plotRightButton;      // point at which rightt button pressed.
   bool   plotRightIsDefined;

   // Keep a list of allocated curves so that we can track and delete them.
   //
   QVector<QwtPlotCurve *> curve_list;

   void releaseCurves ();
   QPointF plotToReal (const QPoint & pos) const; // map plot position to real co-ordinated
   void onCanvasMouseMove (QMouseEvent * event);
   static double selectStep (const double step);
   bool isValidYRangeSelection (const QPoint & origin, const QPoint & offset) const;
   bool isValidTRangeSelection (const QPoint & origin, const QPoint & offset) const;
   void onPlaneScaleSelect     (const QPoint & origin, const QPoint & offset);
};

//------------------------------------------------------------------------------
//
QEStripChart::PrivateData::PrivateData (QEStripChart *chartIn) : QObject (chartIn)
{
   unsigned int slot;
   int x, y;
   QAction* action;

   this->chart = chartIn;

   this->plotLeftIsDefined = false;
   this->plotRightIsDefined = false;

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

   QObject::connect (this->toolBar, SIGNAL (timeZoneSelected (const Qt::TimeSpec)),
                     this->chart,   SLOT   (timeZoneSelected (const Qt::TimeSpec)));

   QObject::connect (this->toolBar, SIGNAL (readArchiveSelected  ()),
                     this->chart,   SLOT   (readArchiveSelected  ()));


   // Create user controllable resize area
   //
   this->toolBarResize = new QEResizeableFrame (8, 8 + this->toolBar->designHeight, this->chart);
   this->toolBarResize->setFixedHeight (8 + this->toolBar->designHeight);
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
      QEStripChartItem * chartItem;

      this->pvNames [slot] = new QLabel (this->pvFrame);
      this->caLabels [slot] = new QELabel (this->pvFrame);

      x = 6 + (slot % 2) * 492;
      y = 6 + (slot / 2) * PV_DELTA_HEIGHT;

      this->pvNames [slot]->setGeometry   (x, y, 344, 15); x += 348;
      this->caLabels [slot]->setGeometry  (x, y, 128, 15);

      chartItem = new QEStripChartItem (this->chart,
                                        this->pvNames [slot],
                                        this->caLabels [slot],
                                        slot);

      this->items [slot] = chartItem;

      QObject::connect (chartItem,   SIGNAL (itemContextMenuRequested (const unsigned int, const QPoint &)),
                        this->chart, SLOT   (itemContextMenuRequested (const unsigned int, const QPoint &)));
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

   this->grid = new QwtPlotGrid ();
   this->grid->attach (this->plot);

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

   // Create/setup context menus.
   //
   // First do overall chart context menu
   //
   this->chartContextMenu = new QMenu (this->chart);

   action = new QAction ("Copy PV names", this->chartContextMenu);
   action->setCheckable (false);
   action->setData (QVariant (int (QEStripChartNames::SCCM_COPY_PV_NAMES)));
   this->chartContextMenu->addAction (action);

   action = new QAction ("Paste PV names", this->chartContextMenu);
   action->setCheckable (false);
   action->setData (QVariant (int (QEStripChartNames::SCCM_PASTE_PV_NAMES)));
   this->chartContextMenu->addAction (action);

   // Connect menu itself to menu handler.
   //
   QObject::connect (this->chartContextMenu, SIGNAL (triggered                 (QAction*)),
                     this->chart,            SLOT   (chartContextMenuTriggered (QAction*)));

   // Set up context menus.
   //
   this->chart->setContextMenuPolicy (Qt::CustomContextMenu);

   QObject::connect (this->chart, SIGNAL (customContextMenuRequested (const QPoint &)),
                     this->chart, SLOT   (chartContextMenuRequested  (const QPoint &)));

   // Don't want to do context menu over plot area - we use right-click for other stuff.
   //
   this->plotFrame->setContextMenuPolicy (Qt::CustomContextMenu);

   QObject::connect (this->plotFrame, SIGNAL (customContextMenuRequested (const QPoint &)),
                     this->chart,     SLOT   (nullContextMenuRequested   (const QPoint &)));



   // Chart item context menus.
   //
   this->inUseMenu = new QEStripChartContextMenu (true, this->chart);
   this->emptyMenu = new QEStripChartContextMenu (false, this->chart);

   // Connect the context menus
   //
   this->connect (this->inUseMenu, SIGNAL (contextMenuSelected     (const unsigned int, const QEStripChartNames::ContextMenuOptions)),
                  this->chart,     SLOT   (itemContextMenuSelected (const unsigned int, const QEStripChartNames::ContextMenuOptions)));

   this->connect (this->emptyMenu, SIGNAL (contextMenuSelected     (const unsigned int, const QEStripChartNames::ContextMenuOptions)),
                  this->chart,     SLOT   (itemContextMenuSelected (const unsigned int, const QEStripChartNames::ContextMenuOptions)));

   // Clear / initialise plot.
   //
   this->chartYScale = QEStripChartNames::manual;
   this->yScaleMode = QEStripChartNames::linear;
   this->chartTimeMode = QEStripChartNames::tmRealTime;
   this->timeScale = 1.0;
   this->timeUnits = "secs";

   this->chartStateList.clear ();
   this->chartStatePointer = 0;
   this->setNormalBackground (true);
   this->pushState ();        // baseline state - there is always at leasts one.
}

//------------------------------------------------------------------------------
//
QEStripChart::PrivateData::~PrivateData ()
{
   this->releaseCurves ();

   if (this->grid) {
      this->grid->detach();
      delete this->grid;
      this->grid  = NULL;
   }

   // All the created QWidget are parented by the QEStripChart and are automatically
   // deleted when the chart itself is deleted.
}

//------------------------------------------------------------------------------
//
QEStripChartItem * QEStripChart::PrivateData::getItem (unsigned int slot)
{
   return (slot < NUMBER_OF_PVS) ? this->items [slot] : NULL;
}


//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::chartContextMenuRequested (const QPoint & pos)
{
   QPoint golbalPos;

   golbalPos = this->chart->mapToGlobal (pos);
   this->chartContextMenu->exec (golbalPos, 0);
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::chartContextMenuTriggered (QAction* action)
{
   bool okay;
   QEStripChartNames::ContextMenuOptions option;
   QClipboard* cb;
   QString text;
   unsigned int slot;

   option = QEStripChartNames::ContextMenuOptions (action->data ().toInt (&okay));
   if (!okay) {
      return;
   }

   switch (option) {
      case QEStripChartNames::SCCM_COPY_PV_NAMES:
         // Create space delimited set of PV names.
         //
         text = "";
         for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
            QEStripChartItem * item = this->getItem (slot);

            if ((item) && (item->isInUse() == true)) {
               if (!text.isEmpty()) {
                  text.append (" ");
               };
               text.append (item->getPvName ());
            }
         }
         cb = QApplication::clipboard ();
         cb->setText (text);
         break;

      case QEStripChartNames::SCCM_PASTE_PV_NAMES:
         cb = QApplication::clipboard ();
         text = cb->text().trimmed();
         if (!text.isEmpty()) {
            this->chart->addPvNameSet (text);
         }
         break;

      default:
         // do nothing
         break;
   }
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::itemContextMenuRequested (const unsigned int slot, const QPoint & pos)
{
   QEStripChartItem* item = this->getItem (slot);
   QPoint tempPos;
   QPoint golbalPos;

   if (!item) return;   // sanity check

   tempPos = pos;
   tempPos.setY (2);   // align with top of label
   golbalPos = this->pvNames [slot]->mapToGlobal (tempPos);

   if (item->isInUse()) {
      this->inUseMenu->setUseReceiveTime (item->getUseReceiveTime ());
      this->inUseMenu->setArchiveReadHow (item->getArchiveReadHow ());
      this->inUseMenu->setLineDrawMode (item->getLineDrawMode ());
      this->inUseMenu->exec (slot, golbalPos, 0);
   } else {
      this->emptyMenu->setPredefinedNames (predefinedPVNameList);
      this->emptyMenu->exec (slot, golbalPos, 0);
   }
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::itemContextMenuSelected (const unsigned int slot,
                                                         const QEStripChartNames::ContextMenuOptions option)
{
   QEStripChartItem* item = this->getItem (slot);

   if (item) {
      item->contextMenuSelected (option);
   }
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::setNormalBackground (bool isNormalVideoIn)
{
   QColor background;
   QRgb gridColour;
   QPen pen;

   this->isNormalVideo = isNormalVideoIn;

   background = this->isNormalVideo ? clWhite : clBlack;
#if QWT_VERSION >= 0x060000
   this->plot->setCanvasBackground (QBrush (background));
#else
   this->plot->setCanvasBackground (background);
#endif

   gridColour =  this->isNormalVideo ? 0x00c0c0c0 : 0x00404040;
   pen.setColor(QColor (gridColour));
   pen.setStyle (Qt::DashLine);
   this->grid->setPen (pen);

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
QPointF QEStripChart::PrivateData::plotToReal (const QPoint & pos) const
{
   double t, y;

   // Perform basic invsere transformation.
   //
   t = this->plot->invTransform (QwtPlot::xBottom, pos.x ());
   y = this->plot->invTransform (QwtPlot::yLeft,   pos.y ());

   // Scale to real world units.
   t = t * this->timeScale;
   if (this->yScaleMode == QEStripChartNames::log) {
       y = EXP10 (y);
   }

   return QPointF (t, y);
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::plotData ()
{
   QEStripChart *chart = this->chart;

   unsigned int slot;
   double d;
   double ylo, yhi;
   double step;
   QString format;
   QString times;
   QDateTime dt;
   QString zoneTLA;

   // First release any/all allocated curves.
   //
   this->releaseCurves ();


   d = chart->getDuration ();
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

   // Update the plot for each PV.
   // Allocate curve and call curve-setSample/setData.
   //
   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
      if (this->getItem (slot)->isInUse ()) {
          this->getItem (slot)->plotData (this->timeScale, this->yScaleMode);
      }
   }

   // Draw selected area box if defined.
   //
   if (this->plotLeftIsDefined) {
      double t1, y1;
      double t2, y2;
      QVector<double> tdata;
      QVector<double> ydata;
      QwtPlotCurve *curve;
      QPen pen;

      // Do inverse transform on button press postions so that they can be re-transformed when plotted ;-)
      // At least we don't need to worry about duration/log scaling here - that looks after itself.
      //
      t1 = this->plot->invTransform (QwtPlot::xBottom, this->plotLeftButton.x ());
      y1 = this->plot->invTransform (QwtPlot::yLeft,   this->plotLeftButton.y ());

      t2 = this->plot->invTransform (QwtPlot::xBottom, this->plotCurrent.x ());
      y2 = this->plot->invTransform (QwtPlot::yLeft,   this->plotCurrent.y ());

      tdata << t1;  ydata << y1;
      tdata << t2;  ydata << y1;
      tdata << t2;  ydata << y2;
      tdata << t1;  ydata << y2;
      tdata << t1;  ydata << y1;

      // Set curve propeties plus item Pen which include its colour.
      //
      curve = this->allocateCurve ();
      curve->setRenderHint (QwtPlotItem::RenderAntialiased);
      curve->setStyle (QwtPlotCurve::Lines);
      if (this->isValidYRangeSelection (this->plotLeftButton, this->plotCurrent) ||
          this->isValidTRangeSelection (this->plotLeftButton, this->plotCurrent) ) {
         pen.setColor(QColor (0x60C060));  // greenish
      } else {
         pen.setColor(QColor (0x808080));  // gray
      }
      pen.setWidth (1);
      curve->setPen (pen);

#if QWT_VERSION >= 0x060000
      curve->setSamples (tdata, ydata);
#else
      curve->setData (tdata, ydata);
#endif
   }

   // Draw origin to target line if defined..
   //
   if (this->plotRightIsDefined) {
      double t1, y1;
      double t2, y2;
      QVector<double> tdata;
      QVector<double> ydata;
      QwtPlotCurve *curve;
      QPen pen;

      // Do inverse transform on button press postions so that they can be re-transformed when plotted ;-)
      // At least we don't need to worry about duration/log scaling here - that looks after itself.
      //
      t1 = this->plot->invTransform (QwtPlot::xBottom, this->plotRightButton.x ());
      y1 = this->plot->invTransform (QwtPlot::yLeft,   this->plotRightButton.y ());

      t2 = this->plot->invTransform (QwtPlot::xBottom, this->plotCurrent.x ());
      y2 = this->plot->invTransform (QwtPlot::yLeft,   this->plotCurrent.y ());

      tdata << t1;  ydata << y1;
      tdata << t2;  ydata << y2;

      // Set curve propeties plus item Pen which include its colour.
      //
      curve = this->allocateCurve ();
      curve->setRenderHint (QwtPlotItem::RenderAntialiased);
      curve->setStyle (QwtPlotCurve::Lines);
      pen.setColor(QColor (0x8080C0));  // blueish
      pen.setWidth (1);
      curve->setPen (pen);

#if QWT_VERSION >= 0x060000
      curve->setSamples (tdata, ydata);
#else
      curve->setData (tdata, ydata);
#endif
   }

   if (this->chartYScale == QEStripChartNames::dynamic) {
      // Re-calculate chart range.
      //
      this->calcDisplayMinMax ();
   }

   if (this->yScaleMode == QEStripChartNames::linear) {
      step = (this->chart->getYMaximum () - this->chart->getYMinimum ())/5.0;
      step = this->selectStep (step);
      ylo = chart->getYMinimum ();
      yhi = chart->getYMaximum ();
   } else {
      // therefore log scale
      step = 1.0;
      ylo = floor (LOG10 (this->chart->getYMinimum ()));
      yhi = ceil  (LOG10 (this->chart->getYMaximum ()));
   }

   this->plot->setAxisScale (QwtPlot::xBottom, -d/this->timeScale, 0.0, (d/this->timeScale)/5.0);
   this->plot->setAxisScale (QwtPlot::yLeft, ylo, yhi, step);
   this->plot->replot ();

   format = "yyyy-MM-dd hh:mm:ss";
   times = " ";

   dt = chart->getStartDateTime ().toTimeSpec (chart->timeZoneSpec);
   zoneTLA = QEUtilities::getTimeZoneTLA (chart->timeZoneSpec, dt);

   times.append (dt.toString (format)).append (" ").append (zoneTLA);
   times.append (" to ");

   dt = chart->getEndDateTime ().toTimeSpec (chart->timeZoneSpec);
   zoneTLA = QEUtilities::getTimeZoneTLA (chart->timeZoneSpec, dt);

   times.append (dt.toString (format)).append (" ").append (zoneTLA);

   // set on tool bar
   this->toolBar->setTimeStatus (times);
}

//------------------------------------------------------------------------------
//
bool QEStripChart::PrivateData::isValidYRangeSelection (const QPoint  & origin, const QPoint  & offset) const
{
   const int minDiff = 8;
   const int deltaX = offset.x () - origin.x ();
   const int deltaY = offset.y () - origin.y ();
   return ((deltaY > minDiff) && (deltaY > ABS (3 * deltaX)));
}

//------------------------------------------------------------------------------
//
bool QEStripChart::PrivateData::isValidTRangeSelection (const QPoint  & origin, const QPoint &  offset) const
{
   const int minDiff = 8;
   const int deltaX = offset.x () - origin.x ();
   const int deltaY = offset.y () - origin.y ();
   return ((deltaX > minDiff) && (deltaX > ABS (3 * deltaY)));
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::onPlaneScaleSelect (const QPoint  & origin, const QPoint  & offset)
{
   QEStripChart *chart = this->chart;

   const QPointF rTopLeft     = this->plotToReal (origin);
   const QPointF rBottomRight = this->plotToReal (offset);

   // Only proceed if user has un-ambiguously selected time scaling or y scaling.
   //
   if (this->isValidYRangeSelection (origin, offset)) {
      // Makeing a Y scale adjustment.
      //
      chart->setYRange (rBottomRight.y (), rTopLeft.y ());
      this->pushState ();

   } else if (this->isValidTRangeSelection (origin, offset)) {
      // Makeing a X (time) scale adjustment.
      //
      double dt;
      int duration;
      QDateTime et;
      QDateTime now;

      dt = rBottomRight.x () - rTopLeft.x ();
      duration = MAX (1, int (dt));

      et = chart->getEndDateTime ().addSecs (rBottomRight.x ());

      now = QDateTime::currentDateTime ();
      if (et >= now) {
         // constrain
         et = now;
      } else {
         this->chartTimeMode = QEStripChartNames::tmHistorical;
      }

      chart->setDuration (duration);
      chart->setEndDateTime (et);
      this->pushState ();
   } // else doing nothing
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::setReadOut (const QString & text)
{
   message_types mt (MESSAGE_TYPE_INFO, MESSAGE_KIND_STATUS);
   this->chart->sendMessage (text, mt);
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::onCanvasMouseMove (QMouseEvent * event)
{
   const QPointF real = this->plotToReal (event->pos ());
   qint64 mSec;
   QDateTime t;
   QString zoneTLA;
   QString format;
   QString mouseReadOut;
   QString f;

   t = this->chart->getEndDateTime ().toTimeSpec (chart->timeZoneSpec);

   // Convert cursor x to absolute cursor time.
   // x is the time (in seconds) relative to the chart end time.
   //
   mSec = qint64(1000.0 * real.x());
   t = t.addMSecs (mSec);

   // Keep only most significant digit of the milli-seconds,
   // i.e. tenths of a second.
   //
   format = "yyyy-MM-dd hh:mm:ss.zzz";
   mouseReadOut = t.toString (format).left (format.length() - 2);

   zoneTLA = QEUtilities::getTimeZoneTLA (chart->timeZoneSpec, t);
   mouseReadOut.append (" ").append (zoneTLA);

   f.sprintf (" %10.2f ", real.x () /this->timeScale);
   mouseReadOut.append (f);
   mouseReadOut.append (this->timeUnits);

   f.sprintf ("    %+.10g", real.y ());
   mouseReadOut.append (f);

   if (this->plotRightIsDefined) {
      const QPointF origin = this->plotToReal (this->plotRightButton);
      const QPointF offset = this->plotToReal (this->plotCurrent);
      const double dt = offset.x() - origin.x ();
      const double dy = offset.y() - origin.y ();

      f.sprintf ("      dt: %.1f s ", dt);
      mouseReadOut.append (f);

      f.sprintf ("      dy: %+.5g", dy);
      mouseReadOut.append (f);

      if (dt > 0.0) {
         f.sprintf ("      dy/dt: %+.5g", dy/dt);
         mouseReadOut.append (f);
      }
   }

   this->setReadOut (mouseReadOut);
}

//------------------------------------------------------------------------------
//
bool QEStripChart::PrivateData::eventFilter (QObject *obj, QEvent *event)
{
   QMouseEvent * mouseEvent = NULL;
   bool isFiltered = false;

   // case on type first else we get a seg fault.
   //
   switch (event->type ()) {

      case QEvent::MouseButtonPress:
         mouseEvent = static_cast<QMouseEvent *> (event);
         if (obj == this->plot->canvas ()) {
            switch (mouseEvent->button()) {
               case Qt::LeftButton:
                  this->plotLeftButton = mouseEvent->pos ();
                  this->plotLeftIsDefined = true;
                  isFiltered = true;
                  break;

               case Qt::RightButton:
                  this->plotRightButton = mouseEvent->pos ();
                  this->plotRightIsDefined = true;
                  isFiltered = true;
                  break;

               default:
                  break;
            }
         }
         break;

      case QEvent::MouseButtonRelease:
         mouseEvent = static_cast<QMouseEvent *> (event);
         if (obj == this->plot->canvas ()) {
            bool needAreplot = false;
            switch (mouseEvent->button ()) {
               case Qt::LeftButton:
                  if (this->plotLeftIsDefined) {
                     this->onPlaneScaleSelect (this->plotLeftButton, this->plotCurrent);
                     this->plotLeftIsDefined = false;
                     needAreplot = true;
                     isFiltered = true;
                  }
                  break;

               case Qt::RightButton:
                  if (this->plotRightIsDefined) {
                     this->plotRightIsDefined = false;
                     needAreplot = true;
                     isFiltered = true;
                  }
                  break;

               default:
                  break;
            }

            if (needAreplot) {
               this->plotData ();
            }
         }
         break;

      case QEvent::MouseMove:
         mouseEvent = static_cast<QMouseEvent *> (event);

         if (obj == this->plot->canvas ()) {
            this->plotCurrent = mouseEvent->pos ();
            if (this->plotLeftIsDefined ||this->plotRightIsDefined) {
               this->plotData ();
            }
            this->onCanvasMouseMove (mouseEvent);
            isFiltered = true;
         }
         break;

      default:
         // Just fall through
         break;
   }

   return isFiltered;
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::captureState (ChartState& chartState)
{
   // Capture current state.
   //
   chartState.isNormalVideo = this->isNormalVideo;
   chartState.yScaleMode = this->yScaleMode;
   chartState.chartYScale = this->chartYScale;
   chartState.yMinimum = this->chart->getYMinimum ();
   chartState.yMaximum = this->chart->getYMaximum ();
   chartState.chartTimeMode = this->chartTimeMode;
   chartState.duration = this->chart->getDuration ();
   chartState.timeZoneSpec = this->chart->timeZoneSpec;
   chartState.endDateTime = this->chart->getEndDateTime ();
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::applyState (const ChartState & chartState)
{
    this->setNormalBackground (chartState.isNormalVideo);
    this->yScaleMode = chartState.yScaleMode;
    this->chartYScale = chartState.chartYScale;
    this->chart->setYRange (chartState.yMinimum, chartState.yMaximum);
    this->chartTimeMode =  chartState.chartTimeMode;
    this->chart->setEndDateTime (chartState.endDateTime);
    this->chart->setDuration (chartState.duration);
    this->chart->timeZoneSpec = chartState.timeZoneSpec;
    this->plotData ();
}

//------------------------------------------------------------------------------
//
void QEStripChart::PrivateData::pushState ()
{
   ChartState chartState;

   // Capture current state.
   //
   this->captureState (chartState);

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
   this->setMinimumSize (1032, 400);   // keep this and sizeHint consistant

   this->timeZoneSpec = Qt::LocalTime;
   this->duration = 600;  // ten minites.

   // We always use UTC (EPICS) time within the strip chart.
   // Set directly here as using setEndTime has side effects.
   //
   this->endDateTime = QDateTime::currentDateTime ().toUTC ();

   this->yMinimum = 0.0;
   this->yMaximum = 100.0;

   this->variableNameSubstitutions = "";

   // Construct dialogs.
   //
   this->timeDialog = new QEStripChartTimeDialog (this);
   this->yRangeDialog = new QEStripChartRangeDialog (this);

   // Construct private data for this chart.
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

   // No overall conext menu - let each sub component do its own thing.
   //
   // Use default context menu.
   //     this->setupContextMenu (this);
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
QSize QEStripChart::sizeHint () const
{
   return QSize (1032, 400);
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
void QEStripChart::setVariableNameSubstitutionsProperty (QString variableNameSubstitutionsIn)
{
   int j;

   // Save local copy - just for getVariableNameSubstitutionsProperty.
   //
   this->variableNameSubstitutions = variableNameSubstitutionsIn;

   // The same subtitutions apply to all PVs.
   //
   for (j = 0; j < NUMBER_OF_PVS; j++ ) {
      QEStripChartItem * item = this->privateData->getItem (j);
      item->pvNameProperyManager.setSubstitutionsProperty (variableNameSubstitutionsIn);
   }
}

//------------------------------------------------------------------------------
//
QString QEStripChart::getVariableNameSubstitutionsProperty ()
{
   return this->variableNameSubstitutions;
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

   // Split input string using white space as delimiter.
   //
   pvNameList = pvNameSet.split (QRegExp ("\\s+"), QString::SkipEmptyParts);
   for (j = 0; j < pvNameList.count (); j++) {
      this->addPvName (pvNameList.value (j));
   }
}

//------------------------------------------------------------------------------
//
void QEStripChart::tickTimeout ()
{
   if (this->privateData->chartTimeMode == QEStripChartNames::tmRealTime) {
      // Note: when end time changes - setEndTime calls plotData ().
      this->setEndDateTime (QDateTime::currentDateTime ());
   } else {
      // must be paused or historical - pretty much the same.
      this->privateData->plotData ();
   }
}


//------------------------------------------------------------------------------
//
void QEStripChart::chartContextMenuRequested (const QPoint& pos)
{
   this->privateData->chartContextMenuRequested (pos);
}

//------------------------------------------------------------------------------
//
void QEStripChart::chartContextMenuTriggered (QAction* action)
{
   this->privateData->chartContextMenuTriggered (action);
}

//------------------------------------------------------------------------------
//
void QEStripChart::itemContextMenuRequested (const unsigned int slot, const QPoint & pos)
{
   this->privateData->itemContextMenuRequested (slot, pos);
}

//------------------------------------------------------------------------------
//
void QEStripChart::itemContextMenuSelected (const unsigned int slot,
                                            const QEStripChartNames::ContextMenuOptions option)
{
   this->privateData->itemContextMenuSelected (slot, option);
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
   this->privateData->yScaleMode = mode;
   this->privateData->plotData ();
   this->privateData->pushState ();
}


//------------------------------------------------------------------------------
//
void QEStripChart::yRangeSelected (const QEStripChartNames::ChartYRanges scale)
{
   int n;
   unsigned int slot;

   switch (scale) {
      case QEStripChartNames::manual:
         this->yRangeDialog->setRange (this->getYMinimum (), this->getYMaximum ());
         n = this->yRangeDialog->exec ();
         if (n == 1) {
            this->privateData->chartYScale = scale;
            // User has selected okay.
            //
            this->setYRange (this->yRangeDialog->getMinimum (),
                             this->yRangeDialog->getMaximum ());
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

         for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
            QEStripChartItem * item = this->privateData->getItem (slot);
            if (item->isInUse ()) {
               item->normalise ();
            }
         }
         this->privateData->pushState ();
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
void QEStripChart::timeZoneSelected (const Qt::TimeSpec timeZoneSpecIn)
{
   this->timeZoneSpec = timeZoneSpecIn;
   this->plotData ();
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
         this->privateData->chartTimeMode = QEStripChartNames::tmRealTime;
         // Note: using setEndTime causes a replot.
         this->setEndDateTime (QDateTime::currentDateTime ());
         this->privateData->pushState ();
         break;

      case QEStripChartNames::pause:
         this->privateData->chartTimeMode = QEStripChartNames::tmPaused;
         this->privateData->pushState ();
         break;

      case QEStripChartNames::forward:
         this->privateData->chartTimeMode = QEStripChartNames::tmHistorical;
         this->setEndDateTime (this->getEndDateTime ().addSecs (+this->duration));
         this->privateData->pushState ();
         break;

      case QEStripChartNames::backward:
         this->privateData->chartTimeMode = QEStripChartNames::tmHistorical;
         this->setEndDateTime (this->getEndDateTime ().addSecs (-this->duration));
         this->privateData->pushState ();
         break;

      case QEStripChartNames::selectTimes:
         this->timeDialog->setMaximumDateTime (QDateTime::currentDateTime ().toTimeSpec (this->timeZoneSpec));
         this->timeDialog->setStartDateTime (this->getStartDateTime().toTimeSpec (this->timeZoneSpec));
         this->timeDialog->setEndDateTime (this->getEndDateTime().toTimeSpec (this->timeZoneSpec));
         n = this->timeDialog->exec ();
         if (n == 1) {
            // User has selected okay.
            //
            this->privateData->chartTimeMode = QEStripChartNames::tmHistorical;
            this->setEndDateTime (this->timeDialog->getEndDateTime ());

            // We use the possibly limited chart end time in order to calculate the
            // duration.
            //
            d = this->timeDialog->getStartDateTime ().secsTo (this->getEndDateTime());
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
void QEStripChart::addToPredefinedList (const QString & pvName)
{
   predefinedPVNameList.prependOrMoveToFirst (pvName);
}

//------------------------------------------------------------------------------
//
QString QEStripChart::getPredefinedItem (int i)
{
   return predefinedPVNameList.value (i, "");
}


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
void QEStripChart::paste (QVariant s)
{
   // Use pasted text to add a PV(s) to the chart.
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

   this->setAllowDrop (allowDrop);
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

//------------------------------------------------------------------------------
//
void QEStripChart::saveConfiguration (PersistanceManager* pm)
{
   const QString formName = this->persistantName ("QEStripChart");

   // Do common stuff first.
   // How can we avoid doing this mutiple times??
   //
   PMElement commonElement = pm->addNamedConfiguration ("QEStripChart_Common");
   predefinedPVNameList.saveConfiguration (commonElement);

   // Now do form instance specific stuff.
   //
   PMElement formElement = pm->addNamedConfiguration (formName);

   // Capture current state.
   //
   ChartState chartState;
   this->privateData->captureState (chartState);
   chartState.saveConfiguration (formElement);

   // Save each active PV.
   //
   PMElement pvListElement = formElement.addElement ("PV_List");
   unsigned int slot;

   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
      QEStripChartItem * item = this->privateData->getItem (slot);
      if (item) {
         item->saveConfiguration (pvListElement);
      }
   }
}

//------------------------------------------------------------------------------
//
void QEStripChart::restoreConfiguration (PersistanceManager * pm, restorePhases restorePhase)
{
   if (restorePhase != FRAMEWORK) return;

   const QString formName = this->persistantName ("QEStripChart");

   // Do common stuff first.
   // How can we avoid doing this mutiple times??
   //
   PMElement commonElement = pm->getNamedConfiguration ("QEStripChart_Common");
   predefinedPVNameList.restoreConfiguration (commonElement);

   // Now do form instance specific stuff.
   //
   PMElement formElement = pm->getNamedConfiguration (formName);

   // Restore chart state.
   //
   ChartState chartState;
   chartState.restoreConfiguration (formElement);
   this->privateData->applyState (chartState);

   // Restore each PV.
   //
   PMElement pvListElement = formElement.getElement ("PV_List");
   unsigned int slot;

   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
      QEStripChartItem * item = this->privateData->getItem (slot);
      if (item) {
         item->restoreConfiguration (pvListElement);
      }
   }
}

// end
