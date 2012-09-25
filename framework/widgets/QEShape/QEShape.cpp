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
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/*!
  This class is a CA aware shape widget based on QWidget.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details
 */

#include <QEShape.h>
#include <QCaDateTime.h>
#include <QtGui>
#include <QDebug>

/*!
    Create without a known variable. Just manage parental hirarchy.
*/
QEShape::QEShape( QWidget *parent ) : QWidget( parent ), QCaWidget( this ) {
    setup();
}

/*!
    Create with a known variable. Subscription occurs immedietly.
*/
QEShape::QEShape( const QString &variableNameIn, QWidget *parent ) : QWidget( parent ), QCaWidget( this ) {
    /// Call common setup code.
    setup();

    /// Use the variable name as the first variable
    setVariableName( variableNameIn, 0 );
}

/*!
    Common setup code.
*/
void QEShape::setup() {
    // Set up data
    // Set the number of variables equal to the base VariableNameManager is to manage
    setNumVariables( QESHAPE_NUM_VARIABLES );

    // Set up default properties
    setAllowDrop( false );

    shape = Rect;
    setAutoFillBackground(false);

    antialiased = false;
    pixmap.load(":/images/qt-logo.png");

    setBackgroundRole(QPalette::NoRole);

    originTranslation = QPoint( 0, 0 );
    scaledOriginTranslation = originTranslation;

    points[0] = QPoint( 0, 0 );
    points[1] = QPoint( width()-1, this->height()-1 );
    unsigned int i;
    for( i = 2; i < POINTS_SIZE; i++ )
        points[i] = QPoint( 0, 0 );

    numPoints = 2;

    colors[0] = QColor( 255,0,0);
    colors[1] = QColor( 0,255,0);
    colors[2] = QColor( 0,0,255);
    colors[3] = QColor( 255,255,255);
    colors[4] = QColor( 0,0,0);

    for( i = 0; i < SCALES_SIZE; i++ )
        scales[i] = 1.0;


    for( i = 0; i < OFFSETS_SIZE; i++ )
        offsets[i] = 0.0;

    animations[0] = Width;
    animations[1] = Height;
    animations[2] = X;
    animations[3] = Y;
    animations[4] = Transperency;
    animations[5] = ColourHue;

    painterCurrentScaleX = 1.0;
    painterCurrentScaleY = 1.0;
    painterCurrentTranslateX = 0;
    painterCurrentTranslateY = 0;
    rotation = 0.0;

    drawBorder = true;

    lineWidth = 1;
    pen.setWidth( lineWidth );

    fill = true;

    brush.setStyle( Qt::SolidPattern );
    currentColor = 0;
    brush.setColor( colors[currentColor] );

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    // Use widget signals
    //!! move this functionality into QCaWidget???
    //!! needs one for single variables and one for multiple variables, or just the multiple variable one for all
    // for each variable name property manager, set up an index to identify it when it signals and
    // set up a connection to recieve variable name property changes.
    // The variable name property manager class only delivers an updated variable name after the user has stopped typing
    for( int i = 0; i < QESHAPE_NUM_VARIABLES; i++ )
    {
        variableNamePropertyManagers[i].setVariableIndex( i );
        QObject::connect( &variableNamePropertyManagers[i], SIGNAL( newVariableNameProperty( QString, QString, unsigned int ) ), this, SLOT( useNewVariableNameProperty( QString, QString, unsigned int ) ) );
    }
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a shape a QCaObject that streams integers is required.
*/
qcaobject::QCaObject* QEShape::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QCaInteger
    return new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );
}

/*!
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QEShape::establishConnection( unsigned int variableIndex ) {

    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots
    if(  qca ) {
//        setValue( 0, QCaAlarmInfo(), QCaDateTime(), variableIndex );  //??? should this be moved up before the create connection? if create connection fails, then any previous data should be cleared? do for all types if required
        QObject::connect( qca,  SIGNAL( integerChanged( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setValue( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}

/*!
    Update the tool tip as requested by QCaToolTip.
*/
void QEShape::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QEShape::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    /// If connected, enable the widget if the QCa enabled property is true
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );

        setDataDisabled( false );
    }

    /// If disconnected always disable the widget.
    else
    {
        isConnected = false;
        updateToolTipConnection( isConnected );

        setDataDisabled( true );
    }
}

