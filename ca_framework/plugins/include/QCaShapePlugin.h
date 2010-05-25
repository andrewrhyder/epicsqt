/*! 
  \class QCaShapePlugin
  \version $Revision: #4 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author andrew.rhyder
  \brief CA Shape Widget Plugin.
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

#ifndef QCASHAPEPLUGIN_H
#define QCASHAPEPLUGIN_H

#include <QTimer>
#include <QString>
#include <QCaShape.h>
#include <QCaVariableNamePropertyManager.h>
#include <QCaStringFormatting.h>

//! ???
#define VARIABLE_NAME_PROPERTY_MANAGERS_SIZE 6

class QCaShapePlugin : public QCaShape {
    Q_OBJECT

  public:
    /// Constructors
    QCaShapePlugin( QWidget *parent = 0 );
    QCaShapePlugin( QString variableNameProperty = "", QWidget *parent = 0 );


    /// Qt Designer Properties - Variable Name 1
    /// Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    /// A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString variable1 READ getVariableName1Property WRITE setVariableName1Property)
    void    setVariableName1Property( QString variable1Name ){ variableNamePropertyManagers[0].setVariableNameProperty( variable1Name ); }
    QString getVariableName1Property(){ return variableNamePropertyManagers[0].getVariableNameProperty(); }

    /// Qt Designer Properties - Variable Name 2
    /// Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    /// A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString variable2 READ getVariableName2Property WRITE setVariableName2Property)
    void    setVariableName2Property( QString variable2Name ){ variableNamePropertyManagers[1].setVariableNameProperty( variable2Name ); }
    QString getVariableName2Property(){ return variableNamePropertyManagers[1].getVariableNameProperty(); }

    /// Qt Designer Properties - Variable Name 3
    /// Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    /// A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString variable3 READ getVariableName3Property WRITE setVariableName3Property)
    void    setVariableName3Property( QString variable3Name ){ variableNamePropertyManagers[2].setVariableNameProperty( variable3Name ); }
    QString getVariableName3Property(){ return variableNamePropertyManagers[2].getVariableNameProperty(); }

    /// Qt Designer Properties - Variable Name 4
    /// Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    /// A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString variable4 READ getVariableName4Property WRITE setVariableName4Property)
    void    setVariableName4Property( QString variable4Name ){ variableNamePropertyManagers[3].setVariableNameProperty( variable4Name ); }
    QString getVariableName4Property(){ return variableNamePropertyManagers[3].getVariableNameProperty(); }

    /// Qt Designer Properties - Variable Name 5
    /// Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    /// A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString variable5 READ getVariableName5Property WRITE setVariableName5Property)
    void    setVariableName5Property( QString variable5Name ){ variableNamePropertyManagers[4].setVariableNameProperty( variable5Name ); }
    QString getVariableName5Property(){ return variableNamePropertyManagers[4].getVariableNameProperty(); }

    /// Qt Designer Properties - Variable Name 6
    /// Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    /// A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString variable6 READ getVariableName6Property WRITE setVariableName6Property)
    void    setVariableName6Property( QString variable6Name ){ variableNamePropertyManagers[5].setVariableNameProperty( variable6Name ); }
    QString getVariableName6Property(){ return variableNamePropertyManagers[5].getVariableNameProperty(); }


    /// Qt Designer Properties - variable 1 animation
    Q_ENUMS(animationOptions)
    enum animationOptions {Width = QCaShape::Width,
                           Height = QCaShape::Height,
                           X = QCaShape::X,
                           Y = QCaShape::Y,
                           Transperency = QCaShape::Transperency,
                           Rotation = QCaShape::Rotation,
                           ColourHue = QCaShape::ColourHue,
                           ColourSaturation = QCaShape::ColourSaturation,
                           ColourValue = QCaShape::ColourValue,
                           ColourIndex = QCaShape::ColourIndex,
                           Penwidth = QCaShape::Penwidth};

    Q_PROPERTY(animationOptions animation1 READ getAnimation1Property WRITE setAnimation1Property)
    void setAnimation1Property( animationOptions animation1 ){ animationProperty[0] = (QCaShape::Animations)animation1; update(); }
    animationOptions getAnimation1Property(){ return (animationOptions)animationProperty[0]; }

    Q_PROPERTY(animationOptions animation2 READ getAnimation2Property WRITE setAnimation2Property)
    void setAnimation2Property( animationOptions animation2 ){ animationProperty[1] = (QCaShape::Animations)animation2; update(); }
    animationOptions getAnimation2Property(){ return (animationOptions)animationProperty[1]; }

    Q_PROPERTY(animationOptions animation3 READ getAnimation3Property WRITE setAnimation3Property)
    void setAnimation3Property( animationOptions animation3 ){ animationProperty[2] = (QCaShape::Animations)animation3; update(); }
    animationOptions getAnimation3Property(){ return (animationOptions)animationProperty[2]; }

    Q_PROPERTY(animationOptions animation4 READ getAnimation4Property WRITE setAnimation4Property)
    void setAnimation4Property( animationOptions animation4 ){ animationProperty[3] = (QCaShape::Animations)animation4; update(); }
    animationOptions getAnimation4Property(){ return (animationOptions)animationProperty[3]; }

    Q_PROPERTY(animationOptions animation5 READ getAnimation5Property WRITE setAnimation5Property)
    void setAnimation5Property( animationOptions animation5 ){ animationProperty[4] = (QCaShape::Animations)animation5; update(); }
    animationOptions getAnimation5Property(){ return (animationOptions)animationProperty[4]; }

    Q_PROPERTY(animationOptions animation6 READ getAnimation6Property WRITE setAnimation6Property)
    void setAnimation6Property( animationOptions animation6 ){ animationProperty[5] = (QCaShape::Animations)animation6; update(); }
    animationOptions getAnimation6Property(){ return (animationOptions)animationProperty[5]; }

    /// Qt Designer Properties - scale 1
    Q_PROPERTY(double scale1 READ getScale1Property WRITE setScale1Property)
    void setScale1Property( double scale1 ){ scalesProperty[0] = scale1; }
    double getScale1Property(){ return scalesProperty[0]; }

    /// Qt Designer Properties - scale 2
    Q_PROPERTY(double scale2 READ getScale2Property WRITE setScale2Property)
    void setScale2Property( double scale2 ){ scalesProperty[1] = scale2; }
    double getScale2Property(){ return scalesProperty[1]; }

    /// Qt Designer Properties - scale 3
    Q_PROPERTY(double scale3 READ getScale3Property WRITE setScale3Property)
    void setScale3Property( double scale3 ){ scalesProperty[2] = scale3; }
    double getScale3Property(){ return scalesProperty[2]; }

    /// Qt Designer Properties - scale 4
    Q_PROPERTY(double scale4 READ getScale4Property WRITE setScale4Property)
    void setScale4Property( double scale4 ){ scalesProperty[3] = scale4; }
    double getScale4Property(){ return scalesProperty[3]; }

    /// Qt Designer Properties - scale 5
    Q_PROPERTY(double scale5 READ getScale5Property WRITE setScale5Property)
    void setScale5Property( double scale5 ){ scalesProperty[4] = scale5; }
    double getScale5Property(){ return scalesProperty[4]; }

    /// Qt Designer Properties - scale 6
    Q_PROPERTY(double scale6 READ getScale6Property WRITE setScale6Property)
    void setScale6Property( double scale6 ){ scalesProperty[5] = scale6; }
    double getScale6Property(){ return scalesProperty[5]; }


    /// Qt Designer Properties - offset 1
    Q_PROPERTY(double offset1 READ getOffset1Property WRITE setOffset1Property)
    void setOffset1Property( double offset1 ){ offsetsProperty[0] = offset1; }
    double getOffset1Property(){ return offsetsProperty[0]; }

    /// Qt Designer Properties - offset 2
    Q_PROPERTY(double offset2 READ getOffset2Property WRITE setOffset2Property)
    void setOffset2Property( double offset2 ){ offsetsProperty[1] = offset2; }
    double getOffset2Property(){ return offsetsProperty[1]; }

    /// Qt Designer Properties - offset 3
    Q_PROPERTY(double offset3 READ getOffset3Property WRITE setOffset3Property)
    void setOffset3Property( double offset3 ){ offsetsProperty[2] = offset3; }
    double getOffset3Property(){ return offsetsProperty[2]; }

    /// Qt Designer Properties - offset 4
    Q_PROPERTY(double offset4 READ getOffset4Property WRITE setOffset4Property)
    void setOffset4Property( double offset4 ){ offsetsProperty[3] = offset4; }
    double getOffset4Property(){ return offsetsProperty[3]; }

    /// Qt Designer Properties - offset 5
    Q_PROPERTY(double offset5 READ getOffset5Property WRITE setOffset5Property)
    void setOffset5Property( double offset5 ){ offsetsProperty[4] = offset5; }
    double getOffset5Property(){ return offsetsProperty[4]; }

    /// Qt Designer Properties - offset 6
    Q_PROPERTY(double offset6 READ getOffset6Property WRITE setOffset6Property)
    void setOffset6Property( double offset6 ){ offsetsProperty[5] = offset6; }
    double getOffset6Property(){ return offsetsProperty[5]; }


    /// Qt Designer Properties - border
    Q_PROPERTY(bool border READ getBorderProperty WRITE setBorderProperty)
    void setBorderProperty( bool border ){ borderProperty = border; }
    bool getBorderProperty(){ return borderProperty; }

    /// Qt Designer Properties - fill
    Q_PROPERTY(bool fill READ getFillProperty WRITE setFillProperty)
    void setFillProperty( bool fill ){ fillProperty = fill; }
    bool getFillProperty(){ return fillProperty; }


    /// Qt Designer Properties - variable substitutions Example: $SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ for( int i = 0; i < VARIABLE_NAME_PROPERTY_MANAGERS_SIZE; i++ ) variableNamePropertyManagers[i].setSubstitutionsProperty( variableNameSubstitutions ); }
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManagers[0].getSubstitutionsProperty(); }

    /// Qt Designer Properties - subscribe
    Q_PROPERTY(bool subscribe READ getSubscribeProperty WRITE setSubscribeProperty)
    void setSubscribeProperty( bool subscribe ){ subscribeProperty = subscribe; }
    bool getSubscribeProperty(){ return subscribeProperty; }

    /// Qt Designer Properties - variable as tool tip
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTipProperty WRITE setVariableAsToolTipProperty)
    void setVariableAsToolTipProperty( bool variableAsToolTip ){ variableAsToolTipProperty = variableAsToolTip; }
    bool getVariableAsToolTipProperty(){ return variableAsToolTipProperty; }

    /// Qt Designer Properties - enabled (override of widget enabled)
    Q_PROPERTY(bool enabled READ getEnabledProperty WRITE setEnabledProperty)
    void setEnabledProperty( bool enabled ){ setEnabled( enabled ); }
    bool getEnabledProperty(){ return enabledProperty; }

    /// Qt Designer Properties - shape
    Q_ENUMS(shapeOptions)
    Q_PROPERTY(shapeOptions shape READ getShapeProperty WRITE setShapeProperty)
    enum shapeOptions { Line = QCaShape::Line,
                  Points = QCaShape::Points,
                  Polyline = QCaShape::Polyline,
                  Polygon = QCaShape::Polygon,
                  Rect = QCaShape::Rect,
                  RoundedRect = QCaShape::RoundedRect,
                  Ellipse = QCaShape::Ellipse,
                  Arc = QCaShape::Arc,
                  Chord = QCaShape::Chord,
                  Pie = QCaShape::Pie,
                  Path = QCaShape::Path,
                  Text = QCaShape::Text,
                  Pixmap = QCaShape::Pixmap };
    void setShapeProperty( shapeOptions shape ){ shapeProperty = (QCaShape::Shape)shape; update(); }
    shapeOptions getShapeProperty(){ return (shapeOptions)shapeProperty; }


    /// Qt Designer Properties - number of points
    Q_PROPERTY(unsigned int numPoints READ getNumPointsProperty WRITE setNumPointsProperty )
    void setNumPointsProperty( unsigned int numPoints ){ numPointsProperty = (numPoints>POINTS_PROPERTY_SIZE)?POINTS_PROPERTY_SIZE:numPoints; update(); }
    unsigned int getNumPointsProperty(){ return numPointsProperty; }

    /// Qt Designer Properties - Origin translation
    Q_PROPERTY(QPoint originTranslation READ getOriginTranslationProperty WRITE setOriginTranslationProperty)
    void setOriginTranslationProperty( QPoint originTranslation ){ originTranslationProperty = originTranslation; update(); }
    QPoint getOriginTranslationProperty(){ return originTranslationProperty; }

    /// Qt Designer Properties - point 1
    Q_PROPERTY(QPoint point1 READ getPoint1Property WRITE setPoint1Property)
    void setPoint1Property( QPoint point1 ){ pointsProperty[0] = point1; update(); }
    QPoint getPoint1Property(){ return pointsProperty[0]; }

    /// Qt Designer Properties - point 2
    Q_PROPERTY(QPoint point2 READ getPoint2Property WRITE setPoint2Property)
    void setPoint2Property( QPoint point2 ){ pointsProperty[1] = point2; update(); }
    QPoint getPoint2Property(){ return pointsProperty[1]; }

    /// Qt Designer Properties - point 3
    Q_PROPERTY(QPoint point3 READ getPoint3Property WRITE setPoint3Property)
    void setPoint3Property( QPoint point3 ){ pointsProperty[2] = point3; update(); }
    QPoint getPoint3Property(){ return pointsProperty[2]; }

    /// Qt Designer Properties - point 4
    Q_PROPERTY(QPoint point4 READ getPoint4Property WRITE setPoint4Property)
    void setPoint4Property( QPoint point4 ){ pointsProperty[3] = point4; update(); }
    QPoint getPoint4Property(){ return pointsProperty[3]; }

    /// Qt Designer Properties - point 5
    Q_PROPERTY(QPoint point5 READ getPoint5Property WRITE setPoint5Property)
    void setPoint5Property( QPoint point5 ){ pointsProperty[4] = point5; update(); }
    QPoint getPoint5Property(){ return pointsProperty[4]; }

    /// Qt Designer Properties - point 6
    Q_PROPERTY(QPoint point6 READ getPoint6Property WRITE setPoint6Property)
    void setPoint6Property( QPoint point6 ){ pointsProperty[5] = point6; update(); }
    QPoint getPoint6Property(){ return pointsProperty[5]; }

    /// Qt Designer Properties - point 7
    Q_PROPERTY(QPoint point7 READ getPoint7Property WRITE setPoint7Property)
    void setPoint7Property( QPoint point7 ){ pointsProperty[6] = point7; update(); }
    QPoint getPoint7Property(){ return pointsProperty[6]; }

    /// Qt Designer Properties - point 8
    Q_PROPERTY(QPoint point8 READ getPoint8Property WRITE setPoint8Property)
    void setPoint8Property( QPoint point8 ){ pointsProperty[7] = point8; update(); }
    QPoint getPoint8Property(){ return pointsProperty[7]; }

    /// Qt Designer Properties - point 9
    Q_PROPERTY(QPoint point9 READ getPoint9Property WRITE setPoint9Property)
    void setPoint9Property( QPoint point9 ){ pointsProperty[8] = point9; update(); }
    QPoint getPoint9Property(){ return pointsProperty[8]; }

    /// Qt Designer Properties - point 10
    Q_PROPERTY(QPoint point10 READ getPoint10Property WRITE setPoint10Property)
    void setPoint10Property( QPoint point10 ){ pointsProperty[9] = point10; update(); }
    QPoint getPoint10Property(){ return pointsProperty[9]; }

    /// Qt Designer Properties - color 1
    Q_PROPERTY(QColor color1 READ getColor1Property WRITE setColor1Property)
    void setColor1Property( QColor color1 ){ colorsProperty[0] = color1; colorChange( 0 ); }
    QColor getColor1Property(){ return colorsProperty[0]; }

    /// Qt Designer Properties - color 2
    Q_PROPERTY(QColor color2 READ getColor2Property WRITE setColor2Property)
    void setColor2Property( QColor color2 ){ colorsProperty[1] = color2; colorChange( 1 ); }
    QColor getColor2Property(){ return colorsProperty[1]; }

    /// Qt Designer Properties - color 3
    Q_PROPERTY(QColor color3 READ getColor3Property WRITE setColor3Property)
    void setColor3Property( QColor color3 ){ colorsProperty[2] = color3; colorChange( 2 ); }
    QColor getColor3Property(){ return colorsProperty[2]; }

    /// Qt Designer Properties - color 4
    Q_PROPERTY(QColor color4 READ getColor4Property WRITE setColor4Property)
    void setColor4Property( QColor color4 ){ colorsProperty[3] = color4; colorChange( 3 ); }
    QColor getColor4Property(){ return colorsProperty[3]; }

    /// Qt Designer Properties - color 5
    Q_PROPERTY(QColor color5 READ getColor5Property WRITE setColor5Property)
    void setColor5Property( QColor color5 ){ colorsProperty[4] = color5; colorChange( 4 ); }
    QColor getColor5Property(){ return colorsProperty[4]; }

    /// Qt Designer Properties - color 6
    Q_PROPERTY(QColor color6 READ getColor6Property WRITE setColor6Property)
    void setColor6Property( QColor color6 ){ colorsProperty[5] = color6; colorChange( 5 ); }
    QColor getColor6Property(){ return colorsProperty[5]; }

    /// Qt Designer Properties - color 7
    Q_PROPERTY(QColor color7 READ getColor7Property WRITE setColor7Property)
    void setColor7Property( QColor color7 ){ colorsProperty[6] = color7; colorChange( 6 ); }
    QColor getColor7Property(){ return colorsProperty[6]; }

    /// Qt Designer Properties - color 8
    Q_PROPERTY(QColor color8 READ getColor8Property WRITE setColor8Property)
    void setColor8Property( QColor color8 ){ colorsProperty[7] = color8; colorChange( 7 ); }
    QColor getColor8Property(){ return colorsProperty[7]; }

    /// Qt Designer Properties - color 9
    Q_PROPERTY(QColor color9 READ getColor9Property WRITE setColor9Property)
    void setColor9Property( QColor color9 ){ colorsProperty[8] = color9; colorChange( 8 ); }
    QColor getColor9Property(){ return colorsProperty[8]; }

    /// Qt Designer Properties - color 10
    Q_PROPERTY(QColor color10 READ getColor10Property WRITE setColor10Property)
    void setColor10Property( QColor color10 ){ colorsProperty[9] = color10; colorChange( 9 ); }
    QColor getColor10Property(){ return colorsProperty[9]; }

    /// Qt Designer Properties - draw border
    Q_PROPERTY(bool drawBorder READ getDrawBorderProperty WRITE setDrawBorderProperty)
    void setDrawBorderProperty( bool drawBorder ){ drawBorderProperty = drawBorder; update(); }
    bool getDrawBorderProperty(){ return drawBorderProperty; }

    /// Qt Designer Properties - line width
    Q_PROPERTY(unsigned int lineWidth READ getLineWidthProperty WRITE setLineWidthProperty)
    void setLineWidthProperty( unsigned int lineWidth ){ lineWidthProperty = lineWidth; update(); }
    unsigned int getLineWidthProperty(){ return lineWidthProperty; }

    /// Qt Designer Properties - start angle
    Q_PROPERTY(double startAngle READ getStartAngleProperty WRITE setStartAngleProperty)
    void setStartAngleProperty( double startAngle ){ startAngleProperty = startAngle; update(); }
    double getStartAngleProperty(){ return startAngleProperty; }

    /// Qt Designer Properties - rotation
    Q_PROPERTY(double rotation READ getRotationProperty WRITE setRotationProperty)
    void setRotationProperty( double rotation ){ rotationProperty = rotation; update(); }
    double getRotationProperty(){ return rotationProperty; }

    /// Qt Designer Properties - arc length
    Q_PROPERTY(double arcLength READ getArcLengthProperty WRITE setArcLengthProperty)
    void setArcLengthProperty( double arcLength ){ arcLengthProperty = arcLength; update(); }
    double getArcLengthProperty(){ return arcLengthProperty; }

    /// Qt Designer Properties - text
    Q_PROPERTY(QString text READ getTextProperty WRITE setTextProperty)
    void setTextProperty( QString text ){ textProperty = text; update(); }
    QString getTextProperty(){ return textProperty; }

  private:
    QCaVariableNamePropertyManager variableNamePropertyManagers[VARIABLE_NAME_PROPERTY_MANAGERS_SIZE];

  private slots:
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );
};

#endif /// QCASHAPEPLUGIN_H
