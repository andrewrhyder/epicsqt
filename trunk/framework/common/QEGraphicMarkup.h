/*  QEGraphicMarkup.h
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
 *  Copyright (c) 2014 Australian Synchrotron.
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QE_GRAPHIC_MARKUP_H
#define QE_GRAPHIC_MARKUP_H

#include <QCursor>
#include <QObject>
#include <QPen>
#include <QPoint>
#include <QEGraphic.h>

//-----------------------------------------------------------------------------
// Base class for all QEGraphic markups.
//
class QEGraphicMarkup {
public:
   explicit QEGraphicMarkup (QEGraphic* owner);
   virtual ~QEGraphicMarkup ();

   virtual void setVisible (const bool visible);
   virtual bool isVisible () const;

   virtual void setEnabled (const bool enabled);
   virtual bool isEnabled () const;

   virtual void setSelected (const bool selected);
   virtual bool isSelected () const;

   virtual void mousePress   (const QPointF& realMousePosition, const Qt::MouseButton button);
   virtual void mouseRelease (const QPointF& realMousePosition, const Qt::MouseButton button);
   virtual void mouseMove    (const QPointF& realMousePosition);

   virtual QCursor getCursor () const;
   virtual bool isOver (const QPointF& point, int& distance) const;
   void plot ();

protected:
   QEGraphic* getOwner () const;

   // Utility function available for use by concrete class isOver functions.
   // To return true the markup must be visible and the point close to here.
   //
   bool isOverHere (const QPointF& here, const QPointF& point, int& distance) const;

   // Conveniance utility function to set owner curve pen/brush and then call
   // owner plot curve function.
   //
   void plotCurve (const QEGraphic::DoubleVector& xData,
                   const QEGraphic::DoubleVector& yData);

   // All concrete classes must provide a means to draw a markup.
   // This is only called when the markup is visible.
   //
   virtual void plotMarkup () = 0;

   QPointF current;   // notional current position
   QPen pen;
   QBrush brush;
   QCursor cursor;
   bool visible;
   bool enabled;
   bool selected;

private:
   QEGraphic* owner;
};

//-----------------------------------------------------------------------------
// Draws rectangle from origin to current (mouse position).
//
class QEGraphicAreaMarkup : public QEGraphicMarkup {
public:
   explicit QEGraphicAreaMarkup (QEGraphic* owner);

   void mousePress   (const QPointF& realMousePosition, const Qt::MouseButton button);
   void mouseRelease (const QPointF& realMousePosition, const Qt::MouseButton button);
   void mouseMove    (const QPointF& realMousePosition);

protected:
   void plotMarkup ();

private:
   bool isValidArea () const;
   QPointF origin;
};

//-----------------------------------------------------------------------------
// Draws a line from origin to current mouse position.
//
class QEGraphicLineMarkup : public QEGraphicMarkup {
public:
   explicit QEGraphicLineMarkup (QEGraphic* owner);

   QPointF getSlope () const;

   void mousePress   (const QPointF& realMousePosition, const Qt::MouseButton button);
   void mouseRelease (const QPointF& realMousePosition, const Qt::MouseButton button);
   void mouseMove    (const QPointF& realMousePosition);

protected:
   void plotMarkup ();

private:
   QPointF origin;
};

//-----------------------------------------------------------------------------
// Draws crosshairs about to current (mouse) position.
//
class QEGraphicCrosshairsMarkup : public QEGraphicMarkup {
public:
   explicit QEGraphicCrosshairsMarkup (QEGraphic* owner);

   void setVisible (const bool visible);
   bool isOver (const QPointF& point, int& distance) const;

   void mousePress   (const QPointF& realMousePosition, const Qt::MouseButton button);
   void mouseRelease (const QPointF& realMousePosition, const Qt::MouseButton button);
   void mouseMove    (const QPointF& realMousePosition);

protected:
   void plotMarkup ();
};


//-----------------------------------------------------------------------------
// (Abstract) base class for QEGraphicHorizontalMarkup/QEGraphicVerticalMarkup.
//
class QEGraphicHVBaseMarkup : public QEGraphicMarkup {
public:
   explicit QEGraphicHVBaseMarkup (QEGraphic* owner, const int instance);

   void mousePress   (const QPointF& realMousePosition, const Qt::MouseButton button);
   void mouseRelease (const QPointF& realMousePosition, const Qt::MouseButton button);
   void mouseMove    (const QPointF& realMousePosition);

protected:
   void setColours (const unsigned int baseRGB);   // Expected 0xff0000 or 0x00ff00 or 0x0000ff
   void plotMarkup ();

   virtual void getLine (double& xmin, double& xmax, double& ymin, double& ymax) = 0;
   virtual void getShape (QPoint shape []) = 0;

   int instance;
   QColor brushDisabled;
   QColor brushEnabled;
   QColor brushSelected;
};


//-----------------------------------------------------------------------------
// Draws horizontal through current position.
//
class QEGraphicHorizontalMarkup : public QEGraphicHVBaseMarkup {
public:
   explicit QEGraphicHorizontalMarkup (QEGraphic* owner, const int instance);
   bool isOver (const QPointF& point, int& distance) const;
protected:
   void getLine (double& xmin, double& xmax, double& ymin, double& ymax);
   void getShape (QPoint shape []);
};


//-----------------------------------------------------------------------------
// Draws vertical through current position.
//
class QEGraphicVerticalMarkup : public QEGraphicHVBaseMarkup {
public:
   explicit QEGraphicVerticalMarkup (QEGraphic* owner, const int instance);
   bool isOver (const QPointF& point, int& distance) const;
protected:
   void getLine (double& xmin, double& xmax, double& ymin, double& ymax);
   void getShape (QPoint shape []);
};

#endif  //  QE_GRAPHIC_MARKUP_H
