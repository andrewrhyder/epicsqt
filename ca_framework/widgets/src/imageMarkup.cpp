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

/*!
  This class manages markups over an image, such as region of interest, line, graticule, etc, and user interaction with same.

*/

#include <QPainter>
#include <QDateTime>

#include <imageMarkup.h>

// Allowable distance in pixels from object which will still be considered 'over'
#define OVER_TOLERANCE 6
#define HANDLE_SIZE 6


//===========================================================================
// Generic markup base class

// Construct a markup item
// All markup items share the same markup image

markupItem::markupItem( imageMarkup* ownerIn, isOverOptions over, bool interactiveIn )
{
    isOverType = over;
    interactive = interactiveIn;
    visible = false;
    markupColor = Qt::blue;
    activeHandle = MARKUP_HANDLE_NONE;
    highlighted = false;
    highlightMargin = 2;
    owner = ownerIn;
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
    p.setPen( markupColor);
    drawMarkup( p );
    visible = true;
}

// Draw out the item, removing it from the user's view
void markupItem::drawMarkupOut()
{
    QPainter p( owner->markupImage );
    p.setCompositionMode( QPainter::CompositionMode_Clear );
    drawMarkup( p );
    visible = false;
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
            drawMarkupIn();
        }
    }
}

//===========================================================================
// Vertical line markup

markupVLine::markupVLine( imageMarkup* ownerIn, bool interactiveIn ) : markupItem( ownerIn, OVER_LINE, interactiveIn )
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
        area.setRight( x - HANDLE_SIZE/2 );
    }
    area.setTop( 0 );
    area.setBottom( owner->markupImage->rect().bottom());
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

bool markupVLine::isOver( QPoint point )
{
    return ( abs( point.x() - x ) <= OVER_TOLERANCE );
}

QPoint markupVLine::origin()
{
    return QPoint( x, 0 );
}

//===========================================================================
// Horizontal line markup

markupHLine::markupHLine( imageMarkup* ownerIn, bool interactiveIn ) : markupItem( ownerIn, OVER_LINE, interactiveIn )
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

bool markupHLine::isOver( QPoint point )
{
    return ( abs( point.y() - y ) <= OVER_TOLERANCE );
}

QPoint markupHLine::origin()
{
    return QPoint( 0, y );
}

//===========================================================================
// Profile line markup

markupLine::markupLine( imageMarkup* ownerIn, bool interactiveIn ) : markupItem( ownerIn, OVER_LINE, interactiveIn )
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

bool markupLine::isOver( QPoint point )
{
    qDebug() << "markupLine::isOver()" << point << area << start << end;

    // Not over the line if outside the drawing rectangle more than the tolerance
    QRect tolArea = area;
    qDebug() << tolArea;
    tolArea.adjust( -OVER_TOLERANCE, -OVER_TOLERANCE, OVER_TOLERANCE, OVER_TOLERANCE);
    qDebug() << tolArea;
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
    if( ( end.y() - start.y() ) < ( end.x() - start.x() ) )
    {
        startA = start.x();
        startB = start.y();
        endA = end.x();
        endB = end.y();
        pointA = point.x();
        pointB = point.y();
    }
    else
    {
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
            activeHandle = MARKUP_HANDLE_START;
        }
        else if(  pointIsNear( point, end ) )
        {
            activeHandle = MARKUP_HANDLE_END;
        }
        else
        {
            activeHandle = MARKUP_HANDLE_NONE;
        }
        qDebug() << activeHandle;
        return true;
    }
    else
    {
        return false;
    }
}

QPoint markupLine::origin()
{
    return start;
}

//===========================================================================
// Region markup

markupRegion::markupRegion( imageMarkup* ownerIn, bool interactiveIn ) : markupItem( ownerIn, OVER_AREA, interactiveIn )
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
//    qDebug() << "drawMarkup()" << rect;
}

