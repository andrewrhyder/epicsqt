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
#include <QIcon>
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QPushButton>
#include <QMenu>
#include <QToolButton>

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
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
   tmPaused,
   TMMAXIMUM
};

struct PushButtonSpecifications {
   const QString caption;
   QIcon icon;
   QString toolTip;
   int width;
   const char * member;
};

#define NUMBER_OF_BUTTONS  8

static const struct PushButtonSpecifications buttonSpecs [NUMBER_OF_BUTTONS] = {
   { QString ("Scale To"),   QIcon (""),  QString ("Scale chart Y axis"),                         76, NULL                             },
   { QString ("Duration"),   QIcon (""),  QString ("Select chart T axis"),                        76, NULL                             },
   { QString (""), QIcon (":/icons/strip_chart_play.png"),          QString ("Play - Real time"), 24, SLOT (playClicked (bool))        },
   { QString (""), QIcon (":/icons/strip_chart_pause.png"),         QString ("Pause"),            24, SLOT (pauseClicked (bool))       },
   { QString (""), QIcon (":/icons/strip_chart_page_forward.png"),  QString ("Forward one page"), 24, SLOT (forwardClicked (bool))     },
   { QString (""), QIcon (":/icons/strip_chart_page_backward.png"), QString ("Back one page"),    24, SLOT (backwardClicked (bool))    },
   { QString ("Select Times"),   QIcon (""),  QString ("Set chart start/end time"),               92, SLOT (selectTimeClicked (bool))  },
   { QString ("Read Archive"),   QIcon (""),  QString ("Extract data from archive(s)"),           92, SLOT (readArchiveClicked (bool)) }
};



//==============================================================================
// Local support classes.
//==============================================================================
//
//------------------------------------------------------------------------------
// The imperitive to create this class is to hold references to created QWidgets.
// If this are declared directly in the header, either none of the widget defined
// in the plugin are visible in designer or designer seg faults. I think the moc
// file generation and other Qt stuff gets very confused.
//
// But given that the class does it exist it is a convient place holder for
// some additional data and associated functions. The placement of there artefacts
// here (PrivateData) or in the main widget (QCaStripChart) is somewhat arbitary.
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
   void setReadOut (QString text);
   enum ChartYScale chartYScale;
   enum ChartTimeMode chartTimeMode;

protected:
   bool eventFilter (QObject *obj, QEvent *event);

private:
   QCaStripChart *chart;
   QFrame *toolFrame;
   QFrame *pvFrame;
   QFrame *plotFrame;
   QFrame *statusFrame;
   QwtPlot *plot;

   QVBoxLayout *layout1;
   QVBoxLayout *layout2;

   QMenu *m1;

   QMenu *m2;
   QMenu *m2s;
   QMenu *m2m;
   QMenu *m2h;
   QMenu *m2d;
   QMenu *m2w;

   QPushButton *pushButtons [NUMBER_OF_BUTTONS];
   QLabel *readOut;
   QLabel *timeStatus;

   QToolButton *channelProperties [NUMBER_OF_PVS];
   QLabel *pvNames [NUMBER_OF_PVS];
   QCaLabel *caLabels [NUMBER_OF_PVS];
   QCaStripChartItem *items [NUMBER_OF_PVS];

   QVector<QwtPlotCurve *> curve_list;
   void releaseCurves ();
   void onCanvasMouseMove (QMouseEvent * event);
   static double selectStep (const double step);
};

