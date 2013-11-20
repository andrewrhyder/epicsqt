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

// This class manages an image markup consisting of an arbitrarty line.
// Refer to markupItem.h for base functionality and to imageMarkup.h for
// general descrition of image markups.

#include <markupLine.h>
#include <imageMarkup.h>

#include <math.h>

markupLine::markupLine( imageMarkup* ownerIn, const bool interactiveIn, const bool reportOnMoveIn, const QString legendIn ) : markupItem( ownerIn, OVER_LINE, interactiveIn, reportOnMoveIn, legendIn )
{
}

void markupLine::drawMarkup( QPainter& p )
{
    // Draw markup
    p.drawLine( start, end );

    if(( abs(QPoint( end-start ).x()) > (HANDLE_SIZE + 2) ) ||
       ( abs(QPoint( end-start ).y()) > (HANDLE_SIZE + 2) ))
    {
        QRect handle( 0, 0, HANDLE_SIZE, HANDLE_SIZE );
        QPoint halfHandle( HANDLE_SIZE/2, HANDLE_SIZE/2 );

        handle.moveTo( start - halfHandle );
        p.drawEllipse( handle );

        handle.moveTo( end - halfHandle );
        p.drawRect( handle );
    }

    // If single pixel thickness, draw a single handle in the middle
    if( thickness == 1 )
    {
        int x = (start.x()+end.x()-HANDLE_SIZE)/2;
        int y = (start.y()+end.y()-HANDLE_SIZE)/2;
        QRect handle( x, y, HANDLE_SIZE, HANDLE_SIZE );
        p.drawRect( handle );
    }

    // If thickness more than one pixel, draw the thickness borders (dotted lines either side of the main line)
    // and draw two handles, one on each border
    else
    {
        int dX = end.x()-start.x();
        int dY = end.y()-start.y();
        if( dX || dY )
        {
            QPen pen = p.pen();
            pen.setStyle( Qt::DashLine );
            p.setPen( pen );

            int len = (int)sqrt( (dX*dX)+(dY*dY) );

            QPoint offset( (int)(thickness) * -dY / (2*len), int(thickness) * dX / (2*len) );

            p.drawLine( start+offset, end+offset );
            p.drawLine( start-offset, end-offset );

            pen.setStyle( Qt::SolidLine );
            p.setPen( pen );
        }
    }

    // Draw markup legend
    legendJustification just;
    (( start.x() < end.x() && start.y() < end.y() ) || ( start.x() > end.x() && start.y() > end.y() ) ) ? just = ABOVE_RIGHT : just = BELOW_RIGHT;
    drawLegend( p, (end-start)/2+start, just );
}

void markupLine::setArea()
{
    // Determine the corners of the area for single pixel thickness
    QPoint topLeft(     std::min( start.x(), end.x() ), std::min( start.y(), end.y() ) );
    QPoint bottomRight( std::max( start.x(), end.x() ), std::max( start.y(), end.y() ) );

    // If thickness more than one pixel, calculate and add the additional width and height
    if( thickness > 1 )
    {
        // Calculate variables
        int dX = end.x()-start.x();
        int dY = end.y()-start.y();
        int len = (int)sqrt( (dX*dX)+(dY*dY) );

        // Add offset only if length is not zero (avoid divide by zero error)
        if( len )
        {
            QPoint offset( abs((int)(thickness) * dY / len), abs((int)(thickness) * dX / len) );

            topLeft -= offset;
            bottomRight += offset;
        }
    }

    // Set the area
    area.setTopLeft( topLeft );
    area.setBottomRight( bottomRight );

    area.adjust( -HANDLE_SIZE, -HANDLE_SIZE, HANDLE_SIZE+1, HANDLE_SIZE+1 );

    addLegendArea();

    owner->markupAreasStale = true;

}

void markupLine::startDrawing( const QPoint pos )
{
    start = pos;
    end = pos;
    activeHandle = MARKUP_HANDLE_END;
}

