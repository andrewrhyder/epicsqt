/*  QEAnalogIndicator.cpp
 *
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
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
 *  Copyright (c) 2011
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

/*!
  This class is a analog version of the Qt progress bar widget.
 */

#include <math.h>
#include <QEAnalogIndicator.h>

#include <QDebug>
#include <QFontMetrics>
#include <QtGui>
#include <QBrush>
#include <QPen>
#include <QPoint>
#include <QPolygon>

#define DEBUG qDebug () << __FUNCTION__ << __LINE__

#define MINIMUM_SPAN        0.000001
#define RADIANS_PER_DEGREE  (M_PI / 180.0)

#define MAX(a, b)           ((a) >= (b) ? (a) : (b))
#define MIN(a, b)           ((a) <= (b) ? (a) : (b))
#define LIMIT(x,low,high)   (MAX(low, MIN(x, high)))


//------------------------------------------------------------------------------
// Constructor with no initialisation
//
QEAnalogIndicator::QEAnalogIndicator (QWidget *parent) : QWidget (parent)
{
   this->mBorderColour      = QColor (  0,   0,  96);   // dark blue
   this->mBackgroundColour  = QColor (220, 220, 220);   // light gray
   this->mForegroundColour  = QColor (128, 192, 255);   // blue
   this->mFontColour        = QColor (0,     0,   0);   // black

   this->mMinimum = 0.0;
   this->mMaximum = 100.0;
   this->mMinorInterval = 4.0;
   this->mMajorMinorRatio = 5;   // => major = 20

   this->mOrientation = Left_To_Right;
   this->mMode = Bar;
   this->mShowText = true;
   this->mShowScale = true;
   this->mLogScale = false;
   this->mValue = 0.0;
   this->mCentreAngle = 0;
   this->mSpanAngle = 180;

   // Do thsi only once, not on paintEvent as it caises another paint event.
   //
   this->setAutoFillBackground (false);
   this->setBackgroundRole (QPalette::NoRole);
}


/*! ---------------------------------------------------------------------------
 *  Define default size for this widget class.
 */
QSize QEAnalogIndicator::sizeHint () const
{
   return QSize (48, 16);
}

//------------------------------------------------------------------------------
//
double QEAnalogIndicator::safeLog (const double x)
{
   return (x >= 1.0e-20) ? log10 (x) : -20.0;
}

//------------------------------------------------------------------------------
//
double QEAnalogIndicator::calcFraction (const double x) {
   double result;

   // Calculate the fractional scale and constrain to be in range.
   //
   if (this->getLogScale ()) {
      result = (safeLog (x) - safeLog (this->mMinimum)) /
               (safeLog (this->mMaximum) - safeLog  (this->mMinimum));
   } else {
      result = (x - this->mMinimum) /
               (this->mMaximum - this->mMinimum);
   }
   result = LIMIT (result, 0.0, 1.0);

   return result;
}

//------------------------------------------------------------------------------
//
bool QEAnalogIndicator::firstValue (int & itc, double & value, bool & isMajor)
{
   double real;
   bool result;

   if (this->getLogScale ()) {
      real = 9.0 * this->safeLog (this->mMinimum);
   } else {
      real = this->mMinimum / this->getMinorInterval ();
   }

   // Use floor to round down and - 0.5 to mitigate any rounding effects.
   // Subtract an addition -1 to ensure first call to nextValue returns a
   // value no greater than the first required value.
   //
   itc = int (floor (real) - 0.5) - 1;

   result = this->nextValue (itc, value, isMajor);
   while (result && (value < this->mMinimum)) {
      result = this->nextValue (itc, value, isMajor);
   }
   return result;
}