void markupRegion::setArea()
{
    int x1, y1, x2, y2;
    rect.getCoords( &x1, &y1, &x2, &y2 );
    area.setLeft( std::min( x1, x2 ) );
    area.setRight( std::max( x1, x2 ) );
    area.setTop( std::min( y1, y2 ) );
    area.setBottom( std::max( y1, y2 ) );

    qDebug() << x1 << y1 << x2 << y2;

    area.adjust( -HANDLE_SIZE, -HANDLE_SIZE, HANDLE_SIZE+1, HANDLE_SIZE+1 );

    qDebug() << "markupRegion::setArea()" << area;
    if( highlighted )
    {
        area.setLeft(   area.left()   - highlightMargin );
        area.setRight(  area.right()  + highlightMargin );
        area.setTop(    area.top()    - highlightMargin );
        area.setBottom( area.bottom() + highlightMargin );
    }
//    qDebug() << "setArea()" << area;
}

void markupRegion::startDrawing( QPoint pos )
{
    rect.setBottomLeft( pos );
    rect.setTopRight( pos );
    activeHandle = MARKUP_HANDLE_TR;

//    qDebug() << "startDrawing()" << rect;
}

void markupRegion::moveTo( QPoint pos )
{
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

        default:
            break;
    }

    setArea();

//    qDebug() << "moveTo()" << rect << pos;
}

bool markupRegion::isOver( QPoint point )
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
        return true;
    }

    // Not over
    return false;
}

QPoint markupRegion::origin()
{
//    qDebug() << "origin()" << rect.topLeft();

    return rect.topLeft();
}

//===========================================================================
// Text markup

markupText::markupText( imageMarkup* ownerIn, bool interactiveIn ) : markupItem( ownerIn, OVER_AREA, interactiveIn )
{

}

void markupText::drawMarkup( QPainter& p )
{
    // Set the area to more than enough.
    // This will be trimmed to the bounding retangle of the text
    rect = QRect(10, 10, 200, 400 );

    // Draw the text
    p.setFont(QFont("Courier", 9));
    p.drawText( rect, Qt::AlignLeft, text, &rect );
}

void markupText::setText( QString textIn )
{
    text = textIn;
}

void markupText::setArea()
{
    area = rect;
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

bool markupText::isOver( QPoint point )
{
    return rect.contains( point );
}

QPoint markupText::origin()
{
    return rect.topLeft();
}

//===========================================================================
// imageMarkup

// Constructor
imageMarkup::imageMarkup()
{
    interaction = WAITING;
    mode = MARKUP_MODE_NONE;
    activeItem = NULL;

    markupImage = new QImage();

    // Create the markup items with a specific handle for each
    lineHoz      = new markupHLine(  this, true );
    lineVert     = new markupVLine(  this, true );
    lineProfile  = new markupLine(   this, true );
    region       = new markupRegion( this, true );
    timeDate     = new markupText(   this, false );

    // Add the items to a list of items so they can be iterated
    items.append( lineHoz );
    items.append( lineVert );
    items.append( lineProfile );
    items.append( region );
    items.append( timeDate );

    // !!!temp testing
    timeDate->setText( "hi there" );
    timeDate->visible = true;
    timeDate->drawMarkupIn();
}

// Destructor
imageMarkup::~imageMarkup()
{
}

// Set the current markup mode - (what is the user doing? selecting an area? drawing a line?)
void imageMarkup::setMode( markupModes modeIn )
{
    mode = modeIn;
}

void imageMarkup::setMarkupTime()
{
    timeDate->setText( QDateTime().currentDateTime().toString("yyyy/MM/dd - hh:mm:ss" ));
}

void imageMarkup::setShowTime( bool visibleIn )
{
    timeDate->visible = visibleIn;
}

bool imageMarkup::getShowTime()
{
    return timeDate->visible;
}

// User pressed a mouse button
void imageMarkup::markupMousePressEvent(QMouseEvent *event)
{
//    qDebug() << "imageMarkup::markupMousePressEvent" << event;

    // Determine if the user clicked over an interactive, visible item,
    // and if so, make the first item found the active item
    activeItem = NULL;
    int n = items.count();
    for( int i = 0; i < n; i++ )
    {
        if( items[i]->interactive && items[i]->visible && items[i]->isOver( event->pos() ) )
        {
            activeItem = items[i];
            grabOffset = event->pos() - activeItem->origin();
            break;
        }
    }

    // If not over an item, start creating a new item
    // move the appropriate item to the point clicked on
    if( !activeItem )
    {
        bool pointAndClick = true;
        switch( mode )
        {
            case MARKUP_MODE_NONE:                                                    break;
            case MARKUP_MODE_H_LINE: activeItem = lineHoz;                            break;
            case MARKUP_MODE_V_LINE: activeItem = lineVert;                           break;
            case MARKUP_MODE_LINE:   activeItem = lineProfile; pointAndClick = false; break;
            case MARKUP_MODE_AREA:   activeItem = region;      pointAndClick = false; break;
        }
        if( activeItem )
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
                if( activeItem->visible )
                {
                    activeItem->erase();
                }
                activeItem->startDrawing( event->pos() );
            }
        }
    }
}

