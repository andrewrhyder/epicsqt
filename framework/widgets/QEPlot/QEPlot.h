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
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Glenn Jackson
 *  Contact details:
 *    glenn.jackson@synchrotron.org.au
 */

#ifndef QEPLOT_H
#define QEPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <QCaWidget.h>
#include <QCaFloating.h>
#include <QCaFloatingFormatting.h>
#include <QCaVariableNamePropertyManager.h>
#include <QVector>
#include <QTimer>
#include <QCaPluginLibrary_global.h>

// Maximum number of variables.
#define QEPLOT_NUM_VARIABLES 4

// Trace related data and properties
class trace {
    public:

    QVector<QCaDateTime> timeStamps;
    QVector<double> xdata;
    QVector<double> ydata;

    QwtPlotCurve* curve;
    QColor color;
    QString legend;
    bool waveform;  // True if displaying a waveform (an array of values arriving in one update), false if displaying a strip chart (individual values arriving over time)
    QwtPlotCurve::CurveStyle style;
};

class QCAPLUGINLIBRARYSHARED_EXPORT QEPlot : public QwtPlot, public QCaWidget {
    Q_OBJECT

  public:
    QEPlot( QWidget *parent = 0 );
    QEPlot( const QString &variableName, QWidget *parent = 0 );

    ~QEPlot();

    // Property convenience functions

    void setYMin( double yMin );
    double getYMin();

    void setYMax( double yMax );
    double getYMax();

    void setAutoScale( bool autoScale );
    bool getAutoScale();

    void setAxisEnableX( bool axisEnableXIn );
    bool getAxisEnableX();

    void setAxisEnableY( bool axisEnableYIn );
    bool getAxisEnableY();

    // No QEPlot::setTitle() needed. Uses QwtPlot::setTitle()
    QString getTitle();

    void    setBackgroundColor( QColor backgroundColor );
    QColor getBackgroundColor();

    void    setTraceStyle( QwtPlotCurve::CurveStyle traceStyle, const unsigned int variableIndex );
    QwtPlotCurve::CurveStyle getTraceStyle( const unsigned int variableIndex );

    void    setTraceColor( QColor traceColor, const unsigned int variableIndex );
    void    setTraceColor1( QColor traceColor );
    void    setTraceColor2( QColor traceColor );
    void    setTraceColor3( QColor traceColor );
    void    setTraceColor4( QColor traceColor );
    QColor getTraceColor( const unsigned int variableIndex );
    QColor getTraceColor1();
    QColor getTraceColor2();
    QColor getTraceColor3();
    QColor getTraceColor4();

    void    setTraceLegend1( QString traceLegend );
    void    setTraceLegend2( QString traceLegend );
    void    setTraceLegend3( QString traceLegend );
    void    setTraceLegend4( QString traceLegend );

    QString getTraceLegend1();
    QString getTraceLegend2();
    QString getTraceLegend3();
    QString getTraceLegend4();

    void    setXUnit( QString xUnit );
    QString getXUnit();

    void    setYUnit( QString yUnit );
    QString getYUnit();

    void setGridEnableMajorX( bool gridEnableMajorXIn );
    void setGridEnableMajorY( bool gridEnableMajorYIn );
    void setGridEnableMinorX( bool gridEnableMinorXIn );
    void setGridEnableMinorY( bool gridEnableMinorYIn );
    bool getGridEnableMajorX();
    bool getGridEnableMajorY();
    bool getGridEnableMinorX();
    bool getGridEnableMinorY();

    void setGridMajorColor( QColor gridMajorColorIn );
    void setGridMinorColor( QColor gridMinorColorIn );
    QColor getGridMajorColor();
    QColor getGridMinorColor();

    void setXStart( double xStart );
    double getXStart();

    void setXIncrement( double xIncrement );
    double getXIncrement();

    void setTimeSpan( unsigned int timeSpan );
    unsigned int getTimeSpan();

    void setTickRate( unsigned int tickRate );
    unsigned int getTickRate();

  protected:
    QCaFloatingFormatting floatingFormatting;
    bool localEnabled;
    bool allowDrop;

