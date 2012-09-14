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
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QESHAPE_H
#define QESHAPE_H

#include <QCaInteger.h>
#include <QCaWidget.h>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QCaPluginLibrary_global.h>
#include <QCaVariableNamePropertyManager.h>

//! Maximum number of variables.
#define QESHAPE_NUM_VARIABLES 6

class QCAPLUGINLIBRARYSHARED_EXPORT QEShape : public QWidget, public QCaWidget {
    Q_OBJECT

  public:
    QEShape( QWidget *parent = 0 );
    QEShape( const QString& variableName, QWidget *parent = 0 );

    enum shapeOptions { Line, Points, Polyline, Polygon, Rect, RoundedRect, Ellipse, Arc, Chord, Pie, Path, Text, Pixmap };
    enum animationOptions {Width, Height, X, Y, Transperency, Rotation, ColourHue, ColourSaturation, ColourValue, ColourIndex, Penwidth};


    // Property convenience functions

    // variable animations
    void setAnimation( animationOptions animation, const int index );
    animationOptions getAnimation( const int index );

    // scales
    void setScale( const double scale, const int index );
    double getScale( const int index );

    // offsets
    void setOffset( const double offset, const int index );
    double getOffset( const int index );

    // border
    void setBorder( const bool border );
    bool getBorder();

    // fill
    void setFill( const bool fill );
    bool getFill();

    // shape
    void setShape( shapeOptions shape );
    shapeOptions getShape();

    // number of points
    void setNumPoints( const unsigned int numPoints );
    unsigned int getNumPoints();

    // Origin translation
    void setOriginTranslation( const QPoint originTranslation );
    QPoint getOriginTranslation();

    // points
    void setPoint( const QPoint point, const int index );
    QPoint getPoint(const int index);

    // colors
    void setColor( const QColor color, const int index );
    QColor getColor( const int index );

    // draw border
    void setDrawBorder( const bool drawBorder );
    bool getDrawBorder();

    // line width
    void setLineWidth( const unsigned int lineWidth );
    unsigned int getLineWidth();

    // start angle
    void setStartAngle( const double startAngle );
    double getStartAngle();

    // rotation
    void setRotation( const double rotation );
    double getRotation();

    // arc length
    void setArcLength( const double arcLength );
    double getArcLength();

    // text
    void setText( const QString text );
    QString getText();



  protected:
    QCaIntegerFormatting integerFormatting;                     /// Integer formatting options

  #define OFFSETS_SIZE QESHAPE_NUM_VARIABLES
    double offsets[OFFSETS_SIZE];

  #define SCALES_SIZE QESHAPE_NUM_VARIABLES
    double scales[SCALES_SIZE];

    shapeOptions shape;
    QPoint originTranslation;

  #define POINTS_SIZE 10
    QPoint points[POINTS_SIZE];
    unsigned int numPoints;

  #define COLORS_SIZE 10
    QColor colors[COLORS_SIZE];

    animationOptions animations[6];

    bool fill;
    bool border;

    double startAngle;
    double arcLength;
    QString text;
    double rotation;
    unsigned int lineWidth;
    bool drawBorder;

    void colorChange( unsigned int index );                     /// Act on a color property change. (will update shape if using the color)
    unsigned int currentColor;                                  /// Index into colorsProperty last used when setting brush color

    void establishConnection( unsigned int variableIndex );     /// Create a CA connection and initiates updates if required

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setValue( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex );
    //!! move this functionality into QCaWidget???
    //!! needs one for single variables and one for multiple variables, or just the multiple variable one for all
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )//!! move into Standard Properties section??
    {
        setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
    }

  public slots:
    void requestEnabled( const bool& state ){ setApplicationEnabled( state ); } //!! move into Standard Properties section??

  signals:
    void dbValueChanged1( const qlonglong& out );
    void dbValueChanged2( const qlonglong& out );
    void dbValueChanged3( const qlonglong& out );
    void dbValueChanged4( const qlonglong& out );
    void dbValueChanged5( const qlonglong& out );
    void dbValueChanged6( const qlonglong& out );

  private:
    void setup();                                               /// Initialisation common to all constructors
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
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

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();

public:
    //=================================================================================
    // Multiple Variable properties
    // These properties should be identical for every widget using multiple variables (The number of variables may vary).
    // WHEN MAKING CHANGES: search for MULTIPLEVARIABLEPROPERTIESBASE and change all occurances.
    private:
        QCaVariableNamePropertyManager variableNamePropertyManagers[QESHAPE_NUM_VARIABLES];
    public:

    // Define a variable
    // Note, the QPROPERTY declaration itself can't be in this macro