//------------------------------------------------------------------------------
//
bool QEAnalogIndicator::nextValue  (int & itc, double & value, bool & isMajor)
{
   const int fs = 9;
   const int offset = 10000;

   int d;
   int f;

   itc++;
   if (this->getLogScale ()) {
      // Ensure round down towards -infinity
      // (Oh how I wish C/C++ has a proper "mod" operator).
      //
      d = (itc + (offset * fs)) /fs - offset;
      f = itc -(fs * d);
      value = (1.0 + f) * pow (10.0, d);
      isMajor = (f == 0);
   } else {
      value = itc * this->getMinorInterval ();
      isMajor = ((itc % this->mMajorMinorRatio) == 0);
   }
   return (value <= this->mMaximum);
}

//------------------------------------------------------------------------------
//
QColor QEAnalogIndicator::getBorderPaintColour ()
{
   return this->isEnabled () ? this->mBorderColour : QColor (160, 160, 160);
}

//------------------------------------------------------------------------------
//
QColor QEAnalogIndicator::getBackgroundPaintColour ()
{
   return this->isEnabled () ? this->mBackgroundColour : QColor (240, 240, 240);
}

//------------------------------------------------------------------------------
//
QColor QEAnalogIndicator::getForegroundPaintColour ()
{
   return this->isEnabled () ? this->mForegroundColour : QColor (220, 220, 220);
}


//------------------------------------------------------------------------------
//
QColor QEAnalogIndicator::getFontPaintColour ()
{
   return this->isEnabled () ? this->mFontColour : QColor (140, 140, 140);
}

//------------------------------------------------------------------------------
//
bool QEAnalogIndicator::isLeftRight ()
{
   return (this->mOrientation == Left_To_Right) || (this->mOrientation == Right_To_Left);
}


//------------------------------------------------------------------------------
//
void QEAnalogIndicator::drawAxis  (QPainter & painter, QRect & axis)
{
   QPen pen;

   int x_first, x_last;
   int y_first, y_last;

   int j;
   bool ok;
   bool isMajor;
   double value;

   switch (this->mOrientation) {

      case Left_To_Right:
         x_first = axis.left ();
         x_last  = axis.right ();
         y_first = axis.top ();
         y_last  = axis.top ();
         break;

      case Top_To_Bottom:
         x_first = axis.left ();
         x_last  = axis.left ();
         y_first = axis.top ();
         y_last  = axis.bottom ();
         break;

      case Right_To_Left:
         x_first = axis.right ();
         x_last  = axis.left ();
         y_first = axis.top ();
         y_last  = axis.top ();
         break;

      case Bottom_To_Top:
         x_first = axis.left ();
         x_last  = axis.left ();
         y_first = axis. bottom ();
         y_last  = axis.top ();
         break;

      default:
         // report an error??
         //
         return;
   }

   pen.setWidth (1);
   pen.setColor (this->getFontPaintColour ());
   painter.setPen (pen);

   for (ok = this->firstValue (j, value, isMajor); ok;
        ok = this->nextValue  (j, value, isMajor)) {

      double f;
      int x, y;
      QPoint p1, p2;

      f = this->calcFraction (value);

      x = int (x_first + int (f * double (x_last - x_first)));
      y = int (y_first + int (f * double (y_last - y_first)));

      p1 = QPoint (x, y);
      if (isMajor) {
         p2 = this->isLeftRight () ? QPoint (x, y + 11) :  QPoint (x + 11, y);
      }  else {
         p2 = this->isLeftRight () ? QPoint (x, y + 6) :  QPoint (x + 6, y);
      }

      painter.drawLine (p1, p2);

      if (isMajor) {
         QString vt;
         vt.sprintf ("%.1f", value);
         p2 = this->isLeftRight () ? QPoint (x, y + 18) :  QPoint (x + 18, y);
         this->drawText (painter, p2, vt, 7);
      }
   }
}

//------------------------------------------------------------------------------
//
void QEAnalogIndicator::drawOutline (QPainter & painter, QRect & outline)
{
   QPen pen;
   QBrush brush;

   pen.setWidth (1);
   pen.setColor (this->getBorderPaintColour ());
   painter.setPen (pen);

   brush.setStyle (Qt::SolidPattern);
   brush.setColor (this->getBackgroundPaintColour ());
   painter.setBrush (brush);

   painter.drawRect (outline);
}

