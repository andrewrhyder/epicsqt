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


#include <QSize>
#include <QPoint>
#include <QLine>
#include <QRect>
#include <QMouseEvent>
#include <QImage>

#include <QDebug>


#ifndef IMAGEMARKUP_H
#define IMAGEMARKUP_H



class markupItem
{
public:
    enum isOverOptions{ OVER_LINE, OVER_BORDER, OVER_AREA }; // test required to determine if pointer is over the object

    markupItem( QImage** imageIn, isOverOptions over, bool interactiveIn );
    ~markupItem(){}

    void setHighlight( bool highlightIn );
    void setActive( bool activeIn );
    bool isActive();

    virtual void drawMarkup()=0;

    bool isOver( QPoint point );



    isOverOptions isOverType;
    QRect         area;
    bool          active;
    bool          visible;
    bool          highlight;
    bool          interactive;
    QImage**      image;
};

class markupHLine : public markupItem
{
public:

    markupHLine( QImage** imageIn, bool interactiveIn );

    void setPos( int yIn );
    void drawMarkup();

    int y;
};

class markupVLine : public markupItem
{
public:

    markupVLine( QImage** imageIn, bool interactiveIn );

    void setPos( int xIn );
    void drawMarkup();

    int x;
};

class markupLine : public markupItem
{
public:

    markupLine( QImage** imageIn, bool interactiveIn );

    void setPos( QPoint startIn, QPoint endIn );
    void drawMarkup();

    QPoint start;
    QPoint end;
};

class markupText : public markupItem
{
public:

    markupText( QImage** imageIn, bool interactiveIn );

    void setText( QString textIn );
    void setPos( QRect areaIn );
    void drawMarkup();

    QString text;
};

class imageMarkup {
public:
    imageMarkup();
    ~imageMarkup();

    void markupMousePressEvent(QMouseEvent *event);
    void markupMouseReleaseEvent ( QMouseEvent* event );
    void markupMouseMoveEvent( QMouseEvent* event );
    void markupMouseWheelEvent( QWheelEvent* event );

protected:
    void markupResize( QSize newSize );   // The viewport size has changed
    void markupScroll( QPoint newPos ); // The underlying image has moved in the viewport
    void markupZoom( double newZoom ); // The underlying image zoom factor has changed

    virtual void markupChange( QImage& markups, QRect changedArea );    // The markup overlay has changed, redraw part of it
    QImage* markupImage;
private:

    enum interactiveStates { WAITING, MOVING };

    interactiveStates interaction;

    markupHLine* lineRoiTop;
    markupHLine* lineRoiBottom;
    markupVLine* lineRoiLeft;
    markupVLine* lineRoiRight;

    markupLine*  lineProfile;

    markupText*  timeDate;

    QList<markupItem*> items;

};

#endif // IMAGEMARKUP_H
