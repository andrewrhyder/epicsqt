/*  QEAnalogProgressBar.cpp
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
 *  Copyright (c) 2011
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

/*
  This class is a CA aware analog progress bar widget based on the analog progress bar widget.
  It is tighly integrated with the base class QEWidget. Refer to QEWidget.cpp for details
 */

#include <alarm.h>

#include <QEAnalogProgressBar.h>
#include <QCaObject.h>
#include <QEStringFormatting.h>


#define ALARM_SATURATION      128
#define NO_ALARM_SATURATION    32


/* ----------------------------------------------------------------------------
    Constructor with no initialisation
*/
QEAnalogProgressBar::QEAnalogProgressBar( QWidget *parent ) :
   QEAnalogIndicator( parent ), QEWidget( this ), QEStringFormattingMethods ()
{
    setup();
}


/* ----------------------------------------------------------------------------
    Constructor with known variable
*/
QEAnalogProgressBar::QEAnalogProgressBar( const QString &variableNameIn,
                                          QWidget *parent ) :
   QEAnalogIndicator( parent ), QEWidget( this ), QEStringFormattingMethods ()
{

    setup();
    setVariableName( variableNameIn, 0 );
}


/* ----------------------------------------------------------------------------
    Setup common to all constructors
*/
void QEAnalogProgressBar::setup() {

    // Set up data
    // This control used a single data source
    setNumVariables( 1 );

    // Set up default properties
    useDbDisplayLimits = false;
    this->alarmSeverityDisplayMode = background;
    setAllowDrop( false );

    // Set the initial state
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    // Use progress bar signals
    // --Currently none--

    // Use default context menu.
    //
    setupContextMenu (this);

    // Set up a connection to recieve variable name property changes
    // The variable name property manager class only delivers an updated
    // variable name after the user has stopped typing.
    //
    QObject::connect( &variableNamePropertyManager,
                      SIGNAL( newVariableNameProperty( QString, QString, unsigned int ) ),
                      this, SLOT( useNewVariableNameProperty( QString, QString, unsigned int) ) );

}

/* ----------------------------------------------------------------------------
    Implementation of QEWidget's virtual funtion to create the specific type of QCaObject required.
    For a progress bar a QCaObject that streams integers is required.
*/
qcaobject::QCaObject* QEAnalogProgressBar::createQcaItem( unsigned int variableIndex ) {

    qcaobject::QCaObject* result;

    if (variableIndex == 0) {
        result = new QEFloating( getSubstitutedVariableName( variableIndex ), this, &floatingFormatting, variableIndex );
    } else {
        result = NULL;  // Unexpected
    }

    return result;
}


/* ----------------------------------------------------------------------------
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a
    connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QEAnalogProgressBar::establishConnection( unsigned int variableIndex )
{
    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    // Note createConnection creates the connection and returns reference to existing QCaObject.
    //
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots.
    //
    if ((  qca ) && (variableIndex == 0)) {
        QObject::connect( qca,  SIGNAL( floatingChanged(   const double&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setProgressBarValue( const double&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );

        QObject::connect( qca,  SIGNAL( floatingArrayChanged( const QVector<double>&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setProgressBarValues(   const QVector<double>&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );

        QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged(   QCaConnectionInfo& ) ) );

        QObject::connect( this, SIGNAL( requestResend() ),
                          qca,  SLOT( resendLastData () ) );
    }
}


/* ----------------------------------------------------------------------------
    Act on a connection change.
    Change how the progress bar looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QEAnalogProgressBar::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    // If connected, enable the widget if the QE enabled property is true
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );

        setDataDisabled( false );

        isFirstUpdate = true;
    }

    // If disconnected always disable the widget.
    else
    {
        isConnected = false;
        updateToolTipConnection( isConnected );

        setDataDisabled( true );
    }
}

/* ----------------------------------------------------------------------------
    Provide image, e.g. with EGU if appropriate
 */
QString QEAnalogProgressBar::getTextImage ()
{
   return this->theImage;
}


/* ----------------------------------------------------------------------------
    Create a single thresholds and colour band item.
 */
