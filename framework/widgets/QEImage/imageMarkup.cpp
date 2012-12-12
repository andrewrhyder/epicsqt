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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

// Refer to imageMarkup.h for general module description

#include <QPainter>
#include <QDateTime>
#include <QCursor>

#include <imageMarkup.h>
#include <imageContextMenu.h>

// Allowable distance in pixels from object which will still be considered 'over'
#define OVER_TOLERANCE 6
#define HANDLE_SIZE 6


//===========================================================================
// Generic markup base class

// Construct a markup item
// All markup items share the same markup image

markupItem::markupItem( imageMarkup* ownerIn, isOverOptions over, bool interactiveIn, bool reportOnMoveIn )
{
    isOverType = over;
    interactive = interactiveIn;
    reportOnMove = reportOnMoveIn;
    visible = false;
    activeHandle = MARKUP_HANDLE_NONE;
    highlighted = false;
    highlightMargin = 2;
    owner = ownerIn;
    color = QColor( 0, 255, 0 ); // green
}

markupItem::~markupItem()
{
}

bool markupItem::pointIsNear( QPoint p1, QPoint p2 )
{
    return QPoint( p1 - p2 ).manhattanLength() < OVER_TOLERANCE;
}

// Draw in the item, showing it to the user
void markupItem::drawMarkupIn()
{
    QPainter p( owner->markupImage );
    QPen pen( color );
    pen.setStyle( Qt::SolidLine );
    p.setPen( pen );
    drawMarkup( p );
    visible = true;
}

// Draw out the item, removing it from the user's view
void markupItem::drawMarkupOut()
{
    QPainter p( owner->markupImage );
    // Erase the item
    p.setPen( QColor( 0, 0, 0, 255 ) ); // black with fully opaque alpha
    drawMarkup( p );

    // Draw a transparent background
    p.setCompositionMode( QPainter::CompositionMode_Clear );
    drawMarkup( p );
    visible = false;
}


void markupItem::setColor( QColor colorIn )
{
    color = colorIn;
}

QColor markupItem::getColor()
{
    return color;
}

// Erase and item and redraw any items that it was over
// (note, this does not change its status. For example, it is used if hiding an item, but also when moving an item)
void markupItem::erase()
{
    // Clear the item
    drawMarkupOut();

    // Redraw any other visible items that have had any part erased as well
    int n = owner->items.count();
    for( int i = 0; i < n; i++ )
    {
        if( owner->items[i] != this && owner->items[i]->visible && owner->items[i]->area.intersects( this->area ) )
        {
            owner->items[i]->drawMarkupIn();
        }
    }
}


// Scale the geometry related to the viewport
void markupItem::scale( double xScale, double yScale )
{
    // Do type specific scaling
    scaleSpecific( xScale, yScale );

    // Update the generic item area
    setArea();
}

//===========================================================================
// Target markup

markupTarget::markupTarget( imageMarkup* ownerIn, bool interactiveIn, bool reportOnMoveIn ) : markupItem( ownerIn, OVER_LINE, interactiveIn, reportOnMoveIn )
{

}

void markupTarget::drawMarkup( QPainter& p )
{
    QPen pen = p.pen();
    pen.setStyle( Qt::DashLine );
    p.setPen( pen );
    p.drawLine( pos.x(), 0, pos.x(), owner->markupImage->rect().height() );
    p.drawLine( 0, pos.y(), owner->markupImage->rect().width(), pos.y() );
}

void markupTarget::setArea()
{
    area = owner->markupImage->rect();

    owner->markupAreasStale = true;
}

void markupTarget::startDrawing( QPoint posIn )
{
    pos = posIn;
    activeHandle = MARKUP_HANDLE_NONE;
}

void markupTarget::moveTo( QPoint posIn )
{
    pos = posIn;
    setArea();
}

bool markupTarget::isOver( const QPoint point, QCursor* cursor )
{
    *cursor = owner->getCircleCursor();
    activeHandle = MARKUP_HANDLE_NONE;
    return ( abs( point.x() - pos.x() ) <= OVER_TOLERANCE ) || ( abs( point.y() - pos.y() ) <= OVER_TOLERANCE );
}

QPoint markupTarget::origin()
{
    return pos;
}

QPoint markupTarget::getPoint1()
{
    return origin();
}

QPoint markupTarget::getPoint2()
{
    return QPoint();
}

QCursor markupTarget::defaultCursor()
{
    return owner->getTargetCursor();
}

void markupTarget::scaleSpecific( double xScale, double yScale )
{
    pos.setX( pos.x() * xScale );
    pos.setY( pos.y() * yScale );
}


//===========================================================================
// Beam markup

markupBeam::markupBeam( imageMarkup* ownerIn, bool interactiveIn, bool reportOnMoveIn ) : markupItem( ownerIn, OVER_LINE, interactiveIn, reportOnMoveIn )
{
    // Size of cross hair
    armSize = 20;
}

void markupBeam::drawMarkup( QPainter& p )
{
    p.drawLine( pos.x()-1, pos.y()+1, pos.x()-1, pos.y()+armSize );
    p.drawLine( pos.x()+1, pos.y()+1, pos.x()+1, pos.y()+armSize );

    p.drawLine( pos.x()-1, pos.y()-1, pos.x()-1, pos.y()-armSize );
    p.drawLine( pos.x()+1, pos.y()-1, pos.x()+1, pos.y()-armSize );

    p.drawLine( pos.x()+1, pos.y()-1, pos.x()+armSize, pos.y()-1 );
    p.drawLine( pos.x()+1, pos.y()+1, pos.x()+armSize, pos.y()+1 );

    p.drawLine( pos.x()-1, pos.y()-1, pos.x()-armSize, pos.y()-1 );
    p.drawLine( pos.x()-1, pos.y()+1, pos.x()-armSize, pos.y()+1 );
}