/*!
    Use a data update to alter the shape's attributes.
    The name setValue is less appropriate for this QCa widget than for others
    such as QELabel where setValue() sets the value displayed. For this
    widget setting the value means modifying one attribute such as position or
    color.
    This is the slot used to recieve data updates from a QCaObject based class.
*/
void QEShape::setValue( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex ) {

    /// Signal a database value change to any Link widgets
    switch( variableIndex )
    {
        case 0: emit dbValueChanged1( value ); break;
        case 1: emit dbValueChanged2( value ); break;
        case 2: emit dbValueChanged3( value ); break;
        case 3: emit dbValueChanged4( value ); break;
        case 4: emit dbValueChanged5( value ); break;
        case 5: emit dbValueChanged6( value ); break;
        default: sendMessage( "Application error: Unexpected variable index", "QEShape.cpp QEShape::setValue()", MESSAGE_TYPE_ERROR );
    }

    /// Scale the data.
    /// For example, a flow of 0 to 10 l/m may adjust a shape size 0 to 200 pixels
    double scaledValue = (double)(value)*scales[variableIndex] + offsets[variableIndex];

    /// Animate the object.
    /// Apply the data to the appropriate attribute of the shape
    switch( animations[variableIndex] ) {
        case Width :
        {
            painterCurrentScaleX = scaledValue;
            break;
        }
        case Height :
        {
            painterCurrentScaleY = scaledValue;
            break;
        }
        case X :
        {
            painterCurrentTranslateX = scaledValue;
            break;
        }
        case Y :
        {
            painterCurrentTranslateY = scaledValue;
            break;
        }
        case Transperency :
        {
            int newAlpha = 0;
            if( scaledValue >= 0.0 ) {
                if ( scaledValue <= 255 ) {
                    newAlpha = (unsigned int)scaledValue;
                } else {
                    newAlpha = 255;
                }
            }
            int h, s, v, a;
            QColor color;
            color = brush.color();
            color.getHsv( &h, &s, &v, &a );
            a = newAlpha;
            color.setHsv( h, s, v, a );
            brush.setColor( color );
            break;
        }
        case Rotation :
        {
            rotation = scaledValue;
            break;
        }
        case ColourHue :
        {
            int newHue = 0;
            if( scaledValue >= 0.0 ) {
                if( scaledValue <= 255 ) {
                    newHue = (unsigned int)scaledValue;
                } else {
                    newHue = 255;
                }
            }
            int h, s, v, a;
            QColor color;
            color = brush.color();
            color.getHsv( &h, &s, &v, &a );
            h = newHue;
            color.setHsv( h, s, v, a );
            brush.setColor( color );
            break;
        }
        case ColourSaturation :
        {
            int newSaturation = 0;
            if( scaledValue >= 0.0 ) {
                if( scaledValue <= 255 ) {
                    newSaturation = (unsigned int)scaledValue;
                } else {
                    newSaturation = 255;
                }
            }
            int h, s, v, a;
            QColor color;
            color = brush.color();
            color.getHsv( &h, &s, &v, &a );
            s = newSaturation;
            color.setHsv( h, s, v, a );
            brush.setColor( color );
            break;
        }
        case ColourValue :
        {
            int newValue = 0;
            if( scaledValue >= 0.0 ) {
                if( scaledValue <= 255 ) {
                    newValue = (unsigned int)scaledValue;
                } else {
                    newValue = 255;
                }
            }
            int h, s, v, a;
            QColor color;
            color = brush.color();
            color.getHsv( &h, &s, &v, &a );
            v = newValue;
            color.setHsv( h, s, v, a );
            brush.setColor( color );
            break;
        }
        case ColourIndex :
        {
            if( scaledValue >= 0.0 ) {
                if ( scaledValue < COLORS_SIZE ) {
                    currentColor = (unsigned int)scaledValue;
                } else {
                    currentColor = COLORS_SIZE-1;
                }
            }
            brush.setColor( colors[currentColor] );
            break;
        }
        case Penwidth :
        {
            pen.setWidth( (int)scaledValue );
            break;
        }
    }

    /// If in alarm, display as an alarm
    if( alarmInfo.getSeverity() != lastSeverity )
    {
            updateToolTipAlarm( alarmInfo.severityName() );
            setStyleSheet( alarmInfo.style() );
            lastSeverity = alarmInfo.getSeverity();
    }

    /// Force the shape to be redrawn
    update();
}

