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


#define PROGRESS_BAR_SIZE     10000
#define MINIMUM_SPAN          0.000001

#define MAX(a, b)           ((a) >= (b) ? (a) : (b))
#define MIN(a, b)           ((a) <= (b) ? (a) : (b))
#define LIMIT(x,low,high)   (MAX(low, MIN(x, high)))


//------------------------------------------------------------------------------
// Constructor with no initialisation
//
QAnalogProgressBar::QAnalogProgressBar (QWidget *parent) : QWidget (parent)
{
    this->mBorderColour      = QColor (  0,   0,  96);   // dark blue
    this->mForegroundColour  = QColor (128, 192, 255);   // blue
    this->mBackgroundColour  = QColor (220, 220, 220);   // light gray
    this->mFontColour        = QColor (0,     0,   0);   // black

    this->mAnalogMinimum = 0.0;
    this->mAnalogMaximum = 10.0;
    this->mOrientation = Left_To_Right;
    this->mMode = Bar;
    this->mShowText = true;
    this->mAnalogValue = 0.0;
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
void QAnalogProgressBar::paintEvent (QPaintEvent * /* event - make warning go away */)
{
   QPainter painter (this);
   QPen pen;
   QBrush brush;
   int right;
   int bottom;
   double fraction;
   QRect background;

   setAutoFillBackground (false);
   setBackgroundRole (QPalette::NoRole);
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

   switch  (this->mMode) {

   // Bar, Scale has 1 pixel bouldary.
   //
   case Bar:
       this->drawBar (painter, 1, 1, bottom - 1, right - 1, fraction);
       break;

   case Scale:
       this->drawScale (painter, 1, 1, bottom - 1, right - 1, fraction);
       break;

   default:
       // report an error??
       //
       break;
   }

   if (this->getShowText ()) {
      QFontMetrics fm = painter.fontMetrics ();
      QString sprintfFormat;
      QString barText;
      int x;
      int y;

      // This is a dispatching call.
      //
      sprintfFormat = getSprintfFormat ();
      barText.sprintf (sprintfFormat.toAscii().data (), this->mAnalogValue);

      // Centre text. For height, pointSize seems better than fm.height ()
      //
      x = (this->width () - fm.width (barText))/2;
      y = (this->height () + this->font ().pointSize ())/2;

      pen.setColor (this->mFontColour);
      painter.setPen (pen);

      // If text too wide, then ensure we show most significant part.
      //
      painter.drawText (MAX (1, x), y, barText);
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
void QAnalogProgressBar::setAnalogRange (const double analogMinimumIn,
                                         const double analogMaximumIn)
{
    this->setAnalogMinimum (analogMinimumIn);
    this->setAnalogMaximum (analogMaximumIn);
}

// Standard propery access macro.
//
#define PROPERTY_ACCESS(type, name, extra)                   \
                                                             \
void QAnalogProgressBar::set##name (const type value)  {     \
   if (this->m##name != value) {                             \
      this->m##name = value;                                 \
      extra                                                  \
      update ();                                             \
    }                                                        \
 }                                                           \
                                                             \
type QAnalogProgressBar::get##name () {                      \
   return this->m##name;                                     \
}

// NOTE: we have to use qualified type for Orientation and Mode.
//
PROPERTY_ACCESS (QAnalogProgressBar::Orientations, Orientation, )

PROPERTY_ACCESS (QAnalogProgressBar::Modes, Mode, )

PROPERTY_ACCESS (QColor, BorderColour, )

PROPERTY_ACCESS (QColor, ForegroundColour, )

PROPERTY_ACCESS (QColor, BackgroundColour, )

PROPERTY_ACCESS (QColor, FontColour, )

PROPERTY_ACCESS (bool, ShowText, )

PROPERTY_ACCESS (double, AnalogValue, )

// Ensure  max - min >= minimum span for all updated
//
PROPERTY_ACCESS (double, AnalogMinimum,
                 this->mAnalogMaximum = MAX (this->mAnalogMaximum, this->mAnalogMinimum + MINIMUM_SPAN); )

PROPERTY_ACCESS (double, AnalogMaximum,
                 this->mAnalogMinimum = MIN (this->mAnalogMinimum, this->mAnalogMaximum - MINIMUM_SPAN); )

#undef PROPERTY_ACCESS

// end
