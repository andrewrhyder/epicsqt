/*  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
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
  This class is a CA aware Bit Status widget based on the BitStatus widget.
  It is tighly integrated with the base class QCaWidget.
  Refer to QCaWidget.cpp for details
 */

#include <QCaBitStatus.h>
#include <QCaObject.h>


/*! ----------------------------------------------------------------------------
    Constructor with no initialisation
*/
QCaBitStatus::QCaBitStatus (QWidget * parent):QBitStatus (parent), QCaWidget ( this )
{
   setup ();
}


/*! ----------------------------------------------------------------------------
    Constructor with known variable
*/
QCaBitStatus::QCaBitStatus (const QString & variableNameIn,
                            QWidget * parent):QBitStatus (parent), QCaWidget ( this )
{
   setup ();
   setVariableName (variableNameIn, 0);
}


/*! ----------------------------------------------------------------------------
    Setup common to all constructors
*/
void QCaBitStatus::setup ()
{
   // Set up data
   //
   // This control used a single data source
   //
   setNumVariables (1);

   // Set up default properties
   //
   localEnabled = true;
   visible = true;
   setAllowDrop( false );

   // Set the initial state
   //
   lastSeverity = QCaAlarmInfo::getInvalidSeverity ();
   isConnected = false;

   setInvalidColour (QColor ( 255, 255, 255));
   setIsValid (isConnected);

   QWidget::setEnabled (false); // Reflects initial disconnected state

   // Use progress bar signals
   // --Currently none--

   // Set up a connection to recieve variable name property changes
   // The variable name property manager class only delivers an updated
   // variable name after the user has stopped typing.
   //
   QObject::connect (&variableNamePropertyManager,
                     SIGNAL (newVariableNameProperty (QString, QString, unsigned int)),
                     this, SLOT (useNewVariableNameProperty (QString, QString, unsigned int)));
}


/*! ----------------------------------------------------------------------------
    Slot to recieve variable name and macro substitutions property changes.
*/
void QCaBitStatus::useNewVariableNameProperty (QString variableNameIn,
                                               QString variableNameSubstitutionsIn,
                                               unsigned int variableIndex)
{
   setVariableNameAndSubstitutions (variableNameIn,
                                    variableNameSubstitutionsIn,
                                    variableIndex);
}


/*! ----------------------------------------------------------------------------
    Implementation of QCaWidget's virtual funtion to create the specific type
    of QCaObject required. For a Bit Status widget a QCaObject that streams
    integers is required.
*/
qcaobject::QCaObject *
    QCaBitStatus::createQcaItem (unsigned int variableIndex)
{
   qcaobject::QCaObject * result;

   // assert variableIndex == 0 ??

   result = new QCaInteger (getSubstitutedVariableName (variableIndex),
                            this, &integerFormatting, variableIndex);
   return result;
}


/*! ----------------------------------------------------------------------------
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a
    connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QCaBitStatus::establishConnection (unsigned int variableIndex)
{
   // Create a connection.
   // If successfull, the QCaObject object that will supply data update signals will be returned
   // Note createConnection creates the connection and returns reference to existing QCaObject.
   //
   qcaobject::QCaObject * qca = createConnection (variableIndex);

   // If a QCaObject object is now available to supply data update signals,
   // connect it to the appropriate slots.
   //
   if ((qca) && (variableIndex == 0)) {
      QObject::connect (qca,  SIGNAL (integerChanged  (const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int &)),
                        this, SLOT (setBitStatusValue (const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int &)));
      QObject::connect (qca,  SIGNAL (connectionChanged (QCaConnectionInfo&)),
                        this, SLOT (connectionChanged   (QCaConnectionInfo&)));
   }
}


/*! ----------------------------------------------------------------------------
    Update the tool tip as requested by QCaToolTip.
*/
void QCaBitStatus::updateToolTip (const QString & tip)
{
   setToolTip (tip);
}


