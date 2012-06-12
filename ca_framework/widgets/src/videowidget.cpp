
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
    update();//!!!required???
}

VideoWidget::~VideoWidget()
{
    if( compositeImage )
    {
        delete compositeImage;
    }
}

/*
QSize VideoWidget::sizeHint() const
{
    return this->surface->surfaceFormat().sizeHint();
}
*/

void VideoWidget::setNewImage( const QImage image )
{
    if( !currentImage.isNull() ) return;//!!!

    currentImage = image;
    setMarkupTime();
    updateCompositeImage( image.rect() );
    update();
}

// The markup overlay has changed, redraw part of it
//!! Call this with a null markups image if no markups
void VideoWidget::markupChange( QImage& markups, QRect changedArea )
{
    // Save the current markups. The markups image is null if no markups
    markupImage = markups;
    updateCompositeImage( changedArea );
    update( changedArea );
}

void VideoWidget::updateCompositeImage( QRect changedArea )
{
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
        if( compositeImage && compositeImage->size() != currentImage.size() )
        {
            delete compositeImage;
            compositeImage = NULL;
        }

        // If the image used for preparing the combined image and markup
        // is not present, create it and prepare to populate it from the entire current image
        QRect redrawArea;
        if( !compositeImage )
        {
            compositeImage = new QImage( currentImage.size(), currentImage.format() );
            redrawArea = currentImage.rect();
        }

        // If the image used for preparing the combined image and markup
        // was already present, prepare to update just the changed areas
        else
        {
            redrawArea = changedArea;
        }

        // Draw the changed markup area over the image
        QPainter playPainter( compositeImage );
        playPainter.drawImage( redrawArea, currentImage, redrawArea );
        playPainter.drawImage( redrawArea, markupImage, redrawArea );

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
