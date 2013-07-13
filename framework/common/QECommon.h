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

#include <QDateTime>
#include <QObject>
#include <QPoint>
#include <QString>
#include <QEPluginLibrary_global.h>


// Useful type neutral numerical macro fuctions.
//
#define ABS(a)             ((a) >= 0  ? (a) : -(a))
#define MIN(a, b)          ((a) <= (b) ? (a) : (b))
#define MAX(a, b)          ((a) >= (b) ? (a) : (b))
#define LIMIT(x,low,high)  (MAX(low, MIN(x, high)))

// Calculates number of items in an array
//
#define ARRAY_LENGTH(xx)   (int (sizeof (xx) /sizeof (xx [0])))


// Provide log and exp 10 macro functions.
//
// Log is a safe log in that it avoids attempting to take the log of negative
// or zero values. The 1.0e-20 limit is somewhat arbitary, but in practice is
// good for most numbers encountered at the synchrotron.
//
// Not all platforms provide exp10. What about pow () ??
//
#define LOG10(x)  ( (x) >=  1.0e-20 ? log10 (x) : -20.0 )
#define EXP10(x)  exp (2.302585092994046 * (x))


// We do not include QColor and QWidget header files in this header file (they are
// called by by QECommon.cpp), we just provide incomplete declarations.
// This particularly usefull for non-gui command line programs.
//
class QColor;
class QWidget;

// Geneal purpose utility functions.
// We use a class of static methods as opposed to a set of regular functions.
// This provide a name space, but also allows inheritance if needs be.
//
class QEPLUGINLIBRARYSHARED_EXPORT QEUtilities {
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


    /// This function returns the image, as a QString, of a enumeration value (cast as an integer).
    /// An invalid enumeration value returns a null string.
    ///
    /// Note: This functions rely on the meta object compiler (moc) generated code.
    /// To use this functions, the enumeration type must be declared within a Q_OBJECT QObject
    /// and the enum type qualifier with the Q_ENUMS, e.g. Q_ENUMS (Shapes).
    ///
    static QString enumToString (const QObject& object,
                                 const QString& enumTypeName,
                                 const int enumValue);

    /// This function returns the enumeration value given an enumeration image. The caller must
    /// cast the result to the appropriate enumeration type. The image must be exact match including
    /// case. The only tolerance allowed for is that the image is trimmed.
    /// An invalid image cause this function to return -1. However, -1 is an uncommon but not invalid
    /// enumeration value. If -1 might be a valid value, then the caller should specify the and
    /// check the ok parameter.
    ///
    /// Note: This functions rely on the meta object compiler (moc) generated code.
    /// To use this functions, the enumeration type must be declared within a Q_OBJECT QObject
    /// and the enum type qualifier with the Q_ENUMS, e.g. Q_ENUMS (Shapes).
    ///
    static int stringToEnum  (const QObject& object,
                              const QString& enumTypeName,
                              const QString& enumImage,
                              bool* ok = 0);

    /// Adjust the geometry and font scaling of the widget and all child widgets
    /// by the ratio m / d. Unless m and d are both positive, no scaling occurs.
    /// The function tree walks the hiearchy of widgets paranted by the specified widget.
    /// The maxDepth can be used to limit any possibility of infinite recursion.
    ///
    static void adjustWidgetScale (QWidget* widget, const int m, const int d, const int maxDepth = 40);

    /// Conveniance function for widget specific 'scaleBy' functions.
    ///
    static void adjustPointScale (QPoint& point, const int m, const int d);


    /// This function tree walks the given parent looking a widget of the specified class
    /// name. The function returns the first found or NULL.
    ///
    static QWidget* findWidget (QWidget* parent, const QString& className);

private:
    /// Scales a single widget
    /// Applies some special processing above and beyond size, min size, max size and font
    /// depending on the type of widget.
    ///
    static void widgetScaleBy (QWidget * widget, const int m, const int d);

    /// Scales a single value.
    ///
    static int scaleBy (const int v, const int m, const int d);
};

# endif // QECOMMON_H
