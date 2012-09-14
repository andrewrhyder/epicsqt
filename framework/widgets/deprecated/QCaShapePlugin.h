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

#ifndef QCASHAPEPLUGIN_H
#define QCASHAPEPLUGIN_H

#include <QEShape.h>

/// QCaLineEditPlugin IS deprecated - use QELineEdit.
class QCaShapePlugin : public QEShape {
    Q_OBJECT

public:
    QCaShapePlugin( QWidget *parent = 0 ) : QEShape( parent ) {}

    // This deprecated widget is now just a shell around its replacement.
    // Its replacement does not recognise any properties defined in using a Q_ENUM in the original widget,
    // so these properties must still be implemented in the deprecated widget
    enum shapeOptions { Line=QEShape::Line,
                        Points=QEShape::Points,
                        Polyline=QEShape::Polyline,
                        Polygon=QEShape::Polygon,
                        Rect=QEShape::Rect,
                        RoundedRect=QEShape::RoundedRect,
                        Ellipse=QEShape::Ellipse,
                        Arc=QEShape::Arc,
                        Chord=QEShape::Chord,
                        Pie=QEShape::Pie,
                        Path=QEShape::Path,
                        Text=QEShape::Text,
                        Pixmap=QEShape::Pixmap };
    Q_ENUMS(shapeOptions)
    Q_PROPERTY(shapeOptions shape READ getShapeProperty WRITE setShapeProperty)
    void setShapeProperty( shapeOptions shape ){ setShape((QEShape::shapeOptions)shape); }
    shapeOptions getShapeProperty(){ return (shapeOptions)getShape(); }

    enum animationOptions {Width=QEShape::Width,
                           Height=QEShape::Height,
                           X=QEShape::X,
                           Y=QEShape::Y,
                           Transperency=QEShape::Transperency,
                           Rotation=QEShape::Rotation,
                           ColourHue=QEShape::ColourHue,
                           ColourSaturation=QEShape::ColourSaturation,
                           ColourValue=QEShape::ColourValue,
                           ColourIndex=QEShape::ColourIndex,
                           Penwidth=QEShape::Penwidth};
    Q_ENUMS(animationOptions)
    Q_PROPERTY(animationOptions animation1 READ getAnimation1Property WRITE setAnimation1Property)
    void setAnimation1Property( animationOptions animation ){ setAnimation( (QEShape::animationOptions)animation, 0 ); }
    animationOptions getAnimation1Property(){ return (animationOptions)getAnimation( 0 ); }

    Q_PROPERTY(animationOptions animation2 READ getAnimation2Property WRITE setAnimation2Property)
    void setAnimation2Property( animationOptions animation ){ setAnimation( (QEShape::animationOptions)animation, 1 ); }
    animationOptions getAnimation2Property(){ return (animationOptions)getAnimation( 1 ); }

    Q_PROPERTY(animationOptions animation3 READ getAnimation3Property WRITE setAnimation3Property)
    void setAnimation3Property( animationOptions animation ){ setAnimation( (QEShape::animationOptions)animation, 2 ); }
    animationOptions getAnimation3Property(){ return (animationOptions)getAnimation( 2 ); }

    Q_PROPERTY(animationOptions animation4 READ getAnimation4Property WRITE setAnimation4Property)
    void setAnimation4Property( animationOptions animation ){ setAnimation( (QEShape::animationOptions)animation, 3 ); }
    animationOptions getAnimation4Property(){ return (animationOptions)getAnimation( 3 ); }

    Q_PROPERTY(animationOptions animation5 READ getAnimation5Property WRITE setAnimation5Property)
    void setAnimation5Property( animationOptions animation ){ setAnimation( (QEShape::animationOptions)animation, 4 ); }
    animationOptions getAnimation5Property(){ return (animationOptions)getAnimation( 4 ); }

    Q_PROPERTY(animationOptions animation6 READ getAnimation6Property WRITE setAnimation6Property)
    void setAnimation6Property( animationOptions animation ){ setAnimation( (QEShape::animationOptions)animation, 5 ); }
    animationOptions getAnimation6Property(){ return (animationOptions)getAnimation( 5 ); }

};

#endif // QCASHAPEPLUGIN_H
