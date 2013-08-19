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
 This class manages the low level presentation of images in a display widget and user interact with the image.
 The image is delivered as a QImage ready for display. There is no need to flip, rotate, clip, etc.
 This class manages zooming the image simply by setting the widget size as required and drawing into it. Qt then performs the scaling required.
 */

#include "videowidget.h"
#include <QPainter>

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent)
{
    panning = false;

    setAutoFillBackground(false);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    setMouseTracking( true );
    setCursor( getDefaultMarkupCursor() );

    setAttribute( Qt::WA_OpaquePaintEvent, true );
    update();// !!!required???
}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::markupSetCursor( QCursor cursor )
{
    setCursor( cursor );
}

// Ensure we have a reference image and it is the same size as the display
// Returns true if a new reference image was created
// Returns false if no new image was required.
bool VideoWidget::createRefImage()
{
    if( refImage.isNull() || refImage.size() != size() )
    {
        refImage = QImage( size(), QImage::Format_RGB32 );
        return true;
    }
    else
    {
        return false;
    }
}

// The displayed image has changed, redraw it
void VideoWidget::setNewImage( const QImage image, QCaDateTime& time )
{
    // Take a copy of the current image
    // (cheap - creates a shallow copy)
    currentImage = image;

    // Create a reference image the same as the display
    // This is cheap if the display is the same size as the image - a shallow copy is done.
    // It is more expensive if different sizes as a scaled version is created.
    if( image.size() == size() )
    {
        // Use the input image as the reference image
        // (cheap - creates a shallow copy)
        refImage = image;
    }
    else
    {
        // Ensure we have a reference image and it is the correct size
        createRefImage();

        // Draw the scaled image into the reference image
        QPainter refPainter( &refImage );
        refPainter.drawImage( refImage.rect(), image, image.rect() );
    }

    // Note the time for markups
    setMarkupTime( time );

    // Cause a repaint with the new image
    update();
}

// The markups have changed redraw the required parts
void VideoWidget::markupChange( QVector<QRect>& changedAreas )
{
    // Start accumulating the changed areas
    QRect nextRect = changedAreas[0];

    // For each additional area, accumulate it, or draw the
    // areas accumulated so far and start a new accumulation.
    for( int i = 1; i < changedAreas.count(); i++ )
    {
        // Determine the total pixel area if the next rectangle united with the area accumulated so far
        QRect unitedRect = nextRect.united( changedAreas[i] );
        int unitedArea = unitedRect.width() * unitedRect.height();

        // Determine the total pixel area if the next rectangle is drawn seperately
        int totalArea = nextRect.width() * nextRect.height() + changedAreas[i].width() * changedAreas[i].height();

        // If it is more efficient to draw the area accumulated so far
        // seperately from the next rectangle, then draw the area accumulated
        // so far and start a fresh accumulation.
        if( totalArea < unitedArea )
        {
            update( nextRect );
            nextRect = changedAreas[i];
        }
        // If it is more efficient to unite the area accumulated so far with the
        // next rectangle, do this
        else
        {
            nextRect = unitedRect;
        }
    }

    // Draw the last accumulated area
    update( nextRect );
}

// Manage a paint event in the video widget
void VideoWidget::paintEvent(QPaintEvent* event )
{
    // Ensure there is a reference image.
    // If this creates one then there has never been an update yet, fill it with black. This is likely
    // to be the first paint event occuring at creation before an image update has arrived.
    if( createRefImage() )
    {
        QPainter refPainter( &refImage );
        QColor bg(0, 0, 0, 255);
        refPainter.fillRect(rect(), bg);
    }

    // Build a painter and only bother about the changed area
    QPainter painter(this);
    painter.setClipRect( event->rect() );

    // Update the display with the reference image.
    painter.drawImage( event->rect(), refImage, event->rect() );

    // Update any markups
    drawMarkups( painter, event->rect() );

    // Report position for pixel info logging
    emit currentPixelInfo( pixelInfoPos );
}

// Manage a resize event
void VideoWidget::resizeEvent( QResizeEvent *event )
{
    // Ensure the markups match the new size
    markupResize( event->size(), event->oldSize(), getScale() );
}

// Act on a markup change
void VideoWidget::markupAction( markupIds mode, bool complete, bool clearing, QPoint point1, QPoint point2, unsigned int thickness )
{
    emit userSelection( mode, complete, clearing, point1, point2, thickness );
}

