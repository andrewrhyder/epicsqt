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
#include <QString>

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

};

# endif // QECOMMON_H