/*!
    Draw the shape.
    This is called whenever the applications forces a redraw using update()
    after data modifies some attribute of the shape, or whenever the windowing
    system decides the object requires drawing, such as when the window
    containing the shape widget is scrolled into view.
*/
void QEShape::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter( this );

    /// Set up the pen and brush (color, thickness, etc)
    pen.setWidth( lineWidth );
    painter.setPen( pen );
    painter.setBrush( brush );

    /// Draw everything with antialiasing
    painter.setRenderHint( QPainter::Antialiasing, true );

    /// Alter the viewport according to the origin translation properties
    /// ???Origin translation was added so 0,0 could be some where other than top left as scaling appeared
    /// to be after translation. This causes the translation to also be scaled which was inappropriate.
    /// This may not be the case, in which case the origin translation could be removed and the same effect
    /// could be achieved by giving variables associated with X and Y a negative offset.
    QRect viewportRect = painter.viewport();
    viewportRect.moveLeft( originTranslation.x() );
    viewportRect.moveTop( originTranslation.y() );
    painter.setWindow( viewportRect );

    /// Apply the current translation, scaling and rotation
    painter.translate( painterCurrentTranslateX+0.5, painterCurrentTranslateY+0.5 );
    painter.scale( painterCurrentScaleX, painterCurrentScaleY );
    painter.rotate( rotation );

    /// Draw the shape
    switch( shape ) {
        case Line :
            painter.drawLine( points[0], points[1] );
            break;
        case Points :
            painter.drawPoints(points, numPoints);
            break;
        case Polyline :
            painter.drawPolyline(points, numPoints);
            break;
        case Polygon :
            painter.drawPolygon(points, numPoints);
            break;
        case Rect :
            if( !drawBorder )
                painter.setPen( Qt::NoPen );
            if( !fill )
                painter.setBrush( Qt::NoBrush );
            painter.drawRect( points[0].x(), points[0].y(), points[1].x(), points[1].y() );
            break;
        case RoundedRect :
            if( !drawBorder )
                painter.setPen( Qt::NoPen );
#if QT_VERSION >= 0x040400
            painter.drawRoundedRect(points[0].x(), points[0].y(), points[1].x(), points[1].y(), 25, 25, Qt::RelativeSize);
#else
            painter.drawRect( points[0].x(), points[0].y(), points[1].x(), points[1].y() );
#endif
            break;
        case Ellipse :
            if( !drawBorder )
                painter.setPen( Qt::NoPen );
            painter.drawEllipse(points[0].x(), points[0].y(), points[1].x(), points[1].y());
            break;
        case Arc :
            painter.drawArc(points[0].x(), points[0].y(), points[1].x(), points[1].y(), (int)(startAngle*16), (int)(arcLength*16) );
            break;
        case Chord :
            if( !drawBorder )
                painter.setPen( Qt::NoPen );
            painter.drawChord(points[0].x(), points[0].y(), points[1].x(), points[1].y(), (int)(startAngle*16), (int)(arcLength*16) );
            break;
        case Pie :
            if( !drawBorder )
                painter.setPen( Qt::NoPen );
            painter.drawPie(points[0].x(), points[0].y(), points[1].x(), points[1].y(), (int)(startAngle*16), (int)(arcLength*16) );
            break;
        case Path :
        {
            QPainterPath path;
            if( numPoints > 1 ) {
                path.moveTo(points[0]);
                for( unsigned int i = 1; i < numPoints; i++ ) {
                    path.lineTo( points[i] );
                }
                painter.drawPath(path);
            }
            break;
        }
        case Text :
        {
            QRect rect( points[0], points[1] );
            QRectF qrect( rect );
            painter.drawText( qrect, Qt::AlignCenter, text );
            break;
        }
        case Pixmap :
            painter.drawPixmap(10, 10, pixmap);
            break;
    }
}

/*!
   Reset the brush color if the color the brush is using is changing
 */
