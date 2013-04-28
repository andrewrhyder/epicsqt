/*  QERadioGroup.cpp
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

#include <QDebug>
#include <QECommon.h>

#include "QERadioGroup.h"

#define DEBUG qDebug () << "QERadioGroup" << __FUNCTION__ << __LINE__

// Normallu we expect 16 for an enueration PV, but framework does a special for
// status (.STAT) PVs, so allow a  little leaway.
//
#define MAXIMUM_BUTTONS   24


//-----------------------------------------------------------------------------
// Constructor with no initialisation
//
QERadioGroup::QERadioGroup (QWidget * parent) :
              QGroupBox (" QERadioGroup ", parent), QEWidget (this)
{
   this->commonSetup ();
}

//-----------------------------------------------------------------------------
// Constructor with known variable
//
QERadioGroup::QERadioGroup (const QString & variableNameIn, QWidget * parent) :
              QGroupBox (" QERadioGroup ", parent), QEWidget (this)
{
   this->commonSetup ();
   this->setVariableName (variableNameIn, 0);
}

//---------------------------------------------------------------------------------
//
QSize QERadioGroup::sizeHint () const
{
   return QSize (200, 80);
}

//-----------------------------------------------------------------------------
// Setup common to all constructors
//
void QERadioGroup::commonSetup ()
{
   int j;
   QRadioButton *button;

   // Set up data
   // This control uses a single data source
   //
   this->setNumVariables (1);

   this->setMinimumSize (120, 40);
   this->setDisplayAlarmState (true);
   this->setAllowDrop (false);

   // Set the initial state
   //
   this->lastSeverity = QCaAlarmInfo::getInvalidSeverity ();
   this->isConnected = false;
   this->currentIndex = -1;

   this->number = 0;
   this->rows = 0;
   this->cols = 1;

   // Create buttons - invisble for now.
   //
   for (j = 0; j < MAXIMUM_BUTTONS; j++) {
      button = new QRadioButton (this);
      button->setVisible (false);

      QObject::connect (button, SIGNAL (clicked (bool)),
			this,   SLOT   (buttonClicked (bool)));

      this->radioButtonList.append (button);
   }

   QGroupBox::setEnabled (false);       // Reflects initial disconnected state

   // Use default context menu.
   //
   this->setupContextMenu (this);

   // Set up a connection to recieve variable name property changes
   // The variable name property manager class only delivers an updated
   // variable name after the user has stopped typing.
   //
   QObject::connect
      (&this->variableNamePropertyManager, SIGNAL (newVariableNameProperty (QString, QString, unsigned int)),
       this,                                SLOT  (useNewVariableNameProperty (QString, QString, unsigned int)));
}

//------------------------------------------------------------------------------
// Implementation of QEWidget's virtual funtion to create the specific type of
// QCaObject required. A QCaObject that streams integers is required.
//
qcaobject::QCaObject * QERadioGroup::createQcaItem (unsigned int variableIndex)
{
   qcaobject::QCaObject * result = NULL;

   if (variableIndex != 0) {
      DEBUG << "unexpected variableIndex" << variableIndex;
      return NULL;
   }

   result = new QEInteger (getSubstitutedVariableName (variableIndex), this, &integerFormatting, variableIndex);
   return result;
}


//------------------------------------------------------------------------------
// Start updating.
// Implementation of VariableNameManager's virtual funtion to establish a
// connection to a PV as the variable name has changed.
// This function may also be used to initiate updates when loaded as a plugin.
//
void QERadioGroup::establishConnection (unsigned int variableIndex)
{
   if (variableIndex != 0) {
      DEBUG << "unexpected variableIndex" << variableIndex;
      return;
   }
   // Create a connection.
   // If successfull, the QCaObject object that will supply data update signals will be returned
   // Note createConnection creates the connection and returns reference to existing QCaObject.
   //
   qcaobject::QCaObject * qca = createConnection (variableIndex);

   // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots.
   //
   if ((qca) && (variableIndex == 0)) {
      QObject::connect (qca, SIGNAL (integerChanged (const long &, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)),
                        this, SLOT  (valueUpdate    (const long &, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)));

      QObject::connect (qca, SIGNAL (integerArrayChanged (const QVector < long >&, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)),
                        this, SLOT  (arrayValueUpdate    (const QVector < long >&, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)));

      QObject::connect (qca, SIGNAL (connectionChanged (QCaConnectionInfo &)),
                        this, SLOT  (connectionChanged (QCaConnectionInfo &)));
   }
}

//------------------------------------------------------------------------------
// Act on a connection change.
// Change how the s looks and change the tool tip
// This is the slot used to recieve connection updates from a QCaObject based class.
//
void QERadioGroup::connectionChanged (QCaConnectionInfo & connectionInfo)
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
}

//-----------------------------------------------------------------------------
//
void QERadioGroup::valueUpdate (const long &value,
                                QCaAlarmInfo & alarmInfo,
                                QCaDateTime &,
                                const unsigned int &variableIndex)
{
   qcaobject::QCaObject * qca = NULL;
   int j;
   QRadioButton *button = NULL;

   if (variableIndex != 0) {
      DEBUG << "unexpected variableIndex" << variableIndex;
      return;
   }
   // If and only iff first update then use enumeratiob values to populate the
   // the radio group.
   //
   if (this->isFirstUpdate) {
      this->isFirstUpdate = false;

      // Associated qca object - avoid the segmentation fault.
      //
      qca = getQcaItem (0);
      if (!qca) {
         DEBUG << "null qca [0]";
         return;
      }

      QStringList enumerations = qca->getEnumerations ();

      this->number = MIN (enumerations.count (), this->radioButtonList.count ());
      this->rows = (number + this->cols - 1) / MAX (this->cols, 1);

      for (j = 0; j < this->radioButtonList.count (); j++) {
         button = this->radioButtonList.value (j);
         button->setVisible (j < number);
         if (j < number) {
            button->setText (enumerations.value (j));
         }
      }

      this->setButtonGeometry ();

   }
   // Set the selected index value.
   //
   this->currentIndex = value;

   if ((this->currentIndex >= 0) && (this->currentIndex < this->number)) {
      button = this->radioButtonList.value (this->currentIndex, NULL);
      if (button) {
         button->setChecked (true);
      }
   }
   // Signal a database value change to any Link widgets
   //
   this->dbValueChanged (value);

   // Choose the alarm state to display.
   // If not displaying the alarm state, use a default 'no alarm' structure. This is
   // required so the any display of an alarm state is reverted if the displayAlarmState
   // property changes while displaying an alarm.
   //
   QCaAlarmInfo ai;
   if (this->getDisplayAlarmState ()) {
      ai = alarmInfo;
   }
   // Update alarm state if required.
   //
   if (ai.getSeverity () != this->lastSeverity) {
      updateToolTipAlarm (ai.severityName ());
      updateStatusStyle (ai.style ());
      lastSeverity = ai.getSeverity ();
   }
}

//-----------------------------------------------------------------------------
//
void QERadioGroup::arrayValueUpdate (const QVector < long >&values,
                                     QCaAlarmInfo & alarmInfo,
                                     QCaDateTime & dateTime,
                                     const unsigned int &variableIndex)
{
   int slot = 0;
   this->valueUpdate (values.value (slot), alarmInfo, dateTime, variableIndex);
}

//---------------------------------------------------------------------------------
//
void QERadioGroup::buttonClicked (bool)
{
   int checkedButton;
   int j;

   checkedButton = -1;
   for (j = 0; j < this->number && j < this->radioButtonList.count (); j++) {
      QRadioButton *button = this->radioButtonList.value (j, NULL);
      if (button && button->isVisible ()) {
         if (button->isChecked ()) {
            // foound it.
            //
            checkedButton = j;
            break;
         }
      }
   }

   if ((checkedButton >= 0) && (checkedButton != this->getCurrentIndex ())) {
      // Write the value.
      // Get the variable to write to
      //
      QEInteger *qca = (QEInteger *) getQcaItem (0);

      // If a QCa object is present (if there is a variable to write to)
      // then write the value
      if (qca) {
         qca->writeInteger (checkedButton);
      }
   }
}

//---------------------------------------------------------------------------------
//
void QERadioGroup::resizeEvent (QResizeEvent *)
{
   this->setButtonGeometry ();
}

//---------------------------------------------------------------------------------
//
void QERadioGroup::setButtonGeometry ()
{
   const int tm = 18;           // top margin
   const int bm = 4;            // bottom margin
   const int sm = 8;            // side margin
   const int s = 4;             // spacing (horizontal and vertical)

   const int tw = (this->width () - (2 * sm) + s - 1);
   const int th = (this->height () - (tm + bm) + s - 1);

   const double dx = double (tw) / double (MAX (this->cols, 1));
   const double dy = double (th) / double (MAX (this->rows, 1));

   int j;
   int row, col;
   double x0, x1;
   double y0, y1;

   for (j = 0; j < this->number && j < this->radioButtonList.count (); j++) {
      QRadioButton *button = this->radioButtonList.value (j, NULL);
      if (button && button->isVisible ()) {

         // Find row and col - row major.
         //
         row = j / MAX (this->cols, 1);
         col = j % MAX (this->cols, 1);

         // Do floating point arithmetic.
         //
         x0 = sm + (col * dx);
         x1 = x0 + dx - s;

         y0 = tm + (row * dy);
         y1 = y0 + dy - s;

         // Apply integer values.
         //
         button->setGeometry (int (x0), int (y0), int (x1) - int (x0), int (y1) - int (y0));
      }
   }
}

//==============================================================================
// Properties
// Update variable name etc.
//
void QERadioGroup::useNewVariableNameProperty (QString variableNameIn,
                                               QString variableNameSubstitutionsIn,
                                               unsigned int variableIndex)
{
   this->setVariableNameAndSubstitutions (variableNameIn, variableNameSubstitutionsIn, variableIndex);
}

//------------------------------------------------------------------------------
//
void QERadioGroup::setCurrentIndex (int indexIn)
{
   this->currentIndex = LIMIT (indexIn, -1, 15);
}

//------------------------------------------------------------------------------
//
int QERadioGroup::getCurrentIndex ()
{
   return this->currentIndex;
}

//------------------------------------------------------------------------------
//
void QERadioGroup::setColumns (int colsIn)
{
   this->cols = LIMIT (colsIn, 1, 16);
   this->rows = (number + this->cols - 1) / MAX (this->cols, 1);
   this->setButtonGeometry ();
}

//------------------------------------------------------------------------------
//
int QERadioGroup::getColumns ()
{
   return this->cols;
}

//==============================================================================
// Drag drop
//
void QERadioGroup::setDrop (QVariant drop)
{
   setVariableName (drop.toString (), 0);
   establishConnection (0);
}

//------------------------------------------------------------------------------
//
QVariant QERadioGroup::getDrop ()
{
   return QVariant (getSubstitutedVariableName (0));
}

//==============================================================================
// Copy (no paste)
//
QString QERadioGroup::copyVariable ()
{
   return getSubstitutedVariableName (0);
}

//------------------------------------------------------------------------------
//
QVariant QERadioGroup::copyData ()
{
   return QVariant (this->currentIndex);
}

// end
