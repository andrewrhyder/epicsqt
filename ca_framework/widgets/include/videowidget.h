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

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QtGui/QWidget>
#include <imageMarkup.h>

class VideoWidget : public QWidget, public imageMarkup
{
    Q_OBJECT
public:
    VideoWidget(QWidget *parent = 0);
    ~VideoWidget();

    void setRotation( double angle );
    void setNewImage( const QImage image );

protected:
    void paintEvent(QPaintEvent*);

    void mousePressEvent( QMouseEvent* event) { markupMousePressEvent( event ); }
    void mouseReleaseEvent ( QMouseEvent* event ) { markupMouseReleaseEvent( event ); }
    void mouseMoveEvent( QMouseEvent* event ) { markupMouseMoveEvent( event ); }
    void wheelEvent( QWheelEvent* event ) { markupMouseWheelEvent( event ); }

    void markupChange( QImage& markups, QVector<QRect>& changedAreas );    // The markup overlay has changed, redraw part of it

    void resizeEvent( QResizeEvent *event );
    void markupSetCursor( QCursor cursor );
    void markupAction( markupIds activeItem, QPoint point1, QPoint point2 );

private:
    void addMarkups( QPainter& screenPainter, QVector<QRect>& changedAreas );
    void updateCompositeImage( bool imageChanged, QVector<QRect>& changedAreas );

    QImage currentImage;              // Latest camera image
    QImage markupImage;               // Image of markups, such as region of interest
    QImage* compositeImage;           // Composite of current and markup images
    QImage* compositeImageBackground; // Current image, scaled to the composite image size
    bool compositeImageBackgroundStale;
    QImage displayImage;              // Image to display, either current image, or composite

    double rotation;
    bool firstUpdate;
};

#endif // VIDEOWIDGET_H
