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

/*!
  This class is a CA aware Plot widget based on the Qwt QwtPlot widget.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details
 */

#include <qwt_legend.h>
#include <QCaPlot.h>

/*!
    Constructor with no initialisation
*/
QCaPlot::QCaPlot( QWidget *parent ) : QwtPlot( parent ), QCaWidget( this ) {
    setup();
}

/*!
    Constructor with known variable
*/
QCaPlot::QCaPlot( const QString &variableNameIn, QWidget *parent ) : QwtPlot( parent ), QCaWidget( this ) {
    setup();
    setVariableName( variableNameIn, 0 );
}

/*!
    Setup common to all constructors
*/
void QCaPlot::setup() {
    // Set up data
    // This control used a single data source
    setNumVariables(QCAPLOT_NUM_VARIABLES);

    // Set up default properties
    visible = true;
    setAllowDrop( false );

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    // General plot properties
    yMin = 0.0;
    yMax = 0.0;
    autoScale = true;
    axisEnableX = true;
    axisEnableY = true;

    //setLabelOrientation (Qt::Orientation)Qt::Vertical

    tickTimer = new QTimer(this);
    connect(tickTimer, SIGNAL(timeout()), this, SLOT(tickTimeout()));
    tickTimer->start( tickRate );

    // Waveform properties
    xStart = 0.0;
    xIncrement = 1.0;

    // Initially no curve or grid, and different trace colors
    for( int i = 0; i < QCAPLOT_NUM_VARIABLES; i++ )
    {
        trace* tr = &traces[i];
        tr->curve = NULL;
        switch( i ) // Note, this assumes 4 traces, but won't break with more or less
        {
            case 0:  tr->color = Qt::black; break;
            case 1:  tr->color = Qt::red;   break;
            case 2:  tr->color = Qt::green; break;
            case 3:  tr->color = Qt::blue;  break;
            default: tr->color = Qt::black; break;
        }
        tr->style = QwtPlotCurve::Lines;
    }

    grid = NULL;
    gridEnableMajorX = false;
    gridEnableMajorY = false;
    gridEnableMinorX = false;
    gridEnableMinorY = false;
    gridMajorColor = Qt::black;
    gridMinorColor = Qt::gray;


    tickRate = 50;
    timeSpan = 59;

    // Use QwtPlot signals
    // --Currently none--
}

