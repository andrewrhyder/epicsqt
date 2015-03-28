/*  QEGraphicMarkup.cpp
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2014 Australian Synchrotron.
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <QDebug>
#include <QColor>
#include <QEPlatform.h>
#include <QECommon.h>
#include <QEGraphic.h>
#include "QEGraphicMarkup.h"

#define DEBUG qDebug () << "QEGraphicMarkup" << __FUNCTION__ << __LINE__ << "::"

// Allowable distance (in pixels) from cursor to object which will still be considered 'over'.
// Plus default distance when not over.
//
#define OVER_TOLERANCE       6
#define NOT_OVER_DISTANCE    1000


//=============================================================================
// QEGraphicMarkup - base class
//=============================================================================
//
QEGraphicMarkup::QEGraphicMarkup (QEGraphicNames::Markups markupIn, QEGraphic* ownerIn) :
    markup (markupIn)
{
   this->owner = ownerIn;
   this->positon = QPointF (0.0, 0.0);
   this->inUse = false;
   this->visible = false;
   this->enabled = false;
   this->selected = false;
   this->cursor = Qt::CrossCursor;
   this->activationButton = Qt::LeftButton;

   this->positon = QPointF (0.0, 0.0);
   this->pen.setColor (QColor (0,0,0));
   this->pen.setStyle (Qt::SolidLine);
   this->pen.setWidth (1);
}

//-----------------------------------------------------------------------------
//
QEGraphicMarkup::~QEGraphicMarkup ()
{
}

//-----------------------------------------------------------------------------
//
void QEGraphicMarkup::setCurrentPosition (const QPointF& currentPositionIn)
{
   this->positon = currentPositionIn;
}

//-----------------------------------------------------------------------------
//
QPointF QEGraphicMarkup::getCurrentPosition () const
{
   return this->positon;
}

//-----------------------------------------------------------------------------
//
QCursor QEGraphicMarkup::getCursor () const
{
   return this->cursor;
}

//-----------------------------------------------------------------------------
//
bool QEGraphicMarkup::isOver (const QPointF& /* point */, int& distance) const
{
   distance = NOT_OVER_DISTANCE;
   return false;
}

//-----------------------------------------------------------------------------
//
bool QEGraphicMarkup::isOverHere (const QPointF& here, const QPointF& point, int& distance) const
{
   const QPoint s = this->getOwner()->pixelDistance (here, point);
   const int dx = s.x ();
   const int dy = s.y ();
   const int adx = ABS (dx);
   const int ady = ABS (dy);
   bool result;

   if (this->isVisible () && (adx <= OVER_TOLERANCE) && (ady <= OVER_TOLERANCE)) {
      result = true;
      distance = MIN (adx, ady);
   } else {
      result = false;
      distance = NOT_OVER_DISTANCE;    // ensure function not erroneous.
   }

   return result;
}

//-----------------------------------------------------------------------------
//
void QEGraphicMarkup::plot ()
{
   if (this->visible) {
      QPen   savedPen = this->owner->getCurvePen ();
      QBrush savedBrush = this->owner->getCurveBrush ();
      this->plotMarkup ();
      this->owner->setCurvePen   (savedPen);
      this->owner->setCurveBrush (savedBrush);
   }
}

//-----------------------------------------------------------------------------
//
void QEGraphicMarkup::plotCurve (const QEGraphicNames::DoubleVector& xData,
                                 const QEGraphicNames::DoubleVector& yData)
{
   this->owner->setCurvePen   (this->pen);
   this->owner->setCurveBrush (this->brush);
   this->owner->plotMarkupCurveData (xData, yData);
}

//-----------------------------------------------------------------------------
//
QEGraphic* QEGraphicMarkup::getOwner () const
{
   return this->owner;
}

//-----------------------------------------------------------------------------
//
void QEGraphicMarkup::setInUse (const bool inUseIn)
{
   this->inUse = inUseIn;
   if (!this->inUse) this->setVisible (false);  // if not in use, cannot be visible.
}

//-----------------------------------------------------------------------------
//
bool QEGraphicMarkup::isInUse () const
{
   return this->inUse;
}