//------------------------------------------------------------------------------
//
QCaStripChart::PrivateData::PrivateData (QCaStripChart *chartIn)
{
   static const int seconds_per_minute = 60;
   static const int seconds_per_hour = 60 * seconds_per_minute;
   static const int seconds_per_day = 24 * seconds_per_hour;
   static const int seconds_per_week = 7 * seconds_per_day;

   int j;
   unsigned int slot;
   int left;
   QPushButton *button;

   this->chart = chartIn;

   // Create tool bar frame and tool buttons.
   //
   this->toolFrame = new QFrame (this->chart);
   this->toolFrame->setFrameShape (QFrame::Panel);
   this->toolFrame->setFixedHeight (32);

   // Create toobar buttons
   // TODO: Try QToolBar - it may auto layout.
   //
   left = 4;
   for (j = 0 ; j < NUMBER_OF_BUTTONS; j++) {
      button = new QPushButton (buttonSpecs[j].caption, this->toolFrame);
      button->setIcon (buttonSpecs[j].icon);
      button->setToolTip(buttonSpecs[j].toolTip);
      button->setGeometry (left, 4, buttonSpecs[j].width, 24);
      left += 4 + buttonSpecs[j].width;
      if (buttonSpecs[j].member != NULL) {
         QObject::connect (button,  SIGNAL (clicked (bool)), this->chart, buttonSpecs[j].member);
      }
      this->pushButtons [j] = button;
   }

   // Create tool bar menus for selected buttons.
   //
   this->m1 = new QMenu (this->toolFrame);
   for (j = 0; j < YSMAXIMUM; j++) {
      this->m1->addAction (chartScaleNames [j])->setData (QVariant (j));
   }
   QObject::connect (this->m1,  SIGNAL (triggered     (QAction *)),
                     this->chart, SLOT (menuSetYScale (QAction *)));

   this->pushButtons [0]->setMenu (this->m1);

   //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   this->m2 = new QMenu (this->toolFrame);

   this->m2s = new QMenu ("seconds", this->m2);
   this->m2m = new QMenu ("minutes", this->m2);
   this->m2h = new QMenu ("hours", this->m2);
   this->m2d = new QMenu ("days", this->m2);
   this->m2w = new QMenu ("weeks", this->m2);

   this->m2->addMenu (this->m2s);
   this->m2->addMenu (this->m2m);
   this->m2->addMenu (this->m2h);
   this->m2->addMenu (this->m2d);
   this->m2->addMenu (this->m2w);

   this->m2s->addAction ("1 sec   ")->setData (QVariant (1));
   this->m2s->addAction ("2 secs  ")->setData (QVariant (2));
   this->m2s->addAction ("5 secs  ")->setData (QVariant (5));
   this->m2s->addAction ("10 secs ")->setData (QVariant (10));
   this->m2s->addAction ("20 secs ")->setData (QVariant (20));
   this->m2s->addAction ("30 secs ")->setData (QVariant (30));

   this->m2m->addAction ("1 min   ")->setData (QVariant (1 * seconds_per_minute));
   this->m2m->addAction ("2 mins  ")->setData (QVariant (2 * seconds_per_minute));
   this->m2m->addAction ("5 mins  ")->setData (QVariant (5 * seconds_per_minute));
   this->m2m->addAction ("10 mins ")->setData (QVariant (10 * seconds_per_minute));
   this->m2m->addAction ("20 mins ")->setData (QVariant (20 * seconds_per_minute));
   this->m2m->addAction ("30 mins ")->setData (QVariant (30 * seconds_per_minute));

   this->m2h->addAction ("1 hour   ")->setData (QVariant (1 * seconds_per_hour));
   this->m2h->addAction ("2 hours  ")->setData (QVariant (2 * seconds_per_hour));
   this->m2h->addAction ("5 hours  ")->setData (QVariant (5 * seconds_per_hour));
   this->m2h->addAction ("10 hours ")->setData (QVariant (10 * seconds_per_hour));
   this->m2h->addAction ("20 hours ")->setData (QVariant (20 * seconds_per_hour));

   this->m2d->addAction ("1 day    ")->setData (QVariant (1 * seconds_per_day));
   this->m2d->addAction ("2 days   ")->setData (QVariant (2 * seconds_per_day));
   this->m2d->addAction ("5 days   ")->setData (QVariant (5 * seconds_per_day));
   this->m2d->addAction ("10 days  ")->setData (QVariant (10 * seconds_per_day));

   this->m2w->addAction ("1 week   ")->setData (QVariant (1 * seconds_per_week));
   this->m2w->addAction ("2 weeks  ")->setData (QVariant (2 * seconds_per_week));
   this->m2w->addAction ("5 weeks  ")->setData (QVariant (5 * seconds_per_week));
   this->m2w->addAction ("10 weeks ")->setData (QVariant (10 * seconds_per_week));

   // Connextion seems to apply to all the sub-menus as well
   //
   QObject::connect (this->m2,  SIGNAL (triggered       (QAction *)),
                     this->chart, SLOT (menuSetDuration (QAction *)));

   this->pushButtons [1]->setMenu (this->m2);


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

      this->channelProperties [slot]->setToolTip ("Modify PV attributes");

      QObject::connect (this->channelProperties [slot],  SIGNAL (clicked                  (bool)),
                        this->items [slot],              SLOT   (channelPropertiesClicked (bool)));

   }

   // Create plotting frame and plot area.
   //
   this->plotFrame = new QFrame (this->chart);
   this->plotFrame->setFrameShape (QFrame::Panel);

   this->plot = new QwtPlot (this->plotFrame);
   this->plot->setCanvasBackground (QBrush (clWhite));
   this->plot->setCanvasLineWidth (1);
   this->plot->setLineWidth (1);

   this->plot->canvas()->setMouseTracking (true);
   // Use the privateData object as the event filter object.
   this->plot->canvas()->installEventFilter (this);

   // Create status frame.
   //
   this->statusFrame = new QFrame (this->chart);
   this->statusFrame->setFrameShape (QFrame::Panel);
   this->statusFrame->setFixedHeight (24);

   this->readOut = new QLabel (this->statusFrame);
   this->readOut->setGeometry (8, 2, 600, 20);
   this->readOut->setFont (QFont ("MonoSpace"));

   this->timeStatus = new QLabel (this->statusFrame);
   this->timeStatus->setGeometry (620, 2, 364, 20);
   this->timeStatus->setFont (QFont ("MonoSpace"));

   // Create layouts.
   //
   this->layout1 = new QVBoxLayout (this->chart);
   this->layout1->setMargin (4);
   this->layout1->setSpacing (4);
   this->layout1->addWidget (this->toolFrame);
   this->layout1->addWidget (this->pvFrame);
   this->layout1->addWidget (this->plotFrame);
   this->layout1->addWidget (this->statusFrame);

   this->layout2 = new QVBoxLayout (this->plotFrame);
   this->layout2->setMargin (4);
   this->layout2->setSpacing (4);
   this->layout2->addWidget (this->plot);

   // Clear / initaialise plot.
   //
   this->chartYScale = ysManual;
   this->chartTimeMode = tmRealTime;

   this->plotData ();
}

