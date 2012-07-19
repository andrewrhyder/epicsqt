/*  QAnalogProgressBar.cpp
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
#include <QAnalogProgressBar.h>

#include <QDebug>
#include <QFontMetrics>
#include <QtGui>
#include <QBrush>
#include <QPen>
#include <QPoint>
#include <QPolygon>

#define DEBUG qDebug () << __FUNCTION__ << __LINE__

#define MINIMUM_SPAN        0.000001
#define PI                  3.14159265358979323846
#define RADIANS_PER_DEGREE  (PI / 180.0)

#define MAX(a, b)           ((a) >= (b) ? (a) : (b))
#define MIN(a, b)           ((a) <= (b) ? (a) : (b))
#define LIMIT(x,low,high)   (MAX(low, MIN(x, high)))


//------------------------------------------------------------------------------
// Constructor with no initialisation
//
QAnalogProgressBar::QAnalogProgressBar (QWidget *parent) : QWidget (parent)
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
QSize QAnalogProgressBar::sizeHint () const
{
   return QSize (48, 16);
}

//------------------------------------------------------------------------------
//
bool QAnalogProgressBar::firstValue (int & itc, double & value, bool & isMajor)
{
   double real;
   bool result;

   real = this->mMinimum / this->getMinorInterval ();

   // Use floor to round down and - 0.5 too mitigate any rounding effects.
   // Subtract an addition -1 to ensure first call to nextValue returns a
   // value no greate than the first required value.
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
bool QAnalogProgressBar::nextValue  (int & itc, double & value, bool & isMajor)
{
   itc++;
   value = itc * this->getMinorInterval ();
   isMajor = ((itc % this->mMajorMinorRatio) == 0);
   return (value <= this->mMaximum);
}

//------------------------------------------------------------------------------
//
QColor QAnalogProgressBar::getBorderPaintColour ()
{
   return this->isEnabled () ? this->mBorderColour : QColor (160, 160, 160);
}

//------------------------------------------------------------------------------
//
QColor QAnalogProgressBar::getBackgroundPaintColour ()
{
   return this->isEnabled () ? this->mBackgroundColour : QColor (240, 240, 240);
}

//------------------------------------------------------------------------------
//
QColor QAnalogProgressBar::getForegroundPaintColour ()
{
   return this->isEnabled () ? this->mForegroundColour : QColor (220, 220, 220);
}


//------------------------------------------------------------------------------
//
QColor QAnalogProgressBar::getFontPaintColour ()
{
   return this->isEnabled () ? this->mFontColour : QColor (140, 140, 140);
}


//------------------------------------------------------------------------------
//
void QAnalogProgressBar::drawBar (QPainter & painter, int top,  int left,
                                  int bottom,  int right,
                                  const double fraction)
{
   const int width_span = right - left;
   const int height_span = bottom - top;

   QPen pen;
   QBrush brush;
   QRect barRect;

   switch (this->mOrientation) {

   case Left_To_Right:
      // Convert fractions back to pixels.
      //
      right  = left + int (fraction * double (width_span));
      break;

   case Top_To_Bottom:
      bottom = top + int (fraction * double (height_span));
      break;

   case Right_To_Left:
      left = right - int (fraction * double (width_span));
      break;

   case Bottom_To_Top:
      top = bottom  - int (fraction * double (height_span));
      break;

   default:
      // report an error??
      //
      break;
   }

   // Set up barRect and paint it.
   // The previously set translation and rotation looks after the rest.
   //
   barRect.setTop (top);
   barRect.setBottom (bottom);
   barRect.setLeft (left);
   barRect.setRight (right);

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
void QAnalogProgressBar::drawScale (QPainter & painter, int top,  int left,
                                    int bottom,  int right,
                                    const double fraction)
{
   const int width_span = right - left;
   const int height_span = bottom - top;

   int t = top;
   int l = left;
   int b = bottom;
   int r = right;

   int span;
   int cx, cy;
   QPen pen;
   QBrush brush;
   QPolygon polygon;

   switch (this->mOrientation) {
   case Left_To_Right:
   case Right_To_Left:
      span =  height_span / 8;
      break;

   case Top_To_Bottom:
   case Bottom_To_Top:
      span =  width_span / 8;
      break;

   default:
      // report an error??
      //
      return;
   }
   span = MAX (span, 4);

   switch (this->mOrientation) {

      // Convert fractions back to pixels.
      //
   case Left_To_Right:
      cx = left + int (fraction * double (width_span));
      cy = (top + bottom) / 2;
      l = cx - span;
      r = cx + span;
      break;

   case Top_To_Bottom:
      cx = (left + right) / 2;
      cy = top + int (fraction * double (height_span));
      t = cy -span;
      b = cy + span;
      break;

   case Right_To_Left:
      cx = right - int (fraction * double (width_span));
      cy = (top + bottom) / 2;
      l = cx - span;
      r = cx + span;
      break;

   case Bottom_To_Top:
      cx = (left + right) / 2;
      cy = bottom  - int (fraction * double (height_span));
      t = cy - span;
      b = cy + span;
      break;

   default:
      return;
   }

   // Ensure within top/left/bottom/right rectacngle.
   //
   t = MAX(top, t);
   l = MAX(left, l);
   b = MIN(bottom, b);
   r = MIN (right, r);

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
void QAnalogProgressBar::drawMeter (QPainter & painter,
                                    int top,  int left,
                                    int bottom,  int right,
                                    const double fraction)
{
   // Macro function to create a point based on centre positon, radius and direction (s, c).
   //
   #define RPOINT(f)  QPoint (int (centre_x + f*radius*s),  int (centre_y - f*radius*c))

   const int width_span = right - left;
   const int height_span = bottom - top;

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
   if (maxS > 0) radius = MIN (radius, ((right - gap)  - centre_x) /maxS);
   if (minS < 0) radius = MIN (radius, ((left + gap)   - centre_x) /minS);
   if (maxC > 0) radius = MIN (radius, (centre_y -    (top + gap)) /maxC);
   if (minC < 0) radius = MIN (radius, (centre_y - (bottom - gap)) /minC);

   pen.setWidth (1);
   pen.setColor (this->getFontPaintColour ());
   painter.setPen (pen);

   // Iterate over interval values.
   //
   for (ok = this->firstValue (j, value, isMajor); ok;
        ok = this->nextValue  (j, value, isMajor)) {

      f = (value - this->mMinimum) / (this->mMaximum - this->mMinimum);

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
void QAnalogProgressBar::drawText (QPainter & painter, QPoint & textCentre, QString & text, const int pointSize)
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
void QAnalogProgressBar::paintEvent (QPaintEvent * /* event - make warning go away */)
{
   QPainter painter (this);
   QPen pen;
   QBrush brush;
   int right;
   int bottom;
   double fraction;
   QRect background;
   QPoint textCentre;

   // Draw everything with antialiasing off.
   //
   painter.setRenderHint (QPainter::Antialiasing, false);

   // Want effective drawing right-most, bottom-most pixels.
   //
   // Note: Pixels are  in range (0 .. size - 2) which is size - 1 pixels.
   //
   right  = this->width () - 2;
   bottom = this->height () - 2;

   // Set up background rectange and paint it.
   //
   background.setTop (0);
   background.setBottom (bottom);
   background.setLeft (0);
   background.setRight (right);

   pen.setWidth (1);
   pen.setColor (this->getBorderPaintColour ());
   brush.setStyle (Qt::SolidPattern);
   brush.setColor (this->getBackgroundPaintColour ());

   painter.setPen (pen);
   painter.setBrush (brush);
   painter.drawRect (background);

   // Calculate the fractional scale and constrain to be in range.
   //
   fraction = (this->mValue   - this->mMinimum) /
              (this->mMaximum - this->mMinimum);
   fraction = LIMIT (fraction, 0.0, 1.0);

   // Set default centre text positions.
   //
   textCentre.setX (right / 2);
   textCentre.setY (bottom / 2);

   switch  (this->mMode) {

      // Bar, Scale etc. has 1 pixel bouldary.
      //
   case Bar:
      this->drawBar (painter, 1, 1, bottom - 1, right - 1, fraction);
      break;

   case Scale:
      this->drawScale (painter, 1, 1, bottom - 1, right - 1, fraction);
      break;

   case Meter:
      this->drawMeter (painter, 1, 1, bottom - 1, right - 1, fraction);
      textCentre.setY (3*bottom / 4);
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
QString QAnalogProgressBar::getSprintfFormat ()
{
   return QString ("%+0.7g");
}


//------------------------------------------------------------------------------
//
void QAnalogProgressBar::setMinorInterval (const double value)
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

double QAnalogProgressBar::getMinorInterval ()
{
   return this->mMinorInterval;
}

//------------------------------------------------------------------------------
//
void QAnalogProgressBar::setMajorInterval (const double value)
{
   int temp;

   temp = int (value / this->mMinorInterval + 0.4);
   temp = MAX (2, temp);
   if (this->mMajorMinorRatio != temp) {
      this->mMajorMinorRatio = temp;
      this->update ();
   }
}

double QAnalogProgressBar::getMajorInterval ()
{
   return this->mMajorMinorRatio * this->mMinorInterval;
}

//------------------------------------------------------------------------------
//
void QAnalogProgressBar::setRange (const double MinimumIn,
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
void QAnalogProgressBar::set##name (const type value)  {     \
   type temp;                                                \
   temp = convert;                                           \
   if (this->m##name != temp) {                              \
      this->m##name = temp;                                  \
      this->update ();                                       \
   }                                                         \
}                                                            \
                                                             \
type QAnalogProgressBar::get##name () {                      \
   return this->m##name;                                     \
}


// NOTE: we have to use qualified type for Orientation and Mode.
//
PROPERTY_ACCESS (QAnalogProgressBar::Orientations, Orientation, value)

PROPERTY_ACCESS (QAnalogProgressBar::Modes, Mode, value)

PROPERTY_ACCESS (int, CentreAngle, LIMIT (value, -180, +180) )

PROPERTY_ACCESS (int, SpanAngle, LIMIT (value, 15, 350) )

PROPERTY_ACCESS (QColor, BorderColour, value)

PROPERTY_ACCESS (QColor, ForegroundColour, value)

PROPERTY_ACCESS (QColor, BackgroundColour, value)

PROPERTY_ACCESS (QColor, FontColour, value)

PROPERTY_ACCESS (bool, ShowText, value)

PROPERTY_ACCESS (double, Value, value)

// Ensure  max - min >= minimum span for all updated
//
PROPERTY_ACCESS (double, Minimum, MIN (value, this->mMaximum - MINIMUM_SPAN) )

PROPERTY_ACCESS (double, Maximum, MAX (value, this->mMinimum + MINIMUM_SPAN) )

#undef PROPERTY_ACCESS

      // end
