/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/widgets/src/QCaShape.cpp $
 * $Revision: #8 $
 * $DateTime: 2009/08/03 09:51:42 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaShape
  \version $Revision: #8 $
  \date $DateTime: 2009/08/03 09:51:42 $
  \author andrew.rhyder
  \brief CA Shape Widget.
 */

/* Copyright (c) 2009 Australian Synchrotron
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * Licence as published by the Free Software Foundation; either
 * version 2.1 of the Licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public Licence for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * Licence along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Contact details:
 * andrew.rhyder@synchrotron.org.au
 * 800 Blackburn Road, Clayton, Victoria 3168, Australia.
 *
 */

#include <QCaShape.h>
#include <QCaDateTime.h>
#include <QtGui>
#include <QDebug>

/*!
    Create without a known variable. Just manage parental hirarchy.
*/
QCaShape::QCaShape( QWidget *parent ) : QWidget( parent ), QCaWidget() {
    setup();
}

/*!
    Create with a known variable. Subscription occurs immedietly.
*/
QCaShape::QCaShape( const QString &variableNameIn, QWidget *parent ) : QWidget( parent ), QCaWidget() {
    /// Call common setup code.
    setup();

    /// Use the variable name as the first variable
    setVariableName( variableNameIn, 0 );
}

/*!
    Common setup code.
*/
void QCaShape::setup() {
    // Set up data
    // Set the number of variables equal to the base VariableNameManager is to manage
    setNumVariables( NUM_VARIABLES );

    // Set up default properties
    subscribeProperty = true;

    shapeProperty = Rect;
    setAutoFillBackground(false);

    antialiased = false;
    pixmap.load(":/images/qt-logo.png");

    enabledProperty = true;

    setBackgroundRole(QPalette::NoRole);

    originTranslationProperty = QPoint( 0, 0 );
    scaledOriginTranslation = originTranslationProperty;

    pointsProperty[0] = QPoint( 0, 0 );
    pointsProperty[1] = QPoint( width()-1, this->height()-1 );
    unsigned int i;
    for( i = 2; i < POINTS_PROPERTY_SIZE; i++ )
        pointsProperty[i] = QPoint( 0, 0 );

    numPointsProperty = 2;

    colorsProperty[0] = QColor( 255,0,0);
    colorsProperty[1] = QColor( 0,255,0);
    colorsProperty[2] = QColor( 0,0,255);
    colorsProperty[3] = QColor( 255,255,255);
    colorsProperty[4] = QColor( 0,0,0);

    for( i = 0; i < SCALES_PROPERTY_SIZE; i++ )
        scalesProperty[i] = 1.0;


    for( i = 0; i < OFFSETS_PROPERTY_SIZE; i++ )
        offsetsProperty[i] = 0.0;

    animationProperty[0] = Width;
    animationProperty[1] = Height;
    animationProperty[2] = X;
    animationProperty[3] = Y;
    animationProperty[4] = Transperency;
    animationProperty[5] = ColourHue;

    painterCurrentScaleX = 1.0;
    painterCurrentScaleY = 1.0;
    painterCurrentTranslateX = 0;
    painterCurrentTranslateY = 0;
    rotationProperty = 0.0;

    drawBorderProperty = 1;

    lineWidthProperty = 1;
    pen.setWidth( lineWidthProperty );

    fillProperty = true;
    borderProperty = true;

    brush.setStyle( Qt::SolidPattern );
    currentColor = 0;
    brush.setColor( colorsProperty[currentColor] );

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    // Use widget signals
    // --Currently none--
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a shape a QCaObject that streams integers is required.
*/
void QCaShape::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QCaInteger
    setQcaItem( new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex ), variableIndex );
}