void imageMarkup::markupMouseMoveEvent( QMouseEvent* event )
{
//    qDebug() << "imageMarkup::markupMouseMoveEvent" << event;

    redrawActiveItemHere( event->pos() );

}

void imageMarkup::markupMouseReleaseEvent ( QMouseEvent* )// event )
{
//    qDebug() << "imageMarkup::markupMouseReleaseEvent" << event;

    activeItem = NULL;

    // Do nothing if no active item
//    if( !activeItem )
//        return;

//    // Erase the active item, make it no longer the active item, the redraw it
//    activeItem->erase();
//    markupItem* item = activeItem;
//    activeItem = NULL;
//    item->drawMarkupIn();

//    // Draw all
//    int n = items.count();
//    for( int i = 0; i < n; i++ )
//    {
//        if( items[i]->visible && items[i]->isOver( event->pos() ) )
//        {
//            activeItem = items[i];
//            break;
//        }
//    }
}

void imageMarkup::markupMouseWheelEvent( QWheelEvent* )//event )
{
//    qDebug() << "imageMarkup::markupMouseWheelEvent" << event;

}

void imageMarkup::redrawActiveItemHere( QPoint pos )
{

    // Do nothing if no active item
    if( !activeItem )
        return;

    // Area to update
    //!!! This is currently just one rectangle that encloses the erased and redrawn object.
    //!!! It could (should?) be a region that includes a single rect for mostly hoz and vert lines,
    //!!! four rects for the four sides of an area, and a number of rectangles that efficiently
    //!!! allows redrawing of diagonal lines
    QRect changedArea;

    // Erase if visible, move, then redraw the item
    if( activeItem->visible )
    {
        changedArea = activeItem->area;
        activeItem->erase();
    }
    activeItem->moveTo( pos );
    activeItem->drawMarkupIn();

    // Extend the changed area to include the item's new area and notify markups require redrawing
    changedArea = changedArea.united( activeItem->area );
    markupChange( *markupImage, changedArea );

}

// The viewport size has changed
void imageMarkup::markupResize( QSize newSize )
{
    // If the markup image is not the right size, create one that is
    if( markupImage->size() != newSize )
    {
        // Delete the old one (may be the initial empty image)
        delete markupImage;
        markupImage = new QImage( newSize, QImage::Format_ARGB32 );

        // Fill with transparent background
        markupImage->fill( QColor ( 0, 0, 0, 0 ) );
    }
}

// The underlying image has moved in the viewport
void imageMarkup::markupScroll( QPoint )//newPos )
{
//    qDebug() << " imageMarkup::markupScroll" << newPos;

}

// The underlying image zoom factor has changed
void imageMarkup::markupZoom( double )//newZoom )
{
//    qDebug() << " imageMarkup::markupZoom" << newZoom;

}

