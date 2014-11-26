/*  QSimpleShape.cpp
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
#include <QPainter>
#include <QECommon.h>

#include "QSimpleShape.h"

#define DEBUG qDebug () << "QSimpleShape" << __LINE__ << __FUNCTION__

//-----------------------------------------------------------------------------
//
QSimpleShape::QSimpleShape (QWidget * parent) : QWidget (parent)
{
   // This class properties.
   //
   this->value = 0;
   this->shape = rectangle;
   this->textFormat = FixedText;
   this->fixedText = "";
   this->isValid = true;
   this->edgeWidth = 1;
   this->flashStateIsOn = false;

   this->edgeColour     = QColor (0,   0,   0);        // black
   this->flashOffColour = QColor (200, 200, 200,  0);  // clear, alpha = 0
   this->invalidColour  = QColor (255, 182, 128);      // orangey

   this->flashRate = QEScanTimers::Medium;
   QEScanTimers::attach (this, SLOT (flashTimeout (const bool)), this->flashRate);

   for (int j = 0; j < 16; j++) {
      this->colourList[j] = QColor (200, 200, 200, 255);
      this->flashList [j] = false;
   }
}

//-----------------------------------------------------------------------------
// Reduce width or height of rectangle so that it becomes square, and offset
// to keep it centred.
//
void QSimpleShape::equaliseRect (QRect& rect)
{
   const int diff = rect.width () - rect.height ();
   const int ew = this->getEdgeWidth ();

   if (diff > 0) {
      rect.setWidth (rect.height ());
      rect.moveLeft ((+diff) / 2 + ew / 2);  // Note: moves left edge, not to the left.

   } else if (diff < 0) {
      rect.setHeight (rect.width ());
      rect.moveTop  ((-diff) / 2 + ew / 2);
   }
}

//------------------------------------------------------------------------------
//
void QSimpleShape::drawText (QPainter & painter, QPoint & textCentre, QString & text)
{
   QFont pf (this->font ());
   painter.setFont (pf);

   QFontMetrics fm = painter.fontMetrics ();
   int x;
   int y;

   // Centre text. For height, pointSize seems better than fm.height ()
   // painter.drawText takes bottom left coordinates.
   //
   x = textCentre.x () - fm.width (text) / 2;
   y = textCentre.y () + (pf.pointSize () + 1) / 2;

   // If text too wide, then ensure we show most significant part.
   //
   painter.drawText (MAX (1, x), y, text);
}

//-----------------------------------------------------------------------------
//
void QSimpleShape::paintEvent (QPaintEvent*)
{
   QPainter painter (this);
   QPen pen;
   QBrush brush;
   QRect rect;
   QPoint polygon[8];
   QColor colour;
   QColor boarderColour;
   bool washedOut = false;
   QString text;
   int x0, x1, x2;
   int y0, y1, y2;

   // Are we in a valid state?
   //
   if (this->getIsValid ()) {
      // Yes - get current value -  constrained 0 .. 15
      //
      const int mv = this->getValue ();

      colour = this->getColourProperty (mv);

      // flash the colour, but not the boarder.
      //
      if (this->flashList [mv] && !this->flashStateIsOn) {
         colour = this->flashOffColour;
      }

   } else {
      // No - go with the invalid colour.
      //
      colour = this->getInvalidColour ();
   }

   boarderColour = this->getEdgeColour ();

   washedOut = !this->isEnabled ();
   if (washedOut) {
      // Disconnected or disabled - grey out colours.
      //
      colour = QEUtilities::blandColour (colour);
      boarderColour = QEUtilities::blandColour (boarderColour);
   }

   const int ew = this->getEdgeWidth ();

   pen.setWidth (ew);
   pen.setColor (boarderColour);
   painter.setPen (pen);

   brush.setStyle (Qt::SolidPattern);
   brush.setColor (colour);
   painter.setBrush (brush);

   rect = this->geometry ();
   rect.moveTo (ew / 2, ew / 2);

   rect.setWidth (rect.width () - ew);
   rect.setHeight (rect.height () - ew);

   switch (this->getShape ()) {

      case circle:
         this->equaliseRect (rect);
         // fall through
      case ellipse:
         painter.drawEllipse (rect);
         break;


      case square:
         this->equaliseRect (rect);
         // fall through
      case rectangle:
         painter.drawRect (rect);
         break;


      case roundSquare:
         this->equaliseRect (rect);
         // fall through
      case roundRectangle:
         // Normalise corner radii.
         //
         painter.drawRoundRect (rect, 1200.0 / rect.width (), 1200.0 / rect.height ());
         break;


      case equalDiamond:
         this->equaliseRect (rect);
         // fall through
      case diamond:
         polygon[0] = QPoint ((rect.left () + rect.right ()) / 2, rect.top ());
         polygon[1] = QPoint (rect.right (), (rect.top () + rect.bottom ()) / 2);
         polygon[2] = QPoint ((rect.left () + rect.right ()) / 2, rect.bottom ());
         polygon[3] = QPoint (rect.left (), (rect.top () + rect.bottom ()) / 2);
         polygon[4] = polygon[0];       // close loop
         painter.drawPolygon (polygon, 5);
         break;


      case triangleUp:
         polygon[0] = QPoint ((rect.left () + rect.right ()) / 2, rect.top ());
         polygon[1] = QPoint (rect.right (), rect.bottom ());
         polygon[2] = QPoint (rect.left (), rect.bottom ());
         polygon[3] = polygon[0];       // close loop
         painter.drawPolygon (polygon, 4);
         break;

      case triangleDown:
         polygon[0] = QPoint ((rect.left () + rect.right ()) / 2, rect.bottom ());
         polygon[1] = QPoint (rect.right (), rect.top ());
         polygon[2] = QPoint (rect.left (), rect.top ());
         polygon[3] = polygon[0];       // close loop
         painter.drawPolygon (polygon, 4);
         break;

      case triangleLeft:
         polygon[0] = QPoint (rect.left (), (rect.top () + rect.bottom ()) / 2);
         polygon[1] = QPoint (rect.right (), rect.top ());
         polygon[2] = QPoint (rect.right (), rect.bottom ());
         polygon[3] = polygon[0];       // close loop
         painter.drawPolygon (polygon, 4);
         break;

      case triangleRight:
         polygon[0] = QPoint (rect.right (), (rect.top () + rect.bottom ()) / 2);
         polygon[1] = QPoint (rect.left (), rect.top ());
         polygon[2] = QPoint (rect.left (), rect.bottom ());
         polygon[3] = polygon[0];       // close loop
         painter.drawPolygon (polygon, 4);
         break;

      case triangleTopRight:
         polygon[0] = QPoint (rect.right (), rect.top ());
         polygon[1] = QPoint (rect.right (), rect.bottom ());
         polygon[2] = QPoint (rect.left (), rect.top ());
         polygon[3] = polygon[0];       // close loop
         painter.drawPolygon (polygon, 4);
         break;

      case triangleBottomRight:
         polygon[0] = QPoint (rect.right (), rect.bottom ());
         polygon[1] = QPoint (rect.left (), rect.bottom ());
         polygon[2] = QPoint (rect.right (), rect.top ());
         polygon[3] = polygon[0];       // close loop
         painter.drawPolygon (polygon, 4);
         break;

      case triangleBottomLeft:
         polygon[0] = QPoint (rect.left (), rect.bottom ());
         polygon[1] = QPoint (rect.left (), rect.top ());
         polygon[2] = QPoint (rect.right (), rect.bottom ());
         polygon[3] = polygon[0];       // close loop
         painter.drawPolygon (polygon, 4);
         break;

      case triangleTopLeft:
         polygon[0] = QPoint (rect.left (), rect.top ());
         polygon[1] = QPoint (rect.right (), rect.top ());
         polygon[2] = QPoint (rect.left (), rect.bottom ());
         polygon[3] = polygon[0];       // close loop
         painter.drawPolygon (polygon, 4);
         break;

      case arrowUp:
         // calculate some intermediate values.
         //
         y0 = rect.top () + rect.width () / 2;
         x0 = (rect.left () + rect.right ()) / 2;
         x1 = (2 * rect.left () + rect.right ()) / 3;
         x2 = (rect.left () + 2 * rect.right ()) / 3;

         polygon[0] = QPoint (x0, rect.top ());
         polygon[1] = QPoint (rect.right (), y0);
         polygon[2] = QPoint (x2, y0);
         polygon[3] = QPoint (x2, rect.bottom ());
         polygon[4] = QPoint (x1, rect.bottom ());
         polygon[5] = QPoint (x1, y0);
         polygon[6] = QPoint (rect.left (), y0);
         polygon[7] = polygon[0];       // close loop
         painter.drawPolygon (polygon, 7);
         break;

      case arrowDown:
         // calculate some intermediate values.
         //
         y0 = rect.bottom () - rect.width () / 2;
         x0 = (rect.left () + rect.right ()) / 2;
         x1 = (2 * rect.left () + rect.right ()) / 3;
         x2 = (rect.left () + 2 * rect.right ()) / 3;

         polygon[0] = QPoint (x0, rect.bottom ());
         polygon[1] = QPoint (rect.right (), y0);
         polygon[2] = QPoint (x2, y0);
         polygon[3] = QPoint (x2, rect.top ());
         polygon[4] = QPoint (x1, rect.top ());
         polygon[5] = QPoint (x1, y0);
         polygon[6] = QPoint (rect.left (), y0);
         polygon[7] = polygon[0];       // close loop
         painter.drawPolygon (polygon, 7);
         break;

      case arrowLeft:
         // calculate some intermediate values.
         //
         x0 = rect.left () + rect.height () / 2;
         y0 = (rect.top () + rect.bottom ()) / 2;
         y1 = (2 * rect.top () + rect.bottom ()) / 3;
         y2 = (rect.top () + 2 * rect.bottom ()) / 3;

         polygon[0] = QPoint (rect.left (),y0);
         polygon[1] = QPoint (x0, rect.top ());
         polygon[2] = QPoint (x0, y1);
         polygon[3] = QPoint (rect.right (), y1);
         polygon[4] = QPoint (rect.right (), y2);
         polygon[5] = QPoint (x0, y2);
         polygon[6] = QPoint (x0, rect.bottom ());
         polygon[7] = polygon[0];       // close loop
         painter.drawPolygon (polygon, 7);
         break;

      case arrowRight:
         // calculate some intermediate values.
         //
         x0 = rect.right () - rect.height () / 2;
         y0 = (rect.top () + rect.bottom ()) / 2;
         y1 = (2 * rect.top () + rect.bottom ()) / 3;
         y2 = (rect.top () + 2 * rect.bottom ()) / 3;

         polygon[0] = QPoint (rect.right (), y0);
         polygon[1] = QPoint (x0, rect.top ());
         polygon[2] = QPoint (x0, y1);
         polygon[3] = QPoint (rect.left (), y1);
         polygon[4] = QPoint (rect.left (), y2);
         polygon[5] = QPoint (x0, y2);
         polygon[6] = QPoint (x0, rect.bottom ());
         polygon[7] = polygon[0];       // close loop
         painter.drawPolygon (polygon, 7);
         break;

      default:
         break;
   }

   // Get the rquired text -f any.
   //
   text = this->calcTextImage ();
   if (!text.isEmpty ()) {
      // Set default centre text positions.
      //
      QPoint textCentre (this->width () / 2, this->height () / 2);

      if (!washedOut) {
         pen.setColor (QEUtilities::fontColour (colour));
      } else {
         pen.setColor (QColor (140, 140, 140, 255));   // gray
      }
      painter.setPen (pen);
      this->drawText (painter, textCentre, text);
   }
}

//------------------------------------------------------------------------------
//
void QSimpleShape::flashTimeout (const bool isOn)
{
   this->flashStateIsOn = isOn;
   this->update ();   // only call is current state marked as flashing???
}

//------------------------------------------------------------------------------
//
QString QSimpleShape::calcTextImage ()
{
   QString result;

   switch (this->getTextFormat ()) {

      case QSimpleShape::FixedText:
         result = this->getFixedText ();
         break;

      case QSimpleShape::UseStrings:
         result = this->strings.value (this->getValue (), "");
         break;

      default:
         result = "?";
   }

   return result;
}

//------------------------------------------------------------------------------
//
void QSimpleShape::setValue (const int valueIn)
{
   // We want the value module 16
   // Note: % operator is remainder not modulo, bit wise and does the trick.
   //
   this->value = valueIn & 15;
   this->update ();
}

//------------------------------------------------------------------------------
//
int QSimpleShape::getValue () const
{
   return this->value & 15;   // pedantic.
}

//------------------------------------------------------------------------------
//
void QSimpleShape::setEdgeWidth (const int edgeWidthIn)
{
   this->edgeWidth = LIMIT (edgeWidthIn, 1, 20);
   this->update ();
}

//------------------------------------------------------------------------------
//
int QSimpleShape::getEdgeWidth () const
{
   return this->edgeWidth;
}

//------------------------------------------------------------------------------
//
void QSimpleShape::setEdgeColour (const QColor edgeColourIn)
{
   this->edgeColour = edgeColourIn;
   this->update ();
}

//------------------------------------------------------------------------------
//
QColor QSimpleShape::getEdgeColour () const
{
   return this->edgeColour;
}

//------------------------------------------------------------------------------
//
void QSimpleShape::setStrings (const QStringList& stringsIn)
{
   this->strings = stringsIn;

   // Pad/truncate as required.
   //
   while (this->strings.count() > 16) {
      this->strings.removeLast ();
   }
   while (this->strings.count() < 16) {
      this->strings.append ("");
   }

   if (this->getTextFormat () == UseStrings) {
      this->update ();
   }
}

//------------------------------------------------------------------------------
//
QStringList QSimpleShape::getStrings () const
{
   return this->strings;
}


//------------------------------------------------------------------------------
//
void QSimpleShape::setShape (Shapes shapeIn)
{
   if (this->shape != shapeIn) {
      this->shape = shapeIn;
      this->update ();
   }
}

//------------------------------------------------------------------------------
//
QSimpleShape::Shapes QSimpleShape::getShape () const
{
   return this->shape;
}

//------------------------------------------------------------------------------
//
void QSimpleShape::setTextFormat (TextFormats textFormatIn)
{
   if (this->textFormat != textFormatIn) {
      this->textFormat = textFormatIn;
      this->update ();
   }
}

//------------------------------------------------------------------------------
//
QSimpleShape::TextFormats QSimpleShape::getTextFormat () const
{
   return this->textFormat;
}

//------------------------------------------------------------------------------
//
void QSimpleShape::setFixedText (QString value)
{
   if (this->fixedText != value) {
      this->fixedText = value;
      if (this->getTextFormat () == FixedText) {
         this->update ();
      }
   }
}

//------------------------------------------------------------------------------
//
QString QSimpleShape::getFixedText () const
{
   return this->fixedText;
}

//------------------------------------------------------------------------------
//
void QSimpleShape::setFlashRate (QEScanTimers::ScanRates flashRateIn)
{
   const char* member = SLOT (flashTimeout (const bool));

   if (this->flashRate != flashRateIn) {
      QEScanTimers::detach (this, member);
      this->flashRate = flashRateIn;
      QEScanTimers::attach (this, member, this->flashRate);
   }
}

//------------------------------------------------------------------------------
//
QEScanTimers::ScanRates QSimpleShape::getFlashRate () const
{
   return this->flashRate;
}

//------------------------------------------------------------------------------
//
void QSimpleShape::setFlashOffColour (const QColor& flashOffColourIn)
{
   this->flashOffColour = flashOffColourIn;
   this->update ();
}

//------------------------------------------------------------------------------
//
QColor QSimpleShape::getFlashOffColour () const
{
   return this->flashOffColour;
}

//------------------------------------------------------------------------------
//
void QSimpleShape::setIsValid (const bool isValidIn)
{
   this->isValid = isValidIn;
   this->update ();
}

//------------------------------------------------------------------------------
//
bool QSimpleShape::getIsValid () const
{
   return this->isValid;
}

//------------------------------------------------------------------------------
//
void QSimpleShape::setInvalidColour (const QColor invalidColourIn)
{
   this->invalidColour = invalidColourIn;
   this->update ();
}

//------------------------------------------------------------------------------
//
QColor QSimpleShape::getInvalidColour () const
{
   return this->invalidColour;
}

//------------------------------------------------------------------------------
//
void QSimpleShape::setColourProperty (int slot, QColor colour)
{
   if ((slot >= 0) && (slot < 16)) {
      if (this->colourList[slot] != colour) {
         this->colourList[slot] = colour;
         if (this->getValue () == slot) {
            this->update ();
         }
      }
   }
}

//------------------------------------------------------------------------------
//
QColor QSimpleShape::getColourProperty (int slot) const
{
   QColor result;

   if ((slot >= 0) && (slot < 16)) {
      result = this->colourList[slot];
   } else {
      result = QColor (0, 0, 0, 255);
   }
   return result;
}


//------------------------------------------------------------------------------
//
void QSimpleShape::setFlashProperty (int slot, bool flash)
{
   if ((slot >= 0) && (slot < 16)) {
      this->flashList [slot] = flash;
   }
}

//------------------------------------------------------------------------------
//
bool QSimpleShape::getFlashProperty (int slot) const
{
   bool result = false;

   if ((slot >= 0) && (slot < 16)) {
      result = this->flashList [slot];
   }
   return result;
}

// end
