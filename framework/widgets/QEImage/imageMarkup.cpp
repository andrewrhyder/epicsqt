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

// Refer to imageMarkup.h for general module description


#include <imageMarkup.h>

#include <markupTarget.h>
#include <markupBeam.h>
#include <markupHLine.h>
#include <markupVLine.h>
#include <markupLine.h>
#include <markupRegion.h>
#include <markupText.h>

#include <imageContextMenu.h>

// Constructor
imageMarkup::imageMarkup()
{
    mode = MARKUP_ID_NONE;
    activeItem = MARKUP_ID_NONE;

    // set up the font used for notations (and time)
    legendFont = QFont("Courier", 12);
    legendFontMetrics = new QFontMetrics( legendFont );
    legendFont.setStyleStrategy( QFont::NoAntialias );  // drawMarkupOut() leaves artifacts if text is drawn antialias

    markupImage = new QImage();

    items.resize(MARKUP_ID_COUNT );
    items[MARKUP_ID_H_SLICE]   = new markupHLine(  this, true,  true, "slice" );
    items[MARKUP_ID_V_SLICE]   = new markupVLine(  this, true,  true, "slice" );
    items[MARKUP_ID_LINE]      = new markupLine(   this, true,  true, "profile" );
    items[MARKUP_ID_REGION1]   = new markupRegion( this, true,  true, "region 1" );
    items[MARKUP_ID_REGION2]   = new markupRegion( this, true,  true, "region 2" );
    items[MARKUP_ID_REGION3]   = new markupRegion( this, true,  true, "region 3" );
    items[MARKUP_ID_REGION4]   = new markupRegion( this, true,  true, "region 4" );
    items[MARKUP_ID_TARGET]    = new markupTarget( this, true,  true, "target" );
    items[MARKUP_ID_BEAM]      = new markupBeam(   this, true,  true, "beam" );
    items[MARKUP_ID_TIMESTAMP] = new markupText(   this, false, false, "" );

    markupAreasStale = true;

    // Create circle cursoe used for target and beam
    QPixmap circlePixmap = QPixmap( ":/qe/image/circleCursor.png" );
    circleCursor = QCursor( circlePixmap );

    // Create target cursor used for target and beam
    QPixmap targetPixmap = QPixmap( ":/qe/image/targetCursor.png" );
    targetCursor = QCursor( targetPixmap );

    // Create red vertical line cursor used for vertical slice
    QPixmap vSlicePixmap = QPixmap( ":/qe/image/vLineCursor.png" );
    vLineCursor = QCursor( vSlicePixmap );

    // Create green horizontal line cursor used for horizontal slice
    QPixmap hSlicePixmap = QPixmap( ":/qe/image/hLineCursor.png" );
    hLineCursor = QCursor( hSlicePixmap );

    // Create yellow cross cursor used for line profile
    QPixmap profilePixmap = QPixmap( ":/qe/image/lineCursor.png" );
    lineCursor = QCursor( profilePixmap );

    // Create purple cross cursor used for area selection
    QPixmap areaPixmap = QPixmap( ":/qe/image/regionCursor.png" );
    regionCursor = QCursor( areaPixmap );

    // Don't show time on image by default
    showTime = false;
}

// Destructor
imageMarkup::~imageMarkup()
{
}

// Get the current markup mode - (what is the user doing? selecting an area? drawing a line?)
imageMarkup::markupIds imageMarkup::getMode()
{
    return mode;
}

// Set the current markup mode - (what is the user doing? selecting an area? drawing a line?)
void imageMarkup::setMode( markupIds modeIn )
{
    mode = modeIn;
}

//===========================================================================

void imageMarkup::setMarkupTime( QCaDateTime& time )
{
    if( showTime )
    {
        markupText* timeDate = (markupText*)items[MARKUP_ID_TIMESTAMP];
        timeDate->setText( time.text().left( 23 ), showTime );

        QVector<QRect> changedAreas;
        changedAreas.append( timeDate->area );
        markupChange( *markupImage, changedAreas );
    }
}