QCaPlot::~QCaPlot()
{
    if( tickTimer )
    {
        tickTimer->stop();
        delete tickTimer;
    }

    for( int i = 0; i < QCAPLOT_NUM_VARIABLES; i++ )
    {
        trace* tr = &traces[i];
        if( tr->curve )
        {
            delete tr->curve;
            tr->curve = NULL;
        }
    }
    if( grid )
    {
        delete grid;
    }
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a strip chart a QCaObject that streams floating point data is required.
*/
qcaobject::QCaObject* QCaPlot::createQcaItem( unsigned int variableIndex ) {

   // Create the item as a QCaFloating
   return new QCaFloating( getSubstitutedVariableName( variableIndex ), this, &floatingFormatting, variableIndex );
}

/*!
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QCaPlot::establishConnection( unsigned int variableIndex ) {

    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots
    if(  qca ) {
        QObject::connect( qca,  SIGNAL( floatingArrayChanged( const QVector<double>&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setPlotData( const QVector<double>&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( qca,  SIGNAL( floatingChanged( const double, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setPlotData( const double, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}


/*!
    Update the tool tip as requested by QCaToolTip.
*/
void QCaPlot::updateToolTip( const QString& tip )
{
    setToolTip( tip );
}

/*!
    Act on a connection change.
    Change how the strip chart looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QCaPlot::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    /// If connected, enable the widget if the QCa enabled property is true
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );
    }

    /// If disconnected always disable the widget.
    else
    {
        isConnected = false;
        updateToolTipConnection( isConnected );
    }
}

/*!
    Update the plotted data with a new single value
    This is a slot used to recieve data updates from a QCaObject based class.
 */

void QCaPlot::setPlotData( const double value, QCaAlarmInfo& alarmInfo, QCaDateTime& timestamp, const unsigned int& variableIndex ) {

    /// Signal a database value change to any Link widgets
    emit dbValueChanged( value );

    // Select the curve information for this variable
    trace* tr = &traces[variableIndex];

    // Flag this trace is displaying a strip chart
    tr->waveform = false;

    // Add the new data point
    tr->timeStamps.append( timestamp );
    tr->ydata.append(value);
    tr->xdata.append( 0.0 ); // keep x and y arrays the same size
    regenerateTickXData( variableIndex );

    // Remove any old data
    QDateTime oldest = QDateTime::currentDateTime();
    oldest = oldest.addSecs( -timeSpan );
    while( tr->timeStamps.count() > 1 )
    {
        if( tr->timeStamps[1] < oldest )
        {
            tr->timeStamps.remove(0);
            tr->xdata.remove(0);
            tr->ydata.remove(0);
        }
        else
        {
            break;
        }
    }

    // Fix the X for a strip chart
    setAxisScale( xBottom, -(double)timeSpan, 0.0 );

    // The data is now ready to plot
    setPlotDataCommon( variableIndex );
    setalarmInfoCommon( alarmInfo );
}

/*!
    Update the plotted data with a new array of values
    This is a slot used to recieve data updates from a QCaObject based class.
 */
void QCaPlot::setPlotData( const QVector<double>& values, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex ) {

    /// Signal a database value change to any Link widgets
    emit dbValueChanged( values );

    // Select the curve information for this variable
    trace* tr = &traces[variableIndex];

    // Flag this trace is displaying a waveform
    tr->waveform = true;

    // Clear any previous data
    tr->xdata.clear();
    tr->ydata.clear();
    tr->timeStamps.clear();

    // If no increment was supplied, use 1 by default
    double inc;
    xIncrement == 0.0 ? inc = 1.0 : inc = xIncrement;

    for( int i = 0; i < values.count(); i++ )
    {
        tr->xdata.append( xStart + ((double)i * inc ) );
        tr->ydata.append( values[i] );
    }

    // Autoscale X for a waveform
    setAxisAutoScale( xBottom );

    // The data is now ready to plot
    setPlotDataCommon( variableIndex );
    setalarmInfoCommon( alarmInfo );

}

// Update the plot with new data.
// The new data may be due to a new value being added to the current values (stripchart)
// or the new data may be due to a new waveform
void QCaPlot::setPlotDataCommon( const unsigned int variableIndex )
{
    trace* tr = &traces[variableIndex];

    // Create the curve if it does not exist
    if( !tr->curve )
    {
        tr->curve = new QwtPlotCurve( tr->legend );

        setCurveColor( tr->color, variableIndex );
        tr->curve->setRenderHint( QwtPlotItem::RenderAntialiased );
        tr->curve->setStyle( tr->style );
        tr->curve->attach(this);
    }

    // Set the curve data
    tr->curve->setSamples( tr->xdata, tr->ydata );

    // Update the plot
    replot();
}

void QCaPlot::setalarmInfoCommon( QCaAlarmInfo& alarmInfo )
{
/// If in alarm, display as an alarm
    if( alarmInfo.getSeverity() != lastSeverity )
    {
            updateToolTipAlarm( alarmInfo.severityName() );
            setStyleSheet( alarmInfo.style() );
            lastSeverity = alarmInfo.getSeverity();
    }
}

/*!
  For strip chart functionality
  Recalculate the x value as time goes by
 */
void QCaPlot::regenerateTickXData( const unsigned int variableIndex )
{
    trace* tr = &traces[variableIndex];

    QDateTime now = QDateTime::currentDateTime();
    for( int i = 0; i < tr->xdata.count(); i++)
    {
        tr->xdata[i] = tr->timeStamps[i].floating( now );
    }
}

// Update the chart if it is a strip chart
void QCaPlot::tickTimeout()
{
    for( int i = 0; i < QCAPLOT_NUM_VARIABLES; i++ )
    {
        trace* tr = &traces[i];
        if( tr->curve && !tr->waveform )
        {
            regenerateTickXData( i );
            setPlotDataCommon( i );
        }
    }
}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaPlot::isEnabled() const
{
    /// Return what the state of widget would be if connected.
    return localEnabled;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaPlot::setEnabled( bool state )
{
    /// Note the new 'enabled' state
    localEnabled = state;

    /// Set the enabled state of the widget only if connected
    if( isConnected )
        QWidget::setEnabled( localEnabled );
}

/*!
   Slot similar to default widget setEnabled, but will use our own setEnabled which will allow alarm states to override current enabled state
 */
void QCaPlot::requestEnabled( const bool& state )
{
    setEnabled(state);
}


/*!
  Manage property to set widget visible or not
 */
void QCaPlot::setRunVisible( bool visibleIn )
{
    // Update the property
    visible = visibleIn;

    // If a container profile has been defined, then this widget is being used in a real GUI and
    // should be visible or not according to the visible property. (While in Designer it can always be displayed)
    ContainerProfile profile;
    if( profile.isProfileDefined() )
    {
        setVisible( visible );
    }

}

// Update the color of the trace
void QCaPlot::setCurveColor( const QColor color, const unsigned int variableIndex )
{
    trace* tr = &traces[variableIndex];
    if( tr->curve )
    {
        tr->curve->setPen( color );
    }
}

// allow drop (Enable/disable as a drop site for drag and drop)
void QCaPlot::setAllowDrop( bool allowDropIn )
{
    allowDrop = allowDropIn;
    setAcceptDrops( allowDrop );
}

bool QCaPlot::getAllowDrop()
{
    return allowDrop;
}

//==============================================================================
// Drag and Drop
void QCaPlot::setDropText( QString text )
{
    QStringList PVs = text.split( ' ' );
    for( int i = 0; i < PVs.size() && i < QCAPLOT_NUM_VARIABLES; i++ )
    {
        setVariableName( PVs[i], i );
        establishConnection( i );
    }
}

QString QCaPlot::getDropText()
{
    QString text;
    for( int i = 0; i < QCAPLOT_NUM_VARIABLES; i++ )
    {
        QString pv = getSubstitutedVariableName(i);
        if( !pv.isEmpty() )
        {
            if( !text.isEmpty() )
                text.append( " " );
            text.append( pv );
        }
    }

        return text;
}

//==============================================================================
// Property convenience functions


// Access functions for variableName and variableNameSubstitutions
// variable substitutions Example: SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
void QCaPlot::setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
{
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
    setVariableName( variableNameIn, variableIndex );
    establishConnection( variableIndex );
}

// Access functions for variableAsToolTip
void QCaPlot::setVariableAsToolTip( bool variableAsToolTipIn )
{
    variableAsToolTip = variableAsToolTipIn;
}
bool QCaPlot::getVariableAsToolTip()
{
    return variableAsToolTip;
}

// Access functions for YMin
void QCaPlot::setYMin( double yMinIn )
{
    yMin = yMinIn;
    if( !autoScale )
    {
        setAxisScale( yLeft, yMin, yMax );
    }
}
double QCaPlot::getYMin()
{
    return yMin;
}

// Access functions for yMax
void QCaPlot::setYMax( double yMaxIn )
{
    yMax = yMaxIn;
    if( !autoScale )
    {
        setAxisScale( yLeft, yMin, yMax );
    }
}
double QCaPlot::getYMax()
{
    return yMax;
}

// Access functions for autoScale
void QCaPlot::setAutoScale( bool autoScaleIn )
{
    autoScale = autoScaleIn;
    
    // Set auto scale if requested, or if manual scale values are invalid
    if( autoScale || yMin == yMax )
    {
        setAxisAutoScale( yLeft );
    }
    else
    {
        setAxisScale( yLeft, yMin, yMax );
    }
}
bool QCaPlot::getAutoScale()
{
    return autoScale;
}

// Access functions for X axis visibility
void QCaPlot::setAxisEnableX( bool axisEnableXIn )
{
    axisEnableX = axisEnableXIn;
    enableAxis( xBottom, axisEnableX );
}

bool QCaPlot::getAxisEnableX()
{
    return axisEnableX;
}

// Access functions for Y axis visibility
void QCaPlot::setAxisEnableY( bool axisEnableYIn )
{
    axisEnableY = axisEnableYIn;
    enableAxis( yLeft, axisEnableY );
}

bool QCaPlot::getAxisEnableY()
{
    return axisEnableY;
}

// Access functions for grid
void QCaPlot::setGridEnableMajorX( bool gridEnableMajorXIn )
{
    gridEnableMajorX = gridEnableMajorXIn;
    setGridEnable();
}
void QCaPlot::setGridEnableMajorY( bool gridEnableMajorYIn )
{
    gridEnableMajorY = gridEnableMajorYIn;
    setGridEnable();
}
void QCaPlot::setGridEnableMinorX( bool gridEnableMinorXIn )
{
    gridEnableMinorX = gridEnableMinorXIn;
    setGridEnable();
}
void QCaPlot::setGridEnableMinorY( bool gridEnableMinorYIn )
{
    gridEnableMinorY = gridEnableMinorYIn;
    setGridEnable();
}
void QCaPlot::setGridEnable()
{
    // If any grid is required, create a grid and set it up
    // Note, Qwt will ignore minor enable if major is not enabled
    if( gridEnableMajorX || gridEnableMajorY || gridEnableMinorX || gridEnableMinorY )
    {
        if( !grid )
        {
            grid = new QwtPlotGrid;
            grid->setMajPen(QPen(gridMajorColor, 0, Qt::DotLine));
            grid->setMinPen(QPen(gridMinorColor, 0 , Qt::DotLine));
            grid->attach( this );
        }
        grid->enableX(gridEnableMajorX);
        grid->enableY(gridEnableMajorY);
        grid->enableXMin(gridEnableMinorX);
        grid->enableYMin(gridEnableMinorY);
    }
    // No grid required, get rid of any grid
    else
    {
        if( grid )
        {
            grid->detach();
            delete grid;
            grid = NULL;
        }
    }
}
bool QCaPlot::getGridEnableMajorX()
{
    return gridEnableMajorX;
}
bool QCaPlot::getGridEnableMajorY()
{
    return gridEnableMajorY;
}
bool QCaPlot::getGridEnableMinorX()
{
    return gridEnableMinorX;
}
bool QCaPlot::getGridEnableMinorY()
{
    return gridEnableMinorY;
}


// Access functions for gridColor
void QCaPlot::setGridMajorColor( QColor gridMajorColorIn )
{
    gridMajorColor = gridMajorColorIn;
    if( grid )
    {
        grid->setMajPen(QPen(gridMajorColor, 0, Qt::DotLine));
    }
}
void QCaPlot::setGridMinorColor( QColor gridMinorColorIn )
{
    gridMinorColor = gridMinorColorIn;
    if( grid )
    {
        grid->setMinPen(QPen(gridMinorColor, 0 , Qt::DotLine));
    }
}
QColor QCaPlot::getGridMajorColor()
{
    return gridMajorColor;
}
QColor QCaPlot::getGridMinorColor()
{
    return gridMinorColor;
}


// Access functions for title
// No QCaPlot::setTitle() needed. Uses QwtPlot::setTitle()
QString QCaPlot::getTitle()
{
    return title().text();
}

// Access functions for backgroundColor
void    QCaPlot::setBackgroundColor( QColor backgroundColor )
{
    QBrush brush = canvasBackground();
    brush.setColor( backgroundColor );
    setCanvasBackground( brush );
}
QColor QCaPlot::getBackgroundColor()
{
    return canvasBackground().color();
}

// Access functions for traceStyle
void QCaPlot::setTraceStyle( QwtPlotCurve::CurveStyle traceStyle, const unsigned int variableIndex ){
    trace* tr = &traces[variableIndex];
    tr->style = traceStyle;
    if( tr->curve )
    {
        tr->curve->setStyle( tr->style );
    }
}

QwtPlotCurve::CurveStyle QCaPlot::getTraceStyle( const unsigned int variableIndex )
{
    return traces[variableIndex].style;
}

// Access functions for traceColor
void QCaPlot::setTraceColor( QColor traceColor, const unsigned int variableIndex ){
    traces[variableIndex].color = traceColor;
    setCurveColor( traceColor, variableIndex );
}
void QCaPlot::setTraceColor1( QColor traceColor ){ setTraceColor( traceColor, 0 ); }
void QCaPlot::setTraceColor2( QColor traceColor ){ setTraceColor( traceColor, 1 ); }
void QCaPlot::setTraceColor3( QColor traceColor ){ setTraceColor( traceColor, 2 ); }
void QCaPlot::setTraceColor4( QColor traceColor ){ setTraceColor( traceColor, 3 ); }

QColor QCaPlot::getTraceColor( const unsigned int variableIndex )
{
    return traces[variableIndex].color;
}
QColor QCaPlot::getTraceColor1(){ return getTraceColor( 0 ); }
QColor QCaPlot::getTraceColor2(){ return getTraceColor( 1 ); }
QColor QCaPlot::getTraceColor3(){ return getTraceColor( 2 ); }
QColor QCaPlot::getTraceColor4(){ return getTraceColor( 3 ); }

// Access functions for traceLegend
void QCaPlot::setTraceLegend( QString traceLegend, const unsigned int variableIndex ){

    trace* tr = &traces[variableIndex];

    tr->legend = traceLegend;
    if( traceLegend.count() )
    {
        insertLegend( new QwtLegend(), QwtPlot::RightLegend );
    }
    else
    {
        insertLegend( NULL, QwtPlot::RightLegend );
    }

    if( tr->curve )
    {
        tr->curve->setTitle( traceLegend );
    }
}
void QCaPlot::setTraceLegend1( QString traceLegend ){ setTraceLegend( traceLegend, 0 ); }
void QCaPlot::setTraceLegend2( QString traceLegend ){ setTraceLegend( traceLegend, 1 ); }
void QCaPlot::setTraceLegend3( QString traceLegend ){ setTraceLegend( traceLegend, 2 ); }
void QCaPlot::setTraceLegend4( QString traceLegend ){ setTraceLegend( traceLegend, 3 ); }

QString QCaPlot::getTraceLegend( const unsigned int variableIndex )
{
    return traces[variableIndex].legend;
}
QString QCaPlot::getTraceLegend1(){ return getTraceLegend( 0 ); }
QString QCaPlot::getTraceLegend2(){ return getTraceLegend( 1 ); }
QString QCaPlot::getTraceLegend3(){ return getTraceLegend( 2 ); }
QString QCaPlot::getTraceLegend4(){ return getTraceLegend( 3 ); }

// Access functions for xUnit
void    QCaPlot::setXUnit( QString xUnit )
{
    setAxisTitle(xBottom, xUnit);
}
QString QCaPlot::getXUnit()
{
    return axisTitle( xBottom ).text();
}

// Access functions for yUnit
void    QCaPlot::setYUnit( QString yUnit )
{
    setAxisTitle( yLeft, yUnit );
}
QString QCaPlot::getYUnit()
{
    return axisTitle( yLeft ).text();
}

// Access functions for xStart
void QCaPlot::setXStart( double xStartIn )
{
    xStart = xStartIn;
}
double QCaPlot::getXStart()
{
    return xStart;
}

// Access functions for xIncrement
void QCaPlot::setXIncrement( double xIncrementIn )
{
    xIncrement = xIncrementIn;
}
double QCaPlot::getXIncrement()
{
    return xIncrement;
}

// Access functions for timeSpan
void QCaPlot::setTimeSpan( unsigned int timeSpanIn )
{
    timeSpan = timeSpanIn;
}
unsigned int QCaPlot::getTimeSpan()
{
    return timeSpan;
}

// Access functions for tickRate
void QCaPlot::setTickRate( unsigned int tickRateIn )
{
    tickRate = tickRateIn;
    if( tickTimer )
    {
        tickTimer->stop();
        tickTimer->start( tickRate );
    }
}
unsigned int QCaPlot::getTickRate()
{
    return tickRate;
}