void markupBeam::setArea()
{
    area.setLeft  ( pos.x()-armSize );
    area.setRight ( pos.x()+armSize );
    area.setTop   ( pos.y()-armSize );
    area.setBottom( pos.y()+armSize );

    owner->markupAreasStale = true;
}

void markupBeam::startDrawing( QPoint posIn )
{
    pos = posIn;
    activeHandle = MARKUP_HANDLE_NONE;
}

void markupBeam::moveTo( QPoint posIn )
{
    pos = posIn;
    setArea();
}

bool markupBeam::isOver( const QPoint point, QCursor* cursor )
{
    *cursor = owner->getCircleCursor();
    activeHandle = MARKUP_HANDLE_NONE;
    return ((( abs( point.x() - pos.x() ) <= OVER_TOLERANCE ) &&
             ( abs( point.y() - pos.y() ) <= (armSize+OVER_TOLERANCE) )) ||
            (( abs( point.y() - pos.y() ) <= OVER_TOLERANCE ) &&
             ( abs( point.x() - pos.x() ) <= (armSize+OVER_TOLERANCE) )));
}

QPoint markupBeam::origin()
{
    return pos;
}

QPoint markupBeam::getPoint1()
{
    return origin();
}

QPoint markupBeam::getPoint2()
{
    return QPoint();
}

QCursor markupBeam::defaultCursor()
{
    return owner->getTargetCursor();
}

void markupBeam::scaleSpecific( double xScale, double yScale )
{
    pos.setX( pos.x() * xScale );
    pos.setY( pos.y() * yScale );
}

//===========================================================================
// Vertical line markup

markupVLine::markupVLine( imageMarkup* ownerIn, bool interactiveIn, bool reportOnMoveIn ) : markupItem( ownerIn, OVER_LINE, interactiveIn, reportOnMoveIn )
{

}

void markupVLine::drawMarkup( QPainter& p )
{
    p.drawLine( x, 0, x, owner->markupImage->rect().height() );
}

void markupVLine::setArea()
{
    if( highlighted )
    {
        area.setLeft( x - highlightMargin );
        area.setRight( x + highlightMargin );
    }
    else
    {
        area.setLeft( x - HANDLE_SIZE/2 );
        area.setRight( x + HANDLE_SIZE/2 );
    }
    area.setTop( 0 );
    area.setBottom( owner->markupImage->rect().bottom());

    owner->markupAreasStale = true;
}

void markupVLine::startDrawing( QPoint pos )
{
    x = pos.x();
    activeHandle = MARKUP_HANDLE_NONE;
}

void markupVLine::moveTo( QPoint pos )
{
    x = pos.x();
    setArea();
}

bool markupVLine::isOver( const QPoint point, QCursor* cursor )
{
    *cursor = Qt::OpenHandCursor;
    activeHandle = MARKUP_HANDLE_NONE;
    return ( abs( point.x() - x ) <= OVER_TOLERANCE );
}

QPoint markupVLine::origin()
{
    return QPoint( x, 0 );
}

QPoint markupVLine::getPoint1()
{
    return origin();
}

QPoint markupVLine::getPoint2()
{
    return QPoint();
}

QCursor markupVLine::defaultCursor()
{
    return Qt::CrossCursor;
}

void markupVLine::scaleSpecific( double xScale, double )
{
    x *= xScale;
}

//===========================================================================
// Horizontal line markup

markupHLine::markupHLine( imageMarkup* ownerIn, bool interactiveIn, bool reportOnMoveIn ) : markupItem( ownerIn, OVER_LINE, interactiveIn, reportOnMoveIn )
{

}

void markupHLine::drawMarkup( QPainter& p )
{
    p.drawLine( 0, y, owner->markupImage->rect().width(), y );
}

void markupHLine::setArea()
{
    if( highlighted )
    {
        area.setTop( y - highlightMargin );
        area.setBottom( y + highlightMargin );
    }
    else
    {
        area.setTop( y );
        area.setBottom( y );
    }
    area.setLeft( 0 );
    area.setRight( owner->markupImage->rect().right());

    owner->markupAreasStale = true;
}

void markupHLine::startDrawing( QPoint pos )
{
    y = pos.x();
    activeHandle = MARKUP_HANDLE_NONE;
}

void markupHLine::moveTo( QPoint pos )
{
    y = pos.y();
    setArea();
}

bool markupHLine::isOver( const QPoint point, QCursor* cursor )
{
    *cursor = Qt::OpenHandCursor;
    activeHandle = MARKUP_HANDLE_NONE;
    return ( abs( point.y() - y ) <= OVER_TOLERANCE );
}

QPoint markupHLine::origin()
{
    return QPoint( 0, y );
}

QPoint markupHLine::getPoint1()
{
    return origin();
}

QPoint markupHLine::getPoint2()
{
    return QPoint();
}

QCursor markupHLine::defaultCursor()
{
    return Qt::CrossCursor;
}

void markupHLine::scaleSpecific( double, double yScale )
{
    y *= yScale;
}

//===========================================================================
// Profile line markup

markupLine::markupLine( imageMarkup* ownerIn, bool interactiveIn, bool reportOnMoveIn ) : markupItem( ownerIn, OVER_LINE, interactiveIn, reportOnMoveIn )
{

}

