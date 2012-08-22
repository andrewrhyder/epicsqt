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
#include <QCaDateTime.h>


#include <QDebug>

class imageMarkup;

class markupItem
{
public:
    enum isOverOptions{ OVER_LINE, OVER_BORDER, OVER_AREA }; // test required to determine if pointer is over the object
    markupItem( imageMarkup* ownerIn, isOverOptions over, bool interactiveIn, bool reportOnMoveIn );
    virtual ~markupItem();


    enum markupHandles { MARKUP_HANDLE_NONE, // Over a markup, but not over any specific handle of the markup
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

    virtual bool isOver( QPoint point, QCursor* cursor )=0;
    virtual QPoint getPoint1()=0;
    virtual QPoint getPoint2()=0;
    virtual void tidy()=0;
    virtual QCursor defaultCursor()=0;

    bool pointIsNear( QPoint p1, QPoint p );


    isOverOptions isOverType;
    QRect         area;         // Area object occupies, used for repainting, and actual object coordinates where appropriate
    bool          visible;      // Object is visible to the user
    bool          interactive;  // Object can be moved by the user
    bool          reportOnMove; // Movements reported (not just on move completion)
    bool          highlighted;  // Object is highlighted
    int           highlightMargin; // Extra margin required for highlighting
    QColor        color;            // Color markup is drawn in
    imageMarkup*  owner;

    void drawMarkupIn();
    void drawMarkupOut();
    void setColor( QColor colorIn );
    QColor getColor();
};

class markupTarget : public markupItem
{
public:

    markupTarget( imageMarkup* ownerIn, bool interactiveIn, bool reportOnMoveIn );

    void startDrawing( QPoint pos );
    void setArea();
    void drawMarkup( QPainter& p );
    void moveTo( QPoint pos );  // Move an item (always make it visible and highlighed)
    bool isOver( QPoint point, QCursor* cursor );
    QPoint origin();
    QPoint getPoint1();
    QPoint getPoint2();
    void tidy();
    QCursor defaultCursor();

private:
    QPoint pos;
};

class markupBeam : public markupItem
{
public:

    markupBeam( imageMarkup* ownerIn, bool interactiveIn, bool reportOnMoveIn );

    void startDrawing( QPoint pos );
    void setArea();
    void drawMarkup( QPainter& p );
    void moveTo( QPoint pos );  // Move an item (always make it visible and highlighed)
    bool isOver( QPoint point, QCursor* cursor );
    QPoint origin();
    QPoint getPoint1();
    QPoint getPoint2();
    void tidy();
    QCursor defaultCursor();

private:
    QPoint pos;
    int armSize;    // Length of arms in cross hair
};

class markupHLine : public markupItem
{
public:

    markupHLine( imageMarkup* ownerIn, bool interactiveIn, bool reportOnMoveIn );

    void startDrawing( QPoint pos );
    void setArea();
    void drawMarkup( QPainter& p );
    void moveTo( QPoint pos );  // Move an item (always make it visible and highlighed)
    bool isOver( QPoint point, QCursor* cursor );
    QPoint origin();
    QPoint getPoint1();
    QPoint getPoint2();
    void tidy();
    QCursor defaultCursor();

private:
    int y;
};

class markupVLine : public markupItem
{
public:

    markupVLine( imageMarkup* ownerIn, bool interactiveIn, bool reportOnMoveIn );

    void startDrawing( QPoint pos );
    void setArea();
    void drawMarkup( QPainter& p );
    void moveTo( QPoint pos );  // Move an item (always make it visible and highlighed)
    bool isOver( QPoint point, QCursor* cursor );
    QPoint origin();
    QPoint getPoint1();
    QPoint getPoint2();
    void tidy();
    QCursor defaultCursor();

private:
    int x;
};

class markupLine : public markupItem
{
public:
    markupLine( imageMarkup* ownerIn, bool interactiveIn, bool reportOnMoveIn );

