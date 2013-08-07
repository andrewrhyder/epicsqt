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
 This class manages the markups that are overlayed of an image, such as region of interest, line, graticule, time and date, etc.
 The class also handles user interaction with the markups, such as creation and draging.
 The class works at the display resolution of the image, but also understands the actual resolution of
 the underlying image and can describe markups in terms of the underlying image.

 There are several markup classes used by imageMarkup, all based on the markupItem class. They are:
    markupTarget
    markupBeam
    markupHLine
    markupVLine
    markupLine
    markupRegion
    markupText
 All these classes are are included in this module

 This module draws markups when interacting with the user, and also when the image changes.

 Interaction with user is as follows:
        On mouse events VideoWidget calls  imageMarkup::markupMousePressEvent(),
                                           imageMarkup::markupMouseReleaseEvent()
                                           imageMarkup::markupMouseMoveEvent()

        imageMarkup then calls VideoWidget back with any image changes required through virtual method markupChange(),
        and calls VideoWidget back with any action to take through the virtual method markupAction().
        In other words, markupChange() is used to signal rendering requirements, markupAction() is used to signal when a task needs to be performed.
        For example, when a user selects an area, markupChange() is called as the selected area moves following the pointer. markupAction() is
        called when selection is over and the application should do something.

 The following exchanges occur when the image changes (generating a paint event), or is resized, or panned:
        When the displayed size of the image changesVideoWidget calls imageMarkup::markupResize().
        When a paint event occurs, VideoWidget calls imageMarkup::anyVisibleMarkups() to determine if any markups need to be displayed.
        When a paint event occurs, VideoWidget calls imageMarkup::getMarkupAreas() to determine what parts of the image need overlaying with markups.
        VideoWidget calls imageMarkup::getDefaultMarkupCursor() to determine what the current cursor should be.
        VideoWidget calls imageMarkup::setMarkupTime() to note the time a new image has been presented.
*/

#ifndef MARKUPITEM_H
#define MARKUPITEM_H

#include <QPainter>

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

// Allowable distance in pixels from object which will still be considered 'over'
#define OVER_TOLERANCE 6
#define HANDLE_SIZE 6

// Profile thickness selection maximum
#define THICKNESS_MAX 51


class imageMarkup;

// Generic markup item.
// Each type of markup (line, area, etc) is based on this class.
class markupItem
{
protected:
    enum isOverOptions{ OVER_LINE, OVER_BORDER, OVER_AREA }; // test required to determine if pointer is over the object
    markupItem( imageMarkup* ownerIn, const isOverOptions over, const bool interactiveIn, const bool reportOnMoveIn, const QString legendIn );
    virtual ~markupItem();

public:
    enum markupHandles { MARKUP_HANDLE_NONE, // Over a markup, but not over any specific handle of the markup
                         MARKUP_HANDLE_START, MARKUP_HANDLE_END, MARKUP_HANDLE_CENTER,  // Lines
                         MARKUP_HANDLE_TL, MARKUP_HANDLE_TR, MARKUP_HANDLE_BL, MARKUP_HANDLE_BR, // Area corners
                         MARKUP_HANDLE_T, MARKUP_HANDLE_B, MARKUP_HANDLE_L, MARKUP_HANDLE_R };   // Area sides
    void erase();                // Erase and item and redraw any items that it was over (note, this does not change its status. For example, it is used if hiding an item, but also when moving an item)
    void drawMarkupIn();
    void drawMarkupIn( QPainter& p );
    void drawMarkupOut();
    void setColor( QColor colorIn );
    void scale( const double xScale, const double yScale, const double zoomScale );

    virtual QPoint origin()=0;                                                      // Origin of the markup, for example, the center of a target
    virtual void   moveTo( const QPoint pos )=0;                                    // Move an item (always make it visible and highlighed)
    virtual void   startDrawing( const QPoint pos ) = 0;                            // Action to take when the user starts drawing a markup, such as note the starting point of a line
    virtual bool   isOver( const QPoint point, QCursor* cursor )=0;                 // Returns true if the cursor is over a markup. Generally, 'over' means near the
                                                                                    // visual parts of the markup. For example in the center of a rectangle is not 'over' near
                                                                                    // the lines of the rectangle is 'over'.
    virtual QCursor cursorForHandle( const markupItem::markupHandles handle ) = 0;  // Returns the cursor for the specified handle. For example, horizontal arrows when over a rectangle
                                                                                    // left or right side handle
    virtual QPoint       getPoint1()=0;                                             // Return the first point of the markup (starting point for a line, top left corner for a rectangle, etc)
    virtual QPoint       getPoint2()=0;                                             // Return the second point of the markup (end point for a line, bottom right corner for a rectangle, etc)
    virtual unsigned int getThickness()=0;                                          // Return the thickness of a markup where relevent. For example the thickness of a profile line
    virtual void         setThickness( const unsigned int thicknessIn )=0;          // Set the thickness of a markup where relevent.
    virtual QCursor      defaultCursor()=0;                                         // Return the default cursor for the markup.
    virtual void         nonInteractiveUpdate( QRect ) {}                           // Only implemented by those objects that are updated by data such as region of interest

    QRect         area;         // Area object occupies, used for repainting, and actual object coordinates where appropriate
    bool          visible;      // Object is visible to the user
    bool          interactive;  // Object can be moved by the user
    bool          reportOnMove; // Movements reported (not just on move completion)
    QColor        color;        // Color markup is drawn in

protected:
    markupHandles activeHandle;                     // The current handle the user is over
    virtual void  setArea()=0;                       // Update the total rectangular area occupied by the markup
    virtual void  drawMarkup( QPainter& p )=0;       // Draw the markup
    bool          pointIsNear( QPoint p1, QPoint p );        // Returns true of point p1 is close to point p
    QColor        getColor();                              // Return the colour used for this markup

    imageMarkup*  owner;                          // Class contining this markup instance

    const QString getLegend();                    // Return the string used to notate the markup
    const QSize getLegendSize();                  // Return the size of the string used to notate the markup
    void addLegendArea();                         // Add the legend area to the markup area

    enum  legendJustification{ ABOVE_RIGHT, BELOW_LEFT, BELOW_RIGHT };      // Options for positioning the legend
    const  QPoint setLegendPos( QPoint pos, legendJustification just );     // Sets (and returns) the position of the legend (top left of text) given the justificaiton
    const  QPoint getLegendPos();                                           // Returns the last drawn legend position
    void   drawLegend( QPainter& p, QPoint pos, legendJustification just ); // Draw the legend beside the markup
    QPoint limitPointToImage( const QPoint pos );                           // Return the input point limited to the image area

private:
    virtual void scaleSpecific( const double xScale, const double yScale, const double zoomScale )=0;   // Scale the markup for presentation at different zoom levels
    QString      legend;                                // Text displayed beside markup
    QSize        legendSize;                            // Size of legend (according to legend font)
    bool         hasLegend();                           // Returns true if legend text is present
    void         setLegend( const QString legendIn );   // Set the string used to notate the markup (and the calculate its size)
    QPoint       legendPos;                             // Last drawn legend position
};

#endif // MARKUPITEM_H