void markupLine::drawMarkup( QPainter& p )
{
    p.drawLine( start, end );

    if(( abs(QPoint( end-start ).x()) > (HANDLE_SIZE + 2) ) ||
       ( abs(QPoint( end-start ).y()) > (HANDLE_SIZE + 2) ))
    {
        QRect handle( 0, 0, HANDLE_SIZE, HANDLE_SIZE );
        QPoint halfHandle( HANDLE_SIZE/2, HANDLE_SIZE/2 );

        handle.moveTo( start - halfHandle );
        p.drawRect( handle );

        handle.moveTo( end - halfHandle );
        p.drawRect( handle );

    }
}

void markupLine::setArea()
{
    area.setCoords( std::min( start.x(), end.x() ),
                    std::min( start.y(), end.y() ),
                    std::max( start.x(), end.x() ),
                    std::max( start.y(), end.y()));

    area.adjust( -HANDLE_SIZE, -HANDLE_SIZE, HANDLE_SIZE+1, HANDLE_SIZE+1 );

    if( highlighted )
    {
        area.adjust( -highlightMargin, -highlightMargin, highlightMargin, highlightMargin );
    }

    owner->markupAreasStale = true;

}

void markupLine::startDrawing( QPoint pos )
{
    start = pos;
    activeHandle = MARKUP_HANDLE_END;
}

void markupLine::moveTo( QPoint pos )
{
    switch( activeHandle )
    {
        case MARKUP_HANDLE_NONE:
            { // Constrain scope of endOffset to case
                QPoint endOffset = end - start;
                start = pos - owner->grabOffset ;
                end = start + endOffset;
            }
            break;

        case MARKUP_HANDLE_START:
            start = pos;
            break;

       case MARKUP_HANDLE_END:
            end = pos;
            break;

        default:
            break;
    }
    setArea();
}

bool markupLine::isOver( const QPoint point, QCursor* cursor )
{
    // Not over the line if outside the drawing rectangle more than the tolerance
    QRect tolArea = area;
    tolArea.adjust( -OVER_TOLERANCE, -OVER_TOLERANCE, OVER_TOLERANCE, OVER_TOLERANCE);
    if( !tolArea.contains( point ) )
    {
        return false;
    }

    // Check if the position is over the slope of the line.
    // Although the tolerance should be measured at right angles to the line, an aproximation
    // is to ensure it is within the tolerance vertically if the line is mostly horizontal,
    // or within the tolerance horizontally if the line is mostly vertical.
    // The same algorithm is used for both conditions using arbitarty orientations of A and B.
    // If the line is mostly horizontal, A = X and B = Y. For mostly vertical lines, A = Y and B = X
    int startA;
    int startB;
    int endA;
    int endB;
    int pointA;
    int pointB;

    // Determine what the arbitrary orientations A and B actually are
    if( abs( end.y() - start.y() ) < abs( end.x() - start.x() ) )
    {
        // Mostly horizontal
        startA = start.x();
        startB = start.y();
        endA = end.x();
        endB = end.y();
        pointA = point.x();
        pointB = point.y();
    }
    else
    {
        // Mostly vertical
        startA = start.y();
        startB = start.x();
        endA = end.y();
        endB = end.x();
        pointA = point.y();
        pointB = point.x();
    }

    // Calculate the slope of the line
    double lineSlope = (double)( endB - startB ) / (double)( endA - startA );

    // For the A of the point, determine the B that would place the point on the line
    int expectedB = (int)((double)(pointA - startA) * lineSlope) + startB;

    // Return 'over' if B is close to as calculated
    if( abs( pointB - expectedB ) <= OVER_TOLERANCE )
    {
        if( pointIsNear( point, start ) )
        {
            *cursor = Qt::SizeAllCursor;
            activeHandle = MARKUP_HANDLE_START;
        }
        else if(  pointIsNear( point, end ) )
        {
            *cursor = Qt::SizeAllCursor;
            activeHandle = MARKUP_HANDLE_END;
        }
        else
        {
            *cursor = Qt::OpenHandCursor;
            activeHandle = MARKUP_HANDLE_NONE;
        }
        return true;
    }
    else
    {
        activeHandle = MARKUP_HANDLE_NONE;
        return false;
    }
}

QPoint markupLine::origin()
{
    return start;
}

QPoint markupLine::getPoint1()
{
    return start;
}

QPoint markupLine::getPoint2()
{
    return end;
}

QCursor markupLine::defaultCursor()
{
    return Qt::CrossCursor;
}

void markupLine::scaleSpecific( double xScale, double yScale )
{
    start.setX( start.x() * xScale );
    start.setY( start.y() * yScale );
    end.setX( end.x() * xScale );
    end.setY( end.y() * yScale );
}

//===========================================================================
// Region markup

markupRegion::markupRegion( imageMarkup* ownerIn, bool interactiveIn, bool reportOnMoveIn ) : markupItem( ownerIn, OVER_AREA, interactiveIn, reportOnMoveIn )
{
}