//-----------------------------------------------------------------------------
//
void QEGraphicMarkup::setVisible (const bool isVisibleIn)
{
   this->visible = isVisibleIn;
   if (!this->visible) this->setEnabled (false);   // if not visible, cannot be enabled.
}

//-----------------------------------------------------------------------------
//
bool QEGraphicMarkup::isVisible () const
{
   return this->visible;
}

//-----------------------------------------------------------------------------
//
void QEGraphicMarkup::setEnabled (const bool isEnabledIn)
{
   this->enabled = isEnabledIn;
   if (!this->enabled) this->setSelected (false);   // if not enabled, cannot be selected.
}

//-----------------------------------------------------------------------------
//
bool QEGraphicMarkup::isEnabled () const
{
   return this->enabled;
}

//-----------------------------------------------------------------------------
//
void QEGraphicMarkup::setSelected (const bool selectedIn)
{
   this->selected = selectedIn;
}

//-----------------------------------------------------------------------------
//
bool QEGraphicMarkup::isSelected () const
{
   return this->selected;
}

//-----------------------------------------------------------------------------
//
void QEGraphicMarkup::emitCurrentPostion ()
{
   if (this->isSelected () && this->owner) {
      emit this->owner->markupMove (this->markup, this->positon);
   }
}

//-----------------------------------------------------------------------------
// Place holders
//
void QEGraphicMarkup::mousePress (const QPointF&, const Qt::MouseButton) { }
void QEGraphicMarkup::mouseRelease (const QPointF&, const Qt::MouseButton) { }
void QEGraphicMarkup::mouseMove (const QPointF&) {}


//=============================================================================
// QEGraphicAreaMarkup
//=============================================================================
//
QEGraphicAreaMarkup::QEGraphicAreaMarkup (QEGraphic* ownerIn) :
   QEGraphicMarkup (QEGraphicNames::Area, ownerIn)
{
   this->pen.setColor (QColor (0xC08080));   // redish gray
   this->origin = QPointF (0.0, 0.0);
}

//-----------------------------------------------------------------------------
//
void QEGraphicAreaMarkup::mousePress (const QPointF& realMousePosition, const Qt::MouseButton button)
{
   if (!this->isInUse()) return;

   if (button == this->activationButton) {
      this->origin = realMousePosition;
      this->positon = realMousePosition;
      this->setVisible (true);
      this->emitCurrentPostion ();
   }
}

//-----------------------------------------------------------------------------
//
void QEGraphicAreaMarkup::mouseRelease (const QPointF& realMousePosition, const Qt::MouseButton button)
{
   if (button == this->activationButton) {
      this->positon = realMousePosition;
      if (this->isValidArea ()) {
         emit this->getOwner()->areaDefinition (this->origin, this->positon);
      }
      this->setSelected (false);
      this->setVisible (false);
   }
}

//-----------------------------------------------------------------------------
//
void QEGraphicAreaMarkup::mouseMove  (const QPointF& realMousePosition)
{
   this->positon = realMousePosition;
   bool valid = this->isValidArea ();
   this->pen.setColor (valid ? QColor (0x60E060)    // greenish
                             : QColor (0xC08080));  // redish gray
   this->emitCurrentPostion ();
}

//-----------------------------------------------------------------------------
// Must be distinctly tall and thin or wide and short.
//
bool QEGraphicAreaMarkup::isValidArea () const
{
   const int minDiff = 8;
   const QPoint diff = this->getOwner()->pixelDistance (this->origin, this->positon);

   bool xokay = ((diff.x () > minDiff) && (diff.x () > ABS (3 * diff.y ())));
   bool yokay = ((diff.y () > minDiff) && (diff.y () > ABS (3 * diff.x ())));

   return xokay || yokay;
}

//-----------------------------------------------------------------------------
//
void QEGraphicAreaMarkup::plotMarkup ()
{
   QEGraphicNames::DoubleVector xdata;
   QEGraphicNames::DoubleVector ydata;

   xdata << this->positon.x ();  ydata << this->positon.y ();
   xdata << this->origin.x ();   ydata << this->positon.y ();
   xdata << this->origin.x ();   ydata << this->origin.y ();
   xdata << this->positon.x ();  ydata << this->origin.y ();
   xdata << this->positon.x ();  ydata << this->positon.y ();

   this->plotCurve (xdata, ydata);
}