//------------------------------------------------------------------------------
//
void QEAnalogIndicator::drawBar (QPainter & painter, QRect &area,
                                 const double fraction)
{
   int temp;
   QPen pen;
   QBrush brush;

   QRect barRect (area);

   switch (this->mOrientation) {

      case Left_To_Right:
         // Convert fractions back to pixels.
         //
         temp = int (fraction * (area.right () - area.left ()));
         barRect.setRight (area.left () + temp);
         break;

      case Top_To_Bottom:
         temp = int (fraction * (area.bottom () - area.top ()));
         barRect.setBottom (area.top () + temp);
         break;

      case Right_To_Left:
         temp = int (fraction * (area.right () - area.left ()));
         barRect.setLeft (area.right () - temp);
         break;

      case Bottom_To_Top:
         temp = int (fraction * (area.bottom () - area.top ()));
         barRect.setTop (area.bottom () - temp);
         break;

      default:
         // report an error??
         //
         return;
   }

   // barRect and paint it.
   //
   pen.setColor (this->getForegroundPaintColour ());
   pen.setWidth (1);
   painter.setPen (pen);

   brush.setStyle (Qt::SolidPattern);
   brush.setColor (this->getForegroundPaintColour() );
   painter.setBrush (brush);

   painter.drawRect (barRect);
}

//------------------------------------------------------------------------------
//
void QEAnalogIndicator::drawMarker (QPainter & painter, QRect &area, const double fraction)
{
   int span;
   int cx, cy;
   int temp;
   int t;
   int l;
   int b;
   int r;

   QPen pen;
   QBrush brush;
   QPolygon polygon;


   // Calcualate "diamond" span
   //
   if (this->isLeftRight ()) {
      span =  (area.bottom () - area.top ()) / 8;
   } else {
      span =  (area.right () - area.left ()) / 8;
   }
   span = MAX (span, 4);


   // Centre co-ordinates.
   //
   cx = (area.left () + area.right ()) / 2;
   cy = ( area.top () + area.bottom () ) / 2;

   t = area.top ();
   l = area.left ();
   b = area.bottom ();
   r = area.right ();

   switch (this->mOrientation) {

      case Left_To_Right:
         // Convert fractions back to pixels.
         //
         temp = int (fraction * (area.right () - area.left ()));
         cx = area.left () + temp;
         l = cx - span;
         r = cx + span;
         break;

      case Top_To_Bottom:
         temp = int (fraction * (area.bottom () - area.top ()));
         cy = area.top () + temp;
         t = cy - span;
         b = cy + span;
         break;

      case Right_To_Left:
         temp = int (fraction * (area.right () - area.left ()));
         cx = area.right () - temp;
         l = cx - span;
         r = cx + span;
         break;

      case Bottom_To_Top:
         temp = int (fraction * (area.bottom () - area.top ()));
         cy = area.bottom () - temp;
         t = cy - span;
         b = cy + span;
         break;

      default:
         // report an error??
         //
         return;
   }

   // Create "diamond" polygon
   //
   polygon << QPoint (l, cy);
   polygon << QPoint (cx, t);
   polygon << QPoint (r, cy);
   polygon << QPoint (cx, b);

   pen.setColor (this->getForegroundPaintColour ());
   pen.setWidth (1);
   painter.setPen (pen);

   brush.setStyle (Qt::SolidPattern);
   brush.setColor (this->getForegroundPaintColour ());
   painter.setBrush (brush);

   painter.drawPolygon (polygon);
}

