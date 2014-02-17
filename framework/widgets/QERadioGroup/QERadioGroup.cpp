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

// Normally we expect 16 for an enueration PV, but framework does a special for
// status (.STAT) PVs so need some more. Also use of local enueration values
// means many more values may be catered for.
//
#define MAXIMUM_BUTTONS   64

//-----------------------------------------------------------------------------
// Constructor with no initialisation
//
QERadioGroup::QERadioGroup (QWidget* parent) :
   QEGroupBox (" QERadioGroup ", parent)
{
   this->commonSetup ();
}

//-----------------------------------------------------------------------------
// Constructor with known variable
//
QERadioGroup::QERadioGroup (const QString& variableNameIn, QWidget* parent) :
   QEGroupBox (" QERadioGroup ", parent)
{
   this->commonSetup ();
   this->setVariableName (variableNameIn, 0);
}

//-----------------------------------------------------------------------------
// Constructor with title and known variable
//
QERadioGroup::QERadioGroup (const QString& title, const QString& variableNameIn, QWidget* parent) :
   QEGroupBox (title, parent)
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

//---------------------------------------------------------------------------------
//
QAbstractButton* QERadioGroup::createButton (QWidget* parent)
{
   QAbstractButton* result = NULL;

   switch (this->buttonStyle) {
      case Radio:
         result = new QRadioButton (parent);
         break;

      case Push:
         result = new QPushButton (parent);
         break;

      default:
         DEBUG  << "Invalid button style" << (int) this->buttonStyle;
         break;
   }
   result->setAutoExclusive (true);
   result->setCheckable (true);
   return result;
}

//-----------------------------------------------------------------------------
//
void QERadioGroup::reCreateAllButtons ()
{
   int j;
   QAbstractButton *button;

   // First selete any old existing buttons.
   //
   if (this->noSelectionButton) {
      delete this->noSelectionButton;
      this->noSelectionButton = NULL;
   }

   while (this->radioButtonList.count() > 0) {
      button = this->radioButtonList.takeFirst ();
      if (button) {
         delete button;
      }
   }

   // Create new buttons -invisble for now.
   // NOTE: radio buttons are added/removed from layout as and when needed.
   //
   for (j = 0; j < MAXIMUM_BUTTONS; j++) {
      button = this->createButton (this);
      button->setGeometry (-40, -40, 20, 20);
      button->setVisible (false);

      QObject::connect (button, SIGNAL (clicked (bool)),
                        this,   SLOT   (buttonClicked (bool)));

      this->radioButtonList.append (button);
   }

   // Hidden button set when no valid selection available.
   // We cannot (in some versions) deselect all.
   //
   this->noSelectionButton = this->createButton (this);
   this->noSelectionButton->setGeometry (-40, -40, 20, 20);
   this->noSelectionButton->setVisible (false);
}