// Return a point from the displayed image as a point in the original image
QPoint VideoWidget::scalePoint( QPoint pnt )
{
    QPoint scaled;
    scaled.setX( scaleOrdinate( pnt.x() ));
    scaled.setY( scaleOrdinate( pnt.y() ));
    return scaled;
}

// Return a point from the original image as a point in the displayed image
QPoint VideoWidget::scaleImagePoint( QPoint pnt )
{
    QPoint scaled;
    scaled.setX( scaleImageOrdinate( pnt.x() ));
    scaled.setY( scaleImageOrdinate( pnt.y() ));
    return scaled;
}

// Return an ordinate from the displayed image as an ordinate in the original image
int VideoWidget::scaleOrdinate( int ord )
{
    return (int)((double)ord / getScale());
}

// Return an ordinate from the original image as an ordinate in the displayed image
int VideoWidget::scaleImageOrdinate( int ord )
{
    return (int)((double)ord * getScale());
}

// Return the scale of the displayed image
double VideoWidget::getScale()
{
    // If for any reason a scale can't be determined, return scale of 1.0
    if( currentImage.isNull() || currentImage.width() == 0 || width() == 0)
        return 1.0;

    // Return the horizontal scale of the displayed image
    return (double)width() / (double)currentImage.width();
}

// The mouse has been pressed over the image
void VideoWidget::mousePressEvent( QMouseEvent* event)
{
    // Only act on left mouse button press
    if( !(event->buttons()&Qt::LeftButton) )
        return;

    // Pass the event to the markup system. It will use it if appropriate.
    // If it doesn't use it, then start a pan if panning
    // Note, the markup system will take into account if panning.
    // When panning, the markup system will not use the event unless actually over a markup.
    if( !markupMousePressEvent( event, panning ) && panning )
    {
        setCursor( Qt::ClosedHandCursor );
        panStart = event->pos();
    }
}

// The mouse has been released over the image
void VideoWidget::mouseReleaseEvent ( QMouseEvent* event )
{
    // Pass the event to the markup system. It will use it if appropriate.
    // If it doesn't use it, then complete panning.
    // Note, the markup system will take into account if panning.
    // When panning, the markup system will not use the event unless moving a markup.
    if( !markupMouseReleaseEvent( event, panning ) && panning )
    {
        setCursor( Qt::OpenHandCursor );
        emit pan( pos() );
    }
}

// The wheel has been moved over the image
void VideoWidget::wheelEvent( QWheelEvent* event )
{
    int zoomAmount = event->delta() / 12;
    emit zoomInOut( zoomAmount );
}

//Manage a mouse move event
void VideoWidget::mouseMoveEvent( QMouseEvent* event )
{
    // Report position for pixel info logging
    pixelInfoPos.setX( int ( (double)(event->pos().x()) / getScale() ) );
    pixelInfoPos.setY( int ( (double)(event->pos().y()) / getScale() ) );
    emit currentPixelInfo( pixelInfoPos );

    // Pass the event to the markup system. It will use it if appropriate.
    // If it doesn't use it, then pan if panning.
    // Note, the markup system will take into account if panning.
    // When panning, the markup system will not use the event unless moving a markup.
    if( !markupMouseMoveEvent( event, panning ) && panning )
    {
        if( event->buttons()&Qt::LeftButton)
        {
            // Determine a new position that will keep the same point in the image under the mouse
            QPoint newPos = pos() - ( panStart - event->pos() ) ;

            // Limit panning. Don't pan beyond the image
            QWidget* p = this->parentWidget();
            if( newPos.x() <  p->width() - width() )
                newPos.setX( p->width() - width());
            if( newPos.y() < p->height() - height() )
                newPos.setY( p->height() - height() );

            if( newPos.x() > 0 )
                newPos.setX( 0 );
            if( newPos.y() > 0 )
                newPos.setY( 0 );

            // Do the pan
            move( newPos );
        }
    }
}

// The video widget handles panning.
// Return if currently panning.
bool VideoWidget::getPanning()
{
    return panning;
}

// The video widget handles panning.
// Tell the video widget it is currently panning.
void VideoWidget::setPanning( bool panningIn )
{
    panning = panningIn;
    if( panning )
    {
        setCursor( Qt::OpenHandCursor );
    }
}