void markupRegion::drawMarkup( QPainter& p )
{
    p.drawRect( rect );

    if(( abs(rect.size().width())  > (HANDLE_SIZE + 2) ) ||
       ( abs(rect.size().height()) > (HANDLE_SIZE + 2) ))
    {
        QRect handle( 0, 0, HANDLE_SIZE, HANDLE_SIZE );
        QPoint halfHandle( HANDLE_SIZE/2, HANDLE_SIZE/2 );

        handle.moveTo( rect.topLeft() - halfHandle );
        p.drawRect( handle );

        handle.moveTo( rect.topRight() - halfHandle );
        p.drawRect( handle );

        handle.moveTo( rect.bottomLeft() - halfHandle );
        p.drawRect( handle );

        handle.moveTo( rect.bottomRight() - halfHandle );
        p.drawRect( handle );

        handle.moveTo( QPoint( rect.left(), rect.top()+rect.height()/2 ) - halfHandle );
        p.drawRect( handle );

        handle.moveTo( QPoint( rect.right(), rect.top()+rect.height()/2 ) - halfHandle );
        p.drawRect( handle );

        handle.moveTo( QPoint( rect.left()+rect.width()/2, rect.top() ) - halfHandle );
        p.drawRect( handle );

        handle.moveTo( QPoint( rect.left()+rect.width()/2, rect.bottom() ) - halfHandle );
        p.drawRect( handle );
    }
}

void markupRegion::setArea()
{
    area = rect;

    // Sanity check - rect should never be non-normallized.
    // Note, drawing a non normalized QRect and a normalized QRect will not draw the same pixels!
    if( rect.width() < 0 || rect.height() < 0 )
    {
        qDebug() << "Error, markupRegion::setArea() rect has negative dimensions" << rect;
    }

    area.adjust( -HANDLE_SIZE, -HANDLE_SIZE, HANDLE_SIZE+1, HANDLE_SIZE+1 );

    if( highlighted )
    {
        area.setLeft(   area.left()   - highlightMargin );
        area.setRight(  area.right()  + highlightMargin );
        area.setTop(    area.top()    - highlightMargin );
        area.setBottom( area.bottom() + highlightMargin );
    }

    owner->markupAreasStale = true;
}

void markupRegion::startDrawing( QPoint pos )
{
    rect.setBottomLeft( pos );
    rect.setTopRight( pos );
    activeHandle = MARKUP_HANDLE_BR;
}

void markupRegion::moveTo( QPoint pos )
{
    // Move the appropriate part of the region, according to which bit the user has grabbed
    switch( activeHandle )
    {
        case MARKUP_HANDLE_NONE: rect.moveTo( pos - owner->grabOffset ); break;
        case MARKUP_HANDLE_TL:   rect.setTopLeft(     pos );     break;
        case MARKUP_HANDLE_TR:   rect.setTopRight(    pos );     break;
        case MARKUP_HANDLE_BL:   rect.setBottomLeft(  pos );     break;
        case MARKUP_HANDLE_BR:   rect.setBottomRight( pos );     break;
        case MARKUP_HANDLE_T:    rect.setTop(         pos.y() ); break;
        case MARKUP_HANDLE_B:    rect.setBottom(      pos.y() ); break;
        case MARKUP_HANDLE_L:    rect.setLeft(        pos.x() ); break;
        case MARKUP_HANDLE_R:    rect.setRight(       pos.x() ); break;

        default: break;
    }

    // If the object is now mirrored, normailze it
    // (if the user has dragged the bottom above the top, or the left to the right of the right)
    bool swapped = false;
    if( rect.width() < 0 )
    {
        int left = rect.right();
        rect.setRight( rect.left() );
        rect.setLeft( left );

        switch( activeHandle )
        {
            case MARKUP_HANDLE_TL: activeHandle = MARKUP_HANDLE_TR; break;
            case MARKUP_HANDLE_TR: activeHandle = MARKUP_HANDLE_TL; break;
            case MARKUP_HANDLE_BL: activeHandle = MARKUP_HANDLE_BR; break;
            case MARKUP_HANDLE_BR: activeHandle = MARKUP_HANDLE_BL; break;
            case MARKUP_HANDLE_L:  activeHandle = MARKUP_HANDLE_R;  break;
            case MARKUP_HANDLE_R:  activeHandle = MARKUP_HANDLE_L;  break;
            default: break;
        }
        swapped = true;
    }

    if( rect.height() < 0 )
    {
        int top = rect.bottom();
        rect.setBottom( rect.top() );
        rect.setTop( top );
        switch( activeHandle )
        {
            case MARKUP_HANDLE_TL: activeHandle = MARKUP_HANDLE_BL; break;
            case MARKUP_HANDLE_TR: activeHandle = MARKUP_HANDLE_BR; break;
            case MARKUP_HANDLE_BL: activeHandle = MARKUP_HANDLE_TL; break;
            case MARKUP_HANDLE_BR: activeHandle = MARKUP_HANDLE_TR; break;
            case MARKUP_HANDLE_B:  activeHandle = MARKUP_HANDLE_T;  break;
            case MARKUP_HANDLE_T:  activeHandle = MARKUP_HANDLE_B;  break;
            default: break;
        }
        swapped = true;
    }


    // Set the cursor according to the bit we are over after manipulation
    if( swapped )
    {
        QCursor cursor = cursorForHandle( activeHandle );
        owner->markupSetCursor( cursor );
    }

    // Update the area the region now occupies
    setArea();
}

