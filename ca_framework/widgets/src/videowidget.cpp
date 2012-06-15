
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
//!!!    if( !currentImage.isNull() ) return;//!!!

    currentImage = image;
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
    qDebug() << "VideoWidget::updateCompositeImage" << imageChanged << markupChangedAreas;

    // If there are no markups, just display the current image
    if( markupImage.isNull() )
    {
        displayImage = currentImage;
    }

    // If there are markups, ensure the composite image is created, composed, and displayed
    else
    {
        // If the image used for preparing the combined image and markup
        // fragments is present, but the wrong size, delete it
        bool newCompositeImage = false;
        if( compositeImage && compositeImage->size() != currentImage.size() )
        {
            delete compositeImage;
            compositeImage = NULL;
        }

        // If the image used for preparing the combined image and markup
        // is not present, create it
        if( !compositeImage )
        {
            compositeImage = new QImage( currentImage.size(), currentImage.format() );
            newCompositeImage = true;
        }

        // Draw the displayed image if it has changed
        QPainter playPainter( compositeImage );
        if( imageChanged || newCompositeImage )
        {
            playPainter.drawImage( compositeImage->rect(), currentImage, currentImage.rect() );
        }

        // Draw the required markup areas over the image
        for( int i = 0; i < markupChangedAreas.count(); i++ )
        {
            if( !imageChanged )
            {
                playPainter.drawImage( markupChangedAreas[i], currentImage, markupChangedAreas[i] );
            }
            playPainter.drawImage( markupChangedAreas[i], markupImage, markupChangedAreas[i] );
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
        painter.drawImage( event->rect(), displayImage, event->rect() );
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
