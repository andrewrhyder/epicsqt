/*  QEGraphic.cpp
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
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

#include <math.h>

#include <QDebug>
#include <qevent.h>       // QEvent maps to qcoreevent.h, not qevent.h
#include <QECommon.h>

#include "QEGraphic.h"

#define DEBUG qDebug () << "QEGraphic" << __FUNCTION__ <<  __LINE__

// These should be consistant with adjustMinMax functions.
//
#define MINIMUM_SPAN              (1.0e-12)
#define MAXIMUM_SPAN              (1.0e+26)
#define NUMBER_TRANISTION_STEPS   6


//==============================================================================
// QEGraphic::Axis class
//==============================================================================
//
QEGraphic::Axis::Axis (QwtPlot* plotIn, const int axisIdIn)
{
   this->plot = plotIn;
   this->axisId = axisIdIn;

   // Set defaults.
   //
   this->isLogarithmic = false;
   this->scale = 1.0;
   this->offset  = 0.0;

   // Set 'current' ranges.
   //
   this->source.minimum = 0.0;
   this->source.maximum = 1.0;
   this->transitionCount = 0;
   this->setRange (0.0, 1.0, QEGraphic::SelectByNumber, 8);
   this->transitionCount = 0;   // reset
}

//------------------------------------------------------------------------------
//
void QEGraphic::Axis::setRange (const double minIn, const double maxIn,
                                const AxisMajorIntervalModes modeIn, const int valueIn)
{
   QEGraphic::DisplayRanges newTarget;

   newTarget.minimum = minIn;
   newTarget.maximum = LIMIT (maxIn, minIn + MINIMUM_SPAN, minIn + MAXIMUM_SPAN);

   // Is this a significant change?
   //
   if ((this->target.minimum != newTarget.minimum) ||
       (this->target.maximum != newTarget.maximum) ||
       (this->intervalMode != modeIn) || (this->intervalValue != valueIn)) {

      this->intervalMode = modeIn;
      this->intervalValue = valueIn;

      // New source is where we currently are.
      //
      this->source = QEGraphic::calcTransitionPoint (this->source,
                                                     this->target,
                                                     this->transitionCount);

      // Set up new target and transition count down.
      //
      this->target = newTarget;
      this->transitionCount = NUMBER_TRANISTION_STEPS;
      this->determineAxis (this->source);
   }
}

//------------------------------------------------------------------------------
//
bool QEGraphic::Axis::doDynamicRescaling ()
{
   bool result = false;
   QEGraphic::DisplayRanges intermediate;

   if (this->transitionCount > 0) {
      this->transitionCount--;
      intermediate = QEGraphic::calcTransitionPoint (this->source, this->target,
                                                     this->transitionCount);
      this->determineAxis (intermediate);
      result = true;
   }

   return result;
}

//------------------------------------------------------------------------------
//
void QEGraphic::Axis::determineAxis (const QEGraphic::DisplayRanges& current)
{
   double useMin;
   double useMax;
   double useStep;
   int canvasSize;
   int number;

   if (this->isLogarithmic) {
      QEGraphic::adjustLogMinMax (current, useMin, useMax, useStep);
   } else {
      if (this->intervalMode == QEGraphic::SelectBySize) {
         switch (this->axisId) {
            case QwtPlot::xTop:
            case QwtPlot::xBottom:
               canvasSize = this->plot->canvas()->width ();
               break;

            case QwtPlot::yLeft:
            case QwtPlot::yRight:
               canvasSize = this->plot->canvas()->height ();
               break;

            default:
               canvasSize = 800;   // avoid compiler warning
               break;
         }

         number = canvasSize / MAX (1, this->intervalValue);
      } else {
         number = this->intervalValue;
      }
      QEGraphic::adjustMinMax (current, number, useMin, useMax, useStep);
   }

   this->plot->setAxisScale (this->axisId, useMin, useMax , useStep);
}


//------------------------------------------------------------------------------
//
double QEGraphic::Axis::pointToReal (const int pos) const
{
   double x;

   // Perform basic inverse transformation.
   //
   x = this->plot->invTransform (this->axisId, pos);

   if (this->isLogarithmic) {
      x = EXP10 (x);
   }

   // Scale to real world units.
   //
   x = (x - this->offset) / this->scale;

   return x;
}

//------------------------------------------------------------------------------
//
int QEGraphic::Axis::realToPoint (const double pos) const
{
   int x;
   double useX;

   // Do linear scaling (if any) followed by log scaling if required.
   //
   useX = this->scale * (double) pos + this->offset;

   if (this->isLogarithmic) {
      useX = LOG10 (useX);
   }

   // Perform basic plot transformation.
   //
   x = this->plot->transform (this->axisId, useX);

   return x;
}

//------------------------------------------------------------------------------
//
double QEGraphic::Axis::scaleValue (const double coordinate) const
{
   double x;

   x = this->scale * coordinate + this->offset;
   if (this->isLogarithmic) {
      x = LOG10 (x);
   }

   return x;
}

//------------------------------------------------------------------------------
//
void QEGraphic::Axis::setScale (const double scaleIn)
{
   this->scale = scaleIn;
}

//------------------------------------------------------------------------------
//
double QEGraphic::Axis::getScale ()
{
   return this->scale;
}

//------------------------------------------------------------------------------
//
void QEGraphic::Axis::setOffset (const double offsetIn)
{
   this->offset = offsetIn;
}

//------------------------------------------------------------------------------
//
double QEGraphic::Axis::getOffset ()
{
   return this->offset;
}

//------------------------------------------------------------------------------
//
void QEGraphic::Axis::setLogarithmic (const bool logarithmicIn)
{
   if (this->isLogarithmic != logarithmicIn) {
      this->isLogarithmic = logarithmicIn;

      // Do immediate trasition and reset
      //
      this->determineAxis (this->target);
      this->transitionCount = 0;
   }
}

//------------------------------------------------------------------------------
//
bool QEGraphic::Axis::getLogarithmic ()
{
   return this->isLogarithmic;
}

//==============================================================================
// QEGraphic class
//==============================================================================
//
QEGraphic::QEGraphic (QWidget* parent) : QWidget (parent)
{
   this->plot = new QwtPlot (parent);
   this->construct ();
}

//------------------------------------------------------------------------------
//
QEGraphic::QEGraphic (const QwtText& title, QWidget* parent) : QWidget (parent)
{
   this->plot = new QwtPlot (title, parent);
   this->construct ();
}

//------------------------------------------------------------------------------
//
void QEGraphic::construct ()
{
   // Create a louout within the containing widget.
   //
   this->layout = new QHBoxLayout (this);
   this->layout->setContentsMargins (0, 0, 0, 0);
   this->layout->setSpacing (0);
   this->layout->addWidget (this->plot);

   this->plotGrid = new QwtPlotGrid ();
   this->plotGrid->attach (this->plot);

   this->xAxis = new Axis (this->plot, QwtPlot::xBottom);
   this->yAxis = new Axis (this->plot, QwtPlot::yLeft);

   // Set defaults.
   //
   this->leftIsDefined = false;
   this->rightIsDefined = false;

   this->pen = QPen (QColor (0, 0, 0, 255));  // black
   this->hint = QwtPlotItem::RenderAntialiased;
   this->style = QwtPlotCurve::Lines;

#if QWT_VERSION < 0x060100
   this->plot->setCanvasLineWidth (1);
#endif
   this->plot->setLineWidth (1);

   this->plot->canvas()->setMouseTracking (true);
   this->plot->canvas()->installEventFilter (this);
}

//------------------------------------------------------------------------------
//
QEGraphic::~QEGraphic ()
{
   // Note: must detach curves and grids, otherwise some (older) versions of qwt
   // cause a segmentation fault when the associated QwtPolot object is deleted.
   //
   this->releaseCurves ();

   if (this->plotGrid) {
      this->plotGrid->detach();
      delete this->plotGrid;
      this->plotGrid  = NULL;
   }
}

// static int k = 100001;

//------------------------------------------------------------------------------
//
bool QEGraphic::doDynamicRescaling ()
{
   bool result;
   bool a, b;

   a = this->xAxis->doDynamicRescaling ();
   b = this->yAxis->doDynamicRescaling ();

   result = a||b;
   if (result) {
      this->plot->replot();
   }

   return result;
}

//------------------------------------------------------------------------------
//
void QEGraphic::setBackgroundColour (const QColor colour)
{
#if QWT_VERSION >= 0x060000
   this->plot->setCanvasBackground (QBrush (colour));
#else
   this->plot->setCanvasBackground (colour);
#endif
}

//------------------------------------------------------------------------------
//
void QEGraphic::setGridPen (const QPen& pen)
{
   this->plotGrid->setPen (pen);
}

//------------------------------------------------------------------------------
//
QPointF QEGraphic::pointToReal (const QPoint& pos) const
{
   double x, y;

   x = this->xAxis->pointToReal (pos.x ());
   y = this->yAxis->pointToReal (pos.y ());

   return QPointF (x, y);
}

//------------------------------------------------------------------------------
//
QPoint QEGraphic::realToPoint (const QPointF& pos) const
{
   int x, y;

   x = this->xAxis->realToPoint (pos.x ());
   y = this->yAxis->realToPoint (pos.y ());

   return QPoint (x, y);
}

//------------------------------------------------------------------------------
//
void QEGraphic::releaseCurves ()
{
   for (int j = 0; j < this->curveList.size (); j++) {
      QwtPlotCurve* curve = this->curveList.value (j);
      if (curve) {
         curve->detach ();
         delete curve;
      }
   }

   // This clears the list of (now) dangaling curve references.
   //
   this->curveList.clear ();
}

//------------------------------------------------------------------------------
//
void QEGraphic::attchOwnCurve (QwtPlotCurve* curve)
{
   if (curve) {
      curve->attach (this->plot);
      this->curveList.append (curve);
   }
}

//------------------------------------------------------------------------------
//
void QEGraphic::plotCurveData (const DoubleVector& xData, const DoubleVector& yData)
{
   QwtPlotCurve* curve;
   DoubleVector useXData;
   DoubleVector useYData;
   int n;
   curve = new QwtPlotCurve ();

   // Set curve propeties using current curve attributes.
   //
   curve->setPen (this->getCurvePen ());
   curve->setRenderHint (this->getCurveRenderHint ());
   curve->setStyle (this->getCurveStyle ());

   // Scale data as need be. Underlying Qwr widget does basic transformation,
   // but we need to do any required real world/log scaling.
   //
   useXData.clear();
   useYData.clear();
   n = MIN (xData.size (), yData.size ());
   for (int j = 0; j < n; j++) {
      double x, y;

      x = this->xAxis->scaleValue (xData.value (j));
      useXData.append (x);

      y = this->yAxis->scaleValue (yData.value (j));
      useYData.append (y);
   }

#if QWT_VERSION >= 0x060000
   curve->setSamples (useXData, useYData);
#else
   curve->setData (useXData, useYData);
#endif

   // Attach to this QwtPlot object.
   //
   this->attchOwnCurve (curve);
}

//------------------------------------------------------------------------------
//
bool QEGraphic::rightButtonPressed ()
{
   return this->rightIsDefined;
}

//------------------------------------------------------------------------------
//
bool QEGraphic::getLeftIsDefined ()
{
   return this->leftIsDefined;
}

//------------------------------------------------------------------------------
//
bool QEGraphic::getLeftIsDefined (QPoint& distance)
{
   if (this->leftIsDefined) {
      distance = this->currentPosition - this->leftOrigin;
   }
   return this->leftIsDefined;
}

//------------------------------------------------------------------------------
//
bool QEGraphic::getLeftIsDefined (QPointF& distance)
{
   if (this->leftIsDefined) {
      QPointF realCurrent = this->pointToReal (this->currentPosition);
      QPointF realLeftOrigin    = this->pointToReal (this->leftOrigin);

      distance = realCurrent - realLeftOrigin;
   }
   return this->leftIsDefined;
}

//------------------------------------------------------------------------------
//
bool QEGraphic::getLeftIsDefined (QPointF& from, QPointF& to)
{
   if (this->leftIsDefined) {
      to   = this->pointToReal (this->currentPosition);
      from = this->pointToReal (this->leftOrigin);
   }
   return this->leftIsDefined;
}

//------------------------------------------------------------------------------
//
bool QEGraphic::getRightIsDefined ()
{
   return this->rightIsDefined;
}

//------------------------------------------------------------------------------
//
bool QEGraphic::getRightIsDefined (QPoint& distance)
{
   if (this->rightIsDefined) {
      distance = this->currentPosition - this->rightOrigin;
   }
   return this->rightIsDefined;
}

//------------------------------------------------------------------------------
//
bool QEGraphic::getRightIsDefined (QPointF& distance)
{
   if (this->rightIsDefined) {
      QPointF realCurrent = this->pointToReal (this->currentPosition);
      QPointF realRightOrigin = this->pointToReal (this->rightOrigin);

      distance = realCurrent - realRightOrigin;
   }
   return this->rightIsDefined;
}

//------------------------------------------------------------------------------
//
bool QEGraphic::getRightIsDefined (QPointF& from, QPointF& to)
{
   if (this->rightIsDefined) {
      to   = this->pointToReal (this->currentPosition);
      from = this->pointToReal (this->rightOrigin);
   }
   return this->leftIsDefined;
}

//------------------------------------------------------------------------------
//
void QEGraphic::plotSelectedLeft (const bool isArea)
{
   QPointF p1;
   QPointF p2;
   DoubleVector xdata;
   DoubleVector ydata;

   if (this->leftIsDefined) {

      // Convert from canvas co-ords to real world co-ords to allow plotting
      //
      p1 = pointToReal (this->leftOrigin);
      p2 = pointToReal (this->currentPosition);

      // Form arrays.
      //
      if (isArea) {
         xdata << p1.x ();  ydata << p1.y ();
         xdata << p2.x ();  ydata << p1.y ();
         xdata << p2.x ();  ydata << p2.y ();
         xdata << p1.x ();  ydata << p2.y ();
         xdata << p1.x ();  ydata << p1.y ();
      } else {
         xdata << p1.x ();  ydata << p1.y ();
         xdata << p2.x ();  ydata << p2.y ();
      }

      this->plotCurveData (xdata, ydata);
   }
}

//------------------------------------------------------------------------------
//
void QEGraphic::plotSelectedRight (const bool isArea)
{
   QPointF p1;
   QPointF p2;
   DoubleVector xdata;
   DoubleVector ydata;

   if (this->rightIsDefined) {
      // Convert from canvas co-ords to real world co-ords to allow plotting
      //
      p1 = pointToReal (this->rightOrigin);
      p2 = pointToReal (this->currentPosition);

      // Form arrays.
      //
      if (isArea) {
         xdata << p1.x ();  ydata << p1.y ();
         xdata << p2.x ();  ydata << p1.y ();
         xdata << p2.x ();  ydata << p2.y ();
         xdata << p1.x ();  ydata << p2.y ();
         xdata << p1.x ();  ydata << p1.y ();
      } else {
         xdata << p1.x ();  ydata << p1.y ();
         xdata << p2.x ();  ydata << p2.y ();
      }

      this->plotCurveData (xdata, ydata);
   }
}

//------------------------------------------------------------------------------
//
QPoint QEGraphic::pixelDistance (const QPointF& from, const QPointF& to)
{
   QPoint pointFrom = this->realToPoint (from);
   QPoint pointTo = this->realToPoint (to);
   return pointTo - pointFrom;
}

//------------------------------------------------------------------------------
//
bool QEGraphic::eventFilter (QObject* obj, QEvent* event)
{
   const QEvent::Type type = event->type ();
   QMouseEvent* mouseEvent = NULL;
   QWheelEvent* wheelEvent = NULL;

   switch (type) {

      case QEvent::MouseButtonPress:
         mouseEvent = static_cast<QMouseEvent *> (event);
         if (obj == this->plot->canvas ()) {
            // save where we are.
            //
            this->currentPosition = mouseEvent->pos ();
            switch (mouseEvent->button ()) {
               case Qt::LeftButton:
                  this->leftOrigin = mouseEvent->pos ();
                  this->leftIsDefined = true;
                  emit mouseMove (this->pointToReal (this->currentPosition));
                  return true;  // we have handled this mouse press
                  break;

               case Qt::RightButton:
                  this->rightOrigin = mouseEvent->pos ();
                  this->rightIsDefined = true;
                  emit mouseMove (this->pointToReal (this->currentPosition));
                  return true;  // we have handled this mouse press
                  break;

               default:
                  break;
            }
         }
         break;


      case QEvent::MouseButtonRelease:
         mouseEvent = static_cast<QMouseEvent *> (event);
         if (obj == this->plot->canvas ()) {
            // save where we are.
            //
            this->currentPosition = mouseEvent->pos ();

            switch (mouseEvent->button ()) {
               case Qt::LeftButton:
                  if (this->leftIsDefined) {
                     this->leftIsDefined = false;
                     emit leftSelected (this->pointToReal (this->leftOrigin),
                                        this->pointToReal (this->currentPosition));
                     emit mouseMove (this->pointToReal (this->currentPosition));
                     return true;  // we have handled this mouse press
                  }
                  break;

               case Qt::RightButton:
                  if (this->rightIsDefined) {
                     this->rightIsDefined = false;
                     emit rightSelected (this->pointToReal (this->rightOrigin),
                                         this->pointToReal (this->currentPosition));
                     emit mouseMove (this->pointToReal (this->currentPosition));
                     return true;  // we have handled this mouse press
                  }
                  break;

               default:
                  break;
            }
         }
         break;

      case QEvent::MouseMove:
         mouseEvent = static_cast<QMouseEvent *> (event);
         if (obj == this->plot->canvas ()) {
            // save where we are.
            //
            this->currentPosition = mouseEvent->pos ();

            // Convert to real world coordinates and send to amyome interested.
            //
            emit mouseMove (this->pointToReal (this->currentPosition));

            return true;  // we have handled move nouse event
         }
         break;

      case QEvent::Wheel:
         wheelEvent = static_cast<QWheelEvent *> (event);
         if (obj == this->plot->canvas ()) {

            emit wheelRotate (this->pointToReal (this->currentPosition),
                              wheelEvent->delta ());

            return true;  // we have handled wheel event
         }
         break;

      default:
         break;

   }

   return false;
}

//------------------------------------------------------------------------------
//
void QEGraphic::setXRange (const double min, const double max,
                           const AxisMajorIntervalModes mode, const int value)
{
   this->xAxis->setRange (min, max, mode, value);
}


//------------------------------------------------------------------------------
//
void QEGraphic::setYRange (const double min, const double max,
                           const AxisMajorIntervalModes mode, const int value)
{
   this->yAxis->setRange (min, max, mode, value);
}

//------------------------------------------------------------------------------
//
void QEGraphic::replot ()
{
   this->plot->replot();
}

//------------------------------------------------------------------------------
//
void QEGraphic::setCurvePen (const QPen& penIn)
{
   this->pen = penIn;
}

QPen QEGraphic::getCurvePen () {
   return this->pen;
}

//------------------------------------------------------------------------------
//
void QEGraphic::setCurveRenderHint (const QwtPlotItem::RenderHint hintIn)
{
   this->hint = hintIn;
}

QwtPlotItem::RenderHint QEGraphic::getCurveRenderHint ()
{
   return this->hint;
}

//------------------------------------------------------------------------------
//
void QEGraphic::setCurveStyle (const QwtPlotCurve::CurveStyle styleIn)
{
   this->style = styleIn;
}

QwtPlotCurve::CurveStyle QEGraphic::getCurveStyle ()
{
   return this->style;
}

//------------------------------------------------------------------------------
// static
//------------------------------------------------------------------------------
//
QEGraphic::DisplayRanges QEGraphic::calcTransitionPoint (const DisplayRanges& start,
                                                         const DisplayRanges& finish,
                                                         const int step)
{
   QEGraphic::DisplayRanges result;

   if (step <= 0) {
      result = finish;
   } else if (step >= NUMBER_TRANISTION_STEPS) {
      result = start;
   } else  {

      // Truely in transition - perform a linear interpolation.
      //
      const double s = double (step) / (double) NUMBER_TRANISTION_STEPS;
      const double f = 1.0 - s;

      result.minimum = (s * start.minimum) + (f * finish.minimum);
      result.maximum = (s * start.maximum) + (f * finish.maximum);
   }
   return result;
}

//------------------------------------------------------------------------------
//
void QEGraphic::adjustLogMinMax (const DisplayRanges& useRange,
                                 double& minOut, double& maxOut, double& majorOut)
{
   minOut = floor (LOG10 (useRange.minimum));
   maxOut = ceil  (LOG10 (useRange.maximum));
   if ((maxOut - minOut) >= 16) {
      majorOut = 2.0;
   } else {
      majorOut = 1.0;
   }
}

//------------------------------------------------------------------------------
// static
//
void QEGraphic::adjustMinMax (const DisplayRanges& useRange, const int number,
                              double& minOut, double& maxOut, double& majorOut)
{
   // The compiler does a better job of evaluating these constants and
   // minimising rounding errors than if self generated by the application.
   //
   static const double majorValues [] = {
      1.0e-12,  2.0e-12,  5.0e-12,    1.0e-11,  2.0e-11,  5.0e-11,
      1.0e-10,  2.0e-10,  5.0e-10,    1.0e-9,   2.0e-9,   5.0e-9,
      1.0e-8,   2.0e-8,   5.0e-8,     1.0e-7,   2.0e-7,   5.0e-7,
      1.0e-6,   2.0e-6,   5.0e-6,     1.0e-5,   2.0e-5,   5.0e-5,
      1.0e-4,   2.0e-4,   5.0e-4,     1.0e-3,   2.0e-3,   5.0e-3,
      1.0e-2,   2.0e-2,   5.0e-2,     1.0e-1,   2.0e-1,   5.0e-1,
      1.0e+0,   2.0e+0,   5.0e+0,     1.0e+1,   2.0e+1,   5.0e+1,
      1.0e+2,   2.0e+2,   5.0e+2,     1.0e+3,   2.0e+3,   5.0e+3,
      1.0e+4,   2.0e+4,   5.0e+4,     1.0e+5,   2.0e+5,   5.0e+5,
      1.0e+6,   2.0e+6,   5.0e+6,     1.0e+7,   2.0e+7,   5.0e+7,
      1.0e+8,   2.0e+8,   5.0e+8,     1.0e+9,   2.0e+9,   5.0e+9,
      1.0e+10,  2.0e+10,  5.0e+10,    1.0e+11,  2.0e+11,  5.0e+11,
      1.0e+12,  2.0e+12,  5.0e+12,    1.0e+13,  2.0e+13,  5.0e+13,
      1.0e+14,  2.0e+14,  5.0e+14,    1.0e+15,  2.0e+15,  5.0e+15,
      1.0e+16,  2.0e+16,  5.0e+16,    1.0e+17,  2.0e+17,  5.0e+17,
      1.0e+18,  2.0e+18,  5.0e+18,    1.0e+19,  2.0e+19,  5.0e+19,
      1.0e+20,  2.0e+20,  5.0e+20,    1.0e+21,  2.0e+21,  5.0e+21,
      1.0e+22,  2.0e+22,  5.0e+22,    1.0e+23,  2.0e+23,  5.0e+23,
      1.0e+24,  2.0e+24,  5.0e+24,    1.0e+25,  2.0e+25,  5.0e+25
   };

   double major;
   double minor;
   int s;
   int p, q;

   // Find estimated major value - use size (width or height) to help here.
   //
   major = (useRange.maximum - useRange.minimum) / MAX (number, 2);

   // Round up major to next standard value.
   //
   s = major <= 1.0 ? 0 : 36;  // short cut
   while ((major > majorValues [s]) &&
          ((s + 1) < ARRAY_LENGTH (majorValues))) s++;

   majorOut = major = majorValues [s];

   if ((s%3) == 1) {
      // Is a 2.0eN number.
      minor = major / 4.0;
   } else {
      // Is a 1.0eN or 5.0eN number.
      minor = major / 5.0;
   }

   // Determine minOut and maxOut such that they are both exact multiples of
   // minor and that:
   //
   //  minOut <= minIn <= maxIn << maxOut
   //
   p = (int) (useRange.minimum / minor);
   if ((p * minor) > useRange.minimum) p--;

   q = (int) (useRange.maximum / minor);
   if ((q * minor) < useRange.maximum) q++;

   q = MAX (q, p+1);   // Ensure p < q

   // Extend lower/upper limit to include 0 if min < 5% max
   //
   if ((p > 0) && (q > 20*p)) {
      p = 0;
   } else if ((q < 0) && (p < 20*q)) {
      q = 0;
   }

   // Subtract/add tolerance as Qwt Axis ploting of minor ticks a bit slack.
   //
   minOut = ((double)p - 0.05) * minor;
   maxOut = ((double)q + 0.05) * minor;
}

// end