QEAnalogIndicator::Band QEAnalogProgressBar::createBand (const double lower,
                                                         const double upper,
                                                         unsigned short severity)
{
   Band result;
   QCaAlarmInfo alarmInfo (0, severity);
   int saturation;

   result.lower = lower;
   result.upper = upper;

   saturation = (severity == NO_ALARM) ? NO_ALARM_SATURATION : ALARM_SATURATION;
   result.colour = this->getColor (alarmInfo, saturation);

   return result;
}

/* ----------------------------------------------------------------------------
    Create a list of alarm thresholds and colours.
 */
QEAnalogIndicator::BandList QEAnalogProgressBar::getBandList ()
{
    BandList result;
    qcaobject::QCaObject* qca;

    result.clear();

    // Associated qca object - avoid the segmentation fault.
    //
    qca = getQcaItem( 0 );
    if (qca) {
        const double dispLower = this->getMinimum ();
        const double dispUpper = this->getMaximum ();
        const double alarmLower = qca->getAlarmLimitLower ();
        const double alarmUpper = qca->getAlarmLimitUpper ();
        const double warnLower =  qca->getWarningLimitLower ();
        const double warnUpper = qca->getWarningLimitUpper ();
        bool alarmIsDefined;
        bool warnIsDefined;

        // Unfortunately, the Channel Access protocol only provides the
        // alarm/warning values, and not the associated severities.
        // We assume major for alarms, and minor for warnings.
        //
        alarmIsDefined = ((alarmLower != 0.0) || (alarmUpper != 0.0) );
        warnIsDefined  = ((warnLower  != 0.0) || (warnUpper  != 0.0) );

        if (alarmIsDefined) {
            if (warnIsDefined) {
                // All alarms defined.
                //
                result << createBand (dispLower,  alarmLower, MAJOR_ALARM);
                result << createBand (alarmLower, warnLower,  MINOR_ALARM);
                result << createBand (warnLower,  warnUpper,  NO_ALARM);
                result << createBand (warnUpper,  alarmUpper, MINOR_ALARM);
                result << createBand (alarmUpper, dispUpper,  MAJOR_ALARM);
            } else {
                // Major alarms defined.
                //
                result << createBand (dispLower,  alarmLower, MAJOR_ALARM);
                result << createBand (alarmLower, alarmUpper, NO_ALARM);
                result << createBand (alarmUpper, dispUpper,  MAJOR_ALARM);
            }
        } else {
           if (warnIsDefined) {
                // Minor alarms defined.
                //
                result << createBand (dispLower,  warnLower,  MINOR_ALARM);
                result << createBand (warnLower,  warnUpper,  NO_ALARM);
                result << createBand (warnUpper,  dispUpper,  MINOR_ALARM);
            } else {
                // No alarms defined at all.
                //
                result << createBand (dispLower,  dispUpper,  NO_ALARM);
            }
        }
    }

    return result;
}


