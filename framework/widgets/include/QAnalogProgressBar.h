/*  $Id: QAnalogProgressBar.h $
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
   This mimics QProgressBar and provides an analog view of the progress bar
   widget including analogous analog functions and properties:

       QProgressBar      QAnalogProgressBar

       setRange()        setAnalogRange()
       setMinimum()      setAnalogMinimum()
       getMinimum()      getAnalogMinimum()
       setMaximum()      setAnalogMaximum()
       getMaximum()      getAnalogMaximum()
       setValue()        setAnalogValue()
       getValue()        getAnalogValue()

       Minimum           analogMinimum
       Maximum           analogMaximum
       Value             analogValue

 */

#ifndef QANALOGPROGRESSBAR_H
#define QANALOGPROGRESSBAR_H

#include <QColor>
#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPoint>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QAnalogProgressBar:public QWidget {
    Q_OBJECT

public:
    enum Orientations { Left_To_Right, Top_To_Bottom, Right_To_Left, Bottom_To_Top };
    Q_ENUMS (Orientations)

    enum Modes { Bar, Scale, Meter };
    Q_ENUMS (Modes)

    Q_PROPERTY (double analogValue       READ getAnalogValue         WRITE setAnalogValue)
    Q_PROPERTY (double analogMinimum     READ getAnalogMinimum       WRITE setAnalogMinimum)
    Q_PROPERTY (double analogMaximum     READ getAnalogMaximum       WRITE setAnalogMaximum)
    Q_PROPERTY (Orientations Orientation READ getOrientation         WRITE setOrientation)
    Q_PROPERTY (Modes  Mode              READ getMode                WRITE setMode)
    Q_PROPERTY (int    centreAngle       READ getCentreAngle         WRITE setCentreAngle)
    Q_PROPERTY (int    spanAngle         READ getSpanAngle           WRITE setSpanAngle)
    Q_PROPERTY (double minorInterval     READ getMinorInterval       WRITE setMinorInterval)
    Q_PROPERTY (double majorInterval     READ getMajorInterval       WRITE setMajorInterval)

    // NOTE: Where possible I spell colour properly.
    //
    Q_PROPERTY (QColor boarderColour     READ getBorderColour        WRITE setBorderColour)
    Q_PROPERTY (QColor foregroundColour  READ getForegroundColour    WRITE setForegroundColour)
    Q_PROPERTY (QColor backgroundColour  READ getBackgroundColour    WRITE setBackgroundColour)
    Q_PROPERTY (QColor fontColour        READ getFontColour          WRITE setFontColour)
    Q_PROPERTY (bool   showText          READ getShowText            WRITE setShowText)

private:
    // class member variable names start with m so as not to clash with
    // the propery names.
    //
    QColor mBorderColour;
    QColor mForegroundColour;
    QColor mBackgroundColour;
    QColor mFontColour;
    double mAnalogMinimum;
    double mAnalogMaximum;
    double mAnalogValue;
    enum Orientations mOrientation;
    enum Modes mMode;
    int mCentreAngle;
    int mSpanAngle;
    bool mShowText;
    double mMinorInterval;
    double mMajorInterval;

    void paintEvent (QPaintEvent *event);

    void drawBar   (QPainter & painter, int top,  int left,  int bottom,  int right, const double fraction);
    void drawScale (QPainter & painter, int top,  int left,  int bottom,  int right, const double fraction);
    void drawMeter (QPainter & painter, int top,  int left,  int bottom,  int right, const double fraction);

    /// Like draw text, but centred on cx an cy.
    //
    void drawText  (QPainter & painter, QPoint & textCentre, QString & text, const int pointSize = 0);

protected:
    // Returns the format parameter for a call to sprintf, used to set
    // underlying bar format property.
    // This may be overridden by a derived class.
    //
    virtual QString getSprintfFormat ();

public:
    /// Constructor
    QAnalogProgressBar (QWidget * parent = 0);
    virtual ~QAnalogProgressBar() {}
    virtual QSize sizeHint () const;

    // property access functions.
    //
    double getAnalogValue   ();

    void setAnalogMinimum (const double value);
    double getAnalogMinimum ();

    void setAnalogMaximum (const double value);
    double getAnalogMaximum ();

    void setOrientation   (const enum Orientations value);
    enum Orientations getOrientation ();

    void setMode          (const enum Modes value);
    enum Modes getMode ();

    void setCentreAngle (const int value);
    int getCentreAngle ();

    void setSpanAngle (const int value);
    int getSpanAngle ();

    void setMinorInterval (const double value);
    double getMinorInterval ();

    void setMajorInterval (const double value);
    double getMajorInterval ();

    void setBorderColour (const QColor value);
    QColor getBorderColour ();

    void setForegroundColour (const QColor value);
    QColor getForegroundColour ();

    void setBackgroundColour (const QColor value);
    QColor getBackgroundColour ();

    void setFontColour (const QColor value);
    QColor getFontColour ();

    void setShowText (const bool value);
    bool getShowText ();

public slots:
    void setAnalogRange   (const double analogMinimumIn, const double analogMaximumIn);
    void setAnalogValue   (const double analogValueIn);
};

#endif /// QANALOGPROGRESSBAR_H
