/*  QEAnalogIndicator.h
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
 *  Copyright (c) 2011
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

/*!
   This class provides a non CA aware graphical analog indicator base class.
   It supports a number of display modes including Bar, Scale and Meter.

   When in Bar mode, it mimics QProgressBar and provides an analog progress bar
   widget.

 */

#ifndef QEANALOGINDICATOR_H
#define QEANALOGINDICATOR_H

#include <QColor>
#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPoint>
#include <QList>
#include <QEPluginLibrary_global.h>

class QEPLUGINLIBRARYSHARED_EXPORT QEAnalogIndicator : public QWidget {
    Q_OBJECT

public:
    /// \enum    Orientations
    /// The orientation of Bar and Scale indicators
    enum Orientations { Left_To_Right,      ///< Left to right
                        Top_To_Bottom,      ///< Top to bottom
                        Right_To_Left,      ///< Right to left
                        Bottom_To_Top       ///< Bottom to top
                    };
    Q_ENUMS (Orientations)

    /// \enum    Modes
    /// The type of analog indicator used to represent the value
    enum Modes { Bar,       ///< Bar (solid bar from minimum up to current value)
                 Scale,     ///< Scale (diamond marker tracks current value)
                 Meter      ///< Meter (Needle moving across an arc scale)
             };
    Q_ENUMS (Modes)

    /// Current indicated value.
    ///
    Q_PROPERTY (double value             READ getValue               WRITE setValue)
    /// Minimum indicated value.
    ///
    Q_PROPERTY (double minimum           READ getMinimum             WRITE setMinimum)
    /// Maximum indicated value.
    ///
    Q_PROPERTY (double maximum           READ getMaximum             WRITE setMaximum)

    /// Minor scale interval.
    /// Only applies for linear scale (not log scale)
    Q_PROPERTY (double minorInterval     READ getMinorInterval       WRITE setMinorInterval)
    /// Minor scale interval.
    /// Only applies for linear scale (not log scale)
    Q_PROPERTY (double majorInterval     READ getMajorInterval       WRITE setMajorInterval)

    /// Log scale interval.
    ///
    Q_PROPERTY (int    logScaleInterval  READ getLogScaleInterval    WRITE setLogScaleInterval)

    /// If set, show textual representation of value on the indicator
    ///
    Q_PROPERTY (bool   showText          READ getShowText            WRITE setShowText)
    /// If set, show the scale
    ///
    Q_PROPERTY (bool   showScale         READ getShowScale           WRITE setShowScale)
    /// If set, use a logarithmic scale. If clear, use a linear scale
    ///
    Q_PROPERTY (bool   logScale          READ getLogScale            WRITE setLogScale)
    /// Selects what type of indicator is used
    /// (refer to Modes)
    Q_PROPERTY (Modes  mode              READ getMode                WRITE setMode)
    /// The orientation of Bar and Scale indicators
    /// (refer to Orientations)
    Q_PROPERTY (Orientations orientation READ getOrientation         WRITE setOrientation)
    /// The angle in degreed of the line that Meter indicators are centered around.
    /// Zero represents a vertical centerline and angles increment clockwise.
    Q_PROPERTY (int    centreAngle       READ getCentreAngle         WRITE setCentreAngle)
    /// The span of the Meter scale arc in degrees
    /// Typical meters are 180 deg and 270 deg
    Q_PROPERTY (int    spanAngle         READ getSpanAngle           WRITE setSpanAngle)

    /// Border colour
    ///
    Q_PROPERTY (QColor borderColour      READ getBorderColour        WRITE setBorderColour)
    /// Background colour
    ///
    Q_PROPERTY (QColor backgroundColour  READ getBackgroundColour    WRITE setBackgroundColour)
    /// Foreground colour
    ///
    Q_PROPERTY (QColor foregroundColour  READ getForegroundColour    WRITE setForegroundColour)
    /// Font colour
    ///
    Q_PROPERTY (QColor fontColour        READ getFontColour          WRITE setFontColour)

private:
    // class member variable names start with m so as not to clash with
    // the propery names.
    //
    QColor mBorderColour;
    QColor mForegroundColour;
    QColor mBackgroundColour;
    QColor mFontColour;
    double mMinimum;
    double mMaximum;
    double mValue;
    enum Orientations mOrientation;
    enum Modes mMode;
    int mCentreAngle;
    int mSpanAngle;
    bool mShowText;
    bool mShowScale;
    bool mLogScale;
    double mMinorInterval;
    int mMajorMinorRatio;
    int mLogScaleInterval;

    // Note: the getXxxxColour functions (line 160-ish) gets the Xxxx property colour.
    // The getXxxxPaintColour functions return actual colour to for drawing the widget.
    //
    QColor getBorderPaintColour ();
    QColor getBackgroundPaintColour ();
    QColor getForegroundPaintColour ();
    QColor getFontPaintColour ();

    void paintEvent (QPaintEvent *event);

    bool isLeftRight ();
    void drawOutline (QPainter & painter, QRect &outline);
    void drawAxis    (QPainter & painter, QRect &axis);