// Set if time should be shown.
// time is a markup that the user doesn;t interact with. It is just displayed, or not
void imageMarkup::setShowTime( bool showTimeIn )
{
    showTime = showTimeIn;

    // Do nothing more (no need to add or remove time) if no markup image yet
    if( markupImage->isNull() )
        return;

    markupText* timeDate = (markupText*)items[MARKUP_ID_TIMESTAMP];
    if( showTime )
        timeDate->drawMarkupIn();
    else
        timeDate->drawMarkupOut();

    QVector<QRect> changedAreas;
    changedAreas.append( timeDate->area );
    markupChange( *markupImage, changedAreas );
}

// Get if the time is currently being displayed
bool imageMarkup::getShowTime()
{
    return showTime;
}

//===========================================================================

// The image has changed, redraw the markups if any
void imageMarkup::drawMarkups( QPainter& p, const QRect& rect )
{
    int n = items.count();
    for( int i = 0; i < n; i ++ )
    {
        markupItem* item = items[i];
        // If the markup is being displayed, redraw it, and act on its 'new' position
        if( item->visible )
        {
            if( rect.contains( item->area ))

            item->drawMarkupIn( p );
        }
    }
}

//===========================================================================

// User pressed a mouse button
bool imageMarkup::markupMousePressEvent(QMouseEvent *event, bool panning)
{
    // Only act on left mouse button press
    if( !(event->buttons()&Qt::LeftButton ))
        return false;

    // Determine if the user clicked over an interactive, visible item,
    // and if so, make the first item found the active item
    setActiveItem( event->pos() );

    // If in panning mode, then we will not take over the event unless we are over an active item
    // Note, buttonDown is cleared so there is no context of any sort of markup action in progress
    if( panning && activeItem == MARKUP_ID_NONE )
    {
        buttonDown = false;
        return false;
    }

    // Keep track of button state
    buttonDown = true;

    // If not over an item, start creating a new item
    // move the appropriate item to the point clicked on
    if( activeItem == MARKUP_ID_NONE )
    {
        bool pointAndClick = true;
        switch( mode )
        {
            default:
            case MARKUP_ID_NONE:
                break;

            case MARKUP_ID_H_SLICE:
            case MARKUP_ID_V_SLICE:
            case MARKUP_ID_TARGET:
            case MARKUP_ID_BEAM:
                activeItem = mode;
                pointAndClick = true;
                break;

            case MARKUP_ID_LINE:
            case MARKUP_ID_REGION1:
            case MARKUP_ID_REGION2:
            case MARKUP_ID_REGION3:
            case MARKUP_ID_REGION4:
                activeItem = mode;
                pointAndClick = false;
                break;
        }
        if( activeItem != MARKUP_ID_NONE )
        {
            // Some items are point-and-click items. They don't require the user to drag to select where the item is.
            // Typical point-and-click items are vertical lines and horizontal lines (traversing the entire window)
            // Other item are point-press-drag-release items as they require the user to drag to select where the item is.
            // A typical point-press-drag-release item is an area or a line.
            //
            // For a point-and-click item, just redraw it where the user clicks
            if( pointAndClick )
            {
                redrawActiveItemHere( event->pos() );
            }
            // For a point-press-drag-release, erase it if visible,
            // and start the process of draging from the current position
            else
            {
                if( items[activeItem]->visible )
                {
                    items[activeItem]->erase();
                    QVector<QRect> changedAreas;
                    changedAreas.append( items[activeItem]->area );
                    markupChange( *markupImage, changedAreas );
                }
                items[activeItem]->startDrawing( event->pos() );

                // Set the cursor according to the bit we are over after creation
                QCursor cursor;
                if( items[activeItem]->isOver( event->pos(), &cursor ) )
                {
                    markupSetCursor( cursor );
                }
            }
        }
    }

    // Return indicating the event was dealt with
    return true;
}

