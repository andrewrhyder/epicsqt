
#include "videowidget.h"

#include <QtMultimedia>

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent)
{
    compositeImageBackground = NULL;
    compositeImage = NULL;

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
    update();//!!!required???
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

/*
QSize VideoWidget::sizeHint() const
{
    return this->surface->surfaceFormat().sizeHint();
}
*/

// The displayed image has changed, redraw it
void VideoWidget::setNewImage( const QImage image )
{
    currentImage = image;
    compositeImageBackgroundStale = true;
    setMarkupTime();
    update();
}

// The markup overlay has changed, redraw part of it
//!! Call this with a null markups image if no markups
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
    painter.rotate( rotation );
    if( firstUpdate && currentImage.isNull() )
    {
        QColor bg(0, 0, 0, 255);
        painter.fillRect(rect(), bg);
    }

    // If there is an image to display, paint the appropriate bits
    else
    {
        // If there are no markups, and the entire image is being drawn, just display the current image
        if( !anyVisibleMarkups() && event->rect() == rect() )
        {
            painter.drawImage( event->rect(), currentImage, currentImage.rect() );
        }

        // If there are markups, draw the current image overlayed with the appropriate markups
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

            // If there is no composite image, create it.
            if( !compositeImage )
            {
                compositeImage = new QImage( size(), currentImage.format() );
            }

            // Flag if a composite background image is needed.
            // (If the current image is the same size as the display widget, then the current
            // image can be used directly for markup backgrounds)
            bool usingCompositeBackground = currentImage.size() != size();

            // If there is no composite background and the current image is a different size to
            // the display widget, then create a composite background.
            if( !compositeImageBackground && usingCompositeBackground )
            {
                compositeImageBackground = new QImage( size(), currentImage.format() );
                compositeImageBackgroundStale = true;
            }

            if( usingCompositeBackground && compositeImageBackgroundStale )
            {
                QPainter bgPainter( compositeImageBackground );
                bgPainter.drawImage( compositeImageBackground->rect(), currentImage, currentImage.rect() );
                compositeImageBackgroundStale = false;
            }

            QPainter compPainter( compositeImage );

            // Draw the required background part.
            // The background part must be taken from an image scaled to the drawing widget
            // If the current image is the same size, then it will be used. If not, then the
            // compositeImageBackground image will have been set up.
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
            // Instead, only markup areas containing visible marklups within the event rectangle are redrawn
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

void VideoWidget::setRotation( double angle )
{
    rotation = angle;
}

void VideoWidget::markupAction( markupIds activeItem, QPoint point1, QPoint point2 )
{
    qDebug() << "VideoWidget::markupAction()" << activeItem << point1 << point2;

    QPoint scaledPoint1;
    QPoint scaledPoint2;

    scaledPoint1.setX( (double)(point1.x()) / getHScale() );
    scaledPoint1.setY( (double)(point1.y()) / getVScale() );

    scaledPoint2.setX( (double)(point2.x()) / getHScale() );
    scaledPoint2.setY( (double)(point2.y()) / getVScale() );


    emit userSelection( point1, point2, scaledPoint1, scaledPoint2 );

}

// Return the vertical scale of the displayed image
double VideoWidget::getVScale()
{
    // If for any reason a scale can't be determined, return scale of 1.0
    if( currentImage.isNull() || currentImage.height() == 0 || height() == 0)
        return 1.0;

    // Return the vertical scale of the displayed image
    return (double)height() / (double)currentImage.height();
}

// Return the horizontal scale of the displayed image
double VideoWidget::getHScale()
{
    // If for any reason a scale can't be determined, return scale of 1.0
    if( currentImage.isNull() || currentImage.width() == 0 || width() == 0)
        return 1.0;

    // Return the horizontal scale of the displayed image
    return (double)width() / (double)currentImage.width();
}
