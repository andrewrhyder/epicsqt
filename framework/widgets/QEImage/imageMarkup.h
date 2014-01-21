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
    markupEllipse
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

#ifndef IMAGEMARKUP_H
#define IMAGEMARKUP_H

#include <QCaDateTime.h>

#include <markupItem.h>
#include <QMouseEvent>
#include <QCursor>

#include <QDebug>

// This class manages markups on an image.
// It manages:
//    - user interaction with the image
//    - resizing of markups as the image is zoomed
//    - drawing of markups
//    - provision of information such as what areas of the image are affected by markups.
//    -
//    -
//    -
//    -


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
                     MARKUP_ID_ELLIPSE,
                     MARKUP_ID_COUNT,  // must be second last
                     MARKUP_ID_NONE }; // must be last

    void setShowTime( bool visibleIn );     // Display timestamp markup if true
    bool getShowTime();                     // Return true if displaying timestamp markup

    markupIds getMode();                    // Return the current markup mode - (what is the user doing? selecting an area? drawing a line?)
    void setMode( markupIds modeIn );       // Set the current markup mode - (what is the user doing? selecting an area? drawing a line?)

    void setMarkupColor( markupIds mode, QColor markupColorIn );    // Set the color for a given markup.
    QColor getMarkupColor( markupIds mode );                        // Get the color for a given markup.

    bool showMarkupMenu( const QPoint& pos, const QPoint& globalPos );// Show the markup menu if required

    void markupRegionValueChange( int areaIndex, QRect area, bool displayMarkups );  // Region of interest data has changed. Change markups to match
    void markupHProfileChange( int y, bool displayMarkups );                         // Horizontal profile data has changed. Change markups to match
    void markupVProfileChange( int x, bool displayMarkups );                         // Horizontal profile data has changed. Change markups to match
    void markupLineProfileChange( QPoint start, QPoint end, bool displayMarkups );   // Arbitrary profile data has changed. Change markups to match
    void markupTargetValueChange( QPoint point, bool displayMarkups );               // Target position data has changed. Change markup to match
    void markupBeamValueChange( QPoint point, bool displayMarkups );                 // Beam position data has changed. Change markup to match
    void markupEllipseValueChange( QPoint point1, QPoint point2, bool displayMarkups ); // Ellipse position data has changed. Change markup to match

    void markupValueChange( int markup, bool displayMarkups, QPoint p1, QPoint p2 = QPoint() ); // A markup related value has changed. Change markups to match

    // The following are only public so they may be accessed by (internal) markup items.
    QVector<markupItem*> items;                                 // List of markup items
    QPoint grabOffset;                                          // Offset between a markup item origin, and where the user grabbed it
    bool markupAreasStale;                                      // True if 'markupAreas' is no longer up to date
    QCursor getCircleCursor();                                  // Returns a circular cursor
    QCursor getTargetCursor();                                  // Returns a target cursor
    QCursor getVLineCursor();                                   // Returns a vertical line cursor
    QCursor getHLineCursor();                                   // Returns a horizontal line cursor
    QCursor getLineCursor();                                    // Returns a profile line cursor
    QCursor getRegionCursor();                                  // Returns a region cursor

    virtual void markupSetCursor( QCursor cursor )=0;           // Inform the VideoWidget that that the cursor should change
    QFont legendFont;                                           // Font used to notate markups (and for time)
    QFontMetrics* legendFontMetrics;                            // Size info about legendFont;

    void setMarkupLegend( markupIds mode, QString legend );       // Set the markup legend (for example, area 1 markup might be called 'ROI 1')
    QString getMarkupLegend( markupIds mode );                   // Get the markup legend

protected:
    void drawMarkups( QPainter& p, const QRect& rect );         // The image has changed, redraw the markups if any
    bool anyVisibleMarkups();                                   // Are there any markups visible
    QCursor getDefaultMarkupCursor();                           // Get the cursor appropriate for the current markup

    void setMarkupTime( QCaDateTime& time );                    // A new image has arrived, note it's time

    bool markupMousePressEvent(QMouseEvent *event, bool panning);      // User has pressed a button
    bool markupMouseReleaseEvent ( QMouseEvent* event, bool panning ); // User has released a button
    bool markupMouseMoveEvent( QMouseEvent* event, bool panning );     // User has moved the mouse

    void markupResize( const QSize& newSize, const QSize& oldSize, const double scale );           // The viewport size has changed


    virtual void markupChange( QVector<QRect>& changedAreas )=0;    // The markup overlay has changed, redraw part of it
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
