/*  QEGraphic.h
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

#include <QEvent>
#include <QHBoxLayout>
#include <QList>
#include <QObject>
#include <QWidget>
#include <QVector>

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>

#include <QEPluginLibrary_global.h>
#include <QEDisplayRanges.h>

#ifndef QE_GRAPHIC_H
#define QE_GRAPHIC_H

/// Provides a basic wrapper around QwtPlot, which:
///
/// a) Allocates and attaches curves and grids, and releases these
///    on delete, and releases curves on request;
///
/// b) Interprets mouse events with real world co-ordinates;
///
/// c) Provides a log scale mode (X and/or Y);
///
/// d) Provides a consistant means to round down/up min/max values and the
///    selection of a major interval value, e.g.:
///    2.1 .. 7.83 (user min/max) =>  2.0 .. 8.0, 1.0  (display min/max, major);
///
/// e) Standardised mouse and wheel zoomimg;
///
/// f) Smart axis re-scaling; and
///
/// g) Provides wrapper functions to hide QWT version API changes.
///
class QEPLUGINLIBRARYSHARED_EXPORT QEGraphic : public QWidget {
   Q_OBJECT
public:
   typedef QVector<double> DoubleVector;

   enum AxisMajorIntervalModes {
      SelectByNumber,  // (span / value)           used for estimated major value
      SelectBySize };  // (span / (size / value))  used for estimated major value

   explicit QEGraphic (QWidget* parent = 0);
   explicit QEGraphic (const QString &title, QWidget* parent = 0);
   ~QEGraphic ();

   // Allow access to the inner QwtPlot object - allows direct access if needed.
   //
   QwtPlot* getPlot () { return this->plot; }

   void releaseCurves ();   // call before replotting.

   // User defined curve attached to the internal QwtPlot object.
   // Will be released by releaseCurves.
   //
   void attchOwnCurve (QwtPlotCurve* curve);

   void setBackgroundColour (const QColor colour);

   void setGridPen (const QPen& pen);

   // Allocates a curve, sets current curve attibutes and attaches to plot.
   //
   void plotCurveData (const DoubleVector& xData, const DoubleVector& yData);

   void setXRange (const double min, const double max, const AxisMajorIntervalModes mode, const int value);
   void setYRange (const double min, const double max, const AxisMajorIntervalModes mode, const int value);

   // Progresses any on-going dynamic axis rescaling.
   // Return true when this is in progress.
   //
   bool doDynamicRescaling ();

   void replot ();   // calls inner replot

   bool rightButtonPressed ();   // to allow inhibition of context menu.

   // Overloaded functions to extract definitions.
   // In distance parameter x, y is to be interpreted as dx, dy
   //
   bool getLeftIsDefined ();
   bool getLeftIsDefined (QPoint& distance);
   bool getLeftIsDefined (QPointF& distance);
   bool getLeftIsDefined (QPointF& from, QPointF& to);

   // Overloaded functions to extract right definition.
   //
   bool getRightIsDefined ();
   bool getRightIsDefined (QPoint& distance);
   bool getRightIsDefined (QPointF& distance);
   bool getRightIsDefined (QPointF& from, QPointF& to);

   // Draw either a rectangle (when isArea true) or line from origin to the
   // current position. Uses the current curve attributes.
   //
   void plotSelectedLeft  (const bool isArea = true);
   void plotSelectedRight (const bool isArea = false);

   // Draws cross hairs at current position
   // Uses the current curve attributes.
   //
   void plotCrossHairs ();

   // Returns the pixel distance between two real points.
   //
   QPoint pixelDistance (const QPointF& from, const QPointF& to);

   // Set and get axis attribute functions
   //
   void setAxisEnableX (const bool enable) { this->xAxis->setAxisEnable (enable);  }
   bool getAxisEnableX () const            { return this->xAxis->getAxisEnable (); }

   void setAxisEnableY (const bool enable) { this->xAxis->setAxisEnable (enable);  }
   bool getAxisEnableY () const            { return this->yAxis->getAxisEnable (); }

   void setXScale (const double scale) { this->xAxis->setScale (scale);    }
   double getXScale () const           { return this->xAxis->getScale ();  }

   void setXOffset (const double offset) { this->xAxis->setOffset (offset);   }
   double getXOffset () const            { return this->xAxis->getOffset ();  }

   void setXLogarithmic (const bool isLog) { this->xAxis->setLogarithmic (isLog);   }
   bool getXLogarithmic () const           { return this->xAxis->getLogarithmic (); }

   void setYScale (const double scale) { this->yAxis->setScale (scale);    }
   double getYScale () const           { return this->yAxis->getScale ();  }

   void setYOffset (const double offset) { this->yAxis->setOffset (offset);   }
   double getYOffset () const            { return this->yAxis->getOffset ();  }

   void setYLogarithmic (const bool isLog) { this->yAxis->setLogarithmic (isLog);   }
   bool getYLogarithmic () const           { return this->yAxis->getLogarithmic (); }

   // Set and get current curve attributes.
   // These are used for internally allocated curves.
   //
   void setCurvePen (const QPen& pen);
   QPen getCurvePen ();

   void setCurveRenderHint (const QwtPlotItem::RenderHint hint);
   QwtPlotItem::RenderHint getCurveRenderHint ();

   void setCurveStyle (const QwtPlotCurve::CurveStyle style);
   QwtPlotCurve::CurveStyle getCurveStyle ();

   // Utility functions.
   //
   // Converts between pixel coords to real world coords taking into
   // account any scaling and/or logarithic scaling.
   //
   QPointF pointToReal (const QPoint& pos) const;
   QPoint realToPoint (const QPointF& pos) const;

signals:
   void mouseMove     (const QPointF& posn);
   void wheelRotate   (const QPointF& posn, const int delta);

   // For left and right buttons respectively, provides down (from) mouse
   // position and current (to) mouse position in user coordinates.
   //
   void leftSelected  (const QPointF& from, const QPointF& to);
   void rightSelected (const QPointF& from, const QPointF& to);

protected:
   bool eventFilter (QObject *obj, QEvent *event);

private:
   // Handle each axis in own class.
   //
   class Axis {
   public:
      explicit Axis (QwtPlot* plot, const int axisId);
      ~Axis ();

      void setRange (const double min, const double max,
                     const AxisMajorIntervalModes mode, const int value);
      void getRange (double& min, double& max);
      bool doDynamicRescaling ();
      void determineAxis (const QEDisplayRanges& current);

      // Converts between pixel coords to real world coords taking into
      // account any scaling and/or logarithic scaling.
      //
      double pointToReal (const int pos) const;
      int realToPoint (const double pos) const;

      double scaleValue (const double coordinate) const;

      void setAxisEnable (const bool axisEnable);
      bool getAxisEnable () const;

      void setScale (const double scale);
      double getScale () const;

      void setOffset (const double offset);
      double getOffset () const;

      void setLogarithmic (const bool isLogarithmic);
      bool getLogarithmic () const;

   private:
      QwtPlot* plot;
      int axisId;
      QEDisplayRanges source;
      QEDisplayRanges target;
      AxisMajorIntervalModes intervalMode;
      int intervalValue;
      int transitionCount;
      bool axisEnabled;

      bool isLogarithmic;   // vs. Linear

      // Data scaling x' = mx + c. This is applied before any log10 scaling.
      // Allows axis scale to be different units to plot scale, e.g. minutes vs. seconds
      double scale;     // m
      double offset;    // c

      double useMin;
      double useMax;
      double useStep;
   };

   void construct ();

   // Steps go from NUMBER_TRANISTION_STEPS (at start) down to 0 (at finish).
   //
   static QEDisplayRanges calcTransitionPoint (const QEDisplayRanges& start,
                                               const QEDisplayRanges& finish,
                                               const int step);

   Axis* xAxis;
   Axis* yAxis;

   QHBoxLayout *layout;
   QwtPlot* plot;
   QwtPlotGrid* plotGrid;

   // Keep a list of allocated curves so that we can track and delete them.
   //
   typedef QList<QwtPlotCurve*> CurveList;
   CurveList curveList;

   // Curve attributes.
   //
   QPen pen;
   QwtPlotItem::RenderHint hint;
   QwtPlotCurve::CurveStyle style;

   QPoint currentPosition;         // current mouse postion

   QPoint leftOrigin;              // point at which left button pressed.
   bool   leftIsDefined;           //

   QPoint rightOrigin;             // point at which right button pressed.
   bool   rightIsDefined;          //
};

# endif  // QE_GRAPHIC_H
