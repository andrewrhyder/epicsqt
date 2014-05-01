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

// This class manages an image markup consisting of a target.
// Refer to markupItem.h for base functionality and to imageMarkup.h for
// general descrition of image markups.

#include <markupTarget.h>
#include <imageMarkup.h>

markupTarget::markupTarget( imageMarkup* ownerIn, const bool interactiveIn, const bool reportOnMoveIn, const QString legendIn ) : markupItem( ownerIn, OVER_LINE, interactiveIn, reportOnMoveIn, legendIn )
{
}

void markupTarget::drawMarkup( QPainter& p )
{
    // Scale markup
    double scale = getZoomScale();
    QPoint scaledPos = QPoint( pos.x() * scale, pos.y() * scale );

    // Draw markup
    QPen pen = p.pen();
    pen.setStyle( Qt::DashLine );
    p.setPen( pen );
    p.drawLine( scaledPos.x(), 0, scaledPos.x(), getImageSize().height() );
    p.drawLine( 0, scaledPos.y(), getImageSize().width(), scaledPos.y() );
    pen.setStyle( Qt::SolidLine );
    p.setPen( pen );

    // Draw markup legend
    drawLegend( p, scaledPos, ABOVE_RIGHT );
}

void markupTarget::setArea()
{
    area = QRect( QPoint(0,0), getImageSize() );

    addLegendArea();

    owner->markupAreasStale = true;
}

void markupTarget::startDrawing( const QPoint posIn )
{
    pos = posIn;
    activeHandle = MARKUP_HANDLE_NONE;
}

void markupTarget::moveTo( const QPoint posIn )
{
    // Limit position to within the image
    QPoint limPos = limitPointToImage( posIn );

    pos = limitPointToImage( limPos );
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

// Return the cursor for each handle
QCursor markupTarget::cursorForHandle( const markupItem::markupHandles )
{
// No special cursors for different handles
//    switch( handle )
//    {
//        case MARKUP_HANDLE_TL:   return Qt::SizeFDiagCursor;
//        case MARKUP_HANDLE_BL:   return Qt::SizeBDiagCursor;
//        ...
//        default: return defaultCursor();
//    }

    return defaultCursor();
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

void markupTarget::nonInteractiveUpdate( QPoint p1, QPoint )
{
    pos = p1;
    setArea();
}
