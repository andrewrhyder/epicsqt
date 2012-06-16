
#include "videowidget.h"

#include <QtMultimedia>

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent)
{
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

    update();//!!!required???
}

VideoWidget::~VideoWidget()
{
    if( compositeImage )
    {
        delete compositeImage;
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

    // Update the composite image with a completely new image, and redraw whatever markup areas are required
    updateCompositeImage( true, getMarkupAreas() );
    update();
}

// The markup overlay has changed, redraw part of it
//!! Call this with a null markups image if no markups
void VideoWidget::markupChange( QImage& markups,  QVector<QRect>& changedAreas )
{
    // Save the current markups. The markups image is null if no markups
    markupImage = markups;
    updateCompositeImage( false, changedAreas );
    for( int i = 0; i < changedAreas.count(); i++ )
    {
        update( changedAreas[i] );
    }
}

// Update the composite image.
// The composite image consists of the display image plus any markups such as a region of interest.
// If there are no markups, the image is used directly as is, so no copying is required
void VideoWidget::updateCompositeImage( bool imageChanged, QVector<QRect>& markupChangedAreas )
{
    // If there are no markups, just display the current image
    if( markupImage.isNull() )
    {
        displayImage = currentImage;
    }

    // If there are markups, ensure the composite image is created, composed, and displayed
    else
    {
        //!!! only create a background image if a different size to the current image

        // If the image used for preparing the combined image and markup
        // fragments is present, but the wrong size, delete it
        bool newCompositeImage = false;
        if( compositeImage && compositeImage->size() != size() )
        {
            delete compositeImage;
            delete compositeImageBackground;
            compositeImage = NULL;
            compositeImageBackground = NULL;
        }

        // If the image used for preparing the combined image and markup
        // is not present, create it
        if( !compositeImage )
        {
            compositeImage = new QImage( size(), currentImage.format() );
            compositeImageBackground = new QImage( size(), currentImage.format() );
            newCompositeImage = true;
            compositeImageBackgroundStale = true;
        }

        // Draw the displayed image if it has changed
        QPainter compositePainter( compositeImage );
        if( imageChanged || newCompositeImage )
        {
            compositePainter.drawImage( compositeImage->rect(), currentImage, currentImage.rect() );
        }

        // Draw the required markup areas over the image
        for( int i = 0; i < markupChangedAreas.count(); i++ )
        {
            if( !imageChanged )
            {
                // If the composite background is out of date, recreate it
                if( compositeImageBackgroundStale )
                {
                    QPainter bgPainter( compositeImageBackground );
                    bgPainter.drawImage( compositeImageBackground->rect(), currentImage, currentImage.rect() );
                    compositeImageBackgroundStale = false;
                }
                // Draw the required background part
                compositePainter.drawImage( markupChangedAreas[i], *compositeImageBackground, markupChangedAreas[i] );
            }
            compositePainter.drawImage( markupChangedAreas[i], markupImage, markupChangedAreas[i] );
        }

        // Display the composite image
        displayImage = *compositeImage;
    }
}

// Manage a paint event in the video widget
void VideoWidget::paintEvent(QPaintEvent* event )
{
    // If this is the first paint event, and there is no image to display, fill it with black
    QPainter painter(this);
    if( firstUpdate && displayImage.isNull() )
    {
        QColor bg(0, 0, 0, 255);
        painter.fillRect(rect(), bg);
    }

    // If there is an image to display, paint the appropriate bits
    else
    {
        painter.rotate( rotation );
        qDebug() << event->rect() << displayImage.rect();
        // 1) The display image has changed, and is not marked up.
        //    A single repaint event will occur for the entire display widget.
        //    The display image can be any size and should be scaled to the
        //    display widget.
        //    So displayImage.rect() as the source area is OK
        //
        // 2) The display image has changed, and is marked up.
        //    A single repaint event will occur for the entire display widget.
        //    The display image will be generated with markups and will be the
        //    same size as the display widget. It doesn't matter if it is scaled to the
        //    display widget, or not.
        //    So, event->rect() as the source area is OK
        //
        // 3) The markups have changed.
        //    Multiple update events will occur for each changed markup area.
        //    The display image will be generated with markups and will be the
        //    same size as the display widget. The pixel area from the display
        //    image should match the event update area.
        //    So, event->rect() as the source area is OK
        if( displayImage.size() != size() )
        {
            painter.drawImage( event->rect(), displayImage, displayImage.rect() );
        }
        else
        {
            painter.drawImage( event->rect(), displayImage, event->rect() );
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
}