#define VARIABLE_PROPERTY_ACCESS(VAR_INDEX) \
    void    setVariableName##VAR_INDEX##Property( QString variableName ){ variableNamePropertyManagers[VAR_INDEX].setVariableNameProperty( variableName ); } \
    QString getVariableName##VAR_INDEX##Property(){ return variableNamePropertyManagers[VAR_INDEX].getVariableNameProperty(); }

    VARIABLE_PROPERTY_ACCESS(0)
    Q_PROPERTY(QString variable1 READ getVariableName0Property WRITE setVariableName0Property)

    VARIABLE_PROPERTY_ACCESS(1)
    Q_PROPERTY(QString variable2 READ getVariableName1Property WRITE setVariableName1Property)

    VARIABLE_PROPERTY_ACCESS(2)
    Q_PROPERTY(QString variable3 READ getVariableName2Property WRITE setVariableName2Property)

    VARIABLE_PROPERTY_ACCESS(3)
    Q_PROPERTY(QString variable4 READ getVariableName3Property WRITE setVariableName3Property)

    VARIABLE_PROPERTY_ACCESS(4)
    Q_PROPERTY(QString variable5 READ getVariableName4Property WRITE setVariableName4Property)

    VARIABLE_PROPERTY_ACCESS(5)
    Q_PROPERTY(QString variable6 READ getVariableName5Property WRITE setVariableName5Property)

    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions )
    {
        for( int i = 0; i < QESHAPE_NUM_VARIABLES; i++ )
        {
            variableNamePropertyManagers[i].setSubstitutionsProperty( variableNameSubstitutions );
        }
    }
    QString getVariableNameSubstitutionsProperty()
    {
        return variableNamePropertyManagers[0].getSubstitutionsProperty();
    }
    //=================================================================================

    //=================================================================================
    // Standard properties
    // These properties should be identical for every widget using them.
    // WHEN MAKING CHANGES: search for STANDARDPROPERTIES and change all occurances.
    bool isEnabled() const { return getApplicationEnabled(); }
    void setEnabled( bool state ){ setApplicationEnabled( state ); }
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(bool allowDrop READ getAllowDrop WRITE setAllowDrop)
    Q_PROPERTY(bool visible READ getRunVisible WRITE setRunVisible)
    Q_PROPERTY(unsigned int messageSourceId READ getMessageSourceId WRITE setMessageSourceId )
    Q_PROPERTY(QString userLevelUserStyle READ getStyleUser WRITE setStyleUser)
    Q_PROPERTY(QString userLevelScientistStyle READ getStyleScientist WRITE setStyleScientist)
    Q_PROPERTY(QString userLevelEngineerStyle READ getStyleEngineer WRITE setStyleEngineer)
    enum UserLevels { User      = USERLEVEL_USER,
                      Scientist = USERLEVEL_SCIENTIST,
                      Engineer  = USERLEVEL_ENGINEER };
    UserLevels getUserLevelVisibilityProperty() { return (UserLevels)getUserLevelVisibility(); }
    void setUserLevelVisibilityProperty( UserLevels level ) { setUserLevelVisibility( (userLevels)level ); }

    UserLevels getUserLevelEnabledProperty() { return (UserLevels)getUserLevelEnabled(); }
    void setUserLevelEnabledProperty( UserLevels level ) { setUserLevelEnabled( (userLevels)level ); }
    Q_ENUMS(UserLevels)
    Q_PROPERTY(UserLevels userLevelVisibility READ getUserLevelVisibilityProperty WRITE setUserLevelVisibilityProperty)
    Q_PROPERTY(UserLevels userLevelEnabled READ getUserLevelEnabledProperty WRITE setUserLevelEnabledProperty)
    //=================================================================================

    // Widget specific properties

    Q_ENUMS(animationOptions)
    Q_PROPERTY(animationOptions animation1 READ getAnimation1Property WRITE setAnimation1Property)
    void setAnimation1Property( animationOptions animation ){ setAnimation( animation, 0 ); }
    animationOptions getAnimation1Property(){ return getAnimation( 0 ); }

    Q_PROPERTY(animationOptions animation2 READ getAnimation2Property WRITE setAnimation2Property)
    void setAnimation2Property( animationOptions animation ){ setAnimation( animation, 1 ); }
    animationOptions getAnimation2Property(){ return getAnimation( 1 ); }

    Q_PROPERTY(animationOptions animation3 READ getAnimation3Property WRITE setAnimation3Property)
    void setAnimation3Property( animationOptions animation ){ setAnimation( animation, 2 ); }
    animationOptions getAnimation3Property(){ return getAnimation( 2 ); }

    Q_PROPERTY(animationOptions animation4 READ getAnimation4Property WRITE setAnimation4Property)
    void setAnimation4Property( animationOptions animation ){ setAnimation( animation, 3 ); }
    animationOptions getAnimation4Property(){ return getAnimation( 3 ); }

    Q_PROPERTY(animationOptions animation5 READ getAnimation5Property WRITE setAnimation5Property)
    void setAnimation5Property( animationOptions animation ){ setAnimation( animation, 4 ); }
    animationOptions getAnimation5Property(){ return getAnimation( 4 ); }

    Q_PROPERTY(animationOptions animation6 READ getAnimation6Property WRITE setAnimation6Property)
    void setAnimation6Property( animationOptions animation ){ setAnimation( animation, 5 ); }
    animationOptions getAnimation6Property(){ return getAnimation( 5 ); }

    Q_PROPERTY(double scale1 READ getScale1Property WRITE setScale1Property)
    void setScale1Property( double scale ){ setScale( scale, 0 ); }
    double getScale1Property(){ return getScale( 0 ); }

    Q_PROPERTY(double scale2 READ getScale2Property WRITE setScale2Property)
    void setScale2Property( double scale ){ setScale( scale, 1 ); }
    double getScale2Property(){ return getScale( 1 ); }

    Q_PROPERTY(double scale3 READ getScale3Property WRITE setScale3Property)
    void setScale3Property( double scale ){ setScale( scale, 2 ); }
    double getScale3Property(){ return getScale( 2 ); }

    Q_PROPERTY(double scale4 READ getScale4Property WRITE setScale4Property)
    void setScale4Property( double scale ){ setScale( scale, 3 ); }
    double getScale4Property(){ return getScale( 3 ); }

    Q_PROPERTY(double scale5 READ getScale5Property WRITE setScale5Property)
    void setScale5Property( double scale ){ setScale( scale, 4 ); }
    double getScale5Property(){ return getScale( 4 ); }

    Q_PROPERTY(double scale6 READ getScale6Property WRITE setScale6Property)
    void setScale6Property( double scale ){ setScale( scale, 5 ); }
    double getScale6Property(){ return getScale( 5 ); }


    Q_PROPERTY(double offset1 READ getOffset1Property WRITE setOffset1Property)
    void setOffset1Property( double offset ){ setOffset( offset, 0 ); }
    double getOffset1Property(){ return getOffset( 0 ); }

    Q_PROPERTY(double offset2 READ getOffset2Property WRITE setOffset2Property)
    void setOffset2Property( double offset ){ setOffset( offset, 1 ); }
    double getOffset2Property(){ return getOffset( 1 ); }

    Q_PROPERTY(double offset3 READ getOffset3Property WRITE setOffset3Property)
    void setOffset3Property( double offset ){ setOffset( offset, 2 ); }
    double getOffset3Property(){ return getOffset( 2 ); }

    Q_PROPERTY(double offset4 READ getOffset4Property WRITE setOffset4Property)
    void setOffset4Property( double offset ){ setOffset( offset, 3 ); }
    double getOffset4Property(){ return getOffset( 3 ); }

    Q_PROPERTY(double offset5 READ getOffset5Property WRITE setOffset5Property)
    void setOffset5Property( double offset ){ setOffset( offset, 4 ); }
    double getOffset5Property(){ return getOffset( 4 ); }

    Q_PROPERTY(double offset6 READ getOffset6Property WRITE setOffset6Property)
    void setOffset6Property( double offset ){ setOffset( offset, 5 ); }
    double getOffset6Property(){ return getOffset( 5 ); }

    Q_PROPERTY(bool border READ getBorder WRITE setBorder)
    Q_PROPERTY(bool fill READ getFill WRITE setFill)

    Q_ENUMS(shapeOptions)
    Q_PROPERTY(shapeOptions shape READ getShapeProperty WRITE setShapeProperty)
    void setShapeProperty( shapeOptions shape ){ setShape(shape); }
    shapeOptions getShapeProperty(){ return getShape(); }


    Q_PROPERTY(unsigned int numPoints READ getNumPoints WRITE setNumPoints )
    Q_PROPERTY(QPoint originTranslation READ getOriginTranslation WRITE setOriginTranslation)

    Q_PROPERTY(QPoint point1 READ getPoint1Property WRITE setPoint1Property)
    void setPoint1Property( QPoint point ){ setPoint( point, 0 ); }
    QPoint getPoint1Property(){ return getPoint( 0 ); }

    Q_PROPERTY(QPoint point2 READ getPoint2Property WRITE setPoint2Property)
    void setPoint2Property( QPoint point ){ setPoint( point, 1 ); }
    QPoint getPoint2Property(){ return getPoint( 1 ); }

    Q_PROPERTY(QPoint point3 READ getPoint3Property WRITE setPoint3Property)
    void setPoint3Property( QPoint point ){ setPoint( point, 2 ); }
    QPoint getPoint3Property(){ return getPoint( 2 ); }

    Q_PROPERTY(QPoint point4 READ getPoint4Property WRITE setPoint4Property)
    void setPoint4Property( QPoint point ){ setPoint( point, 3 ); }
    QPoint getPoint4Property(){ return getPoint( 3 ); }

    Q_PROPERTY(QPoint point5 READ getPoint5Property WRITE setPoint5Property)
    void setPoint5Property( QPoint point ){ setPoint( point, 4 ); }
    QPoint getPoint5Property(){ return getPoint( 4 ); }

    Q_PROPERTY(QPoint point6 READ getPoint6Property WRITE setPoint6Property)
    void setPoint6Property( QPoint point ){ setPoint( point, 5 ); }
    QPoint getPoint6Property(){ return getPoint( 5 ); }

    Q_PROPERTY(QPoint point7 READ getPoint7Property WRITE setPoint7Property)
    void setPoint7Property( QPoint point ){ setPoint( point, 6 ); }
    QPoint getPoint7Property(){ return getPoint( 6 ); }

    Q_PROPERTY(QPoint point8 READ getPoint8Property WRITE setPoint8Property)
    void setPoint8Property( QPoint point ){ setPoint( point, 7 ); }
    QPoint getPoint8Property(){ return getPoint( 7 ); }

    Q_PROPERTY(QPoint point9 READ getPoint9Property WRITE setPoint9Property)
    void setPoint9Property( QPoint point ){ setPoint( point, 8 ); }
    QPoint getPoint9Property(){ return getPoint( 8 ); }

    Q_PROPERTY(QPoint point10 READ getPoint10Property WRITE setPoint10Property)
    void setPoint10Property( QPoint point ){ setPoint( point, 9 ); }
    QPoint getPoint10Property(){ return getPoint( 9 ); }


    Q_PROPERTY(QColor color1 READ getColor1Property WRITE setColor1Property)
    void setColor1Property( QColor color ){ setColor( color, 0 ); }
    QColor getColor1Property(){ return getColor( 0 ); }

    Q_PROPERTY(QColor color2 READ getColor2Property WRITE setColor2Property)
    void setColor2Property( QColor color ){ setColor( color, 1 ); }
    QColor getColor2Property(){ return getColor( 1 ); }

    Q_PROPERTY(QColor color3 READ getColor3Property WRITE setColor3Property)
    void setColor3Property( QColor color ){ setColor( color, 2 ); }
    QColor getColor3Property(){ return getColor( 2 ); }

    Q_PROPERTY(QColor color4 READ getColor4Property WRITE setColor4Property)
    void setColor4Property( QColor color ){ setColor( color, 3 ); }
    QColor getColor4Property(){ return getColor( 3 ); }

    Q_PROPERTY(QColor color5 READ getColor5Property WRITE setColor5Property)
    void setColor5Property( QColor color ){ setColor( color, 4 ); }
    QColor getColor5Property(){ return getColor( 4 ); }

    Q_PROPERTY(QColor color6 READ getColor6Property WRITE setColor6Property)
    void setColor6Property( QColor color ){ setColor( color, 5 ); }
    QColor getColor6Property(){ return getColor( 5 ); }

    Q_PROPERTY(QColor color7 READ getColor7Property WRITE setColor7Property)
    void setColor7Property( QColor color ){ setColor( color, 6 ); }
    QColor getColor7Property(){ return getColor( 6 ); }

    Q_PROPERTY(QColor color8 READ getColor8Property WRITE setColor8Property)
    void setColor8Property( QColor color ){ setColor( color, 7 ); }
    QColor getColor8Property(){ return getColor( 7 ); }

    Q_PROPERTY(QColor color9 READ getColor9Property WRITE setColor9Property)
    void setColor9Property( QColor color ){ setColor( color, 8 ); }
    QColor getColor9Property(){ return getColor( 8 ); }

    Q_PROPERTY(QColor color10 READ getColor10Property WRITE setColor10Property)
    void setColor10Property( QColor color ){ setColor( color, 9 ); }
    QColor getColor10Property(){ return getColor( 9 ); }


    Q_PROPERTY(bool drawBorder READ getDrawBorder WRITE setDrawBorder)
    Q_PROPERTY(unsigned int lineWidth READ getLineWidth WRITE setLineWidth)
    Q_PROPERTY(double startAngle READ getStartAngle WRITE setStartAngle)
    Q_PROPERTY(double rotation READ getRotation WRITE setRotation)
    Q_PROPERTY(double arcLength READ getArcLength WRITE setArcLength)
    Q_PROPERTY(QString text READ getText WRITE setText)
};

#endif // QESHAPE_H