//-----------------------------------------------------------------------------
// Setup common to all constructors
//
void QERadioGroup::commonSetup ()
{
   // QEGroupBox sets this to false (as it's not an EPICS aware widget).
   // But the QERadioGroup is EPICS aware, so set default to true.
   //
   this->setVariableAsToolTip (true);

   // Set up data
   // This control uses a single data source
   //
   this->setNumVariables (1);

   this->setMinimumSize (120, 40);

   // Set up default properties
   //
   this->useDbEnumerations = true;  // as opposed to local enumeations.
   this->setAllowDrop (false);
   this->setDisplayAlarmState (true);

   // Set the initial state
   //
   this->isConnected = false;
   this->currentIndex = -1;

   this->number = 0;
   this->rows = 0;
   this->cols = 2;
   this->buttonStyle = Radio;

   this->radioButtonLayout = new QGridLayout (this);
   this->radioButtonLayout->setContentsMargins (8, 4, 8, 4);  // left, top, right, bottom
   this->radioButtonLayout->setSpacing (4);

   // Create buttons - invisble for now.
   // NOTE: radio buttons are added/removed from layout as and when needed.
   //
   this->noSelectionButton = NULL;
   this->radioButtonList.clear ();
   this->reCreateAllButtons ();

   // Use default context menu.
   //
   this->setupContextMenu();

   // Set up a connection to recieve variable name property changes
   // The variable name property manager class only delivers an updated
   // variable name after the user has stopped typing.
   //
   QObject::connect
      (&this->variableNamePropertyManager, SIGNAL (newVariableNameProperty    (QString, QString, unsigned int)),
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
    // Note the connected state
    isConnected = connectionInfo.isChannelConnected();

    // Display the connected state
    updateToolTipConnection( isConnected );
    updateConnectionStyle( isConnected );

    this->isFirstUpdate = true;  // more trob. than it's worth to check if connect or disconnect.
}

//-----------------------------------------------------------------------------
//
void QERadioGroup::valueUpdate (const long &value,
                                QCaAlarmInfo & alarmInfo,
                                QCaDateTime &,
                                const unsigned int &variableIndex)
{
   QAbstractButton *selectedButton = NULL;
   int selectedButtonIndex;

   if (variableIndex != 0) {
      DEBUG << "unexpected variableIndex" << variableIndex;
      return;
   }

   // If and only iff first update (for this connection) then use enumeration
   // values to re-populate the radio group.
   //
   if (this->isFirstUpdate) {
      this->isFirstUpdate = false;
      this->setButtonText ();
   }

   // Set the selected index value.
   //
   this->currentIndex = value;

   if (this->valueToButtonIndexMap.contains (value)) {
      selectedButtonIndex = this->valueToButtonIndexMap.value (this->currentIndex, -1);
      if ((selectedButtonIndex >= 0) && (selectedButtonIndex < this->number)) {

         selectedButton = this->radioButtonList.value (selectedButtonIndex, NULL);
         if (selectedButton) {
            selectedButton->setChecked (true); // this will uncheck all other buttons
         }

         // On some styles, a down push button looks very mucjh like a non-down
         // button. To help emphasize the selected button, we set the font of
         // the selected button bold, and all the rest non-bold.
         //
         if (this->buttonStyle == Push) {
            for (int j = 0; j < this->number; j++) {
               QAbstractButton* otherButton = this->radioButtonList.value (j, NULL);
               if (otherButton) {
                  QFont otherFont = otherButton->font ();
                  otherFont.setBold (otherButton == selectedButton);
                  otherButton->setFont (otherFont);
               }
            }
         }
      }
   } else {
      // We haven't mapped this value - use hidden selection.
      // This will uncheck all the "real" buttons
      //
      this->noSelectionButton->setChecked (true);
   }

   // Signal a database value change to any Link widgets
   //
   this->dbValueChanged (value);

   // Invoke common alarm handling processing.
   //
   this->processAlarmInfo (alarmInfo);
}

//---------------------------------------------------------------------------------
//
void QERadioGroup::buttonClicked (bool)
{
   QAbstractButton* sendingButton = NULL;
   int buttonIndex;
   int value;

   // Determine signal sending widget
   //
   sendingButton = dynamic_cast<QAbstractButton*> (this->sender());
   if (!sendingButton) {
      return;
   }

   // Extract radio button position.
   //
   buttonIndex = this->radioButtonList.indexOf (sendingButton);
   if ((buttonIndex < 0) && (buttonIndex >= this->number)) {
      return;
   }

   // Determine associated value.
   //
   if (!this->buttonIndexToValueMap.contains (buttonIndex)) {
      return;
   }
   value = this->buttonIndexToValueMap.value (buttonIndex);

   // Don't write current value.
   //
   if (value == this->getCurrentIndex ()) {
      return;
   }

   // Write the new value.
   // Get the variable to write to
   //
   QEInteger* qca = (QEInteger *) getQcaItem (0);

   // If a QCa object is present (if there is a variable to write to)
   // then write the value.
   //
   if (qca) {
      qca->writeInteger (value);
   }
}

//---------------------------------------------------------------------------------
//
void QERadioGroup::setButtonText ()
{
   qcaobject::QCaObject* qca = NULL;
   QAbstractButton* button = NULL;
   QStringList enumerations;
   QString text;
   bool isMatch;
   int n;
   int j;

   // Build forward and revserse EPICS value to button index/position maps.
   // We do this even when using db enuberations and the mapping is trivial.
   //
   // Clear maps.
   //
   this->valueToButtonIndexMap.clear();
   this->buttonIndexToValueMap.clear ();

   if (this->useDbEnumerations) {
      qca = getQcaItem (0);
      if (qca) {
         enumerations = qca->getEnumerations ();

         // Create indentity map.
         //
         for (j = 0; j < enumerations.count (); j++) {
            this->valueToButtonIndexMap.insert (j, j);
            this->buttonIndexToValueMap.insert (j, j);
         }
      }

   } else {

      // Build up enumeration list using the local enumerations.  This may be
      // sparce - e.g.: 1 => Red, 5 => Blue, 63 => Green.   We create a reverse
      // map 0 => 1; 1 => 5; 2 => 63 so that when user selects the an element,
      // say Blue, we can map this directly to integer value of 5.
      //
      // Search upto values range -128 .. 128 - NOTE: this is arbitary.
      // Maybe localEnumeration can be modified to provide a min/max value
      // or a list of values.
      //
      enumerations.clear ();
      for (n = -128; n <= 128; n++) {
         text = this->localEnumerations.valueToText (n, isMatch);

         // Unless exact match, do not use.
         //
         if (!isMatch) continue;
         if (text.isEmpty ()) continue;

         j = enumerations.count ();
         if (j >= this->radioButtonList.count ()) {
            // We are full - ignore the rest.
            break;
         }
         enumerations.append (text);

         this->valueToButtonIndexMap.insert (n, j);
         this->buttonIndexToValueMap.insert (j, n);
      }
   }

   this->number = MIN (enumerations.count (), this->radioButtonList.count ());
   this->rows = (number + this->cols - 1) / MAX (this->cols, 1);

   for (j = 0; j < this->radioButtonList.count (); j++) {
      button = this->radioButtonList.value (j);
      button->setVisible (j < number);
      if (j < number) {
         button->setText (enumerations.value (j));
      }
   }

   this->setRadioButtonLayout ();
}

//---------------------------------------------------------------------------------
//
void QERadioGroup::setRadioButtonLayout ()
{
   int j;
   int row, col;

   // Remove any existing items from the layout.
   //
   while (this->radioButtonLayout->takeAt (0) != NULL);

   // Add buttons that are now required.
   //
   for (j = 0; j < this->number && j < this->radioButtonList.count (); j++) {
      QAbstractButton *button = this->radioButtonList.value (j, NULL);
      if (button) {

         // Find row and col - row major.
         //
         row = j / MAX (this->cols, 1);
         col = j % MAX (this->cols, 1);

         this->radioButtonLayout->addWidget (button, row, col);
      }
   }
}

//------------------------------------------------------------------------------
//
void QERadioGroup::setCurrentIndex (int indexIn)
{
   this->currentIndex = LIMIT (indexIn, -1, (this->number - 1));
}

//------------------------------------------------------------------------------
//
int QERadioGroup::getCurrentIndex ()
{
   return this->currentIndex;
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
void QERadioGroup::setUseDbEnumerations (bool useDbEnumerationsIn)
{
   if (this->useDbEnumerations != useDbEnumerationsIn) {
      this->useDbEnumerations = useDbEnumerationsIn;
      this->setButtonText ();
   }
}

//------------------------------------------------------------------------------
//
bool QERadioGroup::getUseDbEnumerations ()
{
   return this->useDbEnumerations;
}

//------------------------------------------------------------------------------
//
void QERadioGroup::setLocalEnumerations (const QString & localEnumerationsIn)
{
   this->localEnumerations.setLocalEnumeration (localEnumerationsIn);
   if (!this->useDbEnumerations) {
      this->setButtonText ();
   }
}

//------------------------------------------------------------------------------
//
QString QERadioGroup::getLocalEnumerations ()
{
   return this->localEnumerations.getLocalEnumeration ();
}

//------------------------------------------------------------------------------
//
void QERadioGroup::setColumns (int colsIn)
{
   int constrainedCols = LIMIT (colsIn, 1, 16);

   if (this->cols != constrainedCols) {
      this->cols = constrainedCols;
      this->rows = (this->number + this->cols - 1) / MAX (this->cols, 1);
      this->setRadioButtonLayout ();
   }
}

//------------------------------------------------------------------------------
//
int QERadioGroup::getColumns ()
{
   return this->cols;
}

//------------------------------------------------------------------------------
//
void QERadioGroup::setButtonStyle (const ButtonStyles & buttonStyleIn)
{
   if (this->buttonStyle != buttonStyleIn) {
      this->buttonStyle = buttonStyleIn;

      this->reCreateAllButtons ();
      this->setButtonText ();
      this->setRadioButtonLayout ();
   }
}

//------------------------------------------------------------------------------
//
QERadioGroup::ButtonStyles QERadioGroup::getButtonStyle ()
{
   return this->buttonStyle;
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
    if( isDraggingVariable() )
        return QVariant( copyVariable() );
    else
        return copyData();
}

//==============================================================================
// Copy / Paste
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

void QERadioGroup::paste( QVariant v )
{
    if( getAllowDrop() )
    {
        setDrop( v );
    }
}

// end