// Return the cursor for each handle
// !!! MOVE INTO MARKUP ITEM, EXTEND FOR ALL HANDLES, AND USE FOR ALL MARKUP ITEMS???
QCursor markupRegion::cursorForHandle( markupHandles handle )
{
    switch( handle )
    {
        case MARKUP_HANDLE_TL:   return Qt::SizeFDiagCursor;
        case MARKUP_HANDLE_BL:   return Qt::SizeBDiagCursor;
        case MARKUP_HANDLE_TR:   return Qt::SizeBDiagCursor;
        case MARKUP_HANDLE_BR:   return Qt::SizeFDiagCursor;
        case MARKUP_HANDLE_L:    return Qt::SizeHorCursor;
        case MARKUP_HANDLE_R:    return Qt::SizeHorCursor;
        case MARKUP_HANDLE_T:    return Qt::SizeVerCursor;
        case MARKUP_HANDLE_B:    return Qt::SizeVerCursor;
        case MARKUP_HANDLE_NONE: return Qt::OpenHandCursor;
        default: return Qt::SizeAllCursor;
    }

}

bool markupRegion::isOver( const QPoint point, QCursor* cursor )
{
    // If the point is over the left side, return 'is over' after checking the left handles
    QRect l( rect.topLeft(), QSize( 0, rect.height()) );
    l.adjust( -OVER_TOLERANCE, -OVER_TOLERANCE, OVER_TOLERANCE, OVER_TOLERANCE );
    if( l.contains( point ))
    {
        if( pointIsNear( point, rect.topLeft() ) )
        {
            activeHandle = MARKUP_HANDLE_TL;
        }
        else if( pointIsNear( point, rect.bottomLeft() ) )
        {
            activeHandle = MARKUP_HANDLE_BL;
        }
        else if( pointIsNear( point, QPoint( rect.left(), rect.top()+(rect.height()/2) )))
        {
            activeHandle = MARKUP_HANDLE_L;
        }
        else
        {
            activeHandle = MARKUP_HANDLE_NONE;
        }
        *cursor = cursorForHandle( activeHandle );
        return true;
    }

    // If the point is over the right side, return 'is over' after checking the right handles
    QRect r( rect.topRight(), QSize( 0, rect.height()) );
    r.adjust( -OVER_TOLERANCE, -OVER_TOLERANCE, OVER_TOLERANCE, OVER_TOLERANCE );
    if( r.contains( point ))
    {
        if( pointIsNear( point, rect.topRight() ) )
        {
            activeHandle = MARKUP_HANDLE_TR;
        }
        else if( pointIsNear( point, rect.bottomRight() ) )
        {
            activeHandle = MARKUP_HANDLE_BR;
        }
        else if( pointIsNear( point, QPoint( rect.right(), rect.top()+(rect.height()/2) )))
        {
            activeHandle = MARKUP_HANDLE_R;
        }
        else
        {
            activeHandle = MARKUP_HANDLE_NONE;
        }
        *cursor = cursorForHandle( activeHandle );
        return true;
    }

    // If the point is over the top side, return 'is over' after checking the top handles
    QRect t( rect.topLeft(), QSize( rect.width(), 0) );
    t.adjust( -OVER_TOLERANCE, -OVER_TOLERANCE, OVER_TOLERANCE, OVER_TOLERANCE );
    if( t.contains( point ))
    {
        if( pointIsNear( point, rect.topLeft() ) )
        {
            activeHandle = MARKUP_HANDLE_TL;
        }
        else if( pointIsNear( point, rect.topRight() ) )
        {
            activeHandle = MARKUP_HANDLE_TR;
        }
        else if( pointIsNear( point, QPoint( rect.left()+(rect.width()/2), rect.top() )))
        {
            activeHandle = MARKUP_HANDLE_T;
        }
        else
        {
            activeHandle = MARKUP_HANDLE_NONE;
        }
        *cursor = cursorForHandle( activeHandle );
        return true;
    }

    // If the point is over the bottom side, return 'is over' after checking the bottom handles
    QRect b( rect.bottomLeft(), QSize( rect.width(), 0) );
    b.adjust( -OVER_TOLERANCE, -OVER_TOLERANCE, OVER_TOLERANCE, OVER_TOLERANCE );
    if( b.contains( point ))
    {
        if( pointIsNear( point, rect.bottomLeft() ) )
        {
            activeHandle = MARKUP_HANDLE_BL;
        }
        else if( pointIsNear( point, rect.bottomRight() ) )
        {
            activeHandle = MARKUP_HANDLE_BR;
        }
        else if( pointIsNear( point, QPoint( rect.left()+(rect.width()/2), rect.bottom() )))
        {
            activeHandle = MARKUP_HANDLE_B;
        }
        else
        {
            activeHandle = MARKUP_HANDLE_NONE;
        }
        *cursor = cursorForHandle( activeHandle );
        return true;
    }

    // Not over
    activeHandle = MARKUP_HANDLE_NONE;
    return false;
}

QPoint markupRegion::origin()
{
    return rect.topLeft();
}

QPoint markupRegion::getPoint1()
{
    return rect.topLeft();
}

QPoint markupRegion::getPoint2()
{
    return rect.bottomRight();
}

QCursor markupRegion::defaultCursor()
{
    return Qt::CrossCursor;
}

void markupRegion::scaleSpecific( double xScale, double yScale )
{
    rect.moveTo( rect.x() * xScale, rect.y() * yScale );

    rect.setWidth( rect.width() * xScale );
    rect.setHeight( rect.height() * yScale );
}

//===========================================================================
// Text markup

markupText::markupText( imageMarkup* ownerIn, bool interactiveIn, bool reportOnMoveIn ) : markupItem( ownerIn, OVER_AREA, interactiveIn, reportOnMoveIn )
{

}

void markupText::drawMarkup( QPainter& p )
{
    // Set the area to more than enough.
    // This will be trimmed to the bounding retangle of the text
    rect = QRect( 0, 0, 300, 20 );

    // Draw the text
    p.setFont(QFont("Courier", 12));
    p.drawText( rect, Qt::AlignLeft, text, &rect );

    setArea();
}