//------------------------------------------------------------------------------
//
void QEAnalogIndicator::drawMeter (QPainter & painter, QRect &area, const double fraction)
{
   // Macro function to create a point based on centre positon, radius and direction (s, c).
   //
   #define RPOINT(f)  QPoint (int (centre_x + f*radius*s),  int (centre_y - f*radius*c))

   const int width_span = area.right () - area.left ();
   const int height_span = area.bottom () - area.top ();

   double centre_x, centre_y;
   double radius;
   double s, c;
   QPen pen;
   int j;
   double lowerAngle;
   double upperAngle;
   double angle;
   double minS, maxS, minC, maxC;
   double gap;
   bool ok;
   bool isMajor;
   double value;
   double f;
   QPoint p1, p2;

   // Working in degrees.
   //
   lowerAngle = this->mCentreAngle - this->mSpanAngle / 2.0;
   upperAngle = this->mCentreAngle + this->mSpanAngle / 2.0;

   // Find min and max sin/cosine so that we can find optimum centre.
   //
   minS = maxS = minC = maxC = 0.0;
   angle = lowerAngle;
   while (true) {
      s = sin (angle * RADIANS_PER_DEGREE);
      c = cos (angle * RADIANS_PER_DEGREE);

      minS = MIN (minS, s);
      maxS = MAX (maxS, s);
      minC = MIN (minC, c);
      maxC = MAX (maxC, c);

      if (angle >= upperAngle) break;

      // Determine next angle.
      //
      if      (angle < -360.0) { angle = -360.0; }
      else if (angle < -270.0) { angle = -270.0; }
      else if (angle < -180.0) { angle = -180.0; }
      else if (angle < -90.0)  { angle = -90.0;  }
      else if (angle < 0.0)    { angle =  0.0;   }
      else if (angle < 90.0)   { angle =  90.0;  }
      else if (angle < 180.0)  { angle = 180.0;  }
      else if (angle < 270.0)  { angle = 270.0;  }
      else if (angle < 360.0)  { angle = 360.0;  }

      angle = MIN (angle, upperAngle);
   }

   // Determine centre.
   //
   gap  = 6.0;     // gap around edge

   f = (-minS) / (maxS - minS);
   centre_x = gap + f * (width_span - 2.0 * gap);

   f = (+maxC) / (maxC - minC);
   centre_y = 6.0 + f * (height_span - 2.0 * gap);

   radius = MIN (width_span, height_span);
   if (maxS > 0) radius = MIN (radius, ((area.right () - gap)  - centre_x) /maxS);
   if (minS < 0) radius = MIN (radius, ((area.left () + gap)   - centre_x) /minS);
   if (maxC > 0) radius = MIN (radius, (centre_y -    (area.top () + gap)) /maxC);
   if (minC < 0) radius = MIN (radius, (centre_y - (area.bottom () - gap)) /minC);

   pen.setWidth (1);
   pen.setColor (this->getFontPaintColour ());
   painter.setPen (pen);

   painter.setRenderHint (QPainter::Antialiasing, true);

   // Iterate over interval values.
   //
   for (ok = this->firstValue (j, value, isMajor); ok;
        ok = this->nextValue  (j, value, isMajor)) {

      f = this->calcFraction (value);

      angle = lowerAngle +  f * (upperAngle - lowerAngle);
      s = sin (angle * RADIANS_PER_DEGREE);
      c = cos (angle * RADIANS_PER_DEGREE);

      if (isMajor) {
         p1 = RPOINT (0.94);
      } else {
         p1 = RPOINT (0.97);
      }
      p2 = RPOINT (1.0);

      painter.drawLine (p1, p2);

      if (isMajor) {
         QString vt;
         vt.sprintf ("%.1f", value);
         p1 = RPOINT (0.88);
         this->drawText (painter, p1, vt, 7);
      }
   }

   angle = lowerAngle + fraction *(upperAngle - lowerAngle);
   s = sin (angle * RADIANS_PER_DEGREE);
   c = cos (angle * RADIANS_PER_DEGREE);

   pen.setColor (this->getForegroundPaintColour ());
   p1 = RPOINT (0.0);

   p2 = RPOINT (1.0);
   pen.setWidth (2);
   painter.setPen (pen);
   painter.drawLine (p1, p2);

   p2 = RPOINT (0.75);
   pen.setWidth (3);
   painter.setPen (pen);
   painter.drawLine (p1, p2);

   p2 = RPOINT (0.5);
   pen.setWidth (4);
   painter.setPen (pen);
   painter.drawLine (p1, p2);

   p2 = RPOINT (0.25);
   pen.setWidth (5);
   painter.setPen (pen);
   painter.drawLine (p1, p2);

#undef RPOINT
}

