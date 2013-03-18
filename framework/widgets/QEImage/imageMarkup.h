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
        and calls VideoWidget back with any action to take through virtual method markupAction(),
        and calls VideoWidget back with the appropriate cursor through
        In other words, markupChange() is used to signal rendering requirements, markupAction() is used to signal when a task needs to be performed.
        For example, when a user selects an area, markupChange() is called as the selected area moves following the pointer. markupAction() is
        called when selection is over and the application should do something.

 The following exchanges occur when the image changes (generating a paint event), or is resized, or panned:
        When the displayed size of the image changesVideoWidget calls imageMarkup::markupResize()
        When a paint event occurs, VideoWidget calls imageMarkup::anyVisibleMarkups() to determine if any markups need to be displayed.
        When a paint event occurs, VideoWidget calls imageMarkup::getMarkupAreas() to determine what parts of the image need overlaying with markups.
        VideoWidget calls imageMarkup::getDefaultMarkupCursor() to determine what the current cursor should be
        VideoWidget calls imageMarkup::setMarkupTime() to note the time a new image has been presented
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
#include <QFontMetrics>
#include <QFont>
#include <QCaDateTime.h>


#include <QDebug>

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
    void drawMarkupOut();
    void setColor( QColor colorIn );
    void scale( double xScale, double yScale );

    virtual QPoint origin()=0;
    virtual void moveTo( const QPoint pos )=0;  // Move an item (always make it visible and highlighed)
    virtual void startDrawing( const QPoint pos ) = 0;
    virtual bool isOver( const QPoint point, QCursor* cursor )=0;
    virtual QCursor cursorForHandle( const markupItem::markupHandles handle ) = 0;
    virtual QPoint getPoint1()=0;
    virtual QPoint getPoint2()=0;
    virtual unsigned int getThickness()=0;
    virtual void setThickness( const unsigned int thicknessIn )=0;
    virtual QCursor defaultCursor()=0;
    virtual void nonInteractiveUpdate( QRect ) {} // Only implemented by those objects that are updated by data such as region of interest

    QRect         area;         // Area object occupies, used for repainting, and actual object coordinates where appropriate
    bool          visible;      // Object is visible to the user
    bool          interactive;  // Object can be moved by the user
    bool          reportOnMove; // Movements reported (not just on move completion)
    QColor        color;            // Color markup is drawn in


protected:
    markupHandles activeHandle;
    virtual void setArea()=0;
    virtual void drawMarkup( QPainter& p )=0;
    bool pointIsNear( QPoint p1, QPoint p );
    isOverOptions isOverType;
    QColor getColor();

    bool          highlighted;  // Object is highlighted
    int           highlightMargin; // Extra margin required for highlighting
    imageMarkup*  owner;

    const QString getLegend();                    // Return the string used to notate the markup
    const QSize getLegendSize();                  // Return the size of the string used to notate the markup
    void addLegendArea();                         // Add the legend area to the markup area
    enum legendJustification{ ABOVE_RIGHT, BELOW_LEFT, BELOW_RIGHT };          // Options for positioning the legend
    const QPoint setLegendPos( QPoint pos, legendJustification just );          // Sets (and returns) the position of the legend (top left of text) given the justificaiton
    const QPoint getLegendPos();                  // Returns the last drawn legend position
    void drawLegend( QPainter& p, QPoint pos, legendJustification just );   // Draw the legend beside the markup
    QPoint limitPointToImage( const QPoint pos );   // Return the input point limited to the image area

private:
    virtual void scaleSpecific( const double xScale, const double yScale )=0;
    QString legend;                               // Text displayed beside markup
    QSize legendSize;                             // Size of legend (according to legend font)
    bool hasLegend();                             // Returns true if legend text is present
    void setLegend( const QString legendIn );     // Set the string used to notate the markup (and the calculate its size)
    QPoint legendPos;                             // Last drawn legend position
};

class markupTarget : public markupItem
{
public:

    markupTarget( imageMarkup* ownerIn, const bool interactiveIn, const bool reportOnMoveIn, const QString legendIn );

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
    void scaleSpecific( const double xScale, const double yScale );

private:
    QPoint pos;
};

class markupBeam : public markupItem
{
public:

    markupBeam( imageMarkup* ownerIn, const bool interactiveIn, const bool reportOnMoveIn, const QString legendIn );

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
    void scaleSpecific( const double xScale, const double yScale );

private:
    QPoint pos;
    int armSize;    // Length of arms in cross hair
};

class markupHLine : public markupItem
{
public:

    markupHLine( imageMarkup* ownerIn, const bool interactiveIn, const bool reportOnMoveIn, const QString legendIn );

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
    void scaleSpecific( const double xScale, const double yScale );

private:
    int y;
    unsigned int thickness;
};

class markupVLine : public markupItem
{
public:

    markupVLine( imageMarkup* ownerIn, const bool interactiveIn, const bool reportOnMoveIn, const QString legendIn );

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
    void scaleSpecific( const double xScale, const double yScale );

private:
    int x;
    unsigned int thickness;
};

class markupLine : public markupItem
{
public:
    markupLine( imageMarkup* ownerIn, const bool interactiveIn, const bool reportOnMoveIn, const QString legendIn );

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
    void scaleSpecific( const double xScale, const double yScale );

private:
    QPoint start;
    QPoint end;
    unsigned int thickness;
    bool isOverLine( const QPoint point, const QPoint lineStart, const QPoint lineEnd );
};

