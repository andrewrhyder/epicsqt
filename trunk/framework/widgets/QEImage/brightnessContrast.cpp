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

/*
  This class manages the QEImage local brightness and contrast controls.
 */

#include <brightnessContrast.h>
#include <QVBoxLayout>
#include <QPushButton>

localBrightnessContrast::localBrightnessContrast()
{
    setFrameStyle( QFrame::StyledPanel|QFrame::Raised );

    QVBoxLayout* brightnessContrastMainLayout = new QVBoxLayout();
    brightnessContrastMainLayout->setSpacing( 0 );
    setLayout( brightnessContrastMainLayout );

    QHBoxLayout* brightnessContrastSub1Layout = new QHBoxLayout();
    QGridLayout* brightnessContrastSub2Layout = new QGridLayout();

    QLabel* brightnessLabel = new QLabel( "Brightness:", this );
    QLabel* contrastLabel = new QLabel( "Contrast:", this );

    autoBrightnessCheckBox = new QCheckBox( "Auto Brightness and Contrast", this );
    autoBrightnessCheckBox->setToolTip( "Set brightness and contrast to use the full dynamic range of an area when an area is selected");

    QPushButton* autoImageButton = new QPushButton( "Auto all", this );
    autoImageButton->setToolTip( "Set brightness and contrast to use the full dynamic range for the entire image");
    QObject::connect( autoImageButton, SIGNAL( clicked ( bool ) ), this,  SLOT  ( brightnessContrastAutoImageClicked()) );

    QPushButton* resetButton = new QPushButton( "Reset", this );
    resetButton->setToolTip( "Reset brightness and contrast");
    QObject::connect( resetButton, SIGNAL( clicked ( bool ) ), this,  SLOT  ( brightnessContrastResetClicked( bool )) );

    brightnessSlider = new QSlider( Qt::Horizontal, this );
    brightnessSlider->setMinimum( -100 );
    brightnessSlider->setMaximum( 100 );
    brightnessSlider->setValue( 0 );
    QObject::connect( brightnessSlider, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( brightnessSliderValueChanged( int )) );

    contrastSlider = new QSlider( Qt::Horizontal, this );
    contrastSlider->setMinimum( 0 );
    contrastSlider->setMaximum( 1000 );
    contrastSlider->setValue( 100 );
    QObject::connect( contrastSlider, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( contrastSliderValueChanged( int )) );

    brightnessRBLabel = new QLabel( this );
    brightnessRBLabel->setText( QString( "%1%" ).arg( brightnessSlider->value() ) );

    contrastRBLabel = new QLabel( this );
    contrastRBLabel->setText( QString( "%1%" ).arg( contrastSlider->value() ) );


    brightnessContrastSub1Layout->addWidget( autoBrightnessCheckBox, 0, Qt::AlignLeft );
    brightnessContrastSub1Layout->addWidget( autoImageButton, 0, Qt::AlignLeft );
    brightnessContrastSub1Layout->addWidget( resetButton, 1, Qt::AlignLeft );

    brightnessContrastSub2Layout->addWidget( brightnessLabel, 0, 0 );
    brightnessContrastSub2Layout->addWidget( brightnessSlider, 0, 1 );
    brightnessContrastSub2Layout->addWidget( brightnessRBLabel, 0, 2 );

    brightnessContrastSub2Layout->addWidget( contrastLabel, 1, 0 );
    brightnessContrastSub2Layout->addWidget( contrastSlider, 1, 1 );
    brightnessContrastSub2Layout->addWidget( contrastRBLabel, 1, 2 );

    brightnessContrastSub2Layout->setColumnStretch( 1, 1 );  // Read back labels to take all spare room

    brightnessContrastMainLayout->addLayout( brightnessContrastSub1Layout );
    brightnessContrastMainLayout->addLayout( brightnessContrastSub2Layout );

    brightnessSlider->setValue( 0 );    // Range -100% (black) to +100% (white)
    contrastSlider->setValue( 100 );    // Range 0% (no difference in any pixels) to 1000% (10 times normal contrast)
}

int localBrightnessContrast::getBrightness()
{
    return brightnessSlider->value();

}

int localBrightnessContrast::getContrast()
{
    return contrastSlider->value();
}

bool localBrightnessContrast::getAutoBrightnessContrast()
{
    return autoBrightnessCheckBox->isChecked();
}

// Reset the brightness and contrast to normal
void localBrightnessContrast::brightnessContrastResetClicked( bool )
{
    // Reset brightness and contrast
    brightnessSlider->setValue( 0 );
    contrastSlider->setValue( 100 );
}

// Auto brightness and contrast check box has ben checked or unchecked
void localBrightnessContrast::brightnessContrastAutoImageClicked()
{
    emit brightnessContrastAutoImage();
}

// The local brightness slider has been moved
void localBrightnessContrast::brightnessSliderValueChanged( int localBrightnessIn )
{
    emit brightnessContrastChange();

    // Update the brightness
    brightnessRBLabel->setText( QString( "%1%" ).arg( localBrightnessIn ));
}

// The local contrast slider has been moved
void localBrightnessContrast::contrastSliderValueChanged( int localContrastIn )
{
    emit brightnessContrastChange();

    // Update the contrast
    contrastRBLabel->setText( QString( "%1%" ).arg( localContrastIn ));
}


void localBrightnessContrast::setBrightnessContrast( int brightness, int contrast )
{
    brightnessSlider->setValue( brightness );
    contrastSlider->setValue( contrast );
}

void localBrightnessContrast::setAutoBrightnessContrast( bool autoBrightnessContrast )
{
    autoBrightnessCheckBox->setChecked( autoBrightnessContrast );
}
