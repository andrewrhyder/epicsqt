/*  QESimpleShape.cpp
 *
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
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
 *  Copyright (c) 2013,2014 Australian Synchrotron
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <QDebug>
#include <alarm.h>
#include <QECommon.h>
#include <QESimpleShape.h>

#define DEBUG qDebug () << "QESimpleShape" << __LINE__ << __FUNCTION__

//-----------------------------------------------------------------------------
// Constructor with no initialisation
//
QESimpleShape::QESimpleShape (QWidget * parent)
   : QSimpleShape (parent), QEWidget (this)
{
   this->setup ();
}

//-----------------------------------------------------------------------------
// Constructor with known variable
//
QESimpleShape::QESimpleShape (const QString & variableNameIn, QWidget * parent)
   : QSimpleShape (parent), QEWidget (this)
{
   this->setup ();
   this->setVariableName (variableNameIn, 0);
}

//-----------------------------------------------------------------------------
// Setup common to all constructors
//
void QESimpleShape::setup ()
{
   QCaAlarmInfo invalid (NO_ALARM, INVALID_ALARM);

   // Set up data
   //
   // This control used a single data source
   //
   this->setNumVariables (1);
   this->setVariableAsToolTip (true);
   this->setDisplayAlarmState (true);
   this->setAllowDrop (false);

   this->setIsActive (false);

   // Set the initial state
   // Widget is inactive until connected.
   //
   this->arrayIndex = 0;
   this->isStaticValue = false;
   this->channelValue = 0;
   this->channelAlarmColour = this->getColor (invalid, 255);

   // Use default context menu.
   //
   this->setupContextMenu ();

   // Set up a connection to recieve variable name property changes
   // The variable name property manager class only delivers an updated
   // variable name after the user has stopped typing.
   //
   QObject::connect (&this->variableNamePropertyManager, SIGNAL (newVariableNameProperty    (QString, QString, unsigned int)),
                     this,                               SLOT   (useNewVariableNameProperty (QString, QString, unsigned int)));
}


//------------------------------------------------------------------------------
// Update variable name etc.
//
void QESimpleShape::useNewVariableNameProperty (QString variableNameIn,
                                                QString variableNameSubstitutionsIn,
                                                unsigned int variableIndex)
{
   this->isStaticValue = false;

   // Note: essentially calls createQcaItem - provided expanded pv name is not empty.
   //
   this->setVariableNameAndSubstitutions (variableNameIn,
                                          variableNameSubstitutionsIn,
                                          variableIndex);
}


//------------------------------------------------------------------------------
// Implementation of QEWidget's virtual funtion to create the specific type of QCaObject required.
// For shape, a QCaObject that streams integers is required.
//
qcaobject::QCaObject* QESimpleShape::createQcaItem (unsigned int variableIndex)
{
   qcaobject::QCaObject* result = NULL;
   QString pvName;
   int number;
   bool okay;

   if (variableIndex == 0) {
      pvName = this->getSubstitutedVariableName (variableIndex);
      number = pvName.toInt (&okay);

      // Has designer/user just set an integer (as opposed to a PV name)?.
      // Note: no sensible PV names are just integers.
      //
      if (okay) {
         this->isStaticValue = true;
         this->channelValue = number;
         this->setValue (number);
      } else {
         // Assume it is a PV.
         //
         result = new QEInteger (pvName, this, &this->integerFormatting, variableIndex);

         // Apply currently defined array index.
         //
         result->setArrayIndex (this->arrayIndex);
      }

   } else {
      result = NULL;         // Unexpected
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
   qcaobject::QCaObject* qca = this->createConnection (variableIndex);

   // If a QCaObject object is now available to supply data update signals,
   // connect it to the appropriate slots.
   //
   if ((qca) && (variableIndex == 0)) {
      QObject::connect (qca,  SIGNAL (integerChanged  (const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int &)),
                        this, SLOT   (setShapeValue     (const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int &)));

      QObject::connect (qca,  SIGNAL (connectionChanged (QCaConnectionInfo&, const unsigned int &)),
                        this, SLOT   (connectionChanged (QCaConnectionInfo&, const unsigned int &)));
   }
}


//------------------------------------------------------------------------------
//
void QESimpleShape::activated ()
{
    // place holder
}

//------------------------------------------------------------------------------
// Act on a connection change.
// Change how the s looks and change the tool tip
// This is the slot used to recieve connection updates from a QCaObject based class.
//
void QESimpleShape::connectionChanged (QCaConnectionInfo & connectionInfo,
                                       const unsigned int& variableIndex)
{
   bool isConnected;

   // Note the connected state
   //
   isConnected = connectionInfo.isChannelConnected ();

   // Display the connected state
   //
   this->updateToolTipConnection (isConnected, variableIndex);

   // Widget is self draw - styleShheet not applicable per se.
   // No need to call updateConnectionStyle (isConnected),
   // but we do flag is in active (quazi disabled).
   //
   this->setIsActive (isConnected);

   this->isFirstUpdate = true;  // more trob. than it's worth to check if connect or disconnect.
}

//------------------------------------------------------------------------------
// Update the shape value
// This is the slot used to recieve data updates from a QCaObject based class.
//
void QESimpleShape::setShapeValue (const long &valueIn, QCaAlarmInfo & alarmInfo,
                                   QCaDateTime &, const unsigned int& variableIndex)
{
   qcaobject::QCaObject* qca;

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

   if (this->getTextFormat () == LocalEnumeration) {
      this->stringFormatting.setFormat (QEStringFormatting::FORMAT_LOCAL_ENUMERATE);
   } else {
      this->stringFormatting.setFormat (QEStringFormatting::FORMAT_DEFAULT);
   }

   // Save alarm colour.
   // Must do before we set value as getItemColour will get called.
   //
   this->channelAlarmColour = this->getColor (alarmInfo, 255);

   // Save value and update the shape value.
   // This essentially stores data twice, but the QSimpleShape stores the
   // value modulo 16, but we want to keep actual value (for getItemText).
   //
   this->channelValue = valueIn;
   this->setValue ((int) valueIn);

   // Signal a database value change to any Link widgets
   //
   emit dbValueChanged (valueIn);

   // Invoke tool tip handling directly. We don;t want to interfer with the style
   // as widget draws it's own stuff with own, possibly clear, colours.
   //
   this->updateToolTipAlarm (alarmInfo.severityName (), variableIndex);

   // This update is over, clear first update flag.
   //
   this->isFirstUpdate = false;
}

//------------------------------------------------------------------------------
//
QString QESimpleShape::getItemText ()
{
   QString result;

   switch (this->getTextFormat ()) {
      case QSimpleShape::PvText:
      case QSimpleShape::LocalEnumeration:

         if (this->isStaticValue) {
            // There is no channel - just use a plain number.
            //
            result.setNum (this->channelValue);
         } else {
            result = this->stringFormatting.formatString (this->channelValue);
         }
         break;

      default:
         // Just use base class function as is.
         //
         result = QSimpleShape::getItemText ();
         break;
   }

   return result;
}

//------------------------------------------------------------------------------
//
QColor QESimpleShape::getItemColour ()
{
   QColor result;

   if (this->getDisplayAlarmState ()) {
      result = this->channelAlarmColour;
   } else {
      // Just use base class function as is.
      result = QSimpleShape::getItemColour ();
   }
   return result;
}

//------------------------------------------------------------------------------
//
void QESimpleShape::stringFormattingChange()
{
   this->update ();
}

//------------------------------------------------------------------------------
//
void QESimpleShape::setArrayIndex (const int arrayIndexIn)
{
   this->arrayIndex = MAX (0, arrayIndexIn);

   qcaobject::QCaObject* qca = getQcaItem (0);
   if (qca) {
      // Apply to qca object and force update
      qca->setArrayIndex (this->arrayIndex);
      qca->resendLastData ();
   }
}

//------------------------------------------------------------------------------
//
int QESimpleShape::getArrayIndex () const
{
   return this->arrayIndex;
}


//==============================================================================
// Copy / paste
//
QString QESimpleShape::copyVariable ()
{
   return this->getSubstitutedVariableName (0);
}

//------------------------------------------------------------------------------
//
QVariant QESimpleShape::copyData ()
{
   return QVariant (this->getValue ());
}

void QESimpleShape::paste (QVariant v)
{
   this->setVariableName (v.toString (), 0);
   this->establishConnection (0);
}

// end
