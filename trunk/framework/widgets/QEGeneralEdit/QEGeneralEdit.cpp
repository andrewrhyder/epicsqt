/*  QEGeneralEdit.cpp
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
 *  Copyright (c) 2014 Australian Synchrotron.
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <QDebug>
#include <QECommon.h>
#include <QEScaling.h>

#include "QEGeneralEdit.h"

#define DEBUG qDebug () << "QEGeneralEdit" << __FUNCTION__ << __LINE__

//-----------------------------------------------------------------------------
// Constructor with no initialisation
//
QEGeneralEdit::QEGeneralEdit (QWidget * parent):QEFrame (parent)
{
   this->commonSetup ();
}

//-----------------------------------------------------------------------------
// Constructor with known variable
//
QEGeneralEdit::QEGeneralEdit (const QString & variableNameIn,
                              QWidget * parent):QEFrame (parent)
{
   this->commonSetup ();
   this->setVariableName (variableNameIn, 0);
}

//---------------------------------------------------------------------------------
//
QSize QEGeneralEdit::sizeHint () const
{
   return QSize (430, 80);
}

//-----------------------------------------------------------------------------
// Setup common to all constructors
//
void QEGeneralEdit::commonSetup ()
{
   // Configure the panel.
   //
   this->setFrameShape (QFrame::Panel);
   this->setFrameShadow (QFrame::Plain);

   // QEFreame sets this to false (as it's not an EPICS aware widget).
   // But the QEGeneralEdit is EPICS aware, so set default to true.
   //
   this->setVariableAsToolTip (true);

   // Set up data
   // This control uses a single data source
   //true
   this->setNumVariables (1);

   // Set up default properties
   //
   this->setAllowDrop (true);
   this->setDisplayAlarmState (false);

   this->createInternalWidgets ();

   // Use default context menu.
   //
   this->setupContextMenu ();

   // Set up a connection to recieve variable name property changes
   // The variable name property manager class only delivers an updated
   // variable name after the user has stopped typing.
   //
   QObject::connect
       (&this->variableNamePropertyManager,
               SIGNAL (newVariableNameProperty (QString, QString, unsigned int)),
        this,  SLOT   (useNewVariableNameProperty (QString, QString, unsigned int)));
}

//------------------------------------------------------------------------------
//
void QEGeneralEdit::createInternalWidgets ()
{
   this->verticalLayout = new QVBoxLayout (this);
   this->verticalLayout->setSpacing (4);

   this->qelabel = new QELabel (this);
   this->qelabel->setFrameShape (QFrame::Panel);
   this->qelabel->setFrameShadow (QFrame::Plain);
   this->qelabel->setMinimumSize (QSize (0, 19));
   this->qelabel->setMaximumSize (QSize (16777215, 19));
   this->verticalLayout->addWidget (qelabel);

   this->numericEditPanel = new QGroupBox (this);
   this->numericEditPanel->setMinimumSize (QSize (0, 64));
   this->numericEditPanel->setMaximumSize (QSize (16777215, 64));
   this->qenumericedit = new QENumericEdit (this->numericEditPanel);
   this->qenumericedit->setGeometry (QRect (14, 24, 144, 23));
   this->verticalLayout->addWidget (this->numericEditPanel);

   this->radioGroupPanel = new QERadioGroup (this);
   this->radioGroupPanel->setMinimumSize (QSize (412, 192));
   this->radioGroupPanel->setColumns (3);
   this->verticalLayout->addWidget (this->radioGroupPanel);

   this->stringEditPanel = new QGroupBox (this);
   this->stringEditPanel->setMinimumSize (QSize (0, 64));
   this->stringEditPanel->setMaximumSize (QSize (16777215, 64));
   this->qelineedit = new QELineEdit (this->stringEditPanel);
   this->qelineedit->setGeometry (QRect (16, 26, 381, 23));
   this->verticalLayout->addWidget (this->stringEditPanel);
}


//------------------------------------------------------------------------------
// Implementation of QEWidget's virtual funtion to create the specific type of
// QCaObject required. A QCaObject that streams integers is required.
//
qcaobject::QCaObject*  QEGeneralEdit::createQcaItem (unsigned int variableIndex)
{
   qcaobject::QCaObject * result = NULL;

   if (variableIndex != 0) {
      DEBUG << "unexpected variableIndex" << variableIndex;
      return NULL;
   }

   result = new qcaobject::QCaObject (getSubstitutedVariableName (0), this);
   return result;
}


//------------------------------------------------------------------------------
// Start updating.
// Implementation of VariableNameManager's virtual funtion to establish a
// connection to a PV as the variable name has changed.
// This function may also be used to initiate updates when loaded as a plugin.
//
void QEGeneralEdit::establishConnection (unsigned int variableIndex)
{
   if (variableIndex != 0) {
      DEBUG << "unexpected variableIndex" << variableIndex;
      return;
   }
   // Create a connection.
   // If successfull, the QCaObject object that will supply data update signals will be returned
   // Note createConnection creates the connection and returns reference to existing QCaObject.
   //
   qcaobject::QCaObject* qca = createConnection (variableIndex);

   // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots.
   //
   if (qca) {
      QObject::connect (qca,  SIGNAL (connectionChanged (QCaConnectionInfo &)),
                        this, SLOT  (connectionChanged (QCaConnectionInfo &)));

      QObject::connect (qca,SIGNAL (dataChanged (const QVariant&, QCaAlarmInfo&, QCaDateTime&)),
                        this, SLOT (dataChanged (const QVariant&, QCaAlarmInfo&, QCaDateTime&)));
   }
}

//------------------------------------------------------------------------------
// Act on a connection change.
// Change how the s looks and change the tool tip
// This is the slot used to recieve connection updates from a QCaObject based class.
//
void QEGeneralEdit::connectionChanged (QCaConnectionInfo& connectionInfo)
{
   // Note the connected state
   //
   bool isConnected = connectionInfo.isChannelConnected ();

   // Display the connected state
   //
   updateToolTipConnection (isConnected);
   updateConnectionStyle (isConnected);

   this->isFirstUpdate = true;
}

//-----------------------------------------------------------------------------
//
void QEGeneralEdit::dataChanged (const QVariant& value, QCaAlarmInfo& alarmInfo, QCaDateTime&)

{
   qcaobject::QCaObject* qca = this->getQcaItem (0);

   if (qca && this->isFirstUpdate) {
      QGroupBox* useThisPanel = NULL;
      QEWidget* useThisWidget = NULL;
      QString pvName;
      int numElements = 0;



      pvName = this->getSubstitutedVariableName (0).trimmed ();
      this->qelabel->setVariableNameAndSubstitutions (pvName, "", 0);

      this->numericEditPanel->setVisible (false);
      this->radioGroupPanel->setVisible (false);
      this->stringEditPanel->setVisible (false);

      // Use data type to figure out which type of editting widget is most
      // appropriate.
      //
      QVariant::Type type = value.type ();
      switch (type) {
         case QVariant::String:
            useThisPanel = this->stringEditPanel;
            useThisWidget = this->qelineedit;
            break;

         case QVariant::Int:
         case QVariant::UInt:
         case QVariant::LongLong:
         case QVariant::ULongLong:
            numElements = qca->getEnumerations().count();
            if (numElements > 0) {
               int numRows;

               // represents an enumeration.
               //
               useThisPanel = this->radioGroupPanel;
               useThisWidget = this->radioGroupPanel;

               numRows = (numElements + 2) / 3;
               useThisPanel->setMinimumHeight ((numRows + 2) *  QEScaling::scale (20));

            } else {
               // basic integer
               //
               useThisPanel = this->numericEditPanel;
               useThisWidget = this->qenumericedit;

            }
            break;
\
         case QVariant::Double:
            useThisPanel = this->numericEditPanel;
            useThisWidget = this->qenumericedit;
            break;

         default:
            DEBUG << " Unexpcted type:" << type;
            return; // do nothing
      }

      if (useThisPanel && useThisWidget) {
         useThisPanel->setVisible (true);
         useThisPanel->setTitle (" " + pvName + " ");

         useThisWidget->setVariableNameAndSubstitutions (pvName, "", 0);

         int newHeight =
               this->qelabel->minimumHeight () +
               useThisPanel->minimumHeight () +
               QEScaling::scale (32);

         this->setMinimumHeight (newHeight);
         this->setMaximumHeight (newHeight);
      }

      this->isFirstUpdate = false;
   }

   // Invoke common alarm handling processing.
   //
   this->processAlarmInfo (alarmInfo);
}


//==============================================================================
// Properties
// Update variable name etc.
//
void QEGeneralEdit::useNewVariableNameProperty (QString variableNameIn,
                                                QString variableNameSubstitutionsIn,
                                                unsigned int variableIndex)
{
   this->setVariableNameAndSubstitutions (variableNameIn,
                                          variableNameSubstitutionsIn,
                                          variableIndex);
}


//==============================================================================
// Drag drop
//
void QEGeneralEdit::setDrop (QVariant drop)
{
   this->setVariableName (drop.toString (), 0);
   this->establishConnection (0);
}

//------------------------------------------------------------------------------
//
QVariant QEGeneralEdit::getDrop ()
{
   QVariant result;

   if (this->isDraggingVariable ()) {
      result = QVariant (this->copyVariable ());
   } else {
      result = this->copyData ();
   }
   return result;
}

//==============================================================================
// Copy / Paste
//
QString QEGeneralEdit::copyVariable ()
{
   return getSubstitutedVariableName (0);
}

//------------------------------------------------------------------------------
//
QVariant QEGeneralEdit::copyData ()
{
   return QVariant ();
}

//------------------------------------------------------------------------------
//
void QEGeneralEdit::paste (QVariant v)
{
   if (this->getAllowDrop ()) {
      this->setDrop (v);
   }
}

// end
