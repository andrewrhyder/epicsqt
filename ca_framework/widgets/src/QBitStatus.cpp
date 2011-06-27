/* $File: //ASP/Personal/starritt/qt/QBitStatus.cpp $
 * $Revision: #3 $
 * $DateTime: 2011/06/12 15:07:03 $
 * Last checked in by: $Author: starritt $
 */

/*! 
  \class QBitStatus
  \version $Revision: #3 $
  \date $DateTime: 2011/06/12 15:07:03 $
  \author andrew.starritt
  \brief CA Shape Widget.
 */

/*  This file is part of the EPICS QT Framework, initially developed at
 *  the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
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
  This class is a BitStatus widget based on directly QWidget.
  It provides similar functionality to that provided by the edm/medm/dephi
  widgets of the same name.
 */

#include <QBitStatus.h>
#include <QtGui>


#define MAX(a, b)           ((a) >= (b) ? (a) : (b))
#define MIN(a, b)           ((a) <= (b) ? (a) : (b))
#define LIMIT(x,low,high)   (MAX(low, MIN(x, high)))


/*! ---------------------------------------------------------------------------
 *
 */
QBitStatus::QBitStatus( QWidget *parent ) : QWidget (parent)
{
   // Set up data
   //
   mBorderColour  = QColor (  0,   0,  32);   // dark dark blue
   mOffColour     = QColor (255, 128, 128);   // red
   mOnColour      = QColor (128, 255, 128);   // green
   mInvalidColour = QColor (255, 182, 128);   // orange

   mDrawBorder = true;
   mNumberOfBits = 8;      // 1 .. 32
   mShift = 0;             // 0 .. 32
   mIsValid = true;
   mValue = 0;
   mOrientation = LSB_On_Right;

   // move to paintEvent ??
   //
   setAutoFillBackground (false);
   setBackgroundRole (QPalette::NoRole);
   pen.setWidth (1);
   brush.setStyle (Qt::SolidPattern);
   brush.setColor (mBorderColour);
}


/*! ---------------------------------------------------------------------------
 *  Define default size for this widget class.
 */
QSize QBitStatus::sizeHint () const {
   return QSize (48, 16);
}


/*! ---------------------------------------------------------------------------
 *  Draw the bit status.
 */
void QBitStatus::paintEvent (QPaintEvent * /* event - make warning go away */) {

   QPainter painter (this);
   QRect draw_area;
   QRect bit_area;
   int j;
   double fraction;
   double draw_width;
   double draw_height;
   int left;
   int right;
   int work;

   // Draw everything with antialiasing off.
   //
   painter.setRenderHint (QPainter::Antialiasing, false);

   // Set draw width and height and also apply translation and rotation
   // dependent upon widget orientation.
   //
   draw_area = geometry ();

   switch (mOrientation) {
      case LSB_On_Right:
         draw_width = (double) (draw_area.width () - 2);
         draw_height = (double) (draw_area.height () - 2);
         painter.translate (0.0, 0.0);
         painter.rotate (0.0);
         break;


      case LSB_On_Bottom:
         draw_width = (double) (draw_area.height () - 2);
         draw_height = (double) (draw_area.width () - 2);
         painter.translate (draw_area.width (), 0.0);
         painter.rotate (90.0);    // clock wise (degrees)
         break;


      case LSB_On_Left:
         draw_width = (double) (draw_area.width () - 2);
         draw_height = (double) (draw_area.height () - 2);
         painter.translate (draw_area.width (), draw_area.height ());
         painter.rotate (180.0);    // clock wise (degrees)
         break;


      case LSB_On_Top:
         draw_width = (double) (draw_area.height () - 2);
         draw_height = (double) (draw_area.width () - 2);
         painter.translate (0.0, draw_area.height ());
         painter.rotate (270.0);    // clock wise (degrees)
         break;


      default:
         // report an error??
         //
         draw_width = (double) (draw_area.width () - 2);
         draw_height = (double) (draw_area.height () - 2);
         break;
   }

   painter.scale (1.0, 1.0);

   // We do the basic draw from right to left, i.e. LSB_On_Right.
   // The previously set translation and rotation looks after the rest.
   //
   bit_area.setTop(1);
   bit_area.setHeight ((int) draw_height);

   work = mValue >> mShift;
   right = (int) draw_width;
   for (j = mNumberOfBits - 1; j >= 0 ;  j--) {

      fraction = (double) j /(double) mNumberOfBits;
      left = (int) (1.0 + fraction * draw_width);

      bit_area.setLeft (left);
      bit_area.setRight (right);

      // Set up the pen and brush (color, thickness, etc.)
      //
      if (mDrawBorder == true) {
         pen.setColor (mBorderColour);
         pen.setWidth (1);
         painter.setPen (pen);
      } else {
         painter.setPen( Qt::NoPen );
      }

      if (mIsValid) {
         if ((work & 1) == 1) {
            brush.setColor (mOnColour);
         } else {
            brush.setColor (mOffColour);
         }
      } else {
         brush.setColor (mInvalidColour);
      }
      painter.setBrush (brush);

      // Do the actual draw.
      //
      painter.drawRect (bit_area);

      // Pre-pare for next iteration through the loop.
      // We don't worry about checking for last time through the loop.
      //
      work = work >> 1;
      right = left;
   }
}