// Manage the markups as the mouse moves
bool imageMarkup::markupMouseMoveEvent( QMouseEvent* event, bool panning )
{
    // If panning, and we havn't noted a button down for the purposes of image markup, then don't take over this move event
    // (If buttonDown is true then we have already appropriated the button down/move/release for markup purposes)
    if( panning && !buttonDown )
    {
        return false;
    }

    // If the user has the button down, redraw the item in its new position or shape.
    if( buttonDown )
    {
        redrawActiveItemHere( event->pos() );
    }

    // If no button is down, ensure the cursor reflects what it is over
    else
    {
        // Set cursor
        QCursor cursor = getDefaultMarkupCursor();
        int n = items.count();
        for( int i = 0; i < n; i++ )
        {
            QCursor specificCursor;
            if( items[i]->interactive && items[i]->visible && items[i]->isOver( event->pos(), &specificCursor ) )
            {
                cursor = specificCursor;
                break;
            }
        }
        markupSetCursor( cursor );
    }

    // If there is an active item and action is required on move, then report the move
    if( activeItem != MARKUP_ID_NONE && items[activeItem]->reportOnMove )
    {
        markupItem* item = items[activeItem];
        markupAction( getActionMode(), false, false, item->getPoint1(), item->getPoint2(), item->getThickness() );
    }

    // Return indicating the event was appropriated for markup purposes
    return true;
}

// A region of interest value has changed.
// Update any region markup if required
void imageMarkup::markupRegionValueChange( int areaIndex, QRect area )
{
    int region;
    switch( areaIndex )
    {
        case 0:
        default: region = MARKUP_ID_REGION1; break;
        case 1:  region = MARKUP_ID_REGION2; break;
        case 2:  region = MARKUP_ID_REGION3; break;
        case 3:  region = MARKUP_ID_REGION4; break;
    }

    bool isVisible =  items[region]->visible;
    if( isVisible )
    {
        items[region]->drawMarkupOut();
    }

    items[region]->nonInteractiveUpdate( area );
    items[region]->drawMarkupIn();
}

// Return the mode according to the active item.
// Note, this is not the mode as set by setMode(). The mode as set by setMode()
// is what happens when a user initiates action in a part of the display not
// occupied by a markup.
// This mode is related to an existing markup being manipulated.
// For example, if the current mode set by setMode() is MARKUP_MODE_AREA
// (select and area) but the user has draged the profile line the mode
// returned by this method is MARKUP_MODE_LINE
imageMarkup::markupIds imageMarkup::getActionMode()
{
    switch( activeItem )
    {
        case MARKUP_ID_H_SLICE:
        case MARKUP_ID_V_SLICE:
        case MARKUP_ID_LINE:
        case MARKUP_ID_REGION1:
        case MARKUP_ID_REGION2:
        case MARKUP_ID_REGION3:
        case MARKUP_ID_REGION4:
        case MARKUP_ID_TARGET:
        case MARKUP_ID_BEAM:
            return activeItem;

        default:
            return MARKUP_ID_NONE;
    }
}

// Return the default markup cursor (to be displayed when not over any particular markup)
QCursor imageMarkup::getDefaultMarkupCursor()
{
    if( mode < MARKUP_ID_COUNT )
    {
        return items[mode]->defaultCursor();
    }
    else
    {
        return Qt::CrossCursor;
    }
}

// The mouse has been released over the image
bool imageMarkup::markupMouseReleaseEvent ( QMouseEvent*, bool panning  )
{
    // If panning, and we havn't noted a button down for the purposes of image markup, then don't take over this release event
    // (If buttonDown is true then we have already appropriated the button down/move/release for markup purposes)
    if( panning && !buttonDown )
    {
        return false;
    }

    // Determine if an action is now complete
    bool complete;
    switch( activeItem )
    {
        case MARKUP_ID_REGION1:
        case MARKUP_ID_REGION2:
        case MARKUP_ID_REGION3:
        case MARKUP_ID_REGION4:
            complete = true;
            break;

        default:
            complete = false;
            break;
    }

    // If there is an active item, take action
    if( activeItem != MARKUP_ID_NONE )
    {
        markupItem* item = items[activeItem];
        markupAction( getActionMode(), complete, false, item->getPoint1(), item->getPoint2(), item->getThickness() );
    }

    // Flag there is no longer an active item
    activeItem = MARKUP_ID_NONE;
    buttonDown = false;

    // Return indicating the event was appropriated for markup purposes
    return true;
}

