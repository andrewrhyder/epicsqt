/*
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

#ifndef QCAANALOGPROGRESSBAR_H
#define QCAANALOGPROGRESSBAR_H

#include <QString>
#include <QAnalogProgressBar.h>
#include <QCaWidget.h>
#include <QCaFloating.h>
#include <QCaFloatingFormatting.h>
#include <QCaVariableNamePropertyManager.h>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QCaAnalogProgressBar : public QAnalogProgressBar, public QCaWidget {
    Q_OBJECT

    /// Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    /// A property name ending with 'Name' results in some sort of string a variable being displayed,
    /// but will only accept alphanumeric and won't generate callbacks on change.
    //
    Q_PROPERTY( QString variable READ getVariableNameProperty WRITE setVariableNameProperty )
    Q_PROPERTY( QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty )
    Q_PROPERTY( bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip )
    Q_PROPERTY( bool enabled READ isEnabled WRITE setEnabled )
    Q_PROPERTY(bool allowDrop READ getAllowDrop WRITE setAllowDrop)

    /// Display properties
    //
    Q_PROPERTY( bool useDbDisplayLimits READ getUseDbDisplayLimits WRITE setUseDbDisplayLimits )
    Q_PROPERTY( bool visible READ getRunVisible WRITE setRunVisible )


public:
    QCaAnalogProgressBar( QWidget *parent = 0 );
    QCaAnalogProgressBar( const QString &variableName, QWidget *parent = 0 );
    virtual ~QCaAnalogProgressBar(){}

    bool isEnabled() const;
    void setEnabled( bool state );

    // Property convenience functions
    //
    void    setVariableNameProperty( QString variableName );
    QString getVariableNameProperty();

    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions );
    QString getVariableNameSubstitutionsProperty();

    // Variable Name and substitution
    //
    void setVariableNameAndSubstitutions( QString variableNameIn,
                                          QString variableNameSubstitutionsIn,
                                          unsigned int variableIndex );

    // variable as tool tip
    //
    void setVariableAsToolTip( bool variableAsToolTip );
    bool getVariableAsToolTip();

    // Allow user to drop new PVs into this widget
    void setAllowDrop( bool allowDropIn );
    bool getAllowDrop();

    // Display properties

    // useDbPrecision
    //
    void setUseDbPrecision( bool useDbPrecisionIn );
    bool getUseDbPrecision();

    // useDbDisplayLimits, e.g. as specified by LOPR and HOPR fields for ai, ao, longin
    // and longout record types, to call setAnalogMinimum and setAnalogMaximum.
    //
    void setUseDbDisplayLimits( bool useDbDisplayLimitsIn );
    bool getUseDbDisplayLimits();

    // visible (widget is visible outside 'Designer')
    void setRunVisible( bool visibleIn );
    bool getRunVisible();

public slots:
    void requestEnabled( const bool& state );


protected:
    virtual QString getSprintfFormat ();

    QCaFloatingFormatting floatingFormatting;
    bool localEnabled;

    void establishConnection( unsigned int variableIndex );

    bool visible;               // Flag true if the widget should be visible outside 'Designer'
    bool allowDrop;

private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip( const QString& tip );

    QCaVariableNamePropertyManager variableNamePropertyManager;

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;
    bool useDbPrecison;
    bool useDbDisplayLimits;
    bool isFirstUpdate;

private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );

    void setProgressBarValue( const double& value, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );

    void useNewVariableNameProperty( QString variableNameIn,
                                     QString variableNameSubstitutionsIn,
                                     unsigned int variableIndex );

signals:
    void dbValueChanged( const double& out );

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDropText( QString text );
    QString getDropText();

};

#endif /// QCAANALOGPROGRESSBAR_H
