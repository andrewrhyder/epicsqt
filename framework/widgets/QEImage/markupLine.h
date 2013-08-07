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

/*
 This class manages ???
*/

#ifndef MARKUPLINE_H
#define MARKUPLINE_H

#include <markupItem.h>
//#include <QSize>
//#include <QPoint>
//#include <QLine>
//#include <QRect>
//#include <QMouseEvent>
//#include <QImage>
//#include <QColor>
//#include <QFontMetrics>
//#include <QFont>
//#include <QCaDateTime.h>


#include <QDebug>

class imageMarkup;



// Line markup used to select an arbiraty angle slice through an image
class markupLine : public markupItem
{
public:
    markupLine( imageMarkup* ownerIn, const bool interactiveIn, const bool reportOnMoveIn, const QString legendIn );

    //==================================================================
    // Implement base class functions - see markupItem class defition for a description of each of these methods
    void startDrawing( const QPoint pos );
    void setArea();
    void drawMarkup( QPainter& p );
    void moveTo( const QPoint pos );  // Move an item (always make it visible and highlighed)
    bool isOver( const QPoint point, QCursor* cursor );
    QPoint origin();
    QCursor cursorForHandle( const markupItem::markupHandles handle );
    QPoint getPoint1();
    QPoint getPoint2();
    unsigned int getThickness();
    void setThickness( const unsigned int thicknessIn );
    QCursor defaultCursor();
    void scaleSpecific( const double xScale, const double yScale, const double zoomScale );
    //==================================================================

private:
    QPoint start;
    QPoint end;
    unsigned int thickness;     // Selected line thickness
    unsigned int maxThickness;  // Maximum line thickness. Changes according to current zoom
    bool isOverLine( const QPoint point, const QPoint lineStart, const QPoint lineEnd );
};


#endif // MARKUPLINE_H
