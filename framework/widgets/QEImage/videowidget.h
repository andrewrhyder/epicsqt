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

    void setNewImage( const QImage image, QCaDateTime& time );
    void setPanning( bool panningIn );
    bool getPanning();
    QPoint scalePoint( QPoint pnt );
    int scaleOrdinate( int ord );

    QImage getImage(){ return currentImage; }


protected:
    void paintEvent(QPaintEvent*);

    void mousePressEvent( QMouseEvent* event);
    void mouseReleaseEvent ( QMouseEvent* event );
    void mouseMoveEvent( QMouseEvent* event );
    void wheelEvent( QWheelEvent* event );

    void markupChange( QImage& markups, QVector<QRect>& changedAreas );    // The markup overlay has changed, redraw part of it

    void resizeEvent( QResizeEvent *event );
    void markupSetCursor( QCursor cursor );
    void markupAction( markupIds mode, QPoint point1, QPoint point2 );


signals:
    void userSelection( imageMarkup::markupIds mode, QPoint point1, QPoint point2 );
    void zoomInOut( int zoomAmount );
    void currentPixelInfo( QPoint pos );
    void pan( QPoint pos );

private:
    void addMarkups( QPainter& screenPainter, QVector<QRect>& changedAreas );

    QImage currentImage;              // Latest camera image
    QImage markupImage;               // Image of markups, such as region of interest
    QImage* compositeImage;           // Composite of current image and markups
    QImage* compositeImageBackground; // Current image, scaled to the composite image size
    bool compositeImageBackgroundStale;

    bool firstUpdate;

    double getScale();

    bool panning;       // If true user is dragging image with mouse (rather than performing any markup)
    QPoint panStart;

};

#endif // VIDEOWIDGET_H
