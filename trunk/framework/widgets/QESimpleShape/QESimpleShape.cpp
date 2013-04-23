/*  QESimpleShape.cpp
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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <QECommon.h>
#include "QESimpleShape.h"


//-----------------------------------------------------------------------------
// Constructor with no initialisation
//
QESimpleShape::QESimpleShape (QWidget * parent):QWidget (parent), QEWidget (this)
{
   this->setup ();
}


//-----------------------------------------------------------------------------
// Constructor with known variable
//
QESimpleShape::QESimpleShape (const QString & variableNameIn, QWidget * parent):QWidget (parent), QEWidget (this)
{
   this->setup ();
   this->setVariableName (variableNameIn, 0);
}


//-----------------------------------------------------------------------------
// Setup common to all constructors
//
void QESimpleShape::setup ()
{
   int j;

   // Set up data
   // This control uses a single data source
   //
   this->setNumVariables (1);

   this->value = 0;
   this->setDisplayAlarmState (true);
   this->shape = rectangle;
   this->textFormat = FixedText;
   this->fixedText = "";
   this->textImage = "";

   for (j = 0; j < 16; j++) {
      this->colourList[j] = QColor (200, 200, 200, 255);
   }

   this->setAllowDrop (false);

   // Set the initial state
   //
   this->lastSeverity = QCaAlarmInfo::getInvalidSeverity ();
   this->isConnected = false;

   QWidget::setEnabled (false); // Reflects initial disconnected state

   // Use default context menu.
   //
   this->setupContextMenu (this);

   // Set up a connection to recieve variable name property changes
   // The variable name property manager class only delivers an updated
   // variable name after the user has stopped typing.
   //
   QObject::connect (&this->variableNamePropertyManager, SIGNAL (newVariableNameProperty    (QString, QString, unsigned int)),
                     this,                               SLOT   (useNewVariableNameProperty (QString, QString, unsigned int)));

}

//-----------------------------------------------------------------------------
// Reduce width or height of rectangle so that it becomes square, and offset
// to keep it centred.
//
void QESimpleShape::equaliseRect (QRect & rect)
{
   int diff;

   diff = rect.width () - rect.height ();

   if (diff > 0) {
      rect.setWidth (rect.height ());
      rect.moveLeft (diff / 2);

   } else if (diff < 0) {
      rect.setHeight (rect.width ());
      rect.moveTop ((-diff) / 2);
   }
}

//------------------------------------------------------------------------------
//
void QESimpleShape::drawText (QPainter & painter, QPoint & textCentre, QString & text)
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
void QESimpleShape::paintEvent (QPaintEvent * /* event */ )
{
   QPainter painter (this);
   QPen pen;
   QBrush brush;
   QRect rect;
   QPoint polygon[8];
   QColor colour;
   qcaobject::QCaObject * qca;
   QString text;

   // Associated qca object - test if connected but avoid the segmentation fault.
   //
   qca = this->getQcaItem (0);
   if (qca && this->isConnected) {
      if (this->getDisplayAlarmState ()) {
         // Use alarm colour
         //
         QCaAlarmInfo ai = qca->getAlarmInfo ();        // 1st param is & mode - cannot use a function.
         colour = this->getColor (ai, 255);
      } else {
         // Use value to index colour table.
         //
         colour = this->getColourProperty (this->getModuloValue ());
      }
      pen.setColor (QColor (0, 0, 0, 255));
   } else {
      // Not connected - use washed-out gray.
      //
      colour = QColor (220, 220, 220, 255);
      pen.setColor (QColor (140, 140, 140, 255));
   }

   pen.setWidth (1);
   painter.setPen (pen);

   brush.setStyle (Qt::SolidPattern);
   brush.setColor (colour);
   painter.setBrush (brush);

   rect = this->geometry ();
   rect.moveTo (0, 0);

   rect.setWidth (rect.width () - 1);
   rect.setHeight (rect.height () - 1);

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

      default:
         break;
   }

   // Get the rquired text -f any.
   //
   text = this->getTextImage ();
   if (!text.isEmpty ()) {
      // Set default centre text positions.
      //
      QPoint textCentre (this->width () / 2, this->height () / 2);

      int r, g, b, a;
      bool white_text;

      if (qca && this->isConnected) {
         // Split colour
         // Note: this is basically same as Strip Chart - need common colour utilities.
         //
         colour.getRgb (&r, &g, &b, &a);

         // Weight sum of background colour to detrmine if white or black text.
         //
         white_text = ((2 * r + 3 * g + 2 * b) <= (7 * 102));      // 2+3+2 == 7

         // Dark are bright background colour ?
         //
         if (white_text) {
            pen.setColor (QColor (255, 255, 255, 255));    // while font
         } else {
            pen.setColor (QColor (0, 0, 0, 255));  // black font
         }
      } else {
         pen.setColor (QColor (140, 140, 140, 255));   // gray
      }
      painter.setPen (pen);

      this->drawText (painter, textCentre, text);
   }
}

