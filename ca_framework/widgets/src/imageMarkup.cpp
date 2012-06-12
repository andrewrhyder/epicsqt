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
#define OVER_TOLERANCE 4

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


bool markupItem::overlaps( markupItem* other )
{
    return area.intersects( other->area );
}

// Erase and item and redraw any items that it was over (note, this does not change its status. For example, it is used if hiding an item, but also when moving an item)
void markupItem::erase()
{
    // Clear the item
    QPainter p( owner->markupImage );
    p.setCompositionMode( QPainter::CompositionMode_Clear );
    p.fillRect( area, Qt::black );
    p.end();

    // Redraw any other visible items that have had any part erased as well
    int n = owner->items.count();
    for( int i = 0; i < n; i++ )
    {
        if( owner->items[i] != this && owner->items[i]->visible && owner->items[i]->overlaps( this ) )
        {
            drawMarkup();
        }
    }
}

//===========================================================================
// Vertical line markup

markupVLine::markupVLine( imageMarkup* ownerIn, bool interactiveIn ) : markupItem( ownerIn, OVER_LINE, interactiveIn )
{

}

void markupVLine::drawMarkup()
{
    QPainter p( owner->markupImage );
    p.setPen( markupColor);
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
        area.setLeft( x );
        area.setRight( x );
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

void markupHLine::drawMarkup()
{
    QPainter p( owner->markupImage );
    p.setPen( markupColor);
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

void markupLine::drawMarkup()
{
    QPainter p( owner->markupImage );
    p.setPen( markupColor);
    p.drawLine( start, end );
}

void markupLine::setArea()
{
    if( highlighted )
    {
        area.setCoords( std::min( start.x(), end.x() ) - highlightMargin ,
                        std::min( start.y(), end.y() ) - highlightMargin ,
                        std::max( start.x(), end.x() ) + highlightMargin ,
                        std::max( start.y(), end.y() ) + highlightMargin );
    }
    else
    {
        area.setCoords( std::min( start.x(), end.x() ),
                        std::min( start.y(), end.y() ),
                        std::max( start.x(), end.x() ),
                        std::max( start.y(), end.y() ));
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
            start = pos - owner->grabOffset;
            break;

       case MARKUP_HANDLE_END:
            end = pos - owner->grabOffset;
            break;

        default:
            break;
    }
    setArea();
}

bool markupLine::isOver( QPoint point )
{
    // Not over the line if outside the drawing recrangle
    if( !area.contains( point ) )
    {
        return false;
    }

    // Calculate the slope of the line
    double lineSlope = ( end.y() - start.y() ) / ( end.x() - start.x() );

    // For the X of the point, determine the Y that would place the point on the line
    int expectedY = (int)((double)point.x() * lineSlope);

    // Return 'over' if Y is close to as calculated
    return ( abs( point.y() - expectedY ) <= OVER_TOLERANCE );
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

void markupRegion::drawMarkup()
{
    QPainter p( owner->markupImage );
    p.setPen( markupColor);
    p.drawRect( rect );
    qDebug() << "drawMarkup()" << rect;
}

void markupRegion::setArea()
{
    area = rect;
    area.adjust( 0, 0, 1, 1 );

    if( highlighted )
    {
        area.setLeft(   area.left()   - highlightMargin );
        area.setRight(  area.right()  + highlightMargin );
        area.setTop(    area.top()    - highlightMargin );
        area.setBottom( area.bottom() + highlightMargin );
    }
    qDebug() << "setArea()" << area;
}

void markupRegion::startDrawing( QPoint pos )
{
    rect.setBottomLeft( pos );
    rect.setTopRight( pos );
    activeHandle = MARKUP_HANDLE_TR;

    qDebug() << "startDrawing()" << rect;
}

void markupRegion::moveTo( QPoint pos )
{
    switch( activeHandle )
    {
        case MARKUP_HANDLE_NONE: break;
        case MARKUP_HANDLE_TL:   rect.setTopLeft(     pos -= owner->grabOffset );        break;
        case MARKUP_HANDLE_TR:   rect.setTopRight(    pos -= owner->grabOffset );        break;
        case MARKUP_HANDLE_BL:   rect.setBottomLeft(  pos -= owner->grabOffset );        break;
        case MARKUP_HANDLE_BR:   rect.setBottomRight( pos -= owner->grabOffset );        break;
        case MARKUP_HANDLE_T:    rect.setTop(         pos.y() - owner->grabOffset.y() ); break;
        case MARKUP_HANDLE_B:    rect.setBottom(      pos.y() - owner->grabOffset.y() ); break;
        case MARKUP_HANDLE_L:    rect.setLeft(        pos.x() - owner->grabOffset.x() ); break;
        case MARKUP_HANDLE_R:    rect.setRight(       pos.x() - owner->grabOffset.x() ); break;

        default:
            break;
    }

    setArea();

    qDebug() << "moveTo()" << rect << pos;
}

bool markupRegion::isOver( QPoint point )
{
bool temp = (( abs( point.x() - rect.left()   ) <= OVER_TOLERANCE ) ||
             ( abs( point.x() - rect.right()  ) <= OVER_TOLERANCE ) ||
             ( abs( point.y() - rect.top()    ) <= OVER_TOLERANCE ) ||
             ( abs( point.y() - rect.bottom() ) <= OVER_TOLERANCE ) );
   qDebug() << "isOver()" << rect << point << temp;



    return (( abs( point.x() - rect.left()   ) <= OVER_TOLERANCE ) ||
            ( abs( point.x() - rect.right()  ) <= OVER_TOLERANCE ) ||
            ( abs( point.y() - rect.top()    ) <= OVER_TOLERANCE ) ||
            ( abs( point.y() - rect.bottom() ) <= OVER_TOLERANCE ) );

}

QPoint markupRegion::origin()
{
    qDebug() << "origin()" << rect.topLeft();

    return rect.topLeft();
}

//===========================================================================
// Text markup

markupText::markupText( imageMarkup* ownerIn, bool interactiveIn ) : markupItem( ownerIn, OVER_AREA, interactiveIn )
{

}

void markupText::drawMarkup()
{
    // Set the area to more than enough.
    // This will be trimmed to the bounding retangle of the text
    rect = QRect(10, 10, 200, 400 );

    // Draw the text
    QPainter p( owner->markupImage );
    p.setPen( markupColor);
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
    timeDate->drawMarkup();
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
    qDebug() << "imageMarkup::markupMousePressEvent" << event;

    // Determine if the user clicked over an interactive, visible item,
    // and if so, make the first item found the active item
    activeItem = NULL;
    int n = items.count();
    for( int i = 0; i < n; i++ )
    {
        if( items[i]->interactive && items[i]->visible && items[i]->isOver( event->pos() ) )
        {
            activeItem = items[i];
            grabOffset = activeItem->origin() - event->pos();
            break;
        }
    }

    // If not over an item, move the appropriate item to the point clicked on
    if( !activeItem )
    {
        switch( mode )
        {
            case MARKUP_MODE_NONE:                             break;
            case MARKUP_MODE_H_LINE: activeItem = lineHoz;     break;
            case MARKUP_MODE_V_LINE: activeItem = lineVert;    break;
            case MARKUP_MODE_LINE:   activeItem = lineProfile; break;
            case MARKUP_MODE_AREA:   activeItem = region;      break;
        }
        if( activeItem )
        {
            activeItem->startDrawing( event->pos() );
        }
    }
}

void imageMarkup::markupMouseMoveEvent( QMouseEvent* event )
{
    qDebug() << "imageMarkup::markupMouseMoveEvent" << event;

    // Do nothing if no active item
    if( !activeItem )
        return;

    // Area to update
    //!!! This is currently just one rectangle that encloses the erased and redrawn object.
    //!!! It could (should?) be a region that includes a single rect for mostly hoz and vert lines,
    //!!! four rects for the four sides of an area, and a number of rectangles that efficiently
    //!!! allows redrawing of diagonal lines
    QRect changedArea = activeItem->area;

    // Erase, move, then redraw the item
    activeItem->erase();
    activeItem->moveTo( event->pos() );
    activeItem->drawMarkup();

    // Extend the changed area to include the item's new area and notify markups require redrawing
    changedArea = changedArea.united( activeItem->area );
    markupChange( *markupImage, markupImage->rect() );//!!!changedArea );
}

void imageMarkup::markupMouseReleaseEvent ( QMouseEvent* event )
{
    qDebug() << "imageMarkup::markupMouseReleaseEvent" << event;

    // Do nothing if no active item
    if( !activeItem )
        return;

    // Erase the active item, make it no longer the active item, the redraw it
    activeItem->erase();
    markupItem* item = activeItem;
    activeItem = NULL;
    item->drawMarkup();

    // Draw all
    int n = items.count();
    for( int i = 0; i < n; i++ )
    {
        if( items[i]->visible && items[i]->isOver( event->pos() ) )
        {
            activeItem = items[i];
            break;
        }
    }
}

void imageMarkup::markupMouseWheelEvent( QWheelEvent* event )
{
    qDebug() << "imageMarkup::markupMouseWheelEvent" << event;

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
void imageMarkup::markupScroll( QPoint newPos )
{
    qDebug() << " imageMarkup::markupScroll" << newPos;

}

// The underlying image zoom factor has changed
void imageMarkup::markupZoom( double newZoom )
{
    qDebug() << " imageMarkup::markupZoom" << newZoom;

}