/*!
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
*/
void QCaShape::establishConnection( unsigned int variableIndex ) {
    if( createConnection( variableIndex ) == true ) {
//        setValue( 0, QCaAlarmInfo(), QCaDateTime(), variableIndex );  //??? should this be moved up before the create connection? if create connection fails, then any previous data should be cleared? do for all types if required
        QObject::connect( getQcaItem( variableIndex ),
                          SIGNAL( integerChanged( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setValue( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( getQcaItem( variableIndex ), SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}

/*!
    Update the tool tip as requested by QCaWidget.
*/
void QCaShape::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
 */
void QCaShape::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    /// If connected, enable the widget if the QCa enabled property is true
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );

        if( enabledProperty )
            QWidget::setEnabled( true );
    }

    /// If disconnected always disable the widget.
    else
    {
        isConnected = false;
        updateToolTipConnection( isConnected );

        QWidget::setEnabled( false );
    }
}

/*!
    Use a data update to alter the shape's attributes.
    The name setValue is less appropriate for this QCa widget than for others
    such as QCaLabel where setValue() sets the value displayed. For this
    widget setting the value means modifying one attribute such as position or
    color.
*/
void QCaShape::setValue( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex ) {
    /// Scale the data.
    /// For example, a flow of 0 to 10 l/m may adjust a shape size 0 to 200 pixels
    double scaledValue = (double)(value)*scalesProperty[variableIndex] + offsetsProperty[variableIndex];

    /// Animate the object.
    /// Apply the data to the appropriate attribute of the shape
    switch( animationProperty[variableIndex] ) {
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
            if( scaledValue > 0.0 ) {
                if ( scaledValue < 255 ) {
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
            rotationProperty = scaledValue;
            break;
        }
        case ColourHue :
        {
            int newHue = 0;
            if( scaledValue > 0.0 ) {
                if( scaledValue < 255 ) {
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
            qDebug() << "hue " << h;
            color.setHsv( h, s, v, a );
            brush.setColor( color );
            break;
        }
        case ColourSaturation :
        {
            int newSaturation = 0;
            if( scaledValue > 0.0 ) {
                if( scaledValue < 255 ) {
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
            qDebug() << "saturation " << s;
            color.setHsv( h, s, v, a );
            brush.setColor( color );
            break;
        }
        case ColourValue :
        {
            int newValue = 0;
            if( scaledValue > 0.0 ) {
                if( scaledValue < 255 ) {
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
            qDebug() << "value " << v;
            color.setHsv( h, s, v, a );
            brush.setColor( color );
            break;
        }
        case ColourIndex :
        {
            if( scaledValue > 0.0 ) {
                if ( scaledValue < COLORS_PROPERTY_SIZE ) {
                    currentColor = (unsigned int)scaledValue;
                } else {
                    currentColor = COLORS_PROPERTY_SIZE-1;
                }
            }
            brush.setColor( colorsProperty[currentColor] );
            break;
        }
        case Penwidth :
        {
            pen.setWidth( (int)scaledValue );
            qDebug() << "pen width " << (int)scaledValue;
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
void QCaShape::paintEvent(QPaintEvent * /* event */) {
    QPainter painter( this );

    /// Set up the pen and brush (color, thickness, etc)
    pen.setWidth( lineWidthProperty );
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
    viewportRect.moveLeft( originTranslationProperty.x() );
    viewportRect.moveTop( originTranslationProperty.y() );
    painter.setWindow( viewportRect );

    /// Apply the current translation, scaling and rotation
    painter.translate( painterCurrentTranslateX+0.5, painterCurrentTranslateY+0.5 );
    painter.scale( painterCurrentScaleX, painterCurrentScaleY );
    painter.rotate( rotationProperty );

    /// Draw the shape
    switch( shapeProperty ) {
        case Line :
            painter.drawLine( pointsProperty[0], pointsProperty[1] );
            break;
        case Points :
            painter.drawPoints(pointsProperty, numPointsProperty);
            break;
        case Polyline :
            painter.drawPolyline(pointsProperty, numPointsProperty);
            break;
        case Polygon :
            painter.drawPolygon(pointsProperty, numPointsProperty);
            break;
        case Rect :
            if( !drawBorderProperty )
                painter.setPen( Qt::NoPen );
            painter.drawRect( pointsProperty[0].x(), pointsProperty[0].y(), pointsProperty[1].x(), pointsProperty[1].y() );
            break;
        case RoundedRect :
            if( !drawBorderProperty )
                painter.setPen( Qt::NoPen );
            painter.drawRoundedRect(pointsProperty[0].x(), pointsProperty[0].y(), pointsProperty[1].x(), pointsProperty[1].y(), 25, 25, Qt::RelativeSize);
            break;
        case Ellipse :
            if( !drawBorderProperty )
                painter.setPen( Qt::NoPen );
            painter.drawEllipse(pointsProperty[0].x(), pointsProperty[0].y(), pointsProperty[1].x(), pointsProperty[1].y());
            break;
        case Arc :
            painter.drawArc(pointsProperty[0].x(), pointsProperty[0].y(), pointsProperty[1].x(), pointsProperty[1].y(), startAngleProperty, arcLengthProperty );
            break;
        case Chord :
            if( !drawBorderProperty )
                painter.setPen( Qt::NoPen );
            painter.drawChord(pointsProperty[0].x(), pointsProperty[0].y(), pointsProperty[1].x(), pointsProperty[1].y(), startAngleProperty, arcLengthProperty );
            break;
        case Pie :
            if( !drawBorderProperty )
                painter.setPen( Qt::NoPen );
            painter.drawPie(pointsProperty[0].x(), pointsProperty[0].y(), pointsProperty[1].x(), pointsProperty[1].y(), startAngleProperty, arcLengthProperty );
            break;
        case Path :
        {
            QPainterPath path;
            if( numPointsProperty > 1 ) {
                path.moveTo(pointsProperty[0]);
                for( unsigned int i = 1; i < numPointsProperty; i++ ) {
                    path.lineTo( pointsProperty[i] );
                }
                painter.drawPath(path);
            }
            break;
        }
        case Text :
        {
            QRectF rect( pointsProperty[0], pointsProperty[1] );
            painter.drawText( rect, Qt::AlignCenter, textProperty );
            break;
        }
        case Pixmap :
            painter.drawPixmap(10, 10, pixmap);
            break;
    }
}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaShape::isEnabled() const
{
    /// Return what the state of widget would be if connected.
    return enabledProperty;
}

/*!
   Override the default widget setEnabled slot to allow alarm states to override current enabled state
 */
void QCaShape::setEnabled( bool state )
{
    /// Note the new 'enabled' state
    enabledProperty = state;

    /// Set the enabled state of the widget only if connected
    if( isConnected )
        QWidget::setEnabled( enabledProperty );
}

/*!
   Reset the brush color if the color the brush is using is changing
 */
void QCaShape::colorChange( unsigned int index )
{
    // Sanity check. Ignore out of range color index
    if( index >= COLORS_PROPERTY_SIZE )
        return;

    // Update the brush and redraw the shape if the color being changed is in use
    if( currentColor == index )
    {
        brush.setColor( colorsProperty[currentColor] );
        update();
    }
}