/*! ----------------------------------------------------------------------------
    Act on a connection change.
    Change how the progress bar looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QCaBitStatus::connectionChanged (QCaConnectionInfo & connectionInfo)
{
   // If connected, enable the widget if the QCa enabled property is true
   if (connectionInfo.isChannelConnected ()) {
      isConnected = true;
      updateToolTipConnection (isConnected);

      if (localEnabled) {
         QWidget::setEnabled (true);
      }
   }
   // If disconnected always disable the widget.
   else {
      isConnected = false;
      updateToolTipConnection (isConnected);

      QWidget::setEnabled (false);
   }
   setIsValid (isConnected);
}



/*! ----------------------------------------------------------------------------
    Update the progress bar value
    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QCaBitStatus::setBitStatusValue (const long &value,
                                      QCaAlarmInfo & alarmInfo,
                                      QCaDateTime &, const unsigned int &)
{
   // Update the Bit Status
   //
   setValue (value);

   // If in alarm, display as an alarm
   //
   if (alarmInfo.getSeverity () != lastSeverity) {
      updateToolTipAlarm (alarmInfo.severityName ());
      setStyleSheet (alarmInfo.style ());
      lastSeverity = alarmInfo.getSeverity ();
   }

   // Signal a database value change to any Link widgets
   //
   emit dbValueChanged (value);
}


/*! ----------------------------------------------------------------------------
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaBitStatus::isEnabled () const
{
   /// Return what the state of widget would be if connected.
   return localEnabled;
}

/*! ----------------------------------------------------------------------------
   Override the default widget setEnabled to allow alarm states to override
   current enabled state.
 */
void QCaBitStatus::setEnabled (bool state)
{
   /// Note the new 'enabled' state
   localEnabled = state;

   /// Set the enabled state of the widget only if connected
   if (isConnected) {
      QWidget::setEnabled (localEnabled);
   }
}


/*! ----------------------------------------------------------------------------
   Slot similar to default widget setEnabled, but will use our own setEnabled
   which will allow alarm states to override current enabled state.
 */
void QCaBitStatus::requestEnabled (const bool & state)
{
   setEnabled (state);
}


//==============================================================================
// Drag drop
void QCaBitStatus::setDropText( QString text )
{
    setVariableName( text, 0 );
    establishConnection( 0 );
}

QString QCaBitStatus::getDropText()
{
    return getSubstitutedVariableName(0);
}

//==============================================================================
// Property convenience functions
//
void QCaBitStatus::setVariableNameProperty (QString variableName)
{
   variableNamePropertyManager.setVariableNameProperty (variableName);
}

//------------------------------------------------------------------------------
QString QCaBitStatus::getVariableNameProperty ()
{
   return variableNamePropertyManager.getVariableNameProperty ();
}

//------------------------------------------------------------------------------
void QCaBitStatus::setVariableNameSubstitutionsProperty (QString variableNameSubstitutions)
{
   variableNamePropertyManager.setSubstitutionsProperty (variableNameSubstitutions);
}

//------------------------------------------------------------------------------
QString QCaBitStatus::getVariableNameSubstitutionsProperty ()
{
   return variableNamePropertyManager.getSubstitutionsProperty ();
}


//------------------------------------------------------------------------------
// Access functions for variableName and variableNameSubstitutions
// variable substitutions Example: SECTOR=01 will result in any occurance
// of $(SECTOR) in variable name being replaced with 01.
//
void QCaBitStatus::setVariableNameAndSubstitutions (QString variableNameIn,
                                                    QString variableNameSubstitutionsIn,
                                                    unsigned int variableIndex)
{
   setVariableNameSubstitutions (variableNameSubstitutionsIn);

   /// TODO a WTF comment
   setVariableName (variableNameIn, variableIndex);
   establishConnection (variableIndex);

   setVariableName (variableNameIn, variableIndex + 1);
   establishConnection (variableIndex + 1);
}

//------------------------------------------------------------------------------
// variable as tool tip
void QCaBitStatus::setVariableAsToolTip (bool variableAsToolTipIn)
{
   variableAsToolTip = variableAsToolTipIn;
}

//------------------------------------------------------------------------------
bool QCaBitStatus::getVariableAsToolTip ()
{
   return variableAsToolTip;
}

//------------------------------------------------------------------------------
// Display properties
//
// visible (widget is visible outside 'Designer')
//
void QCaBitStatus::setRunVisible (bool visibleIn)
{
   // Update the property
   visible = visibleIn;

   // If a container profile has been defined, then this widget is being
   // used in a real GUI and should be visible or not according to the
   // visible property (while in Designer it can always be displayed).
   //
   ContainerProfile profile;
   if (profile.isProfileDefined ()) {
      setVisible (visible);
   }
}

//------------------------------------------------------------------------------
//
bool QCaBitStatus::getRunVisible ()
{
   return visible;
}

//------------------------------------------------------------------------------
// allow drop (Enable/disable as a drop site for drag and drop)
void QCaBitStatus::setAllowDrop( bool allowDropIn )
{
    allowDrop = allowDropIn;
    setAcceptDrops( allowDrop );
}

bool QCaBitStatus::getAllowDrop()
{
    return allowDrop;
}
// end
