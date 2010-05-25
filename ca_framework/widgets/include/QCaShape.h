/*! 
  \class QCaShape
  \version $Revision: #8 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author andrew.rhyder
  \brief CA Shape Widget.
 */

/* Copyright (c) 2009 Australian Synchrotron
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * Licence as published by the Free Software Foundation; either
 * version 2.1 of the Licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public Licence for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * Licence along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Contact details:
 * andrew.rhyder@synchrotron.org.au
 * 800 Blackburn Road, Clayton, Victoria 3168, Australia.
 *
 */

#ifndef QCASHAPE_H
#define QCASHAPE_H

#include <QCaInteger.h>
#include <QCaWidget.h>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>

//! Maximum number of variables.
#define NUM_VARIABLES 6

class QCaShape : public QWidget, public QCaWidget {
    Q_OBJECT

  public:
    QCaShape( QWidget *parent = 0 );
    QCaShape( const QString& variableName = "", QWidget *parent = 0 );

    bool isEnabled() const;
    void setEnabled( bool state );

  protected:
    QCaIntegerFormatting integerFormatting;                     /// Integer formatting options

    enum Shape { Line, Points, Polyline, Polygon, Rect, RoundedRect, Ellipse, Arc, Chord, Pie, Path, Text, Pixmap };
    enum Animations {Width, Height, X, Y, Transperency, Rotation, ColourHue, ColourSaturation, ColourValue, ColourIndex, Penwidth};

  #define OFFSETS_PROPERTY_SIZE NUM_VARIABLES
    double offsetsProperty[OFFSETS_PROPERTY_SIZE];

  #define SCALES_PROPERTY_SIZE NUM_VARIABLES
    double scalesProperty[SCALES_PROPERTY_SIZE];

    Shape shapeProperty;
    QPoint originTranslationProperty;

  #define POINTS_PROPERTY_SIZE 10
    QPoint pointsProperty[POINTS_PROPERTY_SIZE];
    unsigned int numPointsProperty;

  #define COLORS_PROPERTY_SIZE 10
    QColor colorsProperty[COLORS_PROPERTY_SIZE];

    Animations animationProperty[6];

    bool fillProperty;
    bool borderProperty;

    double startAngleProperty;
    double arcLengthProperty;
    QString textProperty;
    double rotationProperty;
    unsigned int lineWidthProperty;
    bool drawBorderProperty;

    bool enabledProperty;

    void colorChange( unsigned int index );                     /// Act on a color property change. (will update shape if using the color)
    unsigned int currentColor;                                  /// Index into colorsProperty last used when setting brush color

    void establishConnection( unsigned int variableIndex );     /// Create a CA connection and initiates updates if required

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setValue( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex );

  public slots:
    void requestEnabled( const bool& state );

  signals:
    void dbValueChanged1( const qlonglong& out );
    void dbValueChanged2( const qlonglong& out );
    void dbValueChanged3( const qlonglong& out );
    void dbValueChanged4( const qlonglong& out );
    void dbValueChanged5( const qlonglong& out );
    void dbValueChanged6( const qlonglong& out );

  private:
    void setup();                                               /// Initialisation common to all constructors
    void createQcaItem( unsigned int variableIndex );
    void updateToolTip ( const QString & toolTip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    void paintEvent(QPaintEvent *event);
    QPen pen;
    QBrush brush;
    bool antialiased;
    QPixmap pixmap;

    double painterCurrentScaleX;
    double painterCurrentScaleY;
    double painterCurrentTranslateX;
    double painterCurrentTranslateY;

    QPoint scaledOriginTranslation;
};

#endif /// QCASHAPE_H