//=============================================================================
// QEGraphicLineMarkup
//=============================================================================
//
QEGraphicLineMarkup::QEGraphicLineMarkup (QEGraphic* ownerIn) :
   QEGraphicMarkup (QEGraphicNames::Line, ownerIn)
{
   this->pen.setColor(QColor (0x80C0E0));  // blueish
   this->origin = QPointF (0.0, 0.0);
   this->activationButton = MIDDLE_BUTTON;
}

//-----------------------------------------------------------------------------
//
QPointF QEGraphicLineMarkup::getSlope () const
{
   return this->positon - this->origin;
}

//-----------------------------------------------------------------------------
//
void QEGraphicLineMarkup::mousePress (const QPointF& realMousePosition, const Qt::MouseButton button)
{
   if (!this->isInUse()) return;

   if (button == this->activationButton) {
      this->origin = realMousePosition;
      this->positon = realMousePosition;
      this->setVisible (true);
      this->emitCurrentPostion ();
   }
}

//-----------------------------------------------------------------------------
//
void QEGraphicLineMarkup::mouseRelease (const QPointF& realMousePosition, const Qt::MouseButton button)
{
   if (button == this->activationButton) {
      this->positon = realMousePosition;
      emit this->getOwner()->lineDefinition (this->origin, this->positon);
      this->setSelected (false);
      this->setVisible (false);
   }
}

//-----------------------------------------------------------------------------
//
void QEGraphicLineMarkup::mouseMove    (const QPointF& realMousePosition)
{
   this->positon = realMousePosition;
   this->emitCurrentPostion ();
}

//-----------------------------------------------------------------------------
//
void QEGraphicLineMarkup::plotMarkup ()
{
   QEGraphicNames::DoubleVector xdata;
   QEGraphicNames::DoubleVector ydata;

   xdata << this->positon.x ();  ydata << this->positon.y ();
   xdata << this->origin.x ();   ydata << this->origin.y ();

   this->plotCurve (xdata, ydata);
}


//=============================================================================
// QEGraphicCrosshairsMarkup
//=============================================================================
//
QEGraphicCrosshairsMarkup::QEGraphicCrosshairsMarkup (QEGraphic* ownerIn) :
   QEGraphicMarkup (QEGraphicNames::CrossHair, ownerIn)
{
   this->pen.setColor (QColor (0xA0A0A0));  // light grayish
   this->cursor = Qt::PointingHandCursor;
}

//-----------------------------------------------------------------------------
//
bool QEGraphicCrosshairsMarkup::isOver (const QPointF& point, int& distance) const
{
   return this->isOverHere (this->positon, point, distance);
}

//-----------------------------------------------------------------------------
//
void QEGraphicCrosshairsMarkup::mousePress (const QPointF& realMousePosition, const Qt::MouseButton)
{
   this->positon = realMousePosition;
   this->pen.setColor (QColor (0x606060));  // dark grayish
   emit this->getOwner()->crosshairsMove (this->positon);  // depricated
   this->emitCurrentPostion ();
}

//-----------------------------------------------------------------------------
//
void QEGraphicCrosshairsMarkup::mouseRelease (const QPointF& realMousePosition, const Qt::MouseButton)
{
   this->positon = realMousePosition;
   this->setSelected (false);
   this->pen.setColor (QColor (0xA0A0A0));  // light grayish
}

//-----------------------------------------------------------------------------
//
void QEGraphicCrosshairsMarkup::mouseMove (const QPointF& realMousePosition)
{
   this->positon = realMousePosition;
   emit this->getOwner()->crosshairsMove (this->positon);  // depreciated
   this->emitCurrentPostion ();
}

//-----------------------------------------------------------------------------
//
void QEGraphicCrosshairsMarkup::setVisible (const bool visibleIn)
{
   if (!this->isInUse()) return;

   double xmin;
   double xmax;
   double ymin;
   double ymax;
   QPointF middle;

   QEGraphicMarkup::setVisible (visibleIn);

   if (visibleIn) {
      this->getOwner()->xAxis->getRange (xmin, xmax);
      this->getOwner()->yAxis->getRange (ymin, ymax);

      middle = QPointF ((xmin + xmax)/2.0, (ymin + ymax)/2.0);
      this->positon = middle;
   }
}