//------------------------------------------------------------------------------
//
QCaStripChart::PrivateData::~PrivateData ()
{
   this->releaseCurves ();
   // all the created QWidget are (indirectly) parented by the PrivateData,
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
double QCaStripChart::PrivateData::selectStep (const double step)
{
   static const double min_step = 1.0E-4;
   static const double max_step = 1.0E+10;
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
void QCaStripChart::PrivateData::plotData ()
{
   unsigned int slot;
   double d;
   double step;
   QString format;
   QString times;

   d = this->chart->getDuration ();
   this->plot->setAxisScale (QwtPlot::xBottom, -d, 0.0, d/5.0);

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

   step = (this->chart->getYMaximum () - this->chart->getYMinimum ())/5.0;
   step = this->selectStep (step);
   this->plot->setAxisScale (QwtPlot::yLeft,
                             this->chart->getYMinimum (),
                             this->chart->getYMaximum (), step);

   this->plot->replot ();

   format = "yyyy-MM-dd hh:mm:ss UTC";
   times = " ";
   times.append (this->chart->getStartDateTime().toUTC().toString (format));
   times.append (" to ");
   times.append (this->chart->getEndDateTime().toUTC().toString (format));
   this->timeStatus->setText (times);
}

//------------------------------------------------------------------------------
//
void QCaStripChart::PrivateData::setReadOut (QString text)
{
   this->readOut->setText (text);
}

//------------------------------------------------------------------------------
//
void QCaStripChart::PrivateData::onCanvasMouseMove (QMouseEvent * event)
{
   double x;
   double y;
   QDateTime t;
   QString format;
   QString mouseReadOut;
   QString f;

   // Convert pixel (x, y) to real world value (x, y)
   //
   x = this->plot->invTransform(QwtPlot::xBottom, event->x ());
   y = this->plot->invTransform(QwtPlot::yLeft,   event->y ());

   // Convert cursor x to absolute cursor time.
   // x is the time (in seconds) relative to the chart end time.
   //
   t = this->chart->getEndDateTime ().toUTC ().addMSecs ((qint64)(1000.0 * x));

   // Keep only most significant digit of the milli-seconds,
   // i.e. tenths of a second.
   //
   format = "yyyy-MM-dd hh:mm:ss.zzz";
   mouseReadOut = t.toString (format).left (format.length() - 2);
   mouseReadOut.append (" UTC");

   f.sprintf (" %12.1f secs    %+.10g", x, y);
   mouseReadOut.append (f);

   this->setReadOut (mouseReadOut);
}

//------------------------------------------------------------------------------
//
bool QCaStripChart::PrivateData::eventFilter (QObject *obj, QEvent *event)
{
   if ((event->type () == QEvent::MouseMove) && (obj == this->plot->canvas ())) {
      this->onCanvasMouseMove (static_cast<QMouseEvent *> (event));
      return true;
   } else {
      // standard event processing
      return QObject::eventFilter (obj, event);
   }
}


//==============================================================================
// QCaStripChart class functions
//==============================================================================
//
QCaStripChart::QCaStripChart (QWidget * parent) : QFrame (parent), QCaWidget (this)
{
   // Configure the panel and create contents
   //
   this->setFrameShape (Panel);
   this->setMinimumSize (1000, 400);

   this->duration = 120;  // two minites.

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
}

//------------------------------------------------------------------------------
//
QCaStripChart::~QCaStripChart ()
{
   // privateData is a QObject parented by this, so it is automatically deleted.
}

//------------------------------------------------------------------------------
//
QSize QCaStripChart::sizeHint () const {
   return QSize (1000, 400);
}

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
void QCaStripChart::setColourProperty (unsigned int slot, QColor colour)
{
   if (slot < NUMBER_OF_PVS) {
      QCaStripChartItem * item = this->privateData->getItem (slot);
      item->setColour (colour);
   } else {
      DEBUG << "slot out of range " << slot;
   }
}

//------------------------------------------------------------------------------
//
QColor QCaStripChart::getColourProperty (unsigned int slot)
{
   if (slot < NUMBER_OF_PVS) {
      QCaStripChartItem * item = this->privateData->getItem (slot);
      return item->getColour ();
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
   this->evaluateAllowDrop ();
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
   if (this->privateData->chartTimeMode == tmRealTime) {
      // Note: when end time changes - setEndTime calls plotData ().
      this->setEndDateTime (QDateTime::currentDateTime ());
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
void QCaStripChart::playClicked (bool)
{
   this->privateData->chartTimeMode = tmRealTime;
   // Note: using setEndTime causes a replot.
   this->setEndDateTime (QDateTime::currentDateTime ());
}

//------------------------------------------------------------------------------
//
void QCaStripChart::pauseClicked (bool)
{
   this->privateData->chartTimeMode = tmPaused;
}

//------------------------------------------------------------------------------
//
void QCaStripChart::forwardClicked (bool)
{
   this->privateData->chartTimeMode = tmPaused;
   this->setEndDateTime (this->getEndDateTime ().addSecs (+this->duration));
}

//------------------------------------------------------------------------------
//
void QCaStripChart::backwardClicked (bool)
{
   this->privateData->chartTimeMode = tmPaused;
   this->setEndDateTime (this->getEndDateTime ().addSecs (-this->duration));
}

//------------------------------------------------------------------------------
//
void QCaStripChart::selectTimeClicked (bool)
{
   int n;
   int d;

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
   }
}

//------------------------------------------------------------------------------
//
void QCaStripChart::readArchiveClicked (bool checked)
{
   unsigned int slot;

   if (checked) return;

   for (slot = 0; slot < NUMBER_OF_PVS; slot++) {
      QCaStripChartItem * item = this->privateData->getItem (slot);
      if (item->isInUse ()) {
         item->readArchive ();
      }
   }
}

//------------------------------------------------------------------------------
// Start/end time
//
QDateTime QCaStripChart::getStartDateTime ()
{
   return this->getEndDateTime().addSecs (-this->duration);
}

//------------------------------------------------------------------------------
//
QDateTime QCaStripChart::getEndDateTime ()
{
   return this->endDateTime;
}

//------------------------------------------------------------------------------
//
void QCaStripChart::setEndDateTime (QDateTime endDateTimeIn)
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
double QCaStripChart::getYMinimum ()
{
   return this->yMinimum;
}

//----------------------------------------------------------------------------
//
void QCaStripChart::setYMinimum (double yMinimumIn)
{
   this->yMinimum = yMinimumIn;
   this->yMaximum = MAX (this->yMaximum, this->yMinimum + 1.0E-3);
   this->privateData->chartYScale = ysManual;
   this->privateData->plotData ();
}

//----------------------------------------------------------------------------
//
double QCaStripChart::getYMaximum ()
{
   return this->yMaximum;
}

//----------------------------------------------------------------------------
//
void QCaStripChart::setYMaximum (double yMaximumIn)
{
   this->yMaximum = yMaximumIn;
   this->yMinimum = MIN (this->yMinimum, this->yMaximum - 1.0E-3);
   this->privateData->chartYScale = ysManual;
   this->privateData->plotData ();
}

//----------------------------------------------------------------------------
//
void QCaStripChart::setDropText (QString text)
{
   // Use dropped text to add a PV tp the chart.
   //
   this->addPvName (text);
}

//----------------------------------------------------------------------------
// Determine if user allowed to drop new PVs into this widget.
//
void QCaStripChart::evaluateAllowDrop ()
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
}

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
