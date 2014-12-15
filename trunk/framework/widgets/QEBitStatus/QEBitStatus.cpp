/*  QEBitStatus.cpp
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
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
 *  Copyright (c) 2011, 2013 Australian Synchrotron
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

/*
  This class is a CA aware Bit Status widget based on the BitStatus widget.
  It is tighly integrated with the base class QEWidget.
  Refer to QEWidget.cpp for details
 */

#include <alarm.h>

#include <QDebug>
#include <QEBitStatus.h>
#include <QCaObject.h>

#define DEBUG  qDebug () << "QEBitStatus" << __FUNCTION__ << __LINE__

#define PV_VARIABLE_INDEX      0

/* ----------------------------------------------------------------------------
    Constructor with no initialisation
*/
QEBitStatus::QEBitStatus (QWidget * parent):QBitStatus (parent), QEWidget ( this )
{
   setup ();
}


/* ----------------------------------------------------------------------------
    Constructor with known variable
*/
QEBitStatus::QEBitStatus (const QString & variableNameIn,
                            QWidget * parent):QBitStatus (parent), QEWidget ( this )
{
   setup ();
   setVariableName (variableNameIn, 0);
   activate();
}


/* ----------------------------------------------------------------------------
    Setup common to all constructors
*/
void QEBitStatus::setup ()
{
   QCaAlarmInfo invalid (NO_ALARM, INVALID_ALARM);

   // Set up data
   //
   // This control used a single data source
   //
   setNumVariables (1);

   // Set up default properties
   //
   setAllowDrop (false);

   // Set the initial state
   // Widget is inactive until connected.
   //
   this->setIsActive (false);

   setInvalidColour (this->getColor( invalid, 128));

   // Use default context menu.
   //
   setupContextMenu();

   // Set up a connection to recieve variable name property changes
   // The variable name property manager class only delivers an updated
   // variable name after the user has stopped typing.
   //
   QObject::connect (&variableNamePropertyManager,
                     SIGNAL (newVariableNameProperty (QString, QString, unsigned int)),
                     this, SLOT (useNewVariableNameProperty (QString, QString, unsigned int)));
}


/* ----------------------------------------------------------------------------
    Implementation of QEWidget's virtual funtion to create the specific type
    of QCaObject required. For a Bit Status widget a QCaObject that streams
    integers is required.
*/
qcaobject::QCaObject* QEBitStatus::createQcaItem (unsigned int variableIndex)
{
   qcaobject::QCaObject* result;

   if (variableIndex != PV_VARIABLE_INDEX) {
      DEBUG << "unexpected variableIndex" << variableIndex;
      return NULL;
   }

   result = new QEInteger (getSubstitutedVariableName (variableIndex),
                           this, &integerFormatting, variableIndex);

   return result;
}


/* ----------------------------------------------------------------------------
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a
    connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QEBitStatus::establishConnection (unsigned int variableIndex)
{
   if (variableIndex != PV_VARIABLE_INDEX) {
      DEBUG << "unexpected variableIndex" << variableIndex;
      return;
   }

   // Create a connection.
   // If successfull, the QCaObject object that will supply data update signals will be returned
   // Note createConnection creates the connection and returns reference to existing QCaObject.
   //
   qcaobject::QCaObject * qca = createConnection (variableIndex);

   // If a QCaObject object is now available to supply data update signals,
   // connect it to the appropriate slots.
   //
   if (qca) {
      QObject::connect (qca,  SIGNAL (integerChanged  (const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int &)),
                        this, SLOT (setBitStatusValue (const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int &)));

      QObject::connect (qca,  SIGNAL (connectionChanged (QCaConnectionInfo&, const unsigned int &)),
                        this, SLOT   (connectionChanged (QCaConnectionInfo&, const unsigned int &)));
   }
}


/* ----------------------------------------------------------------------------
    Act on a connection change.
    Change how the progress bar looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QEBitStatus::connectionChanged (QCaConnectionInfo& connectionInfo, const unsigned int& variableIndex)
{
    // Note the connected state
    bool isConnected = connectionInfo.isChannelConnected();

    // Display the connected state
    updateToolTipConnection (isConnected, variableIndex);
    updateConnectionStyle (isConnected);

    this->setIsActive (isConnected);
}


/* ----------------------------------------------------------------------------
    Update the progress bar value
    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QEBitStatus::setBitStatusValue (const long &value,
                                     QCaAlarmInfo & alarmInfo,
                                     QCaDateTime &, const unsigned int &variableIndex)
{
   // Update the Bit Status
   //
   this->setValue (int (value));

   // Set validity status.
   //
   this->setIsValid (alarmInfo.getSeverity () != INVALID_ALARM);

   // Invoke common alarm handling processing.
   // Although this sets widget style, we invoke for tool tip processing only.
   //
   this->processAlarmInfo (alarmInfo, variableIndex);

   // Signal a database value change to any Link widgets
   //
   emit dbValueChanged (value);
}


/* ----------------------------------------------------------------------------
    This is the slot used to recieve new PV information.
 */
void QEBitStatus::useNewVariableNameProperty( QString variableNameIn,
                                              QString variableNameSubstitutionsIn,
                                              unsigned int variableIndex )
{
   this->setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
}


//==============================================================================
// Copy (no paste)
//
QString QEBitStatus::copyVariable()
{
   return getSubstitutedVariableName (0);
}

QVariant QEBitStatus::copyData()
{
   return QVariant( this->getValue () );
}

// end