// The active item has moved to a new position. Redraw it.
void imageMarkup::redrawActiveItemHere( QPoint pos )
{
    // Do nothing if no active item
    if( activeItem == MARKUP_ID_NONE )
        return;

    // Area to update
    // !!! This is currently just one rectangle that encloses the erased and redrawn object.
    // !!! It could (should?) be a region that includes a single rect for mostly hoz and vert lines,
    // !!! four rects for the four sides of an area, and a number of rectangles that efficiently
    // !!! allows redrawing of diagonal lines
    QVector<QRect> changedAreas;

    // Erase if visible, move, then redraw the item
    if( items[activeItem]->visible )
    {
        changedAreas.append( items[activeItem]->area );
        items[activeItem]->erase();
    }
    items[activeItem]->moveTo( pos );
    items[activeItem]->drawMarkupIn();

    // Extend the changed area to include the item's new area and notify markups require redrawing
    // !!! if the two areas overlap by much, perhaps smarter to join the two into one, or generate the required four?
    changedAreas.append( items[activeItem]->area );
    markupChange( *markupImage, changedAreas );
}

// The viewport size has changed.
// Note, the zoom scale factor parameter is the scale factor for the current
// user zoom level.The scaling calculated below determines the scaling needed
// to convert markups from their current size to the new size.
// For example, the scaling calculated below will be 2.0 when changing from
// 100% zoom to 200% or changing from 200% to 400%
void imageMarkup::markupResize( QSize newSize, double zoomScale )
{
    // Determine scaling that will be applied to the markups.
    // Note, X and Y factors will be close, but may not be exactly the same
    bool rescale;
    double xScale;
    double yScale;
    if( markupImage->isNull() )
    {
        rescale = false;
        xScale = 1.0;   // Not used when rescale is false, but set to avoid compilation warning on windows
        yScale = 1.0;   // Not used when rescale is false, but set to avoid compilation warning on windows
    }
    else
    {
        rescale = true;
        xScale = (double)(newSize.width())  / (double)(markupImage->size().width());
        yScale = (double)(newSize.height()) / (double)(markupImage->size().height());
    }

    // If the markup image is not the right size, create one that is
    if( markupImage->size() != newSize )
    {
        // Delete the old one (may be the initial empty image)
        // Replace it with a new one, and fill it with a completely transparent background
        delete markupImage;
        markupImage = new QImage( newSize, QImage::Format_ARGB32 );
        markupImage->fill( 0 );
    }

    // Rescale and redraw any visible markups
    // Also act on all visible markups. This is required as the new viewport coordinates will need to be retranslated according to the new viewport size.
    // Note, the results will often be identical, but not always, as the new viewport coordinates may not translate to the same pixels in the original image.
    int n = items.count();
    for( int i = 0; i < n; i ++ )
    {
        // If rescaling is possible (if we have a previous image), then rescale
        if( rescale )
        {
            items[i]->scale( xScale, yScale, zoomScale );
        }
        // If the markup is being displayed, redraw it, and act on its 'new' position
        if( items[i]->visible )
        {
            markupItem* item = items[i];

            item->drawMarkupIn();
            markupAction( (markupIds)i, false, false, item->getPoint1(), item->getPoint2(), item->getThickness() );
        }
    }

    // Notify the change
    markupChange( *markupImage, getMarkupAreas() );
}

// Return areas of the image where markups are currently drawn
// Used when redrawing current markups on a new image.
QVector<QRect>& imageMarkup::getMarkupAreas()
{
    if( markupAreasStale )
    {
        markupAreas.clear();
        int n = items.count();
        for( int i = 0; i < n; i ++ )
        {
            if( items[i]->visible )
            {
                markupAreas.append( items[i]->area );
            }
        }
        markupAreasStale = false;
    }
    return markupAreas;
}

// Return true if there are any markups visible.
// Used for more efficiency when updating the image.
bool imageMarkup::anyVisibleMarkups()
{
    int n = items.count();
    for( int i = 0; i < n; i ++ )
    {
        if( items[i]->visible )
        {
            return true;
        }
    }
    return false;
}

// Set the color for a given mode.
// For example, please draw area selection rectangles in green.
void imageMarkup::setMarkupColor( markupIds mode, QColor markupColorIn )
{
    // Do nothing if mode is invalid
    if( mode < 0 || mode >= MARKUP_ID_NONE )
    {
        return ;
    }

    // Save the new markup color
    items[mode]->setColor( markupColorIn );

    // Do nothing (no need to change drawn colors) if no markup image yet
    if( markupImage->isNull() )
        return;

    // If the item is visible, redraw it in the new color
    QVector<QRect> changedAreas;
    if( items[mode]->visible )
    {
        items[mode]->drawMarkupIn();
        changedAreas.append( items[mode]->area );
        markupChange( *markupImage, changedAreas );
    }

}