//------------------------------------------------------------------------------
// Implementation of QEWidget's virtual funtion to create the specific type of QCaObject required.
// For shape, a QCaObject that streams integers is required.
//
qcaobject::QCaObject * QESimpleShape::createQcaItem (unsigned int variableIndex)
{

   qcaobject::QCaObject * result;

   if (variableIndex == 0) {
      result = new QEInteger (getSubstitutedVariableName (variableIndex), this, &integerFormatting, variableIndex);
   } else {
      result = NULL;            // Unexpected
   }

   return result;
}


//------------------------------------------------------------------------------
// Start updating.
// Implementation of VariableNameManager's virtual funtion to establish a
// connection to a PV as the variable name has changed.
// This function may also be used to initiate updates when loaded as a plugin.
//
void QESimpleShape::establishConnection (unsigned int variableIndex)
{
   // Create a connection.
   // If successfull, the QCaObject object that will supply data update signals will be returned
   // Note createConnection creates the connection and returns reference to existing QCaObject.
   //
   qcaobject::QCaObject * qca = createConnection (variableIndex);

   // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots.
   //
   if ((qca) && (variableIndex == 0)) {
      QObject::connect (qca,  SIGNAL (integerChanged (const long &, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)),
                        this, SLOT   (setShapeValue  (const long &, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)));

      QObject::connect (qca,  SIGNAL (integerArrayChanged (const QVector < long >&, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)),
                        this, SLOT   (setShapeValues      (const QVector < long >&, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)));

      QObject::connect (qca,  SIGNAL (connectionChanged (QCaConnectionInfo &)),
                        this, SLOT   (connectionChanged (QCaConnectionInfo &)));
   }
}


//------------------------------------------------------------------------------
// Act on a connection change.
// Change how the s looks and change the tool tip
// This is the slot used to recieve connection updates from a QCaObject based class.
//
void QESimpleShape::connectionChanged (QCaConnectionInfo & connectionInfo)
{
   // If connected, enable the widget if the QE enabled property is true
   // If disconnected always disable the widget.
   //
   this->isConnected = connectionInfo.isChannelConnected ();
   this->setDataDisabled (!this->isConnected);
   this->updateToolTipConnection (this->isConnected);
   this->isFirstUpdate = true;  // more trob. than it's worth to check if connect or disconnect.

   if (!this->isConnected) {
      // Disconnected - clear the alarm state.
      //
      this->updateToolTipAlarm ("");
      this->lastSeverity = QCaAlarmInfo::getInvalidSeverity ();
   }
   this->update ();
}


//------------------------------------------------------------------------------
// Update the shape value
// This is the slot used to recieve data updates from a QCaObject based class.
//
void QESimpleShape::setShapeValue (const long &valueIn, QCaAlarmInfo & alarmInfo, QCaDateTime &, const unsigned int &)
{
   qcaobject::QCaObject * qca;
   QCAALARMINFO_SEVERITY severity;

   // If in alarm, display as an alarm - only update if changed
   //
   severity = alarmInfo.getSeverity ();
   if (severity != this->lastSeverity) {
      updateToolTipAlarm (alarmInfo.severityName ());
   }
   
   // Associated qca object - avoid the segmentation fault.
   //
   qca = getQcaItem (0);
   if (!qca) {
      return;
   }
   
   // Set up variable details used by some formatting options.
   //
   if (this->isFirstUpdate) {
      this->stringFormatting.setDbEgu (qca->getEgu ());
      this->stringFormatting.setDbEnumerations (qca->getEnumerations ());
      this->stringFormatting.setDbPrecision (qca->getPrecision ());
   }

   if ((this->isFirstUpdate) || (this->value != valueIn) || (severity != this->lastSeverity)) {
      this->value = valueIn;
      this->lastSeverity = severity;
      this->setTextImage ();
      this->update ();
   }

   // Signal a database value change to any Link widgets
   //
   emit dbValueChanged (value);

   // This update is over, clear first update flag.
   //
   this->isFirstUpdate = false;
}

