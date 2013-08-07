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
 *  Copyright (c) 2012, 2013
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

// Refer to imageMarkup.h for general module description

#include <markupItem.h>
#include <imageMarkup.h>

//===========================================================================
// Generic markup base class

// Construct a markup item
markupItem::markupItem( imageMarkup* ownerIn, const isOverOptions /*unused*/, const bool interactiveIn, const bool reportOnMoveIn, const QString legendIn )
{
    interactive = interactiveIn;
    reportOnMove = reportOnMoveIn;
    visible = false;
    activeHandle = MARKUP_HANDLE_NONE;
    owner = ownerIn;
    color = QColor( 0, 255, 0 ); // green

    setLegend( legendIn );
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
    drawMarkupIn( p );
}

// Draw in the item, showing it to the user
void markupItem::drawMarkupIn( QPainter& p )
{
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
void markupItem::scale( const double xScale, const double yScale, const double zoomScale )
{
    // Do type specific scaling
    scaleSpecific( xScale, yScale, zoomScale );

    // Update the generic item area
    setArea();
}


// Set the string used to notate the markup (and the calculate its size)
void markupItem::setLegend( const QString legendIn )
{
    legend = legendIn;
    legendSize = owner->legendFontMetrics->size( Qt::TextSingleLine, legend );
}

// Return the string used to notate the markup
const QString markupItem::getLegend()
{
    return legend;
}

// Return the size of the string used to notate the markup
const QSize markupItem:: getLegendSize()
{
    return legendSize;
}

// Returns true if legend text is present
bool markupItem::hasLegend()
{
    return !(legend.isEmpty());
}

void markupItem::addLegendArea()
{
    if( hasLegend() )
    {
        QRect legendArea;
        legendArea.setSize( getLegendSize() );
        legendArea.moveTo( getLegendPos() );
        area = area.united( legendArea );
    }
}

// Sets the top left position of the rectangle enclosing the legend and returns the text drawing origin within that area
const QPoint markupItem::setLegendPos( QPoint pos, legendJustification just )
{
    legendPos = pos;

    // Position the legend around the position requested according to the justification
    switch( just )
    {
        case ABOVE_RIGHT:
        default:
            legendPos.setY( legendPos.y() - owner->legendFontMetrics->height() );
            break;

        case BELOW_LEFT:
            legendPos.setX( legendPos.x() - legendSize.width() );

        case BELOW_RIGHT:
            // legendPos is correct as is
            break;
    }

    QPoint textOrigin = legendPos;
    textOrigin.setY( textOrigin.y() + owner->legendFontMetrics->ascent() );

    return textOrigin;
}

// Returns the last drawn legend position
const QPoint markupItem::getLegendPos()
{
    return legendPos;
}

void markupItem::drawLegend( QPainter& p, QPoint pos, legendJustification just )
{
    p.setFont( owner->legendFont );
    p.drawText( setLegendPos( pos, just ), getLegend() );
}

// Limit a given point to the image
QPoint markupItem::limitPointToImage( const QPoint pos )
{
    // Limit X
    QPoint retPos = pos;
    if( retPos.x() < 0 )
    {
        retPos.setX( 0 );
    }
    else
    {
        int w = owner->markupImage->rect().width();
        if( retPos.x() > w ) retPos.setX( w-1 );
    }

    // Limit Y
    if( retPos.y() < 0 )
    {
        retPos.setY( 0 );
    }
    else
    {
        int h = owner->markupImage->rect().height();
        if( retPos.y() > h ) retPos.setY( h-1 );
    }

    // Return limited point
    return retPos;
}
