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
   this->source.setRange (0.0, 1.0);
   this->transitionCount = 0;
   this->setRange (0.0, 1.0, QEGraphic::SelectByNumber, 8);
   this->transitionCount = 0;   // reset
}

//------------------------------------------------------------------------------
//
void QEGraphic::Axis::setRange (const double minIn, const double maxIn,
                                const AxisMajorIntervalModes modeIn, const int valueIn)
{
   QEDisplayRanges newTarget;

   newTarget.setRange (minIn, LIMIT (maxIn, minIn + MINIMUM_SPAN, minIn + MAXIMUM_SPAN));

   // Is this a significant change?
   //
   if ((this->target != newTarget) ||
       (this->intervalMode != modeIn) ||
       (this->intervalValue != valueIn)) {

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
   QEDisplayRanges intermediate;

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
void QEGraphic::Axis::determineAxis (const QEDisplayRanges& current)
{
   double useMin;
   double useMax;
   double useStep;
   int canvasSize;
   int number;

   if (this->isLogarithmic) {
      QEDisplayRanges::adjustLogMinMax (current, useMin, useMax, useStep);
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
      QEDisplayRanges::adjustMinMax (current, number, useMin, useMax, useStep);
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
QEDisplayRanges QEGraphic::calcTransitionPoint (const QEDisplayRanges& start,
                                                const QEDisplayRanges& finish,
                                                const int step)
{
   QEDisplayRanges result;
   double minimum;
   double maximum;

   if (step <= 0) {
      result = finish;
   } else if (step >= NUMBER_TRANISTION_STEPS) {
      result = start;
   } else  {

      // Truely in transition - perform a linear interpolation.
      //
      const double s = double (step) / (double) NUMBER_TRANISTION_STEPS;
      const double f = 1.0 - s;

      minimum = (s * start.getMinimum ()) + (f * finish.getMinimum ());
      maximum = (s * start.getMaximum ()) + (f * finish.getMaximum ());

      result.setRange (minimum, maximum);
   }
   return result;
}

// end