QColor imageMarkup::getMarkupColor( markupIds mode )
{
    // Return a valid deault color mode is invalid
    if( mode < 0 || mode >= MARKUP_ID_NONE )
    {
        return QColor( 127, 127, 127 );
    }

    // Return the markup color
    return items[mode]->color;
}

// Return the circle cursor use by the image markup system
QCursor imageMarkup::getCircleCursor()
{
    return circleCursor;
}

// Return the target cursor use by the image markup system
QCursor imageMarkup::getTargetCursor()
{
    return targetCursor;
}

// Return the vertical slice cursor use by the image markup system
QCursor imageMarkup::getVLineCursor()
{
    return vLineCursor;
}

// Return the horizontal slice cursor use by the image markup system
QCursor imageMarkup::getHLineCursor()
{
    return hLineCursor;
}

// Return the line profile cursor use by the image markup system
QCursor imageMarkup::getLineCursor()
{
    return lineCursor;
}

// Return the area selection cursor use by the image markup system
QCursor imageMarkup::getRegionCursor()
{
    return regionCursor;
}

// Show the markup context menu if required.
// Do nothing and return false if nothing to do, for example, the position is not over a markup item
// If required, present the menu, act on the user selection, then return true
//
// This method currently populates a imageContextMenu with one 'clear' option.
// Refer to  QEImage::showContextMenu() to see how imageContextMenu can be populated with checkable, and non checkable items, and sub menus
bool imageMarkup::showMarkupMenu( const QPoint& pos, const QPoint& globalPos )
{
    // Determine if the user clicked over an interactive, visible item,
    // and if so, make the first item found the active item
    setActiveItem( pos );

    // If not over an item, do nothing.
    if( activeItem == MARKUP_ID_NONE )
        return false;

    imageContextMenu menu;

    //                      Title                            checkable  checked                 option
    menu.addMenuItem(       "Clear",                         false,     false,                  imageContextMenu::ICM_CLEAR_MARKUP             );

    // If any thickness, add thickness options (zero means item has no concept of thickness)
    if( items[activeItem]->getThickness() )
    {
        menu.addMenuItem(       "Single Pixel Line Thickness",   false,     false,                  imageContextMenu::ICM_THICKNESS_ONE_MARKUP     );
        menu.addMenuItem(       "Select Line Thickness",         false,     false,                  imageContextMenu::ICM_THICKNESS_SELECT_MARKUP  );
    }

    // Present the menu
    imageContextMenu::imageContextMenuOptions option;
    bool checked;
    menu.getContextMenuOption( globalPos, &option, &checked );

    // Act on the menu selection
    switch( option )
    {
        default:
        case imageContextMenu::ICM_NONE:
            break;

        case imageContextMenu::ICM_CLEAR_MARKUP:
        {
            items[activeItem]->erase();
            QVector<QRect> changedAreas;
            changedAreas.append( items[activeItem]->area );
            markupChange( *markupImage, changedAreas );

            markupAction( activeItem, false, true, QPoint(), QPoint(), 0 );

            activeItem = MARKUP_ID_NONE;
            break;
        }


        case imageContextMenu::ICM_THICKNESS_ONE_MARKUP:
        {
            markupItem* item = items[activeItem];
            item->setThickness( 1 );
            markupAction( activeItem, false, false, item->getPoint1(), item->getPoint2(), item->getThickness() );
            break;
        }

        case imageContextMenu::ICM_THICKNESS_SELECT_MARKUP:
            break;
    }

    // Indicate markup menu has been presented
    return true;
}

// Determine if the user clicked over an interactive, visible item,
// and if so, make the first item found the active item
void imageMarkup::setActiveItem( const QPoint& pos )
{
    activeItem = MARKUP_ID_NONE;
    int n = items.count();
    for( int i = 0; i < n; i++ )
    {
        QCursor cursor;
        if( items[i]->interactive && items[i]->visible && items[i]->isOver( pos, &cursor ) )
        {
            activeItem = (markupIds)i;
            grabOffset = pos - items[i]->origin();
            break;
        }
    }
}