//-----------------------------------------------------------------------------
//
void QEGraphicCrosshairsMarkup::plotMarkup ()
{
   QEGraphicNames::DoubleVector xdata;
   QEGraphicNames::DoubleVector ydata;
   double min;
   double max;

   this->getOwner()->yAxis->getRange (min, max);
   xdata.clear ();    ydata.clear ();
   xdata << positon.x ();  ydata << min;
   xdata << positon.x ();  ydata << max;
   this->plotCurve (xdata, ydata);

   this->getOwner()->xAxis->getRange (min, max);
   xdata.clear ();  ydata.clear ();
   xdata << min;    ydata << positon.y ();
   xdata << max;    ydata << positon.y ();
   this->plotCurve (xdata, ydata);
}


//=============================================================================
// QEGraphicHVBaseMarkup
//=============================================================================
//
QEGraphicHVBaseMarkup::QEGraphicHVBaseMarkup
    (QEGraphicNames::Markups markup, QEGraphic* ownerIn) : QEGraphicMarkup (markup, ownerIn)
{
}

//-----------------------------------------------------------------------------
//
void QEGraphicHVBaseMarkup::setInUse  (const bool inUse)
{
   QEGraphicMarkup::setInUse (inUse);
   // When in use, there markups are always partially visible.
   this->setVisible (inUse);
}

//-----------------------------------------------------------------------------
//
void QEGraphicHVBaseMarkup::mousePress (const QPointF& realMousePosition, const Qt::MouseButton button)
{
   if (!this->isInUse()) return;

   switch (button) {
      case Qt::LeftButton:
         this->positon = realMousePosition;
         this->setEnabled (true);
         this->emitCurrentPostion ();
         break;

      case MIDDLE_BUTTON:
         this->setEnabled (false);
         this->setSelected (false);
         break;

      case Qt::RightButton:
         this->setSelected (false);
         break;

      default:
         break;
   }
}

//-----------------------------------------------------------------------------
//
void QEGraphicHVBaseMarkup::mouseRelease (const QPointF& realMousePosition, const Qt::MouseButton)
{
   this->positon = realMousePosition;
   this->setSelected (false);
}

//-----------------------------------------------------------------------------
//
void QEGraphicHVBaseMarkup::mouseMove (const QPointF& realMousePosition)
{
   this->positon = realMousePosition;
   this->emitCurrentPostion ();
}

//-----------------------------------------------------------------------------
//
void QEGraphicHVBaseMarkup::setColours (const unsigned int baseRGB)
{
   this->pen.setColor   (QColor (baseRGB & 0x606060));   // dark line
   this->brushDisabled = QColor (baseRGB | 0xc0c0c0);    // washed out
   this->brushEnabled  = QColor (baseRGB | 0x808080);    // normal
   this->brushSelected = QColor (baseRGB | 0x000000);    // full saturation
}

//-----------------------------------------------------------------------------
//
void QEGraphicHVBaseMarkup::plotMarkup ()
{
   QPoint item [6];
   double xmin, xmax;
   double ymin, ymax;
   QEGraphicNames::DoubleVector xdata;
   QEGraphicNames::DoubleVector ydata;
   QPointF poiF;
   QPoint  poi;
   QPointF itemF;

   this->getLine (xmin, xmax, ymin, ymax);

   if (this->isEnabled ()) {
      xdata.clear (); ydata.clear ();
      xdata << xmin; ydata << ymin;
      xdata << xmax; ydata << ymax;
      this->pen.setStyle (Qt::DashDotLine);
      this->brush.setStyle (Qt::NoBrush);
      this->plotCurve (xdata, ydata);
   }

   this->getShape (item);
   poiF = QPointF (xmax, ymax);
   poi = this->getOwner()->realToPoint (poiF);

   xdata.clear (); ydata.clear ();
   for (int j = 0; j <  ARRAY_LENGTH (item); j++) {
      itemF = this->getOwner()->pointToReal (poi + item [j]);
      xdata << itemF.x (); ydata << itemF.y ();
   }

   this->pen.setStyle (Qt::SolidLine);
   if (this->isSelected ()) {
      this->brush.setColor (this->brushSelected);
   } else if (this->isEnabled ()) {
      this->brush.setColor (this->brushEnabled);
   } else {
      this->brush.setColor (this->brushDisabled);
   }
   this->brush.setStyle (Qt::SolidPattern);
   this->plotCurve (xdata, ydata);
}


