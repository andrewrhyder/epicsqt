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
 *  Copyright (c) 2013, 2014 Australian Synchrotron.
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
#include <QTimer>
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

class QEGraphicMarkup;  // differed declaration

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
/// f) Smart axis re-scaling;
///
/// g) Provides markups; and
///
/// h) Provides wrapper functions to hide QWT version API changes.
///
class QEPLUGINLIBRARYSHARED_EXPORT QEGraphic : public QWidget {
   Q_OBJECT
public:
   typedef QVector<double> DoubleVector;

   // Determines how the associated value is used to find and estimated major value.
   // The estimated value is then rounded to something more appropriate
   //
   enum AxisMajorIntervalModes {
      SelectByValue,   // estimated major interval = (max - min) / value.
      SelectBySize };  // estimated major interval = ((max - min) / (widget size / value)),
                       // i.e. value represents major interval expressed as a pixel size

   explicit QEGraphic (QWidget* parent = 0);
   explicit QEGraphic (const QString &title, QWidget* parent = 0);
   ~QEGraphic ();

   // Call before any replotting, releases all curves from previous plot.
   //
   void releaseCurves ();

   // User defined curve attached to the internal QwtPlot object.
   // Will be released by releaseCurves.
   //
   void attchOwnCurve (QwtPlotCurve* curve);

   void setBackgroundColour (const QColor colour);

   void setGridPen (const QPen& pen);

   void setCrosshairsVisible (const bool isVisible);

   // Allocates a curve, sets current curve attibutes and attaches to plot.
   //
   void plotCurveData (const DoubleVector& xData, const DoubleVector& yData);

   void setXRange (const double min, const double max, const AxisMajorIntervalModes mode, const int value, const bool immediate);
   void setYRange (const double min, const double max, const AxisMajorIntervalModes mode, const int value, const bool immediate);

   // Last call - renders all curves defined since call to releaseCurves.
   // Calls inner QwtPlot replot.
   //
   void replot ();

   bool rightButtonPressed () const;    // to allow inhibition of context menu.
   bool getSlopeIsDefined (QPointF& slope) const;

   // Returns the pixel distance between two real points.
   //
   QPoint pixelDistance (const QPointF& from, const QPointF& to) const;

   // Set and get axis attribute functions
   //
   void setAxisEnableX (const bool enable) { this->xAxis->setAxisEnable (enable);  }
   bool getAxisEnableX () const            { return this->xAxis->getAxisEnable (); }

   void setAxisEnableY (const bool enable) { this->xAxis->setAxisEnable (enable);  }
   bool getAxisEnableY () const            { return this->yAxis->getAxisEnable (); }

   // Scale and offset scale the x and y data beffor ploting, i.e. allows different
   // axis and data coordinates. For example, with X scale set to 1/60, data could be
   // expressed in seconds, but (more conviently) have time axis is minutes.
   //
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
   QPen getCurvePen () const;

   void setCurveBrush (const QBrush& brush);
   QBrush getCurveBrush () const;

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
   void areaDefinition (const QPointF& from, const QPointF& to);
   void lineDefinition (const QPointF& from, const QPointF& to);
   void crosshairsMove (const QPointF& posn);

protected:
   void canvasMousePress (QMouseEvent* mouseEvent);
   void canvasMouseRelease (QMouseEvent* mouseEvent);
   void canvasMouseMove (QMouseEvent* mouseEvent, const bool isButtonAction);
   bool eventFilter (QObject *obj, QEvent *event);

private:
   // Handle each axis in own class.
   //
   class Axis {
   public:
      explicit Axis (QwtPlot* plot, const int axisId);
      ~Axis ();

      void setRange (const double min, const double max,
                     const AxisMajorIntervalModes mode, const int value,
                     const bool immediate);
      void getRange (double& min, double& max);
      bool doDynamicRescaling ();
      void determineAxisScale ();

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
      QEDisplayRanges source;    // where we started from
      QEDisplayRanges target;    // wehere we are going
      QEDisplayRanges current;   // where we are now
      int transitionCount;
      AxisMajorIntervalModes intervalMode;
      int intervalValue;
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

   // Progresses any on-going dynamic axis rescaling.
   // Return true when this is in progress.
   //
   bool doDynamicRescaling ();

   QEGraphicMarkup* mouseIsOverMarkup (); // uses cursor position to find closest, if any markup
   void plotMarkups ();     // calls each markup's plot functions which call plotMarkupCurveData.
   void plotMarkupCurveData (const DoubleVector& xData, const DoubleVector& yData);
   QwtPlotCurve* createCurveData (const DoubleVector& xData, const DoubleVector& yData);
   void graphicReplot ();   // relases and replots markups, then calls QwtPlot replot

   Axis* xAxis;
   Axis* yAxis;
   QEGraphicMarkup* markups [11];          // set of available markups.

   QHBoxLayout *layout;
   QwtPlot* plot;
   QwtPlotGrid* plotGrid;
   QTimer* tickTimer;

   // Keep a list of allocated curves so that we can track and delete them.
   //
   typedef QList<QwtPlotCurve*> CurveList;
   CurveList userCurveList;
   CurveList markupCurveList;
   void releaseCurveList (CurveList& list);

   // Curve attributes.
   //
   QPen pen;
   QBrush brush;
   QwtPlotItem::RenderHint hint;
   QwtPlotCurve::CurveStyle style;
   QPointF realMousePosition;

   bool   rightIsDefined;          // true when right button pressed

private slots:
   void tickTimeout ();

   // The price we pay for separate classes is we have to befriend them all.
   //
   friend class QEGraphicMarkup;
   friend class QEGraphicAreaMarkup;
   friend class QEGraphicLineMarkup;
   friend class QEGraphicCrosshairsMarkup;
   friend class QEGraphicHorizontalMarkup;
   friend class QEGraphicVerticalMarkup;
};

# endif  // QE_GRAPHIC_H