    void startDrawing( QPoint pos );
    void setArea();
    void drawMarkup( QPainter& p );
    void moveTo( QPoint pos );  // Move an item (always make it visible and highlighed)
    bool isOver( QPoint point, QCursor* cursor );
    QPoint origin();
    QPoint getPoint1();
    QPoint getPoint2();
    void tidy();
    QCursor defaultCursor();

private:
    QPoint start;
    QPoint end;
};

class markupRegion : public markupItem
{
public:

    markupRegion( imageMarkup* ownerIn, bool interactiveIn, bool reportOnMoveIn );

    void startDrawing( QPoint pos );
    void setArea();
    void drawMarkup( QPainter& p );
    void moveTo( QPoint pos );  // Move an item (always make it visible and highlighed)
    bool isOver( QPoint point, QCursor* cursor );
    QPoint origin();
    QPoint getPoint1();
    QPoint getPoint2();
    void tidy();
    QCursor defaultCursor();

private:
    QRect rect;
};

class markupText : public markupItem
{
public:

    markupText( imageMarkup* ownerIn, bool interactiveIn, bool reportOnMoveIn );

    void setText( QString textIn, bool draw );

    void startDrawing( QPoint pos );
    void setArea();
    void drawMarkup( QPainter& p );
    void moveTo( QPoint pos );  // Move an item (always make it visible and highlighed)
    bool isOver( QPoint point, QCursor* cursor );
    QPoint origin();
    QPoint getPoint1();
    QPoint getPoint2();
    void tidy();
    QCursor defaultCursor();

private:
    QString text;
    QRect rect;
};

class imageMarkup {
public:
    imageMarkup();
    virtual ~imageMarkup();

    enum markupIds { MARKUP_ID_REGION,
                     MARKUP_ID_H_SLICE,
                     MARKUP_ID_V_SLICE,
                     MARKUP_ID_LINE,
                     MARKUP_ID_TARGET,
                     MARKUP_ID_BEAM,
                     MARKUP_ID_TIMESTAMP,
                     MARKUP_ID_COUNT,  // must be second last
                     MARKUP_ID_NONE }; // must be last

    void markupMousePressEvent(QMouseEvent *event);
    void markupMouseReleaseEvent ( QMouseEvent* event );
    void markupMouseMoveEvent( QMouseEvent* event );

    void setShowTime( bool visibleIn );     // Display timestamp markup if true
    bool getShowTime();                     // Rturn true if displaying timestamp markup

    markupIds getMode();
    void setMode( markupIds modeIn );
    QImage* markupImage;
    QVector<markupItem*> items;
    QPoint grabOffset;
    QVector<QRect>& getMarkupAreas();
    bool anyVisibleMarkups();
    bool markupAreasStale;
    QCursor getDefaultMarkupCursor();

    void setMarkupTime( QCaDateTime& time );                   // A new image has arrived, note it's time
    void setMarkupColor( markupIds mode, QColor markupColorIn );
    QColor getMarkupColor( markupIds mode );
    QCursor getCircleCursor();
    QCursor getTargetCursor();


protected:
    void markupResize( QSize newSize );   // The viewport size has changed

    virtual void markupChange( QImage& markups, QVector<QRect>& changedAreas )=0;    // The markup overlay has changed, redraw part of it
    virtual void markupSetCursor( QCursor cursor )=0;
    virtual void markupAction( markupIds mode, QPoint point1, QPoint point2 )=0;


private:

    markupIds activeItem;

    markupIds mode;

    void redrawActiveItemHere( QPoint pos );
    QVector<QRect> markupAreas;

    bool buttonDown;
    markupIds getActionMode();

    bool showTime;
    QCursor circleCursor;   // Used as cursoe when over a target or beam markup
    QCursor targetCursor;   // Used as default cursor when in target or beam mode
};

#endif // IMAGEMARKUP_H