//=============================================================================
// QEGraphicHorizontalMarkup
//=============================================================================
//
QEGraphicHorizontalMarkup::QEGraphicHorizontalMarkup
   (const QEGraphicNames::Markups markup, QEGraphic* ownerIn) : QEGraphicHVBaseMarkup (markup, ownerIn)
{
   this->cursor = Qt::SplitVCursor;

   if (markup >= QEGraphicNames::HorizontalLine_3) {
      this->setColours (0xff0000);    // red (3 and 4)
   } else {
      this->setColours (0x00ff00);    // green (1 and 2)
   }
}

//-----------------------------------------------------------------------------
//
bool QEGraphicHorizontalMarkup::isOver (const QPointF& point, int& distance) const
{
   double xmin;
   double xmax;
   QPointF poiF;

   this->getOwner()->xAxis->getRange (xmin, xmax);
   if (this->isEnabled ()) {
      // Allow any x to match.
      poiF = QPointF (point.x (), this->positon.y ());
   } else {
      poiF = QPointF (xmax, this->positon.y());
   }

   return this->isOverHere (poiF, point, distance);
}

//-----------------------------------------------------------------------------
//
void QEGraphicHorizontalMarkup::getLine (double& xmin, double& xmax, double& ymin, double& ymax)
{
   this->getOwner()->xAxis->getRange (xmin, xmax);
   ymin = ymax = this->positon.y ();
}

//-----------------------------------------------------------------------------
//
void QEGraphicHorizontalMarkup::getShape (QPoint shape [])
{
   static const QPoint item [6] = { QPoint (-6, +0), QPoint (-3, +3), QPoint (+2, +3),
                                    QPoint (+2, -3), QPoint (-3, -3), QPoint (-6, +0) };

   for (int j = 0; j < ARRAY_LENGTH (item); j++) {
      shape [j] = item [j];
   }
}


//=============================================================================
// QEGraphicVerticalMarkup
//=============================================================================
//
QEGraphicVerticalMarkup::QEGraphicVerticalMarkup
   (const QEGraphicNames::Markups markup, QEGraphic* ownerIn) :
   QEGraphicHVBaseMarkup (markup, ownerIn)
{
   this->cursor = Qt::SplitHCursor;

   if (markup >= QEGraphicNames::VerticalLine_3) {
      this->setColours (0xff00ff);    // purple (3 and 4)
   } else {
      this->setColours (0x0000ff);    // blue (1 and 2)
   }
}

//-----------------------------------------------------------------------------
//
bool QEGraphicVerticalMarkup::isOver (const QPointF& point, int& distance) const
{
   double ymin;
   double ymax;
   QPointF poiF;

   this->getOwner()->yAxis->getRange (ymin, ymax);
   if (this->isEnabled ()) {
      // Allow any y to match.
      poiF = QPointF (this->positon.x (), point.y ());
   } else {
      poiF = QPointF (this->positon.x (), ymax);
   }

   return this->isOverHere (poiF, point, distance);
}

//-----------------------------------------------------------------------------
//
void QEGraphicVerticalMarkup::getLine (double& xmin, double& xmax, double& ymin, double& ymax)
{
   xmin = xmax = this->positon.x ();
   this->getOwner()->yAxis->getRange (ymin, ymax);
}

//-----------------------------------------------------------------------------
//
void QEGraphicVerticalMarkup::getShape (QPoint shape [])
{
   static const QPoint item [6] = { QPoint (+0, +6), QPoint (-3, +3), QPoint (-3, -2),
                                    QPoint (+3, -2), QPoint (+3, +3), QPoint (+0, +6) };
   for (int j = 0; j < ARRAY_LENGTH (item); j++) {
      shape [j] = item [j];
   }
}

// end