    void establishConnection( unsigned int variableIndex );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setPlotData( const QVector<double>& values, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void setPlotData( const double value, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void tickTimeout();
    // !! move this functionality into QCaWidget???
    // !! needs one for single variables and one for multiple variables, or just the multiple variable one for all
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )// !! move into Standard Properties section??
    {
        setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
    }


  public slots:
    void requestEnabled( const bool& state );

  signals:
    void dbValueChanged( const double& out );
    void dbValueChanged( const QVector<double>& out );

  private:

    void setup();

    QTimer* tickTimer;          // Timer to keep strip chart scrolling
    void setPlotDataCommon( const unsigned int variableIndex );
    void setalarmInfoCommon( QCaAlarmInfo& alarmInfo );


    // General plot properties
    double yMin;
    double yMax;
    bool autoScale;
    bool axisEnableX;
    bool axisEnableY;
    QwtPlotGrid* grid;
    bool gridEnableMajorX;
    bool gridEnableMajorY;
    bool gridEnableMinorX;
    bool gridEnableMinorY;
    QColor gridMajorColor;
    QColor gridMinorColor;

    // Trace update and movement properties
    unsigned int tickRate; //mS
    unsigned int timeSpan; // Seconds

    // Waveform properties
    double xStart;
    double xIncrement;

    // Functions common to most QCa widgets
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip( const QString& tip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;
    
    // Variables and functions to manage plot data

    trace traces[QEPLOT_NUM_VARIABLES];

    void regenerateTickXData( const unsigned int variableIndex );

    void setCurveColor( const QColor color, const unsigned int variableIndex );
    void    setTraceLegend( QString traceLegend, const unsigned int variableIndex );
    QString getTraceLegend( const unsigned int variableIndex );
    void setGridEnable();

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();

public:
    //=================================================================================
    // Multiple Variable properties
    // These properties should be identical for every widget using multiple variables (The number of variables may vary).
    // WHEN MAKING CHANGES: search for MULTIPLEVARIABLEPROPERTIESBASE and change all occurances.
    private:
        QCaVariableNamePropertyManager variableNamePropertyManagers[QEPLOT_NUM_VARIABLES];
    public:

    // Define a variable
    // Note, the QPROPERTY declaration itself can't be in this macro
#define VARIABLE_PROPERTY_ACCESS(VAR_INDEX) \
    void    setVariableName##VAR_INDEX##Property( QString variableName ){ variableNamePropertyManagers[VAR_INDEX].setVariableNameProperty( variableName ); } \
    QString getVariableName##VAR_INDEX##Property(){ return variableNamePropertyManagers[VAR_INDEX].getVariableNameProperty(); }

    VARIABLE_PROPERTY_ACCESS(0)
    Q_PROPERTY(QString variable1 READ getVariableName0Property WRITE setVariableName0Property)

    VARIABLE_PROPERTY_ACCESS(1)
    Q_PROPERTY(QString variable2 READ getVariableName1Property WRITE setVariableName1Property)

    VARIABLE_PROPERTY_ACCESS(2)
    Q_PROPERTY(QString variable3 READ getVariableName2Property WRITE setVariableName2Property)

    VARIABLE_PROPERTY_ACCESS(3)
    Q_PROPERTY(QString variable4 READ getVariableName3Property WRITE setVariableName3Property)

    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions )
    {
        for( int i = 0; i < QEPLOT_NUM_VARIABLES; i++ )
        {
            variableNamePropertyManagers[i].setSubstitutionsProperty( variableNameSubstitutions );
        }
    }
    QString getVariableNameSubstitutionsProperty()
    {
        return variableNamePropertyManagers[0].getSubstitutionsProperty();
    }
    //=================================================================================

    //=================================================================================
    // Standard properties
    // These properties should be identical for every widget using them.
    // WHEN MAKING CHANGES: search for STANDARDPROPERTIES and change all occurances.
    bool isEnabled() const { return getApplicationEnabled(); }
    void setEnabled( bool state ){ setApplicationEnabled( state ); }
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(bool allowDrop READ getAllowDrop WRITE setAllowDrop)
    Q_PROPERTY(bool visible READ getRunVisible WRITE setRunVisible)
    Q_PROPERTY(unsigned int messageSourceId READ getMessageSourceId WRITE setMessageSourceId )
    Q_PROPERTY(QString userLevelUserStyle READ getStyleUser WRITE setStyleUser)
    Q_PROPERTY(QString userLevelScientistStyle READ getStyleScientist WRITE setStyleScientist)
    Q_PROPERTY(QString userLevelEngineerStyle READ getStyleEngineer WRITE setStyleEngineer)
    enum UserLevels { User      = USERLEVEL_USER,
                      Scientist = USERLEVEL_SCIENTIST,
                      Engineer  = USERLEVEL_ENGINEER };
    UserLevels getUserLevelVisibilityProperty() { return (UserLevels)getUserLevelVisibility(); }
    void setUserLevelVisibilityProperty( UserLevels level ) { setUserLevelVisibility( (userLevels)level ); }

    UserLevels getUserLevelEnabledProperty() { return (UserLevels)getUserLevelEnabled(); }
    void setUserLevelEnabledProperty( UserLevels level ) { setUserLevelEnabled( (userLevels)level ); }
    Q_ENUMS(UserLevels)
    Q_PROPERTY(UserLevels userLevelVisibility READ getUserLevelVisibilityProperty WRITE setUserLevelVisibilityProperty)
    Q_PROPERTY(UserLevels userLevelEnabled READ getUserLevelEnabledProperty WRITE setUserLevelEnabledProperty)
    //=================================================================================

    // Widget specific properties

    Q_PROPERTY(QColor traceColor1 READ getTraceColor1 WRITE setTraceColor1)
    Q_PROPERTY(QColor traceColor2 READ getTraceColor2 WRITE setTraceColor2)
    Q_PROPERTY(QColor traceColor3 READ getTraceColor3 WRITE setTraceColor3)
    Q_PROPERTY(QColor traceColor4 READ getTraceColor4 WRITE setTraceColor4)

    Q_ENUMS(TraceStyles)
    Q_PROPERTY(TraceStyles traceStyle1 READ getTraceStyle1 WRITE setTraceStyle1)
    Q_PROPERTY(TraceStyles traceStyle2 READ getTraceStyle2 WRITE setTraceStyle2)
    Q_PROPERTY(TraceStyles traceStyle3 READ getTraceStyle3 WRITE setTraceStyle3)
    Q_PROPERTY(TraceStyles traceStyle4 READ getTraceStyle4 WRITE setTraceStyle4)
    enum TraceStyles { Lines  = QwtPlotCurve::Lines,
                       Sticks = QwtPlotCurve::Sticks,
                       Steps  = QwtPlotCurve::Steps,
                       Dots   = QwtPlotCurve::Dots };
    void setTraceStyle1( TraceStyles traceStyle ){ setTraceStyle( (QwtPlotCurve::CurveStyle)traceStyle, 0 ); }
    void setTraceStyle2( TraceStyles traceStyle ){ setTraceStyle( (QwtPlotCurve::CurveStyle)traceStyle, 1 ); }
    void setTraceStyle3( TraceStyles traceStyle ){ setTraceStyle( (QwtPlotCurve::CurveStyle)traceStyle, 2 ); }
    void setTraceStyle4( TraceStyles traceStyle ){ setTraceStyle( (QwtPlotCurve::CurveStyle)traceStyle, 3 ); }

    TraceStyles getTraceStyle1(){ return (TraceStyles)(getTraceStyle( 0 )); }
    TraceStyles getTraceStyle2(){ return (TraceStyles)(getTraceStyle( 1 )); }
    TraceStyles getTraceStyle3(){ return (TraceStyles)(getTraceStyle( 2 )); }
    TraceStyles getTraceStyle4(){ return (TraceStyles)(getTraceStyle( 3 )); }


    Q_PROPERTY(QString traceLegend1 READ getTraceLegend1 WRITE setTraceLegend1)
    Q_PROPERTY(QString traceLegend2 READ getTraceLegend2 WRITE setTraceLegend2)
    Q_PROPERTY(QString traceLegend3 READ getTraceLegend3 WRITE setTraceLegend3)
    Q_PROPERTY(QString traceLegend4 READ getTraceLegend4 WRITE setTraceLegend4)

    Q_PROPERTY(double yMin READ getYMin WRITE setYMin)
    Q_PROPERTY(double yMax READ getYMax WRITE setYMax)

    Q_PROPERTY(bool autoScale READ getAutoScale WRITE setAutoScale)

    Q_PROPERTY(bool axisEnableX READ getAxisEnableX WRITE setAxisEnableX)
    Q_PROPERTY(bool axisEnableY READ getAxisEnableY WRITE setAxisEnableY)

    Q_PROPERTY(bool gridEnableMajorX READ getGridEnableMajorX WRITE setGridEnableMajorX)
    Q_PROPERTY(bool gridEnableMajorY READ getGridEnableMajorY WRITE setGridEnableMajorY)

    Q_PROPERTY(bool gridEnableMinorX READ getGridEnableMinorX WRITE setGridEnableMinorX)
    Q_PROPERTY(bool gridEnableMinorY READ getGridEnableMinorY WRITE setGridEnableMinorY)

    Q_PROPERTY(QColor gridMajorColor READ getGridMajorColor WRITE setGridMajorColor)
    Q_PROPERTY(QColor gridMinorColor READ getGridMinorColor WRITE setGridMinorColor)

    Q_PROPERTY(QString title READ getTitle WRITE setTitle)
    Q_PROPERTY(QColor backgroundColor READ getBackgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(QString xUnit READ getXUnit WRITE setXUnit)
    Q_PROPERTY(QString yUnit READ getYUnit WRITE setYUnit)
    Q_PROPERTY(double xStart READ getXStart WRITE setXStart)
    Q_PROPERTY(double xIncrement READ getXIncrement WRITE setXIncrement)
    Q_PROPERTY(unsigned int timeSpan READ getTimeSpan WRITE setTimeSpan)
    Q_PROPERTY(unsigned int tickRate READ getTickRate WRITE setTickRate)
};

#endif // QEPLOT_H
