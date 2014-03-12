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
#include <QDebug>

class localBrightnessContrast;

class histogram: public QFrame
{
public:
    histogram( QWidget* parent, localBrightnessContrast* lbc );
    Q_OBJECT
private:
    void paintEvent(QPaintEvent *event);

    localBrightnessContrast* lbc;
};

class localBrightnessContrast : public QFrame
{
    Q_OBJECT

public:
    localBrightnessContrast();
    ~localBrightnessContrast();

    void setBrightnessContrast( const unsigned int max, const unsigned int min, const unsigned int highest, const QVector<unsigned int>& bins );
    void setBrightnessContrast( int brightness, int contrast );
    void setAutoBrightnessContrast( bool autoBrightnessContrast );
    void setContrastReversal( bool contrastReversal );

    bool getAutoBrightnessContrast();
    bool getContrastReversal();
    int  getBrightness();
    int  getContrast();

    void setStatistics( unsigned int minPIn, unsigned int maxPIn, unsigned int bitDepth, QVector<unsigned int> binsIn );

signals:
    void brightnessContrastAutoImage();
    void brightnessContrastChange();

private slots:
    void brightnessSliderValueChanged( int value );
    void contrastSliderValueChanged( int value );
    void minSliderValueChanged( int value );
    void maxSliderValueChanged( int value );
    void gradientSliderValueChanged( int value );

    void brightnessContrastResetClicked( bool state );
    void brightnessContrastAutoImageClicked();
    void contrastReversalToggled( bool );

private:
    // Local brightness and contrast controls
    QCheckBox* autoBrightnessCheckBox;
    QSlider* brightnessSlider;
    QSlider* contrastSlider;
    QSlider* minSlider;
    QSlider* maxSlider;
    QSlider* gradientSlider;
    QLabel* brightnessRBLabel;
    QLabel* contrastRBLabel;
    QLabel* minRBLabel;
    QLabel* maxRBLabel;
    QLabel* gradientRBLabel;
    QCheckBox* contrastReversalCheckBox;
    histogram* hist;

    bool nonInteractive;

public:
    // Current brightness/contrast settings
    unsigned int brightness;
    unsigned int contrast;
    unsigned int gradient; // Derived???
    unsigned int zeroValue;
    unsigned int fullValue;

    unsigned int range;  // Derived

    // Current image stats
    void initialiseImageStats();
    unsigned int maxP;
    unsigned int minP;
    unsigned int depth;
    QVector<unsigned int> bins;

};


#endif // BRIGHTNESSCONTRAST_H
