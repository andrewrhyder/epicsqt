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

#include "QESimpleShape.h"


//-----------------------------------------------------------------------------
// Constructor with no initialisation
//
QESimpleShape::QESimpleShape (QWidget * parent) : QWidget (parent), QEWidget (this)
{
   this->setup ();
}


//-----------------------------------------------------------------------------
// Constructor with known variable
//
QESimpleShape::QESimpleShape (const QString & variableNameIn, QWidget * parent) : QWidget (parent), QEWidget (this)
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

//-----------------------------------------------------------------------------
//
void QESimpleShape::paintEvent (QPaintEvent * /* event */ )
{
   QPainter painter (this);
   QPen pen;
   QBrush brush;
   QRect rect;
   QPoint polygon [8];
   QColor colour;
   qcaobject::QCaObject * qca;

   // Associated qca object - test if connected but avoid the segmentation fault.
   //
   qca = this->getQcaItem (0);
   if (qca && this->isConnected) {
      if (this->getDisplayAlarmState ()) {
         // Use alarm colour
         //
         QCaAlarmInfo ai = qca->getAlarmInfo ();    // 1st param is & mode - cannot use a function.
         colour = this->getColor (ai, 255);
      } else {
         // Use value to index colour table.
         //
         colour = this->getColourProperty (this->getValue ());
      }
      pen.setColor (QColor (0, 0, 0, 255));
   } else {
      // Washed-out gray.
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
         polygon[4] = polygon[0];     // close loop
         painter.drawPolygon (polygon, 5);
         break;


      case triangleUp:
         polygon[0] = QPoint ((rect.left () + rect.right ()) / 2, rect.top ());
         polygon[1] = QPoint (rect.right (), rect.bottom ());
         polygon[2] = QPoint (rect.left (), rect.bottom ());
         polygon[3] = polygon[0];     // close loop
         painter.drawPolygon (polygon, 4);
         break;

      case triangleDown:
         polygon[0] = QPoint ((rect.left () + rect.right ()) / 2, rect.bottom ());
         polygon[1] = QPoint (rect.right (), rect.top ());
         polygon[2] = QPoint (rect.left (), rect.top ());
         polygon[3] = polygon[0];     // close loop
         painter.drawPolygon (polygon, 4);
         break;

      case triangleLeft:
         polygon[0] = QPoint (rect.left (), (rect.top () + rect.bottom ()) / 2);
         polygon[1] = QPoint (rect.right (), rect.top ());
         polygon[2] = QPoint (rect.right (), rect.bottom ());
         polygon[3] = polygon[0];     // close loop
         painter.drawPolygon (polygon, 4);
         break;

      case triangleRight:
         polygon[0] = QPoint (rect.right (), (rect.top () + rect.bottom ()) / 2);
         polygon[1] = QPoint (rect.left (), rect.top ());
         polygon[2] = QPoint (rect.left (), rect.bottom ());
         polygon[3] = polygon[0];     // close loop
         painter.drawPolygon (polygon, 4);
         break;

      default:
         break;
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
      QObject::connect (qca, SIGNAL (integerChanged (const long &, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)),
                        this, SLOT  (setShapeValue  (const long &, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)));

      QObject::connect (qca, SIGNAL (integerArrayChanged (const QVector<long>&, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)),
                        this, SLOT  (setShapeValues      (const QVector<long>&, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)));

      QObject::connect (qca, SIGNAL (connectionChanged (QCaConnectionInfo &)),
                        this, SLOT  (connectionChanged (QCaConnectionInfo &)));
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
   this->setDataDisabled (this->isConnected);
   this->updateToolTipConnection (this->isConnected);

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
   QCAALARMINFO_SEVERITY severity;
   int moduloValue;

   // If in alarm, display as an alarm - only update if changed
   //
   severity = alarmInfo.getSeverity ();
   if (severity != this->lastSeverity) {
      updateToolTipAlarm (alarmInfo.severityName ());
   }

   moduloValue = (int) (valueIn & 0x0F);
   if ((this->value != moduloValue) || (severity != this->lastSeverity)) {
      this->value = moduloValue;
      this->lastSeverity = severity;
      this->update ();
   }

   // Signal a database value change to any Link widgets
   //
   emit dbValueChanged (value);
}


//------------------------------------------------------------------------------
// Extract first element (0 index) and use this value.
//
void QESimpleShape::setShapeValues (const QVector <long>&values,
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
void QESimpleShape::setColourProperty (int slot, QColor colour)
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

QVariant QESimpleShape::copyData ()
{
   return QVariant (this->getValue ());
}


// end