//=============================================================================
// Property functions
//=============================================================================
//
void QBitStatus::setValue (const long value) {
   if (mValue != value) {
      mValue = value;
      update ();  // Force re-draw
   }
}

long QBitStatus::getValue () {
   return mValue;
}

//=============================================================================
//
void QBitStatus::setBorderColour (const QColor value)
{
   if (mBorderColour != value) {
      mBorderColour = value;
      update ();  // Force re-draw
   }
}

QColor QBitStatus::getBorderColour ()
{
   return mBorderColour;
}

//=============================================================================
//
void QBitStatus::setOnColour (const QColor value)
{
   if (mOnColour != value) {
      mOnColour = value;
      update ();  // Force re-draw
   }
}

QColor QBitStatus::getOnColour ()
{
   return mOnColour;
}

//=============================================================================
//
void QBitStatus::setOffColour (const QColor value)
{
   if (mOffColour != value) {
      mOffColour = value;
      update ();  // Force re-draw
   }
}

QColor QBitStatus::getOffColour ()
{
   return mOffColour;
}

//=============================================================================
//
void QBitStatus::setInvalidColour (const QColor value)
{
   if (mInvalidColour != value) {
      mInvalidColour = value;
      update ();  // Force re-draw
   }
}

QColor QBitStatus::getInvalidColour ()
{
   return mInvalidColour;
}

//=============================================================================
//
void QBitStatus::setDrawBorder (const bool value)
{
   if (mDrawBorder != value) {
      mDrawBorder = value;
      update ();  // Force re-draw
   }
}

bool QBitStatus::getDrawBorder ()
{
   return mDrawBorder;
}

//=============================================================================
//
void QBitStatus::setNumberOfBits (const int value)
{
   int temp;

   temp = LIMIT (value, 1, 32);

   if (mNumberOfBits != temp) {
      mNumberOfBits = temp;
      update ();  // Force re-draw
   }
}

int QBitStatus::getNumberOfBits ()
{
   return mNumberOfBits;
}

//=============================================================================
//
void QBitStatus::setShift (const int value)
{
   int temp;

   temp = LIMIT (value, 0, 31);

   if (mShift != temp) {
      mShift = temp;
      update ();  // Force re-draw
   }
}

int QBitStatus::getShift ()
{
   return mShift;
}

//=============================================================================
//
void QBitStatus::setIsValid (const bool value)
{
   if (mIsValid != value) {
      mIsValid = value;
      update ();  // Force re-draw
   }
}

bool QBitStatus::getIsValid ()
{
   return mIsValid;
}

//=============================================================================
//
void QBitStatus::setOrientation (const enum Orientations value)
{
   if (mOrientation != value) {
      mOrientation = value;
      update ();  // Force re-draw
   }
}

// NOTE: we have to qualify function return type here.
//
enum QBitStatus::Orientations QBitStatus::getOrientation ()
{
   return mOrientation;
}

// end