void markupText::setText( QString textIn, bool draw )
{
    if( draw )
        erase();

    text = textIn;

    if( draw )
        drawMarkupIn();
}

void markupText::setArea()
{
    area = rect;

    owner->markupAreasStale = true;

}

void markupText::startDrawing( QPoint pos )
{
    rect.setBottomLeft( pos );
    rect.setTopRight( pos + QPoint( 50,30 ) );
    activeHandle = MARKUP_HANDLE_NONE;
}

void markupText::moveTo( QPoint pos )
{
    rect.translate( pos - owner->grabOffset );
    setArea();
}

bool markupText::isOver( const QPoint point, QCursor* cursor )
{
    *cursor = Qt::OpenHandCursor;
    activeHandle = MARKUP_HANDLE_NONE;
    return rect.contains( point );
}

QPoint markupText::origin()
{
    return rect.topLeft();
}

QPoint markupText::getPoint1()
{
    return rect.topLeft();
}

QPoint markupText::getPoint2()
{
    return rect.bottomRight();
}

QCursor markupText::defaultCursor()
{
    return Qt::CrossCursor;
}

void markupText::scaleSpecific( double xScale, double yScale )
{
    rect.moveTo( rect.x() * xScale, rect.y() * yScale );
}

//===========================================================================
// imageMarkup

// Constructor
imageMarkup::imageMarkup()
{
    mode = MARKUP_ID_NONE;
    activeItem = MARKUP_ID_NONE;

    markupImage = new QImage();

    items.resize(MARKUP_ID_COUNT );
    items[MARKUP_ID_H_SLICE]   = new markupHLine(  this, true,  true );
    items[MARKUP_ID_V_SLICE]   = new markupVLine(  this, true,  true );
    items[MARKUP_ID_LINE]      = new markupLine(   this, true,  true );
    items[MARKUP_ID_REGION]    = new markupRegion( this, true,  true );
    items[MARKUP_ID_TARGET]    = new markupTarget( this, true,  true );
    items[MARKUP_ID_BEAM]      = new markupBeam(   this, true,  true );
    items[MARKUP_ID_TIMESTAMP] = new markupText(   this, false, false );

    markupAreasStale = true;

    // Create circle cursoe used for target and beam
    QPixmap circlePixmap = QPixmap( ":/qe/image/circleCursor.png" );
    circleCursor = QCursor( circlePixmap );

    // Create target cursor used for target and beam
    QPixmap targetPixmap = QPixmap( ":/qe/image/targetCursor.png" );
    targetCursor = QCursor( targetPixmap );

}

// Destructor
imageMarkup::~imageMarkup()
{
}

// Get the current markup mode - (what is the user doing? selecting an area? drawing a line?)
imageMarkup::markupIds imageMarkup::getMode()
{
    return mode;
}

// Set the current markup mode - (what is the user doing? selecting an area? drawing a line?)
void imageMarkup::setMode( markupIds modeIn )
{
    mode = modeIn;
}

//===========================================================================

void imageMarkup::setMarkupTime( QCaDateTime& time )
{
    if( showTime )
    {
        markupText* timeDate = (markupText*)items[MARKUP_ID_TIMESTAMP];
        timeDate->setText( time.text().left( 23 ), showTime );

        QVector<QRect> changedAreas;
        changedAreas.append( timeDate->area );
        markupChange( *markupImage, changedAreas );
    }
}

// Set if time should be shown.
// time is a markup that the user doesn;t interact with. It is just displayed, or not
void imageMarkup::setShowTime( bool showTimeIn )
{
    showTime = showTimeIn;

    // Do nothing more (no need to add or remove time) if no markup image yet
    if( markupImage->isNull() )
        return;

    markupText* timeDate = (markupText*)items[MARKUP_ID_TIMESTAMP];
    if( showTime )
        timeDate->drawMarkupIn();
    else
        timeDate->drawMarkupOut();

    QVector<QRect> changedAreas;
    changedAreas.append( timeDate->area );
    markupChange( *markupImage, changedAreas );
}

// Get if the time is currently being displayed
bool imageMarkup::getShowTime()
{
    return showTime;
}

//===========================================================================

// User pressed a mouse button
bool imageMarkup::markupMousePressEvent(QMouseEvent *event, bool panning)
{
    // Only act on left mouse button press
    if( !(event->buttons()&Qt::LeftButton ))
        return false;

    // Determine if the user clicked over an interactive, visible item,
    // and if so, make the first item found the active item
    setActiveItem( event->pos() );

    // If in panning mode, then we will not take over the event unless we are over an active item
    // Note, buttonDown is cleared so there is no context of any sort of markup action in progress
    if( panning && activeItem == MARKUP_ID_NONE )
    {
        buttonDown = false;
        return false;
    }

    // Keep track of button state
    buttonDown = true;

    // If not over an item, start creating a new item
    // move the appropriate item to the point clicked on
    if( activeItem == MARKUP_ID_NONE )
    {
        bool pointAndClick = true;
        switch( mode )
        {
            default:
            case MARKUP_ID_NONE:
                break;

            case MARKUP_ID_H_SLICE:
            case MARKUP_ID_V_SLICE:
            case MARKUP_ID_TARGET:
            case MARKUP_ID_BEAM:
                activeItem = mode;
                pointAndClick = true;
                break;

            case MARKUP_ID_LINE:
            case MARKUP_ID_REGION:
                activeItem = mode;
                pointAndClick = false;
                break;
        }
        if( activeItem != MARKUP_ID_NONE )
        {
            // Some items are point-and-click items. They don't require the user to drag to select where the item is.
            // Typical point-and-click items are vertical lines and horizontal lines (traversing the entire window)
            // Other item are point-press-drag-release items as they require the user to drag to select where the item is.
            // A typical point-press-drag-release item is an area or a line.
            //
            // For a point-and-click item, just redraw it where the user clicks
            if( pointAndClick )
            {
                redrawActiveItemHere( event->pos() );
            }
            // For a point-press-drag-release, erase it if visible,
            // and start the process of draging from the current position
            else
            {
                if( items[activeItem]->visible )
                {
                    items[activeItem]->erase();
                    QVector<QRect> changedAreas;
                    changedAreas.append( items[activeItem]->area );
                    markupChange( *markupImage, changedAreas );
                }
                items[activeItem]->startDrawing( event->pos() );

                // Set the cursor according to the bit we are over after creation
                QCursor cursor;
                if( items[activeItem]->isOver( event->pos(), &cursor ) )
                {
                    markupSetCursor( cursor );
                }
            }
        }
    }

    // Return indicating the event was dealt with
    return true;
}

