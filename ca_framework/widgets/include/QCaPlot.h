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

#ifndef QCAPLOT_H
#define QCAPLOT_H

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
#define QCAPLOT_NUM_VARIABLES 4

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

class QCAPLUGINLIBRARYSHARED_EXPORT QCaPlot : public QwtPlot, public QCaWidget {
    Q_OBJECT

  public:
    QCaPlot( QWidget *parent = 0 );
    QCaPlot( const QString &variableName, QWidget *parent = 0 );

    ~QCaPlot();

    bool isEnabled() const;
    void setEnabled( bool state );

    // Property convenience functions

    void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

    void setSubscribe( bool subscribe );
    bool getSubscribe();

    void setVariableAsToolTip( bool variableAsToolTip );
    bool getVariableAsToolTip();

    // Allow user to drop new PVs into this widget
    void setAllowDrop( bool allowDropIn );
    bool getAllowDrop();

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

    // No QCaPlot::setTitle() needed. Uses QwtPlot::setTitle()
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

    bool visible;               // Flag true if the widget should be visible outside 'Designer'
    void setRunVisible( bool visibleIn );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setPlotData( const QVector<double>& values, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void setPlotData( const double value, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void tickTimeout();

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

    trace traces[QCAPLOT_NUM_VARIABLES];

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
    void setDropText( QString text );
    QString getDropText();
};


#endif /// QCAPLOT_H