//------------------------------------------------------------------------------
//
void QEAnalogIndicator::drawText (QPainter & painter, QPoint & textCentre, QString & text, const int pointSize)
{
   QFont pf (this->font ());
   QFontMetrics fm = painter.fontMetrics ();
   QPen pen;
   int x;
   int y;

   if (pointSize > 0) {
      pf.setPointSize (pointSize);
   }
   painter.setFont (pf);

   // Centre text. For height, pointSize seems better than fm.height ()
   // painter.drawText needs bottom left coordinates.
   //
   x = textCentre.x () - fm.width (text)/2;
   y = textCentre.y () + (pf.pointSize () + 1) / 2;

   pen.setColor (this->getFontPaintColour ());
   painter.setPen (pen);

   // If text too wide, then ensure we show most significant part.
   //
   painter.drawText (MAX (1, x), y, text);
}

//------------------------------------------------------------------------------
//
void QEAnalogIndicator::paintEvent (QPaintEvent * /* event - make warning go away */)
{
   QPainter painter (this);

   int right;
   int bottom;
   QRect outlineRect;
   QRect areaRect;
   QRect axisRect;
   double fraction;
   QPoint textCentre;

   // Draw everything with antialiasing off.
   //
   painter.setRenderHint (QPainter::Antialiasing, false);

   // Want effective drawing right-most, bottom-most pixels.
   //
   // Note: Actual size appears to be 1 less than widget width/height.
   // Pixels are  in range (0 .. size - 2) which is size - 1 pixels.
   //
   right  = this->width () - 2;
   bottom = this->height () - 2;

   if ( (this->getMode() == Meter ) || (this->getShowScale () == false) ) {
      outlineRect.setTop (0);
      outlineRect.setLeft (0);
      outlineRect.setBottom (bottom);
      outlineRect.setRight (right);

      // No axis required.
      //
      axisRect.setTop (0);
      axisRect.setLeft (0);
      axisRect.setBottom (0);
      axisRect.setRight (0);

   } else {

      // We do draw a separate axis.
      //
      if (this->isLeftRight ()) {

         outlineRect.setTop (0);
         outlineRect.setLeft (0);
         outlineRect.setBottom (bottom - 21);
         outlineRect.setRight (right);

         axisRect.setTop (bottom - 20);
         axisRect.setLeft (0);
         axisRect.setBottom (bottom);
         axisRect.setRight (right);

      } else {

         outlineRect.setTop (0);
         outlineRect.setLeft (0);
         outlineRect.setBottom (bottom);
         outlineRect.setRight (right - 41);

         axisRect.setTop (0);
         axisRect.setLeft (right - 40);
         axisRect.setBottom (bottom);
         axisRect.setRight (right);
      }
   }

   // Set up main graphic paint area rectangle - 1 pixel boarder.
   //
   areaRect.setTop (outlineRect.top () + 1);
   areaRect.setLeft (outlineRect.left () + 1);
   areaRect.setBottom (outlineRect.bottom () - 1);
   areaRect.setRight (outlineRect.right () - 1);


   // Calculate the fractional scale and constrain to be in range.
   //
   fraction = this->calcFraction (this->mValue);


   // Now lets get drawing.
   //
   this->drawOutline (painter, outlineRect);

   // Set default centre text positions.
   //
   textCentre.setX ((areaRect.left () + areaRect.right ()) / 2);
   textCentre.setY ((areaRect.top ()  + areaRect.bottom ()) / 2);

   switch  (this->mMode) {

      case Bar:
         this->drawBar (painter, areaRect, fraction);
         if (this->getShowScale () == true) {
            this->drawAxis(painter, axisRect);
         }
         break;

      case Scale:
         this->drawMarker (painter, areaRect, fraction);
         if (this->getShowScale () == true) {
            this->drawAxis(painter, axisRect);
         }
         break;

      case Meter:
         this->drawMeter (painter, areaRect, fraction);
         textCentre.setY ((areaRect.top ()  + 3*areaRect.bottom ()) / 4);
         break;

      default:
         // report an error??
         //
         break;
   }


   if (this->getShowText ()) {
      QString sprintfFormat;
      QString barText;

      // This is a dispatching call.
      //
      sprintfFormat = getSprintfFormat ();
      barText.sprintf (sprintfFormat.toAscii().data (), this->mValue);
      this->drawText (painter, textCentre, barText);
   }
}

