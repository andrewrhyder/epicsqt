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

// This class manages an image markup consisting of an 'beam' indicator.
// Refer to markupItem.h for base functionality and to imageMarkup.h for
// general descrition of image markups.

#include <markupBeam.h>
#include <imageMarkup.h>

markupBeam::markupBeam( imageMarkup* ownerIn, const bool interactiveIn, const bool reportOnMoveIn, const QString legendIn ) : markupItem( ownerIn, OVER_LINE, interactiveIn, reportOnMoveIn, legendIn )
{
    // Size of cross hair
    armSize = 20;
}

void markupBeam::drawMarkup( QPainter& p )
{
    // Draw markup
    p.drawLine( pos.x()-1, pos.y()+1, pos.x()-1, pos.y()+armSize );
    p.drawLine( pos.x()+1, pos.y()+1, pos.x()+1, pos.y()+armSize );

    p.drawLine( pos.x()-1, pos.y()-1, pos.x()-1, pos.y()-armSize );
    p.drawLine( pos.x()+1, pos.y()-1, pos.x()+1, pos.y()-armSize );

    p.drawLine( pos.x()+1, pos.y()-1, pos.x()+armSize, pos.y()-1 );
    p.drawLine( pos.x()+1, pos.y()+1, pos.x()+armSize, pos.y()+1 );

    p.drawLine( pos.x()-1, pos.y()-1, pos.x()-armSize, pos.y()-1 );
    p.drawLine( pos.x()-1, pos.y()+1, pos.x()-armSize, pos.y()+1 );

    // Draw markup legend
    drawLegend( p, pos, BELOW_LEFT );
}

void markupBeam::setArea()
{
    area.setLeft  ( pos.x()-armSize );
    area.setRight ( pos.x()+armSize );
    area.setTop   ( pos.y()-armSize );
    area.setBottom( pos.y()+armSize );

    addLegendArea();

    owner->markupAreasStale = true;
}

void markupBeam::startDrawing( const QPoint posIn )
{
    pos = posIn;
    activeHandle = MARKUP_HANDLE_NONE;
}

void markupBeam::moveTo( const QPoint posIn )
{
    // Limit position to within the image
    QPoint limPos = limitPointToImage( posIn );

    pos = limitPointToImage( limPos );
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

// Return the cursor for each handle
QCursor markupBeam::cursorForHandle( const markupItem::markupHandles )
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

void markupBeam::scaleSpecific( const double xScale, const double yScale, const double )
{
    pos.setX( pos.x() * xScale );
    pos.setY( pos.y() * yScale );
}

void markupBeam::nonInteractiveUpdate( QPoint p1, QPoint )
{
    pos = p1;
    setArea();
}
