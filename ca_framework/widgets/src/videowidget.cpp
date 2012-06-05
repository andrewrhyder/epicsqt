
#include "videowidget.h"

#include <QtMultimedia>

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent)
{
    playImage = NULL;

    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_PaintOnScreen, true);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    firstUpdate = true;
    update();
}

VideoWidget::~VideoWidget()
{
    if( playImage )
    {
        delete playImage;
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
    currentImage = image;
}

void VideoWidget::paintEvent(QPaintEvent* )
{
    QPainter painter(this);
    if( firstUpdate )
    {
        QColor bg(0, 0, 0, 255);
        painter.fillRect(rect(), bg);
        firstUpdate = false;
    }
    else
    {
        painter.rotate( rotation );

        painter.drawImage(rect(), currentImage, currentImage.rect() );
    }
}

//// The markup overlay has changed, redraw part of it
//void VideoWidget::markupChange( QImage& markups, QRect changedArea )
//{
//    qDebug() << "VideoWidget::markupChange" << changedArea;
//    // If the image used for preparing the combined image and markup
//    // fragments is present, but the wrong size, delete it
//    if( playImage && playImage->size() != markupImage.size() )
//    {
//        delete playImage;
//        playImage = NULL;
//    }

//    // If the image used for preparing the combined image and markup
//    // fragments is not present, create it
//    if( !playImage )
//    {
//        playImage = new QImage( markupImage.size(), markupImage.format() );
//    }

////!!! the following double buffers the drawing of the markups. This avoids bits of markups flickering
////!!! is this achieved easier (or even free) by the fact that the changed image won't be flushed to the screen between two paints to the widget?

//    // Draw the changed markup area over the image
//    QPainter playPainter( playImage );
//    playPainter.drawImage( changedArea, currentImage, changedArea );
//    playPainter.drawImage( changedArea, markups, changedArea );

//    // Update the changed part of the image
//    QPainter screenPainter( this );
//    screenPainter.drawImage( changedArea, *playImage, changedArea );
//}

/*
void VideoWidget::resizeEvent(QResizeEvent *event)
{
    update();
//    QWidget::resizeEvent(event);

//    surface->updateVideoRect();
}
*/
void VideoWidget::setRotation( double angle )
{
    rotation = angle;
//    surface->setRotation( angle );
}

//void VideoWidget::mousePressEvent(QMouseEvent *event)
//{
//
//    qDebug() << event << event->pos();
//}

//void VideoWidget::mouseReleaseEvent ( QMouseEvent * event )
//{
//    qDebug() << event << event->pos();
//}

//void VideoWidget::mouseMoveEvent ( QMouseEvent * event )
//{
//    qDebug() << event << event->pos();

//}
//void VideoWidget::wheelEvent( QWheelEvent* event )
//{
//    qDebug() << event << event->pos();
//}