//------------------------------------------------------------------------------
//
void QESimpleShape::setTextImage ()
{
   switch (this->getTextFormat ()) {

      case QESimpleShape::FixedText:
         this->textImage = this->getFixedText ();
         break;

      case QESimpleShape::PvText:
         this->textImage = this->stringFormatting.formatString (this->getValue ());
         break;

      case QESimpleShape::LocalEnumeration:
         this->textImage = this->stringFormatting.formatString (this->getModuloValue ());
         break;

      default:
         this->textImage = "";
   }
}


//------------------------------------------------------------------------------
//
QString QESimpleShape::getTextImage () {
   return this->textImage;
}

//------------------------------------------------------------------------------
// Extract first element (0 index) and use this value.
//
void QESimpleShape::setShapeValues (const QVector<long> & values,
                                    QCaAlarmInfo & alarmInfo, QCaDateTime & dateTime,
                                    const unsigned int &variableIndex)
{
   int slot = 0;
   this->setShapeValue (values.value (slot), alarmInfo, dateTime, variableIndex);
}

//------------------------------------------------------------------------------
//  Update variable name etc.
//
void QESimpleShape::useNewVariableNameProperty (QString variableNameIn,
                                                QString variableNameSubstitutionsIn,
                                                unsigned int variableIndex)
{
   this->setVariableNameAndSubstitutions (variableNameIn, variableNameSubstitutionsIn, variableIndex);
}

//------------------------------------------------------------------------------
//
int QESimpleShape::getValue ()
{
   return this->value;
}

//------------------------------------------------------------------------------
//
int QESimpleShape::getModuloValue ()
{
   return this->value & 0x0F;
}

//------------------------------------------------------------------------------
//
void QESimpleShape::setShape (Shapes shapeIn)
{
   if (this->shape != shapeIn) {
      this->shape = shapeIn;
      this->update ();
   }
}

//------------------------------------------------------------------------------
//
QESimpleShape::Shapes QESimpleShape::getShape ()
{
   return this->shape;
}

//------------------------------------------------------------------------------
//
void QESimpleShape::setTextFormat (TextFormats value)
{
   if (this->textFormat != value) {
      this->textFormat = value;

      this->textImage = "";

      // Convert local format into appropriate string formmating.
      //
      switch (this->textFormat) {

         case QESimpleShape::FixedText:
            this->textImage = this->fixedText;
            break;

         case QESimpleShape::PvText:
            this->setFormat (QEStringFormatting::FORMAT_DEFAULT);
            break;

         case QESimpleShape::LocalEnumeration:
            this->setFormat (QEStringFormatting::FORMAT_LOCAL_ENUMERATE);
            break;

         default:
            break;
      }

      this->update ();
   }
}

//------------------------------------------------------------------------------
//
QESimpleShape::TextFormats QESimpleShape::getTextFormat ()
{
   return this->textFormat;
}

//------------------------------------------------------------------------------
//
void QESimpleShape::setFixedText (QString value)
{
   if (this->fixedText != value) {
      this->fixedText = value;
      if (this->getTextFormat () == FixedText) {
         this->textImage = this->fixedText;
         this->update ();
      }
   }
}

//------------------------------------------------------------------------------
//
QString QESimpleShape::getFixedText ()
{
   return this->fixedText;
}

//------------------------------------------------------------------------------
//
void QESimpleShape::setColourProperty (int slot, QColor colour)
{
   if ((slot >= 0) && (slot < 16)) {
      if (this->colourList[slot] != colour) {
         this->colourList[slot] = colour;
         if (this->getModuloValue () == slot) {
            this->update ();
         }
      }
   }
}

//------------------------------------------------------------------------------
//
QColor QESimpleShape::getColourProperty (int slot)
{
   QColor result;

   if ((slot >= 0) && (slot < 16)) {
      result = this->colourList[slot];
   } else {
      result = QColor (0, 0, 0, 255);
   }
   return result;
}

//==============================================================================
// Drag drop
//
void QESimpleShape::setDrop (QVariant drop)
{
   setVariableName (drop.toString (), 0);
   establishConnection (0);
}

//------------------------------------------------------------------------------
//
QVariant QESimpleShape::getDrop ()
{
   return QVariant (getSubstitutedVariableName (0));
}

//==============================================================================
// Copy (no paste)
//
QString QESimpleShape::copyVariable ()
{
   return getSubstitutedVariableName (0);
}

//------------------------------------------------------------------------------
//
QVariant QESimpleShape::copyData ()
{
   return QVariant (this->getValue ());
}

// end
