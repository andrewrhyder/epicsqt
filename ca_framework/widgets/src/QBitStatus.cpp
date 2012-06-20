/* $Id: QBitStatus.cpp $
 */

/*! 
  \class QBitStatus
  \version $Revision: #4 $
  \date $DateTime: 2012/06/16 17:40:00 $
  \author andrew.starritt
  \brief Bit wise display of integer values.
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

#include <QDebug>
#include <QtGui>

#include <QBitStatus.h>


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
   mClearColour   = QColor (230, 230, 230);   // light gray

   mDrawBorder = true;
   mNumberOfBits = 8;      // 1 .. 32
   mGap = 0;
   mShift = 0;             // 0 .. 32
   mIsValid = true;
   mValue = 0;
   mOrientation = LSB_On_Right;
   mOnClearMask = 0x00000000;
   mOffClearMask = 0x00000000;
   mReversePolarityMask = 0x00000000;

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
   double draw_width;
   double draw_height;
   double rNumberOfBits;
   double gap_fraction;
   double bit_fraction;
   double left_fraction;
   double right_fraction;
   int left;
   int right;
   int work;
   int onApplies;
   int offApplies;

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
   bit_area.setTop (1);
   bit_area.setHeight ((int) draw_height);

   // Calulate fractional widths of the gaps and the bits.
   // Re-adjust the gaps if the fractonal bits are too small.
   // We draw 1 .. draw_width, and do fractions 0 .. (draw_width - 1).
   //
   // Just keep in mind:
   //    (number - 1)*gap_fraction + number*bit_fraction == 1
   //
   rNumberOfBits = double (mNumberOfBits);
   gap_fraction = double (this->mGap) / (draw_width - 1.0);
   bit_fraction = (1.0 - (rNumberOfBits - 1.0) * gap_fraction) / rNumberOfBits;
   if (bit_fraction <= 0.5 / rNumberOfBits) {
      bit_fraction = 0.5 / rNumberOfBits;
      gap_fraction = (1.0 - rNumberOfBits * bit_fraction) / (rNumberOfBits - 1.0);
   }

   work = (mValue >> mShift) ^ mReversePolarityMask;
   onApplies  = (-1) ^ mOnClearMask;
   offApplies = (-1) ^ mOffClearMask;

   for (j = mNumberOfBits - 1; j >= 0;  j--) {

      left_fraction = double (j) * (bit_fraction + gap_fraction);
      right_fraction = left_fraction + bit_fraction;

      // Convert fractions back to pixels.
      //
      left  = 1 + int (left_fraction  * (draw_width - 1.0));
      right = 1 + int (right_fraction * (draw_width - 1.0));

      bit_area.setLeft (left);
      bit_area.setRight (right);

      // Set up the pen and brush (color, thickness, etc.)
      //
      if (mDrawBorder == true) {
         pen.setColor (mBorderColour);
         pen.setWidth (1);
         painter.setPen (pen);
      } else {
         painter.setPen (Qt::NoPen);
      }

      if (mIsValid) {

         if ((work & 1) == 1) {
            // Bit is on
            if ((onApplies & 1) == 1) {
               brush.setColor (mOnColour);
            }  else {
               brush.setColor (mClearColour);
            }
         } else {
            // Bit is off
            if ((offApplies & 1) == 1) {
               brush.setColor (mOffColour);
            }  else {
               brush.setColor (mClearColour);
            }
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
      onApplies = onApplies >> 1;
      offApplies = offApplies >> 1;
   }
}


//=============================================================================
// Property functions
//=============================================================================
//
void QBitStatus::setValue (const long value)
{
   if (this->mValue != value) {
      this->mValue = value;
      this->update ();  // Force re-draw
   }
}

long QBitStatus::getValue () {
   return this->mValue;
}

//=============================================================================
//
void QBitStatus::setBorderColour (const QColor value)
{
   if (this->mBorderColour != value) {
      this->mBorderColour = value;
      this->update ();  // Force re-draw
   }
}

QColor QBitStatus::getBorderColour ()
{
   return this->mBorderColour;
}

//=============================================================================
//
void QBitStatus::setOnColour (const QColor value)
{
   if (this->mOnColour != value) {
      this->mOnColour = value;
      this->update ();  // Force re-draw
   }
}

QColor QBitStatus::getOnColour ()
{
   return this->mOnColour;
}

//=============================================================================
//
void QBitStatus::setOffColour (const QColor value)
{
   if (this->mOffColour != value) {
      this->mOffColour = value;
      this->update ();  // Force re-draw
   }
}

QColor QBitStatus::getOffColour ()
{
   return this->mOffColour;
}

//=============================================================================
//
void QBitStatus::setInvalidColour (const QColor value)
{
   if (this->mInvalidColour != value) {
      this->mInvalidColour = value;
      this->update ();  // Force re-draw
   }
}

QColor QBitStatus::getInvalidColour ()
{
   return this->mInvalidColour;
}

//=============================================================================
//
void QBitStatus::setClearColour (const QColor value)
{
   if (this->mClearColour != value) {
      this->mClearColour = value;
      this->update ();  // Force re-draw
   }
}

QColor QBitStatus::getClearColour ()
{
   return this->mClearColour;
}

//=============================================================================
//
void QBitStatus::setDrawBorder (const bool value)
{
   if (this->mDrawBorder != value) {
      this->mDrawBorder = value;
      this->update ();  // Force re-draw
   }
}

bool QBitStatus::getDrawBorder ()
{
   return this->mDrawBorder;
}

//=============================================================================
//
void QBitStatus::setNumberOfBits (const int value)
{
   int temp;

   temp = LIMIT (value, 1, 32);

   if (this->mNumberOfBits != temp) {
      this->mNumberOfBits = temp;
      this->update ();  // Force re-draw
   }
}

int QBitStatus::getNumberOfBits ()
{
   return this->mNumberOfBits;
}


//=============================================================================
//
void QBitStatus::setGap (const int value)
{
   int temp;

   temp = LIMIT (value, 0, 40);

   if (this->mGap != temp) {
      this->mGap = temp;
      this->update ();  // Force re-draw
   }
}

int QBitStatus::getGap ()
{
   return this->mGap;
}

//=============================================================================
//
void QBitStatus::setShift (const int value)
{
   int temp;

   temp = LIMIT (value, 0, 31);

   if (this->mShift != temp) {
      this->mShift = temp;
      this->update ();  // Force re-draw
   }
}

int QBitStatus::getShift ()
{
   return this->mShift;
}

//=============================================================================
//
void QBitStatus::setOnClearMask (const QString value)
{
   int temp;

   temp = this->maskToInt (value);

   if (this->mOnClearMask != temp) {
      this->mOnClearMask = temp;
      this->update ();
   }
}

QString QBitStatus::getOnClearMask ()
{
   return this->intToMask (this->mOnClearMask);
}

//=============================================================================
//
void QBitStatus::setOffClearMask (const QString value)
{
   int temp;

   temp = this->maskToInt (value);

   if (this->mOffClearMask != temp) {
      this->mOffClearMask = temp;
      this->update ();
   }
}

QString QBitStatus::getOffClearMask ()
{
   return this->intToMask (this->mOffClearMask);
}

//=============================================================================
//
void QBitStatus::setReversePolarityMask (const QString value)
{
   int temp;

   temp = this->maskToInt (value);

   if (this->mReversePolarityMask != temp) {
      this->mReversePolarityMask = temp;
      this->update ();
   }
}

QString QBitStatus::getReversePolarityMask ()
{
   return this->intToMask (this->mReversePolarityMask);
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


//=============================================================================
// Private static functions
//=============================================================================
//
/*! ---------------------------------------------------------------------------
 *  Converts integer to a binary mask of hexadecimal characters.
 */
QString QBitStatus::intToMask (int n)
{
   QString result ("");
   result.sprintf ("%02X-%02X-%02X-%02X", (n>>24)&255, (n>>16)&255, (n>>8)&255, (n>>0)&255);
   return result;
}

/*! ---------------------------------------------------------------------------
 *  Converts a hexadecimal character mask to an integer;
 */
int QBitStatus::maskToInt (const QString mask)
{
   int result;
   int j;
   char c;

   result = 0;
   for (j = 0; j < mask.length(); j++) {
      c = mask.at (j).toAscii ();

      switch (c) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
         result = (result << 4) + int (c) - int ('0');
         break;

      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
         result = (result << 4) + int (c) - int ('A') + 10;
         break;

      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
         result = (result << 4) + int (c) - int ('a') + 10;
         break;

      default:
         // not a hex digit - don't care - just ignore.
         break;
      }
   }
   return result;
}

// end
