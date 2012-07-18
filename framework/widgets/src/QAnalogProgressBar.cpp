/*
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
   this->mForegroundColour  = QColor ( 90, 140, 190);   // blue
   this->mBackgroundColour  = QColor (220, 220, 220);   // light gray
   this->mFontColour        = QColor (0,     0,   0);   // black

   this->mAnalogMinimum = 0.0;
   this->mAnalogMaximum = 100.0;
   this->mMajorInterval = 20.0;
   this->mMinorInterval = 4.0;

   this->mOrientation = Left_To_Right;
   this->mMode = Bar;
   this->mShowText = true;
   this->mAnalogValue = 0.0;
   this->mCentreAngle = 0.0;
   this->mSpanAngle = 180;

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

    pen.setColor (this->mForegroundColour);
    pen.setWidth (1);
    painter.setPen (pen);

    brush.setStyle (Qt::SolidPattern);
    brush.setColor (this->mForegroundColour);
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

    pen.setColor (this->mForegroundColour);
    pen.setWidth (1);
    painter.setPen (pen);

    brush.setStyle (Qt::SolidPattern);
    brush.setColor (this->mForegroundColour);
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
   const int width_span = right - left;
   const int height_span = bottom - top;

   QPen pen;
   double centre_x, centre_y;
   double radius;
   int j;
   double lowerAngle;
   double upperAngle;
   double angle;
   double minS, maxS, minC, maxC;
   double s, c;
   int first;
   int last;
   int m;
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
      if (angle < -360.0)      { angle = -360.0; }
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
   f = (-minS) / (maxS - minS);
   centre_x = 6.0 + f * (width_span - 12);

   f = (+maxC) / (maxC - minC);
   centre_y = 6.0 + f * (height_span - 12);

   radius = MIN (width_span, height_span);
   if (maxS > 0) radius = MIN (radius, ((right - 6) - centre_x) /maxS);
   if (minS < 0) radius = MIN (radius, ((left + 6)  - centre_x) /minS);
   if (maxC > 0) radius = MIN (radius, (centre_y - (top + 6)) /maxC);
   if (minC < 0) radius = MIN (radius, (centre_y - (bottom - 6)) /minC);

   pen.setWidth (1);
   pen.setColor (this->mFontColour);
   painter.setPen (pen);

#define RPOINT(f)  QPoint (int (centre_x + f*radius*s),  int (centre_y - f*radius*c))

   first = int (this->getAnalogMinimum() / this->getMinorInterval() - 0.5);
   last  = int (this->getAnalogMaximum() / this->getMinorInterval() + 0.5);
   m = int (this->getMajorInterval() / this->getMinorInterval());
   for (j = first; j <= last; j++) {

      value = j *  this->getMinorInterval();

      f = (value                     - this->getAnalogMinimum ()) /
          (this->getAnalogMaximum()  - this->getAnalogMinimum ());

      angle = lowerAngle +  f * (upperAngle - lowerAngle);
      s = sin (angle * RADIANS_PER_DEGREE);
      c = cos (angle * RADIANS_PER_DEGREE);

      // major or minor tick ?
      //
      if (j%m == 0) {
         QString v;

         v.sprintf ("%.1f", value);
         p1 = RPOINT (0.88);
         this->drawText (painter, p1, v, 7);

         p1 = RPOINT (0.92);

      } else {
         p1 = RPOINT (0.96);
      }
      p2 = RPOINT (1.0);

      painter.drawLine (p1, p2);
   }

   angle = lowerAngle + fraction *(upperAngle - lowerAngle);
   s = sin (angle * RADIANS_PER_DEGREE);
   c = cos (angle * RADIANS_PER_DEGREE);

   pen.setColor (this->mForegroundColour);
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
void QAnalogProgressBar::drawText  (QPainter & painter, QPoint & textCentre, QString & text, const int pointSize)
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
   // painter.drawText needs bottom left.
   //
   x = textCentre.x () - fm.width (text)/2;
   y = textCentre.y () + this->font ().pointSize ()/2;

   pen.setColor (this->mFontColour);
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

   pen.setWidth (1);
   brush.setStyle (Qt::SolidPattern);
   brush.setColor (this->mBorderColour);

   // Draw everything with antialiasing off.
   //
   painter.setRenderHint (QPainter::Antialiasing, false);

   // Want effective drawing right-most, bottom-most pixels.
   //
   right  = this->width () - 2;
   bottom = this->height () - 2;

   // Set up background rectange and paint it.
   // The previously set translation and rotation looks after the rest.
   //
   background.setTop (0);
   background.setBottom (bottom);
   background.setLeft (0);
   background.setRight (right);

   pen.setColor (this->mBorderColour);
   brush.setColor (this->mBackgroundColour);
   painter.setPen (pen);
   painter.setBrush (brush);
   painter.drawRect (background);

   // Calculate the fractional scale and constrain to be in range.
   //
   fraction = (this->mAnalogValue - this->mAnalogMinimum) /
              (this->mAnalogMaximum - this->mAnalogMinimum);
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
      barText.sprintf (sprintfFormat.toAscii().data (), this->mAnalogValue);

      this->drawText(painter, textCentre, barText);
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

   temp = MAX (0.001, value);
   if (this->mMinorInterval != temp) {
      this->mMinorInterval = temp;
      this->setMajorInterval (this->getMajorInterval());
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
   double temp;
   long n;

   n = long (value / this->mMinorInterval + 0.4999);
   n = MAX (1, n);
   temp = n*this->mMinorInterval;
   if (this->mMajorInterval != temp) {
      this->mMajorInterval = temp;
     this->update ();
   }
}

double QAnalogProgressBar::getMajorInterval ()
{
   return this->mMajorInterval;
}

//------------------------------------------------------------------------------
//
void QAnalogProgressBar::setAnalogRange (const double analogMinimumIn,
                                         const double analogMaximumIn)
{
    this->setAnalogMinimum (analogMinimumIn);
    this->setAnalogMaximum (analogMaximumIn);
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
    }                                                        \
 }                                                           \
                                                             \
type QAnalogProgressBar::get##name () {                      \
   return this->m##name;                                     \
}

// NOTE: we have to use qualified type for Orientation and Mode.
//
PROPERTY_ACCESS (QAnalogProgressBar::Orientations, Orientation, value)

PROPERTY_ACCESS (QAnalogProgressBar::Modes, Mode, value)

PROPERTY_ACCESS (int, CentreAngle, LIMIT (value, -180, +180) )

PROPERTY_ACCESS (int, SpanAngle, LIMIT (value, 5, 350) )

PROPERTY_ACCESS (QColor, BorderColour, value)

PROPERTY_ACCESS (QColor, ForegroundColour, value)

PROPERTY_ACCESS (QColor, BackgroundColour, value)

PROPERTY_ACCESS (QColor, FontColour, value)

PROPERTY_ACCESS (bool, ShowText, value)

PROPERTY_ACCESS (double, AnalogValue, value)

// Ensure  max - min >= minimum span for all updated
//
PROPERTY_ACCESS (double, AnalogMinimum, MIN (value, this->mAnalogMaximum - MINIMUM_SPAN) )

PROPERTY_ACCESS (double, AnalogMaximum, MAX (value, this->mAnalogMinimum + MINIMUM_SPAN) )

#undef PROPERTY_ACCESS

// end