    void drawBar     (QPainter & painter, QRect &area, const double fraction);
    void drawMarker  (QPainter & painter, QRect &area, const double fraction);
    void drawMeter   (QPainter & painter, QRect &area, const double fraction);

    // Like painter drawText, but centred on textCentre.
    // (drawText aligns bottom left corner on given point).
    //
    void drawText (QPainter & painter, QPoint & textCentre, QString & text, const int pointSize = 0);

    // In left right mode text centred on x, just below y.
    // In top bottom mode text centred on y, just to right of x.
    //
    void drawAxisText (QPainter & painter, QPoint & textCentre, QString & text, const int pointSize = 0);

    // Value iterator.
    // itc is the iterator control value.
    //
    bool firstValue (int & itc, double & value, bool & isMajor);
    bool nextValue  (int & itc, double & value, bool & isMajor);

    // Safe log10 function - caters for negative/zero arguments.
    // Result is always >= -20
    //
    static double safeLog (const double x);
    double calcFraction (const double value);

protected:
    // Returns thetest image to be displayed.
    // This may be overridden by a derived class.
    //
    virtual QString getTextImage ();

    struct Band {
       double lower;
       double upper;
       QColor colour;
    };

    class BandList : public QList <Band> { };

    virtual BandList getBandList ();

public:
    /// Constructor
    QEAnalogIndicator (QWidget * parent = 0);
    /// Destructor
    virtual ~QEAnalogIndicator() {}

    /// Size hint
    virtual QSize sizeHint () const;

    // property access functions.
    //
    double getValue ();                                         ///< Access function for #value property - refer to #value property for details

    void setMinimum (const double value);                       ///< Access function for #minimum - refer to #minimum property for details
    double getMinimum ();                                       ///< Access function for #minimum - refer to #minimum property for details

    void setMaximum (const double value);                       ///< Access function for #maximum - refer to #maximum property for details
    double getMaximum ();                                       ///< Access function for #maximum - refer to #maximum property for details

    void setOrientation   (const enum Orientations value);      ///< Access function for #orientation - refer to #orientation property for details
    enum Orientations getOrientation ();                        ///< Access function for #orientation - refer to #orientation property for details

    void setMode          (const enum Modes value);             ///< Access function for #mode - refer to #mode property for details
    enum Modes getMode ();                                      ///< Access function for #mode - refer to #mode property for details

    void setCentreAngle (const int value);                      ///< Access function for #centreAngle - refer to #centreAngle property for details
    int getCentreAngle ();                                      ///< Access function for #centreAngle - refer to #centreAngle property for details

    void setSpanAngle (const int value);                        ///< Access function for #spanAngle - refer to #spanAngle property for details
    int getSpanAngle ();                                        ///< Access function for #spanAngle - refer to #spanAngle property for details

    void setMinorInterval (const double value);                 ///< Access function for #minorInterval - refer to #minorInterval property for details
    double getMinorInterval ();                                 ///< Access function for #minorInterval - refer to #minorInterval property for details

    void setMajorInterval (const double value);                 ///< Access function for #majorInterval - refer to #majorInterval property for details
    double getMajorInterval ();                                 ///< Access function for #majorInterval - refer to #majorInterval property for details

    void setLogScaleInterval (const int value);                 ///< Access function for #logScaleInterval - refer to #logScaleInterval property for details
    int getLogScaleInterval ();                                 ///< Access function for #logScaleInterval - refer to #logScaleInterval property for details

    void setBorderColour (const QColor value);                  ///< Access function for #borderColour - refer to #borderColour property for details
    QColor getBorderColour ();                                  ///< Access function for #borderColour - refer to #borderColour property for details

    void setForegroundColour (const QColor value);              ///< Access function for #foregroundColour - refer to #foregroundColour property for details
    QColor getForegroundColour ();                              ///< Access function for #foregroundColour - refer to #foregroundColour property for details

    void setBackgroundColour (const QColor value);              ///< Access function for #backgroundColour - refer to #backgroundColour property for details
    QColor getBackgroundColour ();                              ///< Access function for #backgroundColour - refer to #backgroundColour property for details

    void setFontColour (const QColor value);                    ///< Access function for #fontColour - refer to #fontColour property for details
    QColor getFontColour ();                                    ///< Access function for #fontColour - refer to #fontColour property for details

    void setShowText (const bool value);                        ///< Access function for #showText - refer to #showText property for details
    bool getShowText ();                                        ///< Access function for #showText - refer to #showText property for details

    void setShowScale (const bool value);                       ///< Access function for #showScale - refer to #showScale property for details
    bool getShowScale ();                                       ///< Access function for #showScale - refer to #showScale property for details

    void setLogScale (const bool value);                        ///< Access function for #logScale - refer to #logScale property for details
    bool getLogScale ();                                        ///< Access function for #logScale - refer to #logScale property for details

public slots:
    void setRange   (const double MinimumIn, const double MaximumIn);
    void setValue   (const double ValueIn);
};

#endif // QEANALOGINDICATOR_H
