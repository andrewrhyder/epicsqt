/*  QEScaling.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 *
 */

#ifndef QE_SCALING_H
#define QE_SCALING_H

#include <QObject>
#include <QPoint>
#include <QEPluginLibrary_global.h>

/// The scaling is defined using a rational number specifed by two integers (m, d).
/// The first (m) integer is the multiplier and the second (d) integer is the divisor.
/// For example, if m = 4 and d = 5, then this specifies an 80%; and if m = 5 and d = 4,
/// this specifies that a 125% scaling is required.
///
/// Scaling is deemed to be application wide, hence all scaling data (and functions)
/// are static.
///
// Note: This was formerly part of QECommon.
//
class QEPLUGINLIBRARYSHARED_EXPORT QEScaling {
public:
    /// Set currently applied scaling.
    /// Both values default to 1, which is a null scaling.
    /// Only valid scaling values (m > 0, d > 0) are accepted.
   ///
    static void setScaling (const int m, const int d);

    /// Extract currently applied scaling - allows widgets to perform widget class
    /// specific scaling functionality.
    ///
    static void getScaling (int& m, int& d);

    /// Adjust the geometry and font scaling of the widget and all child widgets
    /// by the defined scaling parameters (m, d). Unless m and d different, no scaling occurs.
    /// The function tree walks the hiearchy of widgets paranted by the specified widget.
    /// The maxDepth can be used to limit any possibility of infinite recursion.
    ///
    static void applyToWidget (QWidget* widget, const int maxDepth = 60);

    /// Conveniance functions for widget specific 'scaleBy' functions.
    ///
    /// Scales a single value. Note: all other scaling functions take a object by reference
    /// and modify that object. Only this function returns a scaled value.
    ///
    static inline int scale (const int v) { return (v * QEScaling::currentScaleM) /
                                                    QEScaling::currentScaleD; }

    /// Scales a point.
    ///
    static void applyToPoint (QPoint& point);

private:
    static int currentScaleM;
    static int currentScaleD;

    /// Tree walks the QWidget hierarchy in order to apply scaling.
    ///
    static void widgetTreeWalk (QWidget* widget, const int maxDepth);

    /// Scales a single widget
    /// Applies some special processing above and beyond size, min size, max size and font
    /// depending on the type of widget. Also if is a QEWidget then calls QEWidget's scaleBy
    /// method.
    ///
    static void widgetScale (QWidget * widget);
};

# endif // QE_SCALING_H
