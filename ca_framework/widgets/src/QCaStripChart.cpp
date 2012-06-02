/*  $Id: QCaStripChart.cpp $
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

#include <QDebug>
#include <QBoxLayout>
#include <QLabel>
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QPushButton>
#include <QMenu>
#include <QToolButton>

#include <qwt_plot.h>
#include <qwt_plot_grid.h>

#include <alarm.h>

#include <QCaObject.h>
#include <QCaLabel.h>
#include <QCaVariableNamePropertyManager.h>

#include <QCaStripChart.h>
#include <QCaStripChartItem.h>


#define MAX(a, b)           ((a) >= (b) ? (a) : (b))
#define MIN(a, b)           ((a) <= (b) ? (a) : (b))
#define LIMIT(x,low,high)   (MAX(low, MIN(x, high)))

#define DEBUG   qDebug () << __FILE__  << ":" << __LINE__ << "(" << __FUNCTION__ << ")"


static const QColor clWhite (0xFF, 0xFF, 0xFF, 0xFF);

// Chart scale options and associated menu names.
//
enum ChartYScale {
   ysManual,
   ysLoprHopr,
   ysDisplayed,
   ysBuffered,
   ysDynamic,
   YSMAXIMUM
};

static const QString chartScaleNames [YSMAXIMUM] = {
   "Manual... ",
   "PV LOPR/HOPRs ",
   "Displayed Min/Max ",
   "Buffered Min/Max ",
   "Dynamic "
};

// Chart time mode options and associated menu names.
//
enum ChartTimeMode {
   tmRealTime,
   tmPause,
   tmBackward,
   tmForward,
   tmHistorical,
   TMMAXIMUM
};

static const QString chartTimeModeNames [TMMAXIMUM] = {
   "Real Time ",
   "Pause ",
   "Backward ",
   "Forward ",
   "Historical... "
};


//==============================================================================
// Local support classes.
//==============================================================================
// The imperitive to create this class is to hold references to created QWidgets.
// If this are declared directly in the header, either none of the widget defined
// in the plugin are visible in designer or it seg faults. I think the moc file
// generation and other Qt stuff gets very confused.
//
// But given that the class does it exist it is a convient palce holder for
// some additional data and associated functions. The placement of some here or
// in the main component is somewhat arbitary.
//
class QCaStripChart::PrivateData : public QObject {
public:
   // Constructor
   //
   PrivateData (QCaStripChart *chartIn);
   ~PrivateData();
   QCaStripChartItem * getItem (unsigned int slot);
   QwtPlotCurve *allocateCurve ();
   void calcDisplayMinMax ();
   void plotData ();

   enum ChartYScale chartYScale;
   enum ChartTimeMode chartTimeMode;

private:
   QCaStripChart *chart;
   QFrame *toolFrame;
   QFrame *pvFrame;
   QFrame *plotFrame;
   QwtPlot *plot;
   QVBoxLayout *layout1;
   QVBoxLayout *layout2;
   QPushButton * b1;
   QMenu *m1;
   QPushButton * b2;
   QMenu *m2;
   QPushButton * b3;
   QMenu *m3;
   QPushButton * b4;

   QToolButton *channelProperties [NUMBER_OF_PVS];
   QLabel *pvNames [NUMBER_OF_PVS];
   QCaLabel *caLabels [NUMBER_OF_PVS];
   QCaStripChartItem *items [NUMBER_OF_PVS];

   QVector<QwtPlotCurve *> curve_list;

   double minY, maxY;

   void releaseCurves ();
};

//------------------------------------------------------------------------------
//
QCaStripChart::PrivateData::PrivateData (QCaStripChart *chartIn)
{
   int j;
   unsigned int slot;
   int left;

   this->chart = chartIn;

   // Create tool bar frame and tool buttons.
   //
   this->toolFrame = new QFrame (this->chart);
   this->toolFrame->setFrameShape (QFrame::Panel);
   this->toolFrame->setFixedHeight (32);

   // Create tool bar menus and buttons.
   //
   this->m1 = new QMenu ();
   for (j = 0; j < YSMAXIMUM; j++) {
      this->m1->addAction (chartScaleNames [j])->setData (QVariant (j));
   }
   QObject::connect (this->m1,  SIGNAL (triggered     (QAction *)),
                     this->chart, SLOT (menuSetYScale (QAction *)));

   this->b1 = new QPushButton ("Scale To", this->toolFrame);
   this->b1->setMenu (this->m1);

   //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   this->m2 = new QMenu ();
   this->m2->addAction ("1s  ")->setData (QVariant (1));
   this->m2->addAction ("2s  ")->setData (QVariant (2));
   this->m2->addAction ("5s  ")->setData (QVariant (5));
   this->m2->addAction ("10s ")->setData (QVariant (10));
   this->m2->addAction ("20s ")->setData (QVariant (20));
   this->m2->addAction ("30s ")->setData (QVariant (30));

   this->m2->addAction ("1m  ")->setData (QVariant (1*60));
   this->m2->addAction ("2m  ")->setData (QVariant (2*60));
   this->m2->addAction ("5m  ")->setData (QVariant (5*60));
   this->m2->addAction ("10m ")->setData (QVariant (10*60));
   this->m2->addAction ("20m ")->setData (QVariant (20*60));
   this->m2->addAction ("30m ")->setData (QVariant (30*60));

   this->m2->addAction ("1h  ")->setData (QVariant (1*3600));
   this->m2->addAction ("2h  ")->setData (QVariant (2*3600));
   this->m2->addAction ("4h  ")->setData (QVariant (4*3600));
   this->m2->addAction ("12h ")->setData (QVariant (12*3600));

   this->m2->addAction ("1d  ")->setData (QVariant (1*86400));
   this->m2->addAction ("2d  ")->setData (QVariant (2*86400));

   QObject::connect (this->m2,  SIGNAL (triggered       (QAction *)),
                     this->chart, SLOT (menuSetDuration (QAction *)));

   this->b2 = new QPushButton ("Duration", this->toolFrame);
   this->b2->setMenu (this->m2);

   //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   this->m3 = new QMenu ();
   for (j = 0; j < TMMAXIMUM; j++) {
      this->m3->addAction (chartTimeModeNames [j])->setData (QVariant (j));
   }
   QObject::connect (this->m3,  SIGNAL (triggered       (QAction *)),
                     this->chart, SLOT (menuSetTimeMode (QAction *)));

   this->b3 = new QPushButton ("Time Mode", this->toolFrame);
   this->b3->setMenu (this->m3);

   //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   this->b4 = new QPushButton ("Read Archive", this->toolFrame);
   QObject::connect (this->b4,  SIGNAL (clicked            (bool)),
                     this->chart, SLOT (readArchiveClicked (bool)));


   // TODO: Try QToolBar - it may auto layout.
   //
   left = 4;
   this->b1->setGeometry (left, 4, 76, 24);   left += 4 + 76;
   this->b2->setGeometry (left, 4, 76, 24);   left += 4 + 76;
   this->b3->setGeometry (left, 4, 96, 24);   left += 4 + 96;
   this->b4->setGeometry (left, 4, 100, 24);  left += 4 + 100;

   // Create PV frame and PV name labels and associated CA labels.
   //
   this->pvFrame = new QFrame (this->chart);
   this->pvFrame->setFrameShape (QFrame::Panel);
   this->pvFrame->setFixedHeight ( (NUMBER_OF_PVS / 2) * 18 + 8);

   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
      int x, y;

      this->channelProperties [slot] = new QToolButton (this->pvFrame);
      this->pvNames [slot] = new QLabel (this->pvFrame);
      this->caLabels [slot] = new QCaLabel (this->pvFrame);

      x = (slot % 2) * 492 + 4;
      y = (slot / 2) * 18  + 4;

      this->channelProperties [slot]->setGeometry (x, y,  16, 15); x += 20;
      this->pvNames [slot]->setGeometry           (x, y, 340, 15); x += 344;
      this->caLabels [slot]->setGeometry          (x, y, 120, 15); x += 124;

      this->items [slot] = new QCaStripChartItem (this->chart,
                                                  this->pvNames [slot],
                                                  this->caLabels [slot],
                                                  slot);

      QObject::connect (this->channelProperties [slot],  SIGNAL (clicked            (bool)),
                        this->items [slot],              SLOT   (channelPropertiesClicked (bool)));

   }

   // Create plotting frame and plot area.
   //
   this->plotFrame = new QFrame (this->chart);
   this->plotFrame->setFrameShape (QFrame::Panel);

   this->plot = new QwtPlot (this->plotFrame);
   this->plot->setCanvasBackground (QBrush (clWhite));
   this->plot->setCanvasLineWidth (1);

   // Create layouts.
   //
   this->layout1 = new QVBoxLayout (this->chart);
   this->layout1->setMargin (4);
   this->layout1->setSpacing (4);
   this->layout1->addWidget (this->toolFrame);
   this->layout1->addWidget (this->pvFrame);
   this->layout1->addWidget (this->plotFrame);

   this->layout2 = new QVBoxLayout (this->plotFrame);
   this->layout2->setMargin (4);
   this->layout2->setSpacing (4);
   this->layout2->addWidget (this->plot);

   // Clear / initaialise plot.
   //
   this->minY = 0.0;
   this->maxY = 100.0;
   this->chartYScale = ysManual;
   this->chartTimeMode = tmRealTime;

   this->plotData ();
}

//------------------------------------------------------------------------------
//
QCaStripChart::PrivateData::~PrivateData ()
{
   this->releaseCurves ();
   // all the created QWidget are (indirectly) parented by this QCaStripChart,
   // they are automatically deleted.
}

//------------------------------------------------------------------------------
//
QCaStripChartItem * QCaStripChart::PrivateData::getItem (unsigned int slot)
{
   return (slot < NUMBER_OF_PVS) ? this->items [slot] : NULL;
}

//------------------------------------------------------------------------------
//
QwtPlotCurve * QCaStripChart::PrivateData::allocateCurve ()
{
   QwtPlotCurve * result = NULL;

   result = new QwtPlotCurve ();
   result->setRenderHint (QwtPlotItem::RenderAntialiased);
   result->setStyle (QwtPlotCurve::Lines);
   result->attach (this->plot);

   this->curve_list.append (result);
   return result;
}

//------------------------------------------------------------------------------
//
void QCaStripChart::PrivateData::releaseCurves ()
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
void QCaStripChart::PrivateData::calcDisplayMinMax ()
{
   bool atLeastOne;
   int slot;
   TrackRange tr;
   double min;
   double max;

   if (this->chartYScale == ysManual) return;

   atLeastOne = false;
   tr.clear ();

   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {

      QCaStripChartItem * item = this->getItem (slot);
      if (item->isInUse() == true) {
         switch (this->chartYScale) {
         case ysLoprHopr:  tr.merge (item->getLoprHopr ());         break;
         case ysDisplayed: tr.merge (item->getDisplayedMinMax ());  break;
         case ysBuffered:  tr.merge (item->getBufferedMinMax ());   break;
         case ysDynamic:   tr.merge (item->getDisplayedMinMax ());  break;
         default:          DEBUG "Well this is unexpected"; return; break;
         }
      }
   }

   if (tr.getMinMax (min, max) == true) {
      this->minY = min;
      this->maxY = MAX (max, min + 1.0E-3);
   } // else do not change.
}

//------------------------------------------------------------------------------
//
void QCaStripChart::PrivateData::plotData ()
{
   unsigned int slot;
   double d;
   double step;

   d = this->chart->getDuration ();
   this->plot->setAxisScale (QwtPlot::xBottom, -d,         0.0,         d/5.0);

   // Update the plot
   //
   this->releaseCurves ();
   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
      if (this->getItem (slot)->isInUse ()) {
          this->getItem (slot)->plotData ();
      }
   }

   if (this->chartYScale == ysDynamic) {
      // Re-calculate chart range.
      //
      this->calcDisplayMinMax ();
   }
   step = (this->maxY - this->maxY)/5.0;
   this->plot->setAxisScale (QwtPlot::yLeft, this->minY, this->maxY, step);

   this->plot->replot ();
}


//==============================================================================
// QCaStripChart class functions
//==============================================================================
//
QCaStripChart::QCaStripChart (QWidget * parent) : QFrame (parent), QCaWidget (this)
{
   // configure the panel and create contents
   //
   this->setFrameShape (Panel);
   this->setMinimumSize (1000, 400);

   this->duration = 120;  // two minites.
   this->endTime = QDateTime::currentDateTime ();

   // construct private data for this chart.
   //
   this->privateData = new PrivateData (this);

   this->tickTimer = new QTimer (this);
   connect (this->tickTimer, SIGNAL (timeout ()), this, SLOT (tickTimeout ()));
   this->tickTimer->start (1000);  // mSec = 1.0 s

   // Enable drag drop onto this widget.
   //
   this->calcAllowDrop ();
}

//------------------------------------------------------------------------------
//
QCaStripChart::~QCaStripChart ()
{
   // privateData is a QObject parented by this, so it is automativally deleted.
}

//------------------------------------------------------------------------------
//
QSize QCaStripChart::sizeHint () const {
   return QSize (1000, 400);
}   // sizeHint

//------------------------------------------------------------------------------
//
void QCaStripChart::setVariableNameProperty (unsigned int slot, QString pvName)
{
   if (slot < NUMBER_OF_PVS) {
      QCaStripChartItem * item = this->privateData->getItem (slot);
      item->pvNameProperyManager.setVariableNameProperty (pvName);
   } else {
      DEBUG << "slot out of range " << slot;
   }
}

//------------------------------------------------------------------------------
//
QString QCaStripChart::getVariableNameProperty (unsigned int slot)
{
   if (slot < NUMBER_OF_PVS) {
      QCaStripChartItem * item = this->privateData->getItem (slot);
      return item->pvNameProperyManager.getVariableNameProperty ();
   } else {
      DEBUG << "slot out of range " << slot;
      return "";
   }
}

//------------------------------------------------------------------------------
//
void QCaStripChart::setColorProperty (unsigned int slot, QColor color)
{
   if (slot < NUMBER_OF_PVS) {
      QCaStripChartItem * item = this->privateData->getItem (slot);
      item->setColor (color);
   } else {
      DEBUG << "slot out of range " << slot;
   }
}

//------------------------------------------------------------------------------
//
QColor QCaStripChart::getColorProperty (unsigned int slot)
{
   if (slot < NUMBER_OF_PVS) {
      QCaStripChartItem * item = this->privateData->getItem (slot);
      return item->getColor ();
   } else {
      DEBUG << "slot out of range " << slot;
      return QColor (0x00, 0x00, 0x00, 0xFF);
   }
}

//------------------------------------------------------------------------------
//
void QCaStripChart::addPvName (QString pvName)
{
   unsigned int slot;

   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
      QCaStripChartItem * item = this->privateData->getItem (slot);
      if (item->isInUse() == false) {
         // Found an empty slot.
         //
         item->setPvName (pvName, "");
         break;
      }
   }

   // Determine if we are now full.
   //
   this->calcAllowDrop ();
}


//------------------------------------------------------------------------------
//
void QCaStripChart::updateToolTip (const QString& tip)
{
   this->setToolTip (tip);
}

//------------------------------------------------------------------------------
//
void QCaStripChart::tickTimeout ()
{
   // qDebug () << QDateTime::currentDateTime ();

   // Go with just real time for now.
   //
   if (this->privateData->chartTimeMode == tmRealTime) {
      this->setEndTime (QDateTime::currentDateTime ());
   } else {
      this->privateData->plotData ();
   }
}

//------------------------------------------------------------------------------
//
void QCaStripChart::menuSetDuration (QAction *action)
{
   int d;
   bool okay;

   d = action->data().toInt (&okay);
   if (okay) {
      this->setDuration (d);
   }
}

//------------------------------------------------------------------------------
//
void QCaStripChart::menuSetYScale (QAction *action)
{
   int n;
   bool okay;
   ChartYScale ys;

   n = action->data().toInt (&okay);
   if (!okay) {
      DEBUG << " action data tag unavailable";
      return;
   }

   ys = (ChartYScale) n;

   switch (ys) {
   case ysManual:
      this->privateData->chartYScale = ys;
      DEBUG << "TBD manual mode";
      break;

   case ysLoprHopr:
   case ysDisplayed:
   case ysBuffered:
   case ysDynamic:
      this->privateData->chartYScale = ys;

      this->privateData->calcDisplayMinMax ();
      this->privateData->plotData ();
      break;

   default:
      DEBUG "Well this is unexpected, action tag = " << n;
      break;
   }
}

//------------------------------------------------------------------------------
//
void QCaStripChart::menuSetTimeMode (QAction *action)
{
   int n;
   bool okay;
   ChartTimeMode tm;

   n = action->data().toInt (&okay);
   if (!okay) {
      DEBUG << " action data tag unavailable";
      return;
   }

   tm = (ChartTimeMode) n;

   switch (tm) {
   case tmRealTime:
      this->privateData->chartTimeMode = tmRealTime;
      // Note: using setEndTime causes a replot.
      this->setEndTime (QDateTime::currentDateTime ());
      break;

   case tmPause:
      this->privateData->chartTimeMode = tmPause;
      break;

   case tmBackward:
      this->privateData->chartTimeMode = tmPause;
      this->setEndTime (this->endTime.addSecs (-this->duration));
      break;

   case tmForward:
      this->privateData->chartTimeMode = tmPause;
      this->setEndTime (this->endTime.addSecs (+this->duration));
      break;

   case tmHistorical:
      this->privateData->chartTimeMode = tmHistorical;
      DEBUG << "TBD historical mode";
      break;

   default:
      DEBUG "Well this is unexpected, action tag = " << n;
      break;
   }
}

//------------------------------------------------------------------------------
//
void QCaStripChart::readArchiveClicked (bool checked)
{
   DEBUG << "TBD" << checked;
}

//------------------------------------------------------------------------------
// Start/end time
//
QDateTime QCaStripChart::getStartTime ()
{
   return this->getEndTime().addSecs (-this->duration);
}

//------------------------------------------------------------------------------
//
QDateTime QCaStripChart::getEndTime ()
{
   return this->endTime;
}

//------------------------------------------------------------------------------
//
void QCaStripChart::setEndTime (QDateTime endTimeIn)
{
   if (this->endTime != endTimeIn) {
      this->endTime = endTimeIn;
      this->privateData->plotData ();
   }
}

//------------------------------------------------------------------------------
//
int QCaStripChart::getDuration ()
{
   return this->duration;
}

//------------------------------------------------------------------------------
//
void QCaStripChart::setDuration (int durationIn)
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
void QCaStripChart::setDropText (QString text)
{
   // Use dropped text to add a PV tp the chart.
   //
   this->addPvName (text);
}   // setDropText


//----------------------------------------------------------------------------
// Determinte if user allowed to drop new PVs into this widget
//
void QCaStripChart::calcAllowDrop ()
{
   unsigned int slot;
   bool allowDrop;

   // Hypoyhesize that the strip chart is full.
   //
   allowDrop = false;
   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
      QCaStripChartItem * item = this->privateData->getItem (slot);

      if (item->isInUse () == false) {
         // Found an empty slot.
         //
         allowDrop = true;
         break;
      }
   }

   this->setAcceptDrops (allowDrop);
}   // calcAllowDrop

//----------------------------------------------------------------------------
//
QwtPlotCurve * QCaStripChart::allocateCurve () {
   return this->privateData->allocateCurve ();
}

//------------------------------------------------------------------------------
//
void QCaStripChart::setup ()
{
   DEBUG << "unexpected call";
}

//------------------------------------------------------------------------------
//
qcaobject::QCaObject* QCaStripChart::createQcaItem (unsigned int variableIndex)
{
   DEBUG << "unexpected call,  variableIndex = " << variableIndex;
   return NULL;
}

//------------------------------------------------------------------------------
//
void QCaStripChart::establishConnection (unsigned int variableIndex)
{
   DEBUG << "unexpected call,  variableIndex = " << variableIndex;
}

// end
