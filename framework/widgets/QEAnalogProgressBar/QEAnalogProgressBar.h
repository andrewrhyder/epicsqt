/*  QEAnalogProgressBar.h
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

#ifndef QEANALOGPROGRESSBAR_H
#define QEANALOGPROGRESSBAR_H

#include <QString>
#include <QEAnalogIndicator.h>
#include <QCaWidget.h>
#include <QCaFloating.h>
#include <QCaFloatingFormatting.h>
#include <QCaVariableNamePropertyManager.h>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QEAnalogProgressBar : public QEAnalogIndicator, public QCaWidget {
    Q_OBJECT

#include <singleVariableProperties.inc>
#include <standardProperties.inc>

    // Display properties
    Q_PROPERTY( bool useDbDisplayLimits READ getUseDbDisplayLimits WRITE setUseDbDisplayLimits )

    enum AlarmSeverityDisplayModes { none, foreground, background };
    Q_ENUMS (AlarmSeverityDisplayModes)

    Q_PROPERTY( AlarmSeverityDisplayModes alarmSeverityDisplayMode READ getAlarmSeverityDisplayMode WRITE setAlarmSeverityDisplayMode )

public:
    QEAnalogProgressBar( QWidget *parent = 0 );
    QEAnalogProgressBar( const QString &variableName, QWidget *parent = 0 );
    virtual ~QEAnalogProgressBar(){}

    // Variable Name and substitution
    //
    void setVariableNameAndSubstitutions( QString variableNameIn,
                                          QString variableNameSubstitutionsIn,
                                          unsigned int variableIndex );

    // Property convenience functions

    // useDbDisplayLimits, e.g. as specified by LOPR and HOPR fields for ai, ao, longin
    // and longout record types, to call setAnalogMinimum and setAnalogMaximum.
    //
    void setUseDbDisplayLimits( bool useDbDisplayLimitsIn );
    bool getUseDbDisplayLimits();

    void setAlarmSeverityDisplayMode( AlarmSeverityDisplayModes value );
    AlarmSeverityDisplayModes getAlarmSeverityDisplayMode ();

public slots:
    void requestEnabled( const bool& state ){ setApplicationEnabled( state ); } //!! with the MOC mind if this is moved into standardProperties.inc


protected:
    QString getSprintfFormat ();
    BandList getBandList ();

    QCaFloatingFormatting floatingFormatting;

    void establishConnection( unsigned int variableIndex );

private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip( const QString& tip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;
    bool useDbPrecison;
    bool useDbDisplayLimits;
    AlarmSeverityDisplayModes alarmSeverityDisplayMode;
    bool isFirstUpdate;
    QColor savedForegroundColour;
    QColor savedBackgroundColour;


private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );

    void setProgressBarValue( const double& value, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );

    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
    {
        setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
    }
//#include <variablePropertiesSlots.inc>  // MOC doesn't seem to like included private slots.

signals:
    void dbValueChanged( const double& out );

protected:
    // Drag and Drop
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();

    // Copy paste
    QString copyVariable();
    QVariant copyData();

};

#endif /// QEANALOGPROGRESSBAR_H
