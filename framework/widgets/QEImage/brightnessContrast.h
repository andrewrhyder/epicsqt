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

class localBrightnessContrast : public QFrame
{
    Q_OBJECT

public:
    localBrightnessContrast();

    void setBrightnessContrast( int brightness, int contrast );
    void setAutoBrightnessContrast( bool autoBrightnessContrast );

    bool getAutoBrightnessContrast();
    int  getBrightness();
    int  getContrast();

signals:
    void brightnessContrastAutoImage();
    void brightnessContrastChange();

private slots:
    void brightnessSliderValueChanged( int value );
    void contrastSliderValueChanged( int value );
    void brightnessContrastResetClicked( bool state );
    void brightnessContrastAutoImageClicked();

private:
    // Local brightness and contrast controls
    QCheckBox* autoBrightnessCheckBox;
    QSlider* brightnessSlider;
    QSlider* contrastSlider;
    QLabel* brightnessRBLabel;
    QLabel* contrastRBLabel;

};


#endif // BRIGHTNESSCONTRAST_H
