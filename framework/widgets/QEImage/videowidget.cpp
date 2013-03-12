
#include "videowidget.h"
#include <QPainter>

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent)
{
    compositeImageBackground = NULL;
    compositeImage = NULL;

    panning = false;

    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_PaintOnScreen, true);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    firstUpdate = true;

    setMouseTracking( true );
    setCursor( getDefaultMarkupCursor() );

    setAttribute( Qt::WA_OpaquePaintEvent, true );
    update();// !!!required???
}

VideoWidget::~VideoWidget()
{
    if( compositeImageBackground )
    {
        delete compositeImageBackground;
    }
}

void VideoWidget::markupSetCursor( QCursor cursor )
{
    setCursor( cursor );
}

// The displayed image has changed, redraw it
void VideoWidget::setNewImage( const QImage image, QCaDateTime& time )
{
    currentImage = image;
    compositeImageBackgroundStale = true;
    setMarkupTime( time );
    update();
}

// The markup overlay has changed, redraw part of it
// !! Call this with a null markups image if no markups
void VideoWidget::markupChange( QImage& markups,  QVector<QRect>& changedAreas )
{
    // Save the current markups. The markups image is null if no markups
    markupImage = markups;
    for( int i = 0; i < changedAreas.count(); i++ )
    {
        update( changedAreas[i] );
    }
}

// Manage a paint event in the video widget
void VideoWidget::paintEvent(QPaintEvent* event )
{
    // If this is the first paint event, and there is no image to display, fill it with black
    QPainter painter(this);

    if( firstUpdate || currentImage.isNull() )
    {
        QColor bg(0, 0, 0, 255);
        painter.fillRect(rect(), bg);
    }

    // If there is an image to display, paint the appropriate parts
    else
    {
        // If there are no markups, and the entire image is being drawn, just display the current image
        if( !anyVisibleMarkups() && event->rect() == rect() )
        {
            painter.drawImage( event->rect(), currentImage, currentImage.rect() );
        }

        // If there are markups, or only a part of the display is being painted,
        // draw the appropriate current image overlayed with the appropriate markups
        else
        {
            // If there is a composite background, but it is a different size to the display widget, delete it.
            if( compositeImageBackground && compositeImageBackground->size() != size())
            {
                delete compositeImageBackground;
                compositeImageBackground = NULL;
            }

            // If there is a composite image, but it is a different size to the display widget, delete it.
            if( compositeImage && compositeImage->size() != size())
            {
                delete compositeImage;
                compositeImage = NULL;
            }

            // If there is no composite image (because there never has been one, or we have just deleted it) then create it.
            if( !compositeImage )
            {
                compositeImage = new QImage( size(), currentImage.format() );
            }

            // Flag if a composite background image is needed.
            // (If the current image is the same size as the display widget, then the current
            // image can be used directly for markup backgrounds)
            bool usingCompositeBackground = currentImage.size() != size();

            // If there is no composite background and one is required then create a composite background.
            if( !compositeImageBackground && usingCompositeBackground )
            {
                compositeImageBackground = new QImage( size(), currentImage.format() );
                compositeImageBackgroundStale = true;
            }

            // If using a composite background, and it isn't up to date, refresh it
            if( usingCompositeBackground && compositeImageBackgroundStale )
            {
                QPainter bgPainter( compositeImageBackground );
                bgPainter.drawImage( compositeImageBackground->rect(), currentImage, currentImage.rect() );
                compositeImageBackgroundStale = false;
            }

            // Draw the required background part.
            // The background part must be taken from an image scaled to the drawing widget
            // If the current image is the same size, then it will be used. If not, then the
            // compositeImageBackground image will have been set up and will be used.
            QPainter compPainter( compositeImage );
            if( usingCompositeBackground )
            {
                compPainter.drawImage( event->rect(), *compositeImageBackground, event->rect() );
            }
            else
            {
                compPainter.drawImage( event->rect(), currentImage, event->rect() );
            }

            // Draw markups for the required section
            // Repainting the markup image as follows works fine:
            //    painter.drawImage(  event->rect(), markupImage,  event->rect() );
            // and is all that is required when this paint event is due to markup changes.
            // When the current image changes, it is a bit of overkill to repaint the entire markup image.
            // Instead, only markup areas containing visible markups within the event rectangle are redrawn.
            // When the repaint is due to markup changes, this loop may finding only the single markup
            // rectangle used when to generating this paint event.
            for( int i = 0; i < getMarkupAreas().count(); i++ )
            {
                if( getMarkupAreas()[i].intersects( event->rect() ))
                {
                    compPainter.drawImage(  getMarkupAreas()[i], markupImage,  getMarkupAreas()[i] );
                }
            }

            // Apply the appropriate part of the composite image to the displayed image
            painter.drawImage( event->rect(), *compositeImage, event->rect() );
        }

        // Report position for pixel info logging
        emit currentPixelInfo( pixelInfoPos );
    }

    // Flag first update is over
    firstUpdate = false;
}

// Manage a resize event
void VideoWidget::resizeEvent( QResizeEvent *event )
{
    // Ensure the markups match the new size
    markupResize( event->size() );
}

// Act on a markup change
void VideoWidget::markupAction( markupIds mode, bool complete, bool clearing, QPoint point1, QPoint point2, unsigned int thickness )
{
    emit userSelection( mode, complete, clearing, point1, point2, thickness );
}

// Return an ordinate from the displayed image as an ordinate in the original image
QPoint VideoWidget::scalePoint( QPoint pnt )
{
    QPoint scaled;
    scaled.setX( scaleOrdinate( pnt.x() ));
    scaled.setY( scaleOrdinate( pnt.y() ));
    return scaled;
}

// Return an ordinate from the displayed image as an ordinate in the original image
int VideoWidget::scaleOrdinate( int ord )
{
    return (int)((double)ord / getScale());
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

// !! what if draging image (with mouse button down) when wheel event occurs???

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

bool VideoWidget::getPanning()
{
    return panning;
}

void VideoWidget::setPanning( bool panningIn )
{
    panning = panningIn;
    if( panning )
    {
        setCursor( Qt::OpenHandCursor );
    }
}