void QEShape::colorChange( unsigned int index )
{
    // Sanity check. Ignore out of range color index
    if( index >= COLORS_SIZE )
        return;

    // Update the brush and redraw the shape if the color being changed is in use
    if( currentColor == index )
    {
        brush.setColor( colors[currentColor] );
        update();
    }
}

//==============================================================================
// Drag and Drop
void QEShape::setDrop( QVariant drop )
{
    QStringList PVs = drop.toString().split( ' ' );
    for( int i = 0; i < PVs.size() && i < QESHAPE_NUM_VARIABLES; i++ )
    {
        setVariableName( PVs[i], i );
        establishConnection( i );
    }
}

QVariant QEShape::getDrop()
{
    QString text;
    for( int i = 0; i < QESHAPE_NUM_VARIABLES; i++ )
    {
        QString pv = getSubstitutedVariableName(i);
        if( !pv.isEmpty() )
        {
            if( !text.isEmpty() )
                text.append( " " );
            text.append( pv );
        }
    }

    return QVariant( text );
}

//==============================================================================
// Property convenience functions

// variable animations
void QEShape::setAnimation( QEShape::animationOptions animation, const int index )
{
    animations[index] = animation;
    update();
}
QEShape::animationOptions QEShape::getAnimation( const int index )
{
    return animations[index];
}

// scales
void QEShape::setScale( const double scale, const int index )
{
    scales[index] = scale;
    qcaobject::QCaObject* qca = this->getQcaItem( index );
    if( qca )
    {
        qca->resendLastData();
    }
}
double QEShape::getScale( const int index )
{
    return scales[index];
}


// offsets
void QEShape::setOffset( const double offset, const int index )
{
    offsets[index] = offset;
    qcaobject::QCaObject* qca = this->getQcaItem( index );
    if( qca )
    {
        qca->resendLastData();
    }
}
double QEShape::getOffset( const int index )
{
    return offsets[index];
}

// shape
void QEShape::setShape( QEShape::shapeOptions shapeIn )
{
    shape = shapeIn;
    update();
}
QEShape::shapeOptions QEShape::getShape()
{
    return shape;
}


// number of points
void QEShape::setNumPoints( unsigned int numPointsIn )
{
    numPoints = (numPointsIn>POINTS_SIZE)?POINTS_SIZE:numPointsIn;
    update();
}
unsigned int QEShape::getNumPoints()
{
    return numPoints;
}

// Origin translation
void QEShape::setOriginTranslation( QPoint originTranslationIn )
{
    originTranslation = originTranslationIn;
    update();
}
QPoint QEShape::getOriginTranslation()
{
    return originTranslation;
}

// points
void QEShape::setPoint( const QPoint point, const int index  )
{
    points[index] = point;
    update();
}
QPoint QEShape::getPoint( const int index )
{
    return points[index];
}

// colors
void QEShape::setColor( const QColor color, const int index  )
{
    colors[index] = color;
    colorChange( index );
}
QColor QEShape::getColor( const int index )
{
    return colors[index];
}

// draw border
void QEShape::setDrawBorder( bool drawBorderIn )
{
    drawBorder = drawBorderIn;
    update();
}
bool QEShape::getDrawBorder()
{
    return drawBorder;
}

// fill
void QEShape::setFill( bool fillIn )
{
    fill = fillIn;
    update();
}
bool QEShape::getFill()
{
    return fill;
}

// line width
void QEShape::setLineWidth( unsigned int lineWidthIn )
{
    lineWidth = lineWidthIn;
    update();
}
unsigned int QEShape::getLineWidth()
{
    return lineWidth;
}

// start angle
void QEShape::setStartAngle( double startAngleIn )
{
    startAngle = startAngleIn;
    update();
}
double QEShape::getStartAngle()
{
    return startAngle;
}

// rotation
void QEShape::setRotation( double rotationIn )
{
    rotation = rotationIn;
    update();
}
double QEShape::getRotation()
{
    return rotation;
}

// arc length
void QEShape::setArcLength( double arcLengthIn )
{
    arcLength = arcLengthIn;
    update();
}
double QEShape::getArcLength()
{
    return arcLength;
}

// text
void QEShape::setText( QString textIn )
{
    text = textIn;
    update();
}
QString QEShape::getText()
{
    return text;
}

