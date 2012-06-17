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
 This class manages the markups that are overlayed of an image, such as region of interest and time and date.
 The class also handles user interaction with the markups, such as creation and draging.
 The class works at the display resolution of the image, but also understands the actual resolution of
 the underlying image and can describe markups in terms of the underlying image.
 For example: a user is working in a 500x500 pixel window viewing the center of a 2000x2000 pixel image
 that has been scaled to 1000x1000 pixels. In this example, this class manages all markups in the context
 of the 500x500 pixel viewport but keeps those markups aligned with the underlying image as it is scrolled
 and zoomed and reports those markups in terms of the underlying image.
 */

#ifndef IMAGEMARKUP_H
#define IMAGEMARKUP_H

#include <QSize>
#include <QPoint>
#include <QLine>
#include <QRect>
#include <QMouseEvent>
#include <QImage>
#include <QColor>

#include <QDebug>

class imageMarkup;

class markupItem
{
public:
    enum isOverOptions{ OVER_LINE, OVER_BORDER, OVER_AREA }; // test required to determine if pointer is over the object
    markupItem( imageMarkup* ownerIn, isOverOptions over, bool interactiveIn );
    ~markupItem();


    enum markupHandles { MARKUP_HANDLE_NONE,
                         MARKUP_HANDLE_START, MARKUP_HANDLE_END,  // Lines
                         MARKUP_HANDLE_TL, MARKUP_HANDLE_TR, MARKUP_HANDLE_BL, MARKUP_HANDLE_BR, // Area corners
                         MARKUP_HANDLE_T, MARKUP_HANDLE_B, MARKUP_HANDLE_L, MARKUP_HANDLE_R };   // Area sides

    markupHandles activeHandle;


    virtual void setArea()=0;
    virtual QPoint origin()=0;

    virtual void moveTo( QPoint pos )=0;  // Move an item (always make it visible and highlighed)

    void erase();                // Erase and item and redraw any items that it was over (note, this does not change its status. For example, it is used if hiding an item, but also when moving an item)



    virtual void drawMarkup( QPainter& p )=0;
    virtual void startDrawing( QPoint pos ) = 0;

    virtual bool isOver( QPoint point )=0;
    virtual QPoint getPoint1()=0;
    virtual QPoint getPoint2()=0;

    bool pointIsNear( QPoint p1, QPoint p );


    isOverOptions isOverType;
    QRect         area;         // Area object occupies, used for repainting, and actual object coordinates where appropriate
    bool          visible;      // Object is visible to the user
    bool          interactive;  // Object can be moved by the user
    bool          highlighted;  // Object is highlighted
    int           highlightMargin; // Extra margin required for highlighting
    QColor        markupColor;  // Object color
    imageMarkup*  owner;

    void drawMarkupIn();
    void drawMarkupOut();

};

class markupHLine : public markupItem
{
public:

    markupHLine( imageMarkup* ownerIn, bool interactiveIn );

    void startDrawing( QPoint pos );
    void setArea();
    void drawMarkup( QPainter& p );
    void moveTo( QPoint pos );  // Move an item (always make it visible and highlighed)
    bool isOver( QPoint point );
    QPoint origin();
    QPoint getPoint1();
    QPoint getPoint2();

    int y;
};

class markupVLine : public markupItem
{
public:

    markupVLine( imageMarkup* ownerIn, bool interactiveIn );

    void startDrawing( QPoint pos );
    void setArea();
    void drawMarkup( QPainter& p );
    void moveTo( QPoint pos );  // Move an item (always make it visible and highlighed)
    bool isOver( QPoint point );
    QPoint origin();
    QPoint getPoint1();
    QPoint getPoint2();

    int x;
};

class markupLine : public markupItem
{
public:
    markupLine( imageMarkup* ownerIn, bool interactiveIn );

    void startDrawing( QPoint pos );
    void setArea();
    void drawMarkup( QPainter& p );
    void moveTo( QPoint pos );  // Move an item (always make it visible and highlighed)
    bool isOver( QPoint point );
    QPoint origin();
    QPoint getPoint1();
    QPoint getPoint2();

    QPoint start;
    QPoint end;
};

class markupRegion : public markupItem
{
public:

    markupRegion( imageMarkup* ownerIn, bool interactiveIn );

    void startDrawing( QPoint pos );
    void setArea();
    void drawMarkup( QPainter& p );
    void moveTo( QPoint pos );  // Move an item (always make it visible and highlighed)
    bool isOver( QPoint point );
    QPoint origin();
    QPoint getPoint1();
    QPoint getPoint2();

    QRect rect;
};

class markupText : public markupItem
{
public:

    markupText( imageMarkup* ownerIn, bool interactiveIn );

    void setText( QString textIn );

    void startDrawing( QPoint pos );
    void setArea();
    void drawMarkup( QPainter& p );
    void moveTo( QPoint pos );  // Move an item (always make it visible and highlighed)
    bool isOver( QPoint point );
    QPoint origin();
    QPoint getPoint1();
    QPoint getPoint2();

    QString text;
    QRect rect;
};

class imageMarkup {
public:
    imageMarkup();
    ~imageMarkup();

    enum markupModes { MARKUP_MODE_NONE, MARKUP_MODE_H_LINE, MARKUP_MODE_V_LINE, MARKUP_MODE_LINE, MARKUP_MODE_AREA };

    enum markupIds { MARKUP_ID_REGION, MARKUP_ID_H_SLICE, MARKUP_ID_V_SLICE, MARKUP_ID_LINE, MARKUP_ID_TIMESTAMP, MARKUP_ID_COUNT, MARKUP_ID_NONE };

    void markupMousePressEvent(QMouseEvent *event);
    void markupMouseReleaseEvent ( QMouseEvent* event );
    void markupMouseMoveEvent( QMouseEvent* event );
    void markupMouseWheelEvent( QWheelEvent* event );

    void setShowTime( bool visibleIn );     // Display timestamp markup if true
    bool getShowTime();                     // Rturn true if displaying timestamp markup

    void setMode( markupModes modeIn );
    QImage* markupImage;
    QVector<markupItem*> items;
    QPoint grabOffset;
    QVector<QRect>& getMarkupAreas();
    bool anyVisibleMarkups();
    bool markupAreasStale;
    QCursor getDefaultMarkupCursor();


protected:
    void markupResize( QSize newSize );   // The viewport size has changed
    void markupScroll( QPoint newPos );   // The underlying image has moved in the viewport
    void markupZoom( double newZoom );    // The underlying image zoom factor has changed

    virtual void markupChange( QImage& markups, QVector<QRect>& changedAreas )=0;    // The markup overlay has changed, redraw part of it
    virtual void markupSetCursor( QCursor cursor )=0;
    virtual void markupAction( markupIds activeItem, QPoint point1, QPoint point2 )=0;

    void setMarkupTime();                   // A new image has arrived, build a timestamp

private:

    markupIds activeItem;

    markupModes mode;

    void redrawActiveItemHere( QPoint pos );
    QVector<QRect> markupAreas;

    bool buttonDown;

};

#endif // IMAGEMARKUP_H