// Manage the markups as the mouse moves
bool imageMarkup::markupMouseMoveEvent( QMouseEvent* event, bool panning )
{
    // If panning, and we havn't noted a button down for the purposes of image markup, then don't take over this move event
    // (If buttonDown is true then we have already appropriated the button down/move/release for markup purposes)
    if( panning && !buttonDown )
    {
        return false;
    }

    // If the user has the button down, redraw the item in its new position or shape.
    if( buttonDown )
    {
        redrawActiveItemHere( event->pos() );
    }

    // If no button is down, ensure the cursor reflects what it is over
    else
    {
        // Set cursor
        QCursor cursor = getDefaultMarkupCursor();
        int n = items.count();
        for( int i = 0; i < n; i++ )
        {
            QCursor specificCursor;
            if( items[i]->interactive && items[i]->visible && items[i]->isOver( event->pos(), &specificCursor ) )
            {
                cursor = specificCursor;
                break;
            }
        }
        markupSetCursor( cursor );
    }

    // If there is an active item and action is required on move, then report the move
    if( activeItem != MARKUP_ID_NONE && items[activeItem]->reportOnMove )
    {
        markupAction( getActionMode(), false, items[activeItem]->getPoint1(), items[activeItem]->getPoint2() );
    }

    // Return indicating the event was appropriated for markup purposes
    return true;
}

// Return the mode according to the active item.
// Note, this is not the mode as set by setMode(). The mode as set by setMode()
// is what happens when a user initiates action in a part of the display not
// occupied by a markup.
// This mode is related to an existing markup being manipulated.
// For example, if the current mode set by setMode() is MARKUP_MODE_AREA
// (select and area) but the user has draged the profile line the mode
// returned by this method is MARKUP_MODE_LINE
imageMarkup::markupIds imageMarkup::getActionMode()
{
    switch( activeItem )
    {
        case MARKUP_ID_H_SLICE:
        case MARKUP_ID_V_SLICE:
        case MARKUP_ID_LINE:
        case MARKUP_ID_REGION:
        case MARKUP_ID_TARGET:
        case MARKUP_ID_BEAM:
            return activeItem;

        default:
            return MARKUP_ID_NONE;
    }
}

QCursor imageMarkup::getDefaultMarkupCursor()
{
    if( mode < MARKUP_ID_COUNT )
    {
        return items[mode]->defaultCursor();
    }
    else
    {
        return Qt::CrossCursor;
    }
}

bool imageMarkup::markupMouseReleaseEvent ( QMouseEvent*, bool panning  )
{
    // If panning, and we havn't noted a button down for the purposes of image markup, then don't take over this release event
    // (If buttonDown is true then we have already appropriated the button down/move/release for markup purposes)
    if( panning && !buttonDown )
    {
        return false;
    }

    // If there is an active item, take action
    if( activeItem != MARKUP_ID_NONE )
    {
        markupAction( getActionMode(), false, items[activeItem]->getPoint1(), items[activeItem]->getPoint2() );
    }

    // Flag there is no longer an active item
    activeItem = MARKUP_ID_NONE;
    buttonDown = false;

    // Return indicating the event was appropriated for markup purposes
    return true;
}

// The active item has moved to a new position. Redraw it.
void imageMarkup::redrawActiveItemHere( QPoint pos )
{
    // Do nothing if no active item
    if( activeItem == MARKUP_ID_NONE )
        return;

    // Area to update
    // !!! This is currently just one rectangle that encloses the erased and redrawn object.
    // !!! It could (should?) be a region that includes a single rect for mostly hoz and vert lines,
    // !!! four rects for the four sides of an area, and a number of rectangles that efficiently
    // !!! allows redrawing of diagonal lines
    QVector<QRect> changedAreas;

    // Erase if visible, move, then redraw the item
    if( items[activeItem]->visible )
    {
        changedAreas.append( items[activeItem]->area );
        items[activeItem]->erase();
    }
    items[activeItem]->moveTo( pos );
    items[activeItem]->drawMarkupIn();

    // Extend the changed area to include the item's new area and notify markups require redrawing
    // !!! if the two areas overlap by much, perhaps smarter to join the two into one, or generate the required four?
    changedAreas.append( items[activeItem]->area );
    markupChange( *markupImage, changedAreas );
}