//------------------------------------------------------------------------------
//
QString QEAnalogIndicator::getSprintfFormat ()
{
   return QString ("%+0.7g");
}


//------------------------------------------------------------------------------
//
void QEAnalogIndicator::setMinorInterval (const double value)
{
   double temp;
   double hold;

   temp = MAX (0.001, value);
   if (this->mMinorInterval != temp) {
      hold = this->getMajorInterval ();
      this->mMinorInterval = temp;
      this->setMajorInterval (hold);
      this->update ();
   }
}

double QEAnalogIndicator::getMinorInterval ()
{
   return this->mMinorInterval;
}

//------------------------------------------------------------------------------
//
void QEAnalogIndicator::setMajorInterval (const double value)
{
   int temp;

   temp = int (value / this->mMinorInterval + 0.4);
   temp = MAX (2, temp);
   if (this->mMajorMinorRatio != temp) {
      this->mMajorMinorRatio = temp;
      this->update ();
   }
}

double QEAnalogIndicator::getMajorInterval ()
{
   return this->mMajorMinorRatio * this->mMinorInterval;
}

//------------------------------------------------------------------------------
//
void QEAnalogIndicator::setRange (const double MinimumIn,
                                   const double MaximumIn)
{
   this->setMinimum (MinimumIn);
   this->setMaximum (MaximumIn);
}

//------------------------------------------------------------------------------
// Standard propery access macro.
//
#define PROPERTY_ACCESS(type, name, convert)                 \
                                                             \
void QEAnalogIndicator::set##name (const type value)  {     \
   type temp;                                                \
   temp = convert;                                           \
   if (this->m##name != temp) {                              \
      this->m##name = temp;                                  \
      this->update ();                                       \
   }                                                         \
}                                                            \
                                                             \
type QEAnalogIndicator::get##name () {                      \
   return this->m##name;                                     \
}


// NOTE: we have to use qualified type for Orientation and Mode.
//
PROPERTY_ACCESS (QEAnalogIndicator::Orientations, Orientation, value)

PROPERTY_ACCESS (QEAnalogIndicator::Modes, Mode, value)

PROPERTY_ACCESS (int, CentreAngle, LIMIT (value, -180, +180) )

PROPERTY_ACCESS (int, SpanAngle, LIMIT (value, 15, 350) )

PROPERTY_ACCESS (QColor, BorderColour, value)

PROPERTY_ACCESS (QColor, ForegroundColour, value)

PROPERTY_ACCESS (QColor, BackgroundColour, value)

PROPERTY_ACCESS (QColor, FontColour, value)

PROPERTY_ACCESS (bool, ShowText, value)

PROPERTY_ACCESS (bool, ShowScale, value)

PROPERTY_ACCESS (bool, LogScale, value)

PROPERTY_ACCESS (double, Value, value)

// Ensure  max - min >= minimum span for all updated
//
PROPERTY_ACCESS (double, Minimum, MIN (value, this->mMaximum - MINIMUM_SPAN) )

PROPERTY_ACCESS (double, Maximum, MAX (value, this->mMinimum + MINIMUM_SPAN) )

#undef PROPERTY_ACCESS

// end