class markupRegion : public markupItem
{
public:

    markupRegion( imageMarkup* ownerIn, const bool interactiveIn, const bool reportOnMoveIn, const QString legendIn );

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
    void scaleSpecific( const double xScale, const double yScale );

    void nonInteractiveUpdate( QRect );


private:
    QRect rect;
};

class markupText : public markupItem
{
public:

    markupText( imageMarkup* ownerIn, const bool interactiveIn, const bool reportOnMoveIn, const QString legendIn );

    void setText( QString textIn, bool draw );

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
    void scaleSpecific( const double xScale, const double yScale );

private:
    QString text;
    QRect rect;
};

class imageMarkup {
public:
    imageMarkup();
    virtual ~imageMarkup();

    // IDs to indicate what mode the markup system is in (for example, marking out an area), and to identify each of the markup items.
    enum markupIds { MARKUP_ID_REGION1,
                     MARKUP_ID_REGION2,
                     MARKUP_ID_REGION3,
                     MARKUP_ID_REGION4,
                     MARKUP_ID_H_SLICE,
                     MARKUP_ID_V_SLICE,
                     MARKUP_ID_LINE,
                     MARKUP_ID_TARGET,
                     MARKUP_ID_BEAM,
                     MARKUP_ID_TIMESTAMP,
                     MARKUP_ID_COUNT,  // must be second last
                     MARKUP_ID_NONE }; // must be last

    void setShowTime( bool visibleIn );     // Display timestamp markup if true
    bool getShowTime();                     // Return true if displaying timestamp markup

    markupIds getMode();                    // Return the current markup mode - (what is the user doing? selecting an area? drawing a line?)
    void setMode( markupIds modeIn );       // Set the current markup mode - (what is the user doing? selecting an area? drawing a line?)

    void setMarkupColor( markupIds mode, QColor markupColorIn );    // Set the color for a given markup.
    QColor getMarkupColor( markupIds mode );                        // Get the color for a given markup.

    bool showMarkupMenu( const QPoint& pos, const QPoint& globalPos );// Show the markup menu if required

    void markupRegionValueChange( int areaIndex, QRect area ); // Region of interest data has changed. Change region areas to match

    // The following are only public so they may be accessed by (internal) markup items.
    QImage* markupImage;                                        // Image used to draw markups in. Relevent areas will be copied over updating image
    QVector<markupItem*> items;                                 // List of markup items
    QPoint grabOffset;                                          // Offset between a markup item origin, and where the user grabbed it
    bool markupAreasStale;                                      // True if 'markupAreas' is no longer up to date
    QCursor getCircleCursor();                                  // Returns a circular cursor
    QCursor getTargetCursor();                                  // Returns a target cursor
    QCursor getVLineCursor();                                  // Returns a vertical line cursor
    QCursor getHLineCursor();                                  // Returns a horizontal line cursor
    QCursor getLineCursor();                                 // Returns a profile line cursor
    QCursor getRegionCursor();                                    // Returns a region cursor

    virtual void markupSetCursor( QCursor cursor )=0;           // Inform the VideoWidget that that the cursor should change
    QFont legendFont;                                           // Font used to notate markups (and for time)
    QFontMetrics* legendFontMetrics;                            // Size info about legendFont;

protected:
    bool anyVisibleMarkups();                                   // Are there any markups visible
    QVector<QRect>& getMarkupAreas();                           // Get the visible areas contining markups
    QCursor getDefaultMarkupCursor();                           // Get the cursor appropriate for the current markup

    void setMarkupTime( QCaDateTime& time );                    // A new image has arrived, note it's time

    bool markupMousePressEvent(QMouseEvent *event, bool panning);      // User has pressed a button
    bool markupMouseReleaseEvent ( QMouseEvent* event, bool panning ); // User has released a button
    bool markupMouseMoveEvent( QMouseEvent* event, bool panning );     // User has moved the mouse

    void markupResize( QSize newSize );                         // The viewport size has changed


    virtual void markupChange( QImage& markups, QVector<QRect>& changedAreas )=0;    // The markup overlay has changed, redraw part of it
    virtual void markupAction( markupIds mode, bool complete, bool clearing, QPoint point1, QPoint point2, unsigned int thickness )=0;     // There is an application task to do in response to user interaction with the markups

private:
    void setActiveItem( const QPoint& pos );    // // Determine if the user clicked over an interactive, visible item
    markupIds activeItem;                       // Current markup being interacted with
    markupIds mode;                             // Current operation
    void redrawActiveItemHere( QPoint pos );    // The active item has moved to a new position. Redraw it.
    QVector<QRect> markupAreas;                 // Areas occupied by markups. Each markup may occupy one or more areas.

    bool buttonDown;                            // True while left button is pressed
    markupIds getActionMode();                  // Return the mode according to the active item.

    bool showTime;                              // True if the time is being displayed
    QCursor circleCursor;                       // Used as default cursor when over a target or beam markup
    QCursor targetCursor;                       // Used as default cursor when in target or beam mode
    QCursor vLineCursor;                        // Used as default cursor when in vertical slice mode
    QCursor hLineCursor;                        // Used as default cursor when in horizontal slicemode
    QCursor lineCursor;                         // Used as default cursor when in line profile mode
    QCursor regionCursor;                       // Used as default cursor when in area selection mode

};

#endif // IMAGEMARKUP_H
