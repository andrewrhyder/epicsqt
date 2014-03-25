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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef BRIGHTNESSCONTRAST_H
#define BRIGHTNESSCONTRAST_H

#include <QFrame>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include <QSpinBox>
#include <QDebug>
#include <QIntValidator>

#define HISTOGRAM_BINS 256
class localBrightnessContrast;

class histogram: public QFrame
{
public:
    histogram( QWidget* parent, localBrightnessContrast* lbc );
    Q_OBJECT
private:
    void paintEvent( QPaintEvent* event);
    void resizeEvent( QResizeEvent* event );

    localBrightnessContrast* lbc;
};

class localBrightnessContrast : public QFrame
{
    Q_OBJECT

public:
    localBrightnessContrast();
    ~localBrightnessContrast();

    void setBrightnessContrast( const unsigned int max, const unsigned int min );
    void setAutoBrightnessContrast( bool autoBrightnessContrast );
    void setContrastReversal( bool contrastReversal );
    void setLog( bool log );
    void setFalseColour( bool falseColour );

    bool getAutoBrightnessContrast();
    bool getContrastReversal();
    bool getLog();
    bool getFalseColour();

    int getLowPixel();
    int getHighPixel();

    struct rgbPixel
    {
        unsigned char p[4]; // R/G/B/Alpha
    };
    void setStatistics( unsigned int minPIn, unsigned int maxPIn, unsigned int bitDepth, unsigned int binsIn[HISTOGRAM_BINS], rgbPixel pixelLookup[256] );

signals:
    void brightnessContrastAutoImage();
    void brightnessContrastChange();

private slots:
    void brightnessSliderValueChanged( int value );
    void minSliderValueChanged( int value );
    void maxSliderValueChanged( int value );
    void gradientSliderValueChanged( int value );

    void brightnessSpinBoxChanged( int );
    void gradientSpinBoxChanged( int );
    void minSpinBoxChanged( int );
    void maxSpinBoxChanged( int );

    void brightnessContrastResetClicked( bool state );
    void brightnessContrastAutoImageClicked();
    void contrastReversalToggled( bool );
    void logToggled( bool );
    void falseColourToggled( bool );

private:
    // Local brightness and contrast controls and monitors
    QCheckBox* autoBrightnessCheckBox;
    QSlider* brightnessSlider;
    QSlider* contrastSlider;
    QSlider* zeroValueSlider;
    QSlider* fullValueSlider;
    QSlider* gradientSlider;
    QSpinBox* brightnessSpinBox;
    QSpinBox* zeroValueSpinBox;
    QSpinBox* fullValueSpinBox;
    QSpinBox* gradientSpinBox;
    QCheckBox* contrastReversalCheckBox;
    QCheckBox* logCheckBox;
    QCheckBox* falseColourCheckBox;

    histogram* hist;

    // Flags to avoid loops when setting controls
    bool nonInteractive;
    bool inBrightnessSliderCallback;
    bool inGradientSliderCallback;
    bool inZeroValueSliderCallback;
    bool inFullValueSliderCallback;

    bool inBrightnessEditCallback;
    bool inGradientEditCallback;
    bool inZeroValueEditCallback;
    bool inFullValueEditCallback;

    //
    void updateBrightness( double val );
    void updateGradient( double val );
    void updateZeroValue( unsigned int val );
    void updateFullValue( unsigned int val );
    void updateZeroValueFullValue( unsigned int min, unsigned int max );


    void updateBrightnessInterface();
    void updateGradientInterface();
    void updateZeroValueInterface();
    void updateFullValueInterface();

public:
    // Current brightness/contrast settings
    unsigned int zeroValue;
    unsigned int fullValue;

    unsigned int range;  // Derived

    // Current image stats
    void initialiseImageStats();
    unsigned int maxP;
    unsigned int minP;
    unsigned int depth;
    unsigned int* bins; // [HISTOGRAM_BINS]
    rgbPixel* pixelLookup; // [256]

    QLabel* histXLabel;
};


#endif // BRIGHTNESSCONTRAST_H