// The viewport size has changed
void imageMarkup::markupResize( QSize newSize )
{
    // Determine scaling that will be applied
    // Note, X and Y factors will be close, but may not be exactly the same
    bool rescale;
    double xScale;
    double yScale;
    if( markupImage->isNull() )
    {
        rescale = false;
    }
    else
    {
        rescale = true;
        xScale = (double)(newSize.width())  / (double)(markupImage->size().width());
        yScale = (double)(newSize.height()) / (double)(markupImage->size().height());
    }

    // If the markup image is not the right size, create one that is
    if( markupImage->size() != newSize )
    {
        // Delete the old one (may be the initial empty image)
        // Replace it with a new one, and fill it with a completely transparent background
        delete markupImage;
        markupImage = new QImage( newSize, QImage::Format_ARGB32 );
        markupImage->fill( 0 );
    }

    // Rescale and redraw any visible markups
    // Also act on all visible markups. This is required as the new viewport coordinates will need to be retranslated according to the new viewport size.
    // Note, the results will often be identical, but not always, as the new viewport coordinates may not translate to the same pixels in the original image.
    int n = items.count();
    for( int i = 0; i < n; i ++ )
    {
        // If rescaling is possible (if we have a previous image), then rescale
        if( rescale )
        {
            items[i]->scale( xScale, yScale );
        }
        // If the markup is being displayed, redraw it, and act on it's 'new' position
        if( items[i]->visible )
        {
            items[i]->drawMarkupIn();
            markupAction( (markupIds)i, false, items[i]->getPoint1(), items[i]->getPoint2() );
        }
    }

    // Notify the change
    markupChange( *markupImage, getMarkupAreas() );
}

QVector<QRect>& imageMarkup::getMarkupAreas()
{
    if( markupAreasStale )
    {
        markupAreas.clear();
        int n = items.count();
        for( int i = 0; i < n; i ++ )
        {
            if( items[i]->visible )
            {
                markupAreas.append( items[i]->area );
            }
        }
        markupAreasStale = false;
    }
    return markupAreas;
}

bool imageMarkup::anyVisibleMarkups()
{
    int n = items.count();
    for( int i = 0; i < n; i ++ )
    {
        if( items[i]->visible )
        {
            return true;
        }
    }
    return false;
}

// Set the color for a given mode.
// For example, please draw area selection rectangles in green.
void imageMarkup::setMarkupColor( markupIds mode, QColor markupColorIn )
{
    // Do nothing if mode is invalid
    if( mode < 0 || mode >= MARKUP_ID_NONE )
    {
        return ;
    }

    // Save the new markup color
    items[mode]->setColor( markupColorIn );

    // Do nothing (no need to change drawn colors) if no markup image yet
    if( markupImage->isNull() )
        return;

    // If the item is visible, redraw it in the new color
    QVector<QRect> changedAreas;
    if( items[mode]->visible )
    {
        items[mode]->drawMarkupIn();
        changedAreas.append( items[mode]->area );
        markupChange( *markupImage, changedAreas );
    }

}

QColor imageMarkup::getMarkupColor( markupIds mode )
{
    // Return a valid deault color mode is invalid
    if( mode < 0 || mode >= MARKUP_ID_NONE )
    {
        return QColor( 127, 127, 127 );
    }

    // Return the markup color
    return items[mode]->color;
}

QCursor imageMarkup::getCircleCursor()
{
    return circleCursor;
}

QCursor imageMarkup::getTargetCursor()
{
    return targetCursor;
}

// Show the markup context menu if required.
// Do nothing and return false if nothing to do , for example, the position is not over a markup item
// If required, present the menu, act on the user selectino, then return true
//
// This method currently populates a imageContextMenu with one 'clear' option.
// Refer to  QEImage::showContextMenu() to see how imageContextMenu can be populated with checkable, and non checkable items, and sub menus
bool imageMarkup::showMarkupMenu( const QPoint& pos, const QPoint& globalPos )
{
    // Determine if the user clicked over an interactive, visible item,
    // and if so, make the first item found the active item
    setActiveItem( pos );

    // If not over an item, do nothing.
    if( activeItem == MARKUP_ID_NONE )
        return false;

    imageContextMenu menu;

    //                      Title                            checkable  checked                 option
    menu.addMenuItem(       "Clear",                         false,     false,                  imageContextMenu::ICM_CLEAR_MARKUP             );


    // Present the menu
    imageContextMenu::imageContextMenuOptions option;
    bool checked;
    menu.getContextMenuOption( globalPos, &option, &checked );

    // Act on the menu selection
    switch( option )
    {
        default:
        case imageContextMenu::ICM_NONE:
            break;

        case imageContextMenu::ICM_CLEAR_MARKUP:
            items[activeItem]->erase();
            QVector<QRect> changedAreas;
            changedAreas.append( items[activeItem]->area );
            markupChange( *markupImage, changedAreas );

            markupAction( activeItem, true, QPoint(), QPoint() );

            activeItem = MARKUP_ID_NONE;
            break;
    }

    // Indicate markup menu has been presented
    return true;
}

// Determine if the user clicked over an interactive, visible item,
// and if so, make the first item found the active item
void imageMarkup::setActiveItem( const QPoint& pos )
{
    activeItem = MARKUP_ID_NONE;
    int n = items.count();
    for( int i = 0; i < n; i++ )
    {
        QCursor cursor;
        if( items[i]->interactive && items[i]->visible && items[i]->isOver( pos, &cursor ) )
        {
            activeItem = (markupIds)i;
            grabOffset = pos - items[i]->origin();
            break;
        }
    }
}
