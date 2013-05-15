/*  QECommon.h
 *
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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 *
 */

#ifndef QECOMMON_H
#define QECOMMON_H

#include <QColor>
#include <QDateTime>
#include <QString>
#include <QWidget>

// Useful type neutral numerical macro fuctions.
//
#define ABS(a)             ((a) >= 0  ? (a) : -(a))
#define MIN(a, b)          ((a) <= (b) ? (a) : (b))
#define MAX(a, b)          ((a) >= (b) ? (a) : (b))
#define LIMIT(x,low,high)  (MAX(low, MIN(x, high)))

// Calculates number of items in an array
//
#define ARRAY_LENGTH(xx)   (int (sizeof (xx) /sizeof (xx [0])))

// Geneal purpose utility functions.
// We use a class of static methods as opposed to a set of regular functions.
// This provide a name space, but also allows inheritance if needs be.
//
class QEUtilities {
public:
    /// For the specified bacground colour, returns font colour (black or white)
    /// with a suitable contrast. Currently based on rgb values, and ignores alpha.
    ///
    static QColor fontColour (const QColor & backgroundColour);

    /// Converts a given colour to associated background-color style,
    /// and sets foreground (font) colour to black or white accordingly.
    ///
    static QString colourToStyle (const QColor backgroundColour);


    /// Get the local time zone offset (in seconds) for the nominated time.
    /// This is not fixed for the location and will depend on if the the time
    /// is a standard time or a daylight savings/summer time.
    ///
    //  Option: default atTime = QDateTime::currentDateTime () ??
    //
    static int getTimeZoneOffset (const QDateTime & atTime);

    /// Extract the local time zone three letter acronym for the time.
    /// If the timeSpec patameter is Qt::UTC, then this simply returns "UTC".
    /// However, if the timeSpec patameter is Qt::LocalTime, then extract the TLA
    /// application of the time in question (and NOT for the current time).
    /// E.g. for the UK this would be GMT or BST, for Victoria Australia EST or EST.
    ///
    static QString getTimeZoneTLA (const Qt::TimeSpec timeSpec,
                                   const QDateTime & atTime);

    /// Overloaded function that uses the timeSpec assocaited with atTime.
    ///
    static QString getTimeZoneTLA (const QDateTime & atTime);

    /// Adjust the geometry and font scaling of the widget and all child widgets
    /// by the ratio m / d. Unless m and d are both positive, no scaling occurs.
    /// The function tree walks the hiearchy of widgets paranted by the specified widget.
    /// The maxDepth can be used to limit any possibility of infinite recursion.
    ///
    static void adjustWidgetScale (QWidget * widget, const int m, const int d, const int maxDepth = 40);

private:
    /// Scales a single widget
    ///
    static void widgetScaleBy (QWidget * widget, const int m, const int d);

    /// Scales a single value.
    ///
    static int scaleBy (const int v, const int m, const int d);
};

# endif // QECOMMON_H
