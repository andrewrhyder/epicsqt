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
  This class manages markups over an image, such as regioN of interest, line, graticule, etc, and user interaction with same.
 */

#include <imageMarkup.h>
#include <QPainter>


//===========================================================================
// Generic markup base class

// Construct a markup item
// All markup items share the same markup image
markupItem::markupItem( QImage** imageIn, isOverOptions over, bool interactiveIn )
{
    isOverType = over;
    interactive = interactiveIn;
    image = imageIn;
    highlight = false;
    visible = false;
    active = false;
}

void markupItem::setHighlight( bool highlightIn )
{
    highlight = highlightIn;
    drawMarkup();
}

void markupItem::setActive( bool activeIn )
{
    active = activeIn;
}
bool markupItem::isActive()
{
    return active;
}

bool markupItem::isOver( QPoint point )
{
    switch( isOverType )
    {
        case OVER_LINE:
            return true; // not implemented yet

        case OVER_BORDER:
            return true; // not implemented yet

        case OVER_AREA:
            return area.contains( point );
    }
    return false;
}

//===========================================================================
// Vertical line markup

markupVLine::markupVLine( QImage** imageIn, bool interactiveIn ) : markupItem( imageIn, OVER_LINE, interactiveIn )
{

}

void markupVLine::drawMarkup()
{
    QImage* i = *image;
    QPainter p( i );
    p.drawLine( x, 0, x, i->rect().height() );
    qDebug()<< "markupVLine::drawMarkup";
}

void markupVLine::setPos( int xIn )
{
    QImage* i = *image;

    x = xIn;

    area.setLeft( xIn );
    area.setRight( xIn );
    area.setTop( 0 );
    area.setBottom( i->rect().bottom());
}

//===========================================================================
// Horizontal line markup

markupHLine::markupHLine( QImage** imageIn, bool interactiveIn ) : markupItem( imageIn, OVER_LINE, interactiveIn )
{

}

void markupHLine::drawMarkup()
{
    QImage* i = *image;
    QPainter p( i );
    qDebug() << "markupHLine::drawMarkup" << i->size();
    p.setPen(Qt::blue);  //!!! testing
    p.drawLine( 0, y, i->rect().width(), y );
    qDebug()<< "markupHLine::drawMarkup";
}

void markupHLine::setPos( int yIn )
{
    QImage* i = *image;

    y = yIn;

    area.setTop( yIn );
    area.setBottom( yIn );
    area.setLeft( 0 );
    area.setRight( i->rect().right());
}

//===========================================================================
// Profile line markup

markupLine::markupLine( QImage** imageIn, bool interactiveIn ) : markupItem( imageIn, OVER_LINE, interactiveIn )
{

}

void markupLine::drawMarkup()
{
    QImage* i = *image;
    QPainter p( i );
    p.drawLine( start, end );
    qDebug()<< "markupLine::drawMarkup";
}

void markupLine::setPos( QPoint startIn, QPoint endIn )
{
    start = startIn;
    end = endIn;

    area.setCoords( std::min( start.x(), end.x() ),
                    std::min( start.y(), end.y() ),
                    std::max( start.x(), end.x() ),
                    std::max( start.y(), end.y() ));
}

//===========================================================================
// Text markup

markupText::markupText( QImage** imageIn, bool interactiveIn ) : markupItem( imageIn, OVER_AREA, interactiveIn )
{

}

void markupText::drawMarkup()
{
    QImage* i = *image;
    QPainter p( i );

    p.setPen(Qt::blue);  //!!! testing
    p.setFont(QFont("Arial", 30));//!!! testing

    p.drawText( area, Qt::AlignLeft, text);
    qDebug()<< "markupText::drawMarkup";
}

void markupText::setText( QString textIn )
{
    text = textIn;
}

void markupText::setPos( QRect areaIn )
{
    area = areaIn;
}

//===========================================================================
// imageMarkup

// Constructor
imageMarkup::imageMarkup()
{
    interaction = WAITING;


    markupImage = new QImage();

    markupHLine* lineRoiTop    = new markupHLine( &markupImage, true );
    markupHLine* lineRoiBottom = new markupHLine( &markupImage, true );
    markupVLine* lineRoiLeft   = new markupVLine( &markupImage, true );
    markupVLine* lineRoiRight  = new markupVLine( &markupImage, true );
    markupLine*  lineProfile   = new markupLine( &markupImage, true );
    markupText*  timeDate      = new markupText( &markupImage, false );



    items.append( lineRoiTop );
    items.append( lineRoiBottom );
    items.append( lineRoiLeft );
    items.append( lineRoiRight );
    items.append( lineProfile );
    items.append( timeDate );

    // !!!temp testing
    timeDate->setText( "hi there" );
    timeDate->setPos( QRect(10, 10, 200, 40 ) );
    timeDate->setActive( true );
    lineRoiTop->setPos( 100 );
    lineRoiTop->setActive( true );
}

// Destructor
imageMarkup::~imageMarkup()
{
}

// User pressed a mouse button
void imageMarkup::markupMousePressEvent(QMouseEvent *event)
{
    qDebug() << "imageMarkup::markupMousePressEvent" << event;

    // if over any item
    int n = items.count();
    for( int i = 0; i < n; i++ )
    {
        if( items[i]->isOver( event->pos() ) )
        {
            items[i]->setHighlight( true );
            items[i]->setActive( true );
            interaction = MOVING;
        }
    }

}

void imageMarkup::markupMouseReleaseEvent ( QMouseEvent* event )
{
    qDebug() << "imageMarkup::markupMouseReleaseEvent" << event;

    if( interaction == MOVING )
    {
        ////check what item activity is needed for each possible response and respond as nessesary
    }

    // Un-highlight all items being manipulated
    int n = items.count();
    for( int i = 0; i < n; i++ )
    {
        if( items[i]->isActive() )
        {
            items[i]->setHighlight( false );
        }
    }

    // Ensure all changed items are drawn and finalise activity
    for( int i = 0; i < n; i++ )
    {
        if( items[i]->isActive() )
        {
            markupChange( *markupImage, items[i]->area );
//!!!            items[i]->setActive( false );
        }
    }

    // Flip back to waiting for user activity
    interaction = WAITING;
}

void imageMarkup::markupMouseMoveEvent( QMouseEvent* event )
{
    qDebug() << "imageMarkup::markupMouseMoveEvent" << event;

    if( interaction == MOVING )
    {
        ////for each active item
        {
            //// move item
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
    if( markupImage->size() != newSize )
    {
        delete markupImage;
        markupImage = new QImage( newSize, QImage::Format_RGB32 );
    }

    qDebug() << " imageMarkup::markupResize" << newSize;

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

// The markup overlay has changed, redraw part of it
void imageMarkup::markupChange( QImage& markups, QRect area )
{
    qDebug() << " imageMarkup::markupChange" << area;

}