/* ----------------------------------------------------------------------------
    Update the progress bar value
    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QEAnalogProgressBar::setProgressBarValue( const double& value,
                                               QCaAlarmInfo& alarmInfo,
                                               QCaDateTime&, const unsigned int& )
{
    qcaobject::QCaObject* qca;
    int saturation;

    // If not enabled then do nothing.
    // NOTE: the regular isEnabled is hidden by function in the standard properties
    //
    if (!QWidget::isEnabled ()) return;

    // Associated qca object - avoid the segmentation fault.
    //
    qca = getQcaItem( 0 );
    if (!qca) return;

    if (isFirstUpdate) {

       // Set up variable details used by some formatting options
       //
       this->stringFormatting.setDbEgu( qca->getEgu() );
       this->stringFormatting.setDbEnumerations( qca->getEnumerations() );
       this->stringFormatting.setDbPrecision( qca->getPrecision() );

       // Update display limits if requested and defined.
       //
       if (this->getUseDbDisplayLimits ()) {

          double lower;
          double upper;

          lower = qca->getDisplayLimitLower();
          upper = qca->getDisplayLimitUpper();

          // Check that sensible limits have been defined and not just left
          // at the default (i.e. zero) values by a lazy database creator.
          // Otherwise, leave as design time limits.
          //
          if ((lower != 0.0) || (upper != 0.0)) {
             this->setRange (lower, upper);
          }
       }
    }

    // Form and save the image - must do before call to setValue.
    //
    this->theImage = this->stringFormatting.formatString( value );

    // Update the progress bar
    //
    this->setValue( value );

    // Choose the alarm state to display.
    // If not displaying the alarm state, use a default 'no alarm' structure. This is
    // required so the any display of an alarm state is reverted if the displayAlarmState
    // property changes while displaying an alarm.
    QCaAlarmInfo ai;
    if( getDisplayAlarmState() )
    {
        ai = alarmInfo;
    }

    switch (this->getAlarmSeverityDisplayMode()) {
       case foreground:
          // Use low saturation when no alarm, otherwise set a medium saturation level.
          //
          saturation = ALARM_SATURATION;
          setForegroundColour( getColor( ai, saturation ) );
          break;

       case background:
          // Use low saturation when no alarm, otherwise set a medium saturation level.
          //
          saturation = (ai.getSeverity() == NO_ALARM) ? NO_ALARM_SATURATION : ALARM_SATURATION;
          setBackgroundColour( getColor( ai, saturation ) );
          break;
    }

    // Invoke common alarm handling processing.
    // Although this sets widget style, we invoke for tool tip processing only.
    //
    this->processAlarmInfo (alarmInfo);

    // Signal a database value change to any Link widgets
    emit dbValueChanged( value );

    // This update is over, clear first update flag.
    isFirstUpdate = false;
}


/* ----------------------------------------------------------------------------
    Extract first element (0 index) and use this value.
 */
void QEAnalogProgressBar::setProgressBarValues (const QVector<double>& values,
                                                QCaAlarmInfo& alarmInfo,
                                                QCaDateTime& dateTime,
                                                const unsigned int& variableIndex)
{
   int slot = 0;
   this->setProgressBarValue (values.value (slot), alarmInfo, dateTime, variableIndex);
}

/* ----------------------------------------------------------------------------
    Update variable name etc.
 */
void QEAnalogProgressBar::useNewVariableNameProperty( QString variableNameIn,
                                                      QString variableNameSubstitutionsIn,
                                                      unsigned int variableIndex )
{
    setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
}


//==============================================================================
// Drag drop
void QEAnalogProgressBar::setDrop( QVariant drop )
{
    setVariableName( drop.toString(), 0 );
    establishConnection( 0 );
}

QVariant QEAnalogProgressBar::getDrop()
{
    return QVariant( getSubstitutedVariableName(0) );
}

//==============================================================================
// Copy (no paste)
//
QString QEAnalogProgressBar::copyVariable()
{
   return getSubstitutedVariableName (0);
}

QVariant QEAnalogProgressBar::copyData()
{
   return QVariant( this->getValue () );
}

//------------------------------------------------------------------------------
// useDbDisplayLimits
void QEAnalogProgressBar::setUseDbDisplayLimits( bool useDbDisplayLimitsIn )
{
    useDbDisplayLimits = useDbDisplayLimitsIn;
}

//------------------------------------------------------------------------------
bool QEAnalogProgressBar::getUseDbDisplayLimits()
{
    return useDbDisplayLimits;
}


//------------------------------------------------------------------------------
void QEAnalogProgressBar::setAlarmSeverityDisplayMode( AlarmSeverityDisplayModes value )
{
    if (this->alarmSeverityDisplayMode != value) {

        // case on old value and restore colour
        //
        switch (this->alarmSeverityDisplayMode) {
            case foreground:
                this->setForegroundColour (this->savedForegroundColour);
                break;

            case background:
                this->setBackgroundColour (this->savedBackgroundColour);
                break;
        }

        // Do actual property update.
        //
        this->alarmSeverityDisplayMode = value;

        // case on new value and restore colour
        //
        switch (this->alarmSeverityDisplayMode) {
            case foreground:
                this->savedForegroundColour = this->getForegroundColour();
                break;

            case background:
                this->savedBackgroundColour = this->getBackgroundColour();
                break;
        }
    }
}

//------------------------------------------------------------------------------
QEAnalogProgressBar::AlarmSeverityDisplayModes QEAnalogProgressBar::getAlarmSeverityDisplayMode ()
{
    return this->alarmSeverityDisplayMode;
}

// end