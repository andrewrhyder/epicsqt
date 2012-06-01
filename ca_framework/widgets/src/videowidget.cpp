
#include "videowidget.h"

#include <QtMultimedia>

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent)
{
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

void VideoWidget::mousePressEvent(QMouseEvent *event)
{

    qDebug() << event << event->pos();
}

void VideoWidget::mouseReleaseEvent ( QMouseEvent * event )
{
    qDebug() << event << event->pos();
}

void VideoWidget::mouseMoveEvent ( QMouseEvent * event )
{
    qDebug() << event << event->pos();

}
void VideoWidget::wheelEvent( QWheelEvent* event )
{
    qDebug() << event << event->pos();
}