void markupLine::moveTo( const QPoint posIn )
{
    // Limit position to within the image
    QPoint limPos = limitPointToImage( posIn );

    // Move the appropriate part of the line, according to which bit the user has grabbed
    switch( activeHandle )
    {
        case MARKUP_HANDLE_NONE:
            { // Constrain scope of endOffset to case
                QPoint endOffset = end - start;
                start = limPos - owner->grabOffset ;
                end = start + endOffset;
            }
            break;

        case MARKUP_HANDLE_START:
            start = limPos;
            break;

       case MARKUP_HANDLE_END:
            end = limPos;
            break;

        case MARKUP_HANDLE_CENTER:
            { // contain scope of variables local to case
                // Calculate distance from current position to nearest point on the line.

                // Line X and Y dimensions
                double dX = end.x()-start.x();
                double dY = end.y()-start.y();

                if( dX == 0 )
                {
                    thickness = abs(limPos.x()-start.x())*2+1;
                }
                else if( dY == 0 )
                {
                    thickness = abs(limPos.y()-start.y())*2+1;
                }
                else
                {
                    // Slope
                    double slope = dY/dX;

                    // Y intercept
                    double yInt = (double)(start.y())-((double)(start.x()) * slope);
                    double distance = abs( ((double)(limPos.x())*slope) - (double)(limPos.y()) + yInt) / (int)sqrt( (slope*slope)+1 );
                    thickness = 2*(int)(distance)+1;
                }

                // Limit thickness
                if( thickness > maxThickness ) thickness = maxThickness;

            }
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

    // If over the line, return true
    // Check if over the end in preference to the start. This is especially important when
    // just starting as the start and end will be set initially to the same point
    if( isOverLine( point, start, end ) )
    {
        if(  pointIsNear( point, end ) )
        {
            *cursor = Qt::SizeAllCursor;
            activeHandle = MARKUP_HANDLE_END;
        }
        else if( pointIsNear( point, start ) )
        {
            *cursor = Qt::SizeAllCursor;
            activeHandle = MARKUP_HANDLE_START;
        }
        else if( ( thickness == 1 ) && pointIsNear( point, (start+end)/2 ) )
        {
            *cursor = Qt::SizeAllCursor;
            activeHandle = MARKUP_HANDLE_CENTER;
        }
        else
        {
            *cursor = Qt::OpenHandCursor;
            activeHandle = MARKUP_HANDLE_NONE;
        }
        return true;
    }

    // If any thickness, and over one of the thickness lines, return true
    if( thickness > 1 )
    {
        // Calculate variables
        int dX = end.x()-start.x();
        int dY = end.y()-start.y();
        if( dX || dX )
        {
            int len = (int)sqrt( (dX*dX)+(dY*dY) );
            int t = thickness / 2;

            // Calculate the offset to the thickness lines
            QPoint offset( -t*(end.y()-start.y())/len, t*(end.x()-start.x())/len );

            // If over a thickness line, return true
            if( isOverLine( point+offset, start, end ) ||
                isOverLine( point-offset, start, end ) )
            {
                activeHandle = MARKUP_HANDLE_CENTER;
                *cursor = cursorForHandle( activeHandle );
                return true;
            }
        }
    }

    // Not over
    activeHandle = MARKUP_HANDLE_NONE;
    return false;
}

bool markupLine::isOverLine( const QPoint point, const QPoint lineStart, const QPoint lineEnd )
{
    // If line is zero length, compare to the point. (avoid divide by zero)
    if( lineStart == lineEnd )
    {
        return pointIsNear( point, lineStart );
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
    if( abs( lineEnd.y() - lineStart.y() ) < abs( lineEnd.x() - lineStart.x() ) )
    {
        // Mostly horizontal
        startA = lineStart.x();
        startB = lineStart.y();
        endA = lineEnd.x();
        endB = lineEnd.y();
        pointA = point.x();
        pointB = point.y();
    }
    else
    {
        // Mostly vertical
        startA = lineStart.y();
        startB = lineStart.x();
        endA = lineEnd.y();
        endB = lineEnd.x();
        pointA = point.y();
        pointB = point.x();
    }

    // Calculate the slope of the line
    double lineSlope = (double)( endB - startB ) / (double)( endA - startA );

    // For the A of the point, determine the B that would place the point on the line
    int expectedB = (int)((double)(pointA - startA) * lineSlope) + startB;

    // Return 'over' if B is close to as calculated
    return( abs( pointB - expectedB ) <= OVER_TOLERANCE );
}

QPoint markupLine::origin()
{
    return start;
}

// Return the cursor for each handle
QCursor markupLine::cursorForHandle( const markupItem::markupHandles handle )
{
    switch( handle )
    {
        case MARKUP_HANDLE_START:  return Qt::SizeAllCursor;
        case MARKUP_HANDLE_END:    return Qt::SizeAllCursor;
        case MARKUP_HANDLE_CENTER: return Qt::SizeAllCursor;
        case MARKUP_HANDLE_NONE:   return defaultCursor();
        default:                   return defaultCursor();
    }
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
    return owner->getLineCursor();
}

void markupLine::scaleSpecific( const double xScale, const double yScale, const double zoomScale )
{
    // Scale the line position
    start.setX( start.x() * xScale );
    start.setY( start.y() * yScale );
    end.setX( end.x() * xScale );
    end.setY( end.y() * yScale );

    // Scale the line thickness.
    // Note, one pixel wide is always one pixel wide
    if( thickness != 1 )
    {
        thickness *= xScale;
    }
    maxThickness = THICKNESS_MAX * zoomScale;
}

void markupLine::nonInteractiveUpdate( QPoint p1, QPoint p2 )
{
    start = p1;
    end = p2;
    setArea();
